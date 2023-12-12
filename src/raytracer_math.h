#if !defined(RAYTRACER_MATH_H)

#include <random>

#define SQUARE(x) ((x) * (x))
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) < (y) ? (y) : (x))

#define PI32 3.14159265358979f

#define DEG_TO_RAD(deg) (((deg) * PI32) / 180.f)

#define E1 v3f(1.0f, 0.0f, 0.0f);
#define E2 v3f(0.0f, 1.0f, 0.0f);
#define E3 v3f(0.0f, 0.0f, 1.0f);


const f32 inf = std::numeric_limits<f32>::infinity();

class v3f
{
	public:
		v3f() {}
		v3f(f32 e0, f32 e1, f32 e2) {e[0] = e0; e[1] = e1; e[2] = e2;}

		inline f32 x() const {return e[0];}
		inline f32 y() const {return e[1];}
		inline f32 z() const {return e[2];}
		inline f32 r() const {return e[0];}
		inline f32 g() const {return e[1];}
		inline f32 b() const {return e[2];}

		inline const v3f& operator+() const {return *this; }
		inline v3f operator-() const {return v3f(-e[0], -e[1], -e[2]);}
		inline f32 operator[](int i) const {return e[i]; }
		inline f32& operator[](int i) {return e[i]; }

		inline v3f& operator+=(const v3f &V);
		inline v3f& operator-=(const v3f &V);
		inline v3f& operator*=(const v3f &V);
		inline v3f& operator/=(const v3f &V);
		inline v3f& operator*=(const f32 t);
		inline v3f& operator/=(const f32 t);

		inline f32 length() const {return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);}
		inline f32 length_squared() const {return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];}
		inline void normalize();

		f32 e[3];
};

inline void
v3f::normalize()
{
	f32 k = (1.0 / sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]));
	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
}

inline v3f&
v3f::operator+=(const v3f &V)
{
	e[0] += V.e[0];
	e[1] += V.e[1];
	e[2] += V.e[2];

	return *this;
}

inline v3f&
v3f::operator*=(const v3f &V)
{
	e[0] *= V.e[0];
	e[1] *= V.e[1];
	e[2] *= V.e[2];

	return *this;
}

inline v3f&
v3f::operator-=(const v3f& V)
{
	e[0] -= V.e[0];
	e[1] -= V.e[1];
	e[2] -= V.e[2];

	return *this;
}

inline v3f&
v3f::operator*=(const f32 t)
{
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;

	return *this;
}

inline v3f
operator+(const v3f &U, const v3f &V)
{
	return v3f(U.e[0] + V.e[0], U.e[1] + V.e[1], U.e[2] + V.e[2]);
}

inline v3f
operator-(const v3f &U, const v3f &V)
{
	return v3f(U.e[0] - V.e[0], U.e[1] - V.e[1], U.e[2] - V.e[2]);
}

inline v3f
operator*(f32 t, const v3f &V)
{
	return v3f(t * V.e[0], t * V.e[1], t * V.e[2]);
}

inline v3f
operator*(const v3f &V, f32 t)
{
	return(t * V);
}

inline f32
dot(const v3f &U, const v3f &V)
{
	return(U.e[0] * V.e[0] + U.e[1] * V.e[1] + U.e[2] * V.e[2]);
}

inline v3f
cross(const v3f &U, const v3f &V)
{
	return v3f(U.e[1] * V.e[2] - U.e[2] * V.e[1],
			   U.e[2] * V.e[0] - U.e[0] * V.e[2],
			   U.e[0] * V.e[1] - U.e[1] * V.e[0]);
}

inline v3f
unit_vector(v3f V)
{
	return (1.0f / V.length()) * V;
}

inline v3f
v3f_hadamard(v3f U, v3f V)
{
	v3f Result;

	Result[0] = U[0] * V[0];
	Result[1] = U[1] * V[1];
	Result[2] = U[2] * V[2];

	return(Result);
}

inline v3f
reflect(const v3f &V, const v3f &N)
{
	v3f Result = {};

	Result = V - 2 * dot(V, N) * N;

	return(Result);
}

inline v3f
refract(const v3f & V, const v3f &Normal, f32 etai_over_etat)
{
	v3f Result;

	f32 cos_theta = MIN(dot(-V, Normal), 1.0f);
	v3f Perp = etai_over_etat * (V + cos_theta * Normal);
	v3f Parallel = -sqrt(ABS(1.0f - Perp.length_squared())) * Normal;
	
	Result = Perp + Parallel;

	return(Result);
}

inline f32
reflectance(f32 cosine, f32 ref_idx)
{
	f32 Result = 0.0f;

	f32 r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = SQUARE(r0);

	Result = r0 + (1.0f - r0) * pow((1.0f - cosine), 5);

	return(Result);
}

inline f32
random_unit()
{
	static std::uniform_real_distribution<f32> distribution(0.0f, 1.0f);
	static std::mt19937 Generator;

	return(distribution(Generator));
}

inline f32
random_interval(f32 min, f32 max)
{
	// NOTE(Justin): Generates rand real in [min, max).
	f32 Result = 0.0f;

	Result = min + (max - min) * random_unit();

	return(Result);
}

inline v3f
v3f_rand()
{
	v3f Result = v3f(random_unit(), random_unit(), random_unit());

	return(Result);
}

inline v3f
v3f_rand_interval(f32 min, f32 max)
{
	v3f Result = v3f(random_interval(min, max), random_interval(min, max), random_interval(min, max));

	return(Result);
}

inline v3f
v3f_rand_in_unit_sphere()
{
	v3f Result = {};
	do
	{
		Result = 2.0f * v3f(random_unit(), random_unit(), random_unit()) - v3f(1.0f, 1.0f, 1.0f);
	} while(Result.length_squared() >= 1.0f);

	return(Result);
}

inline v3f
v3f_rand_unit_vector()
{
	v3f Result = unit_vector(v3f_rand_in_unit_sphere());

	return(Result);
}

inline v3f
v3f_rand_in_hemisphere(const v3f &Normal)
{
	v3f Result = v3f_rand_unit_vector();

	if((dot(Result, Normal) <= 0.0f))
	{
		Result = -1.0f * Result;
	}

	return(Result);
}

inline f32
clamp(f32 a, f32 x, f32 b)
{
	f32 Result = x;
	if(x < a)
	{
		Result = a;
	}
	else if(x > b)
	{
		Result = b;
	}

	return(Result);
}

inline v3f
color_linear_to_gamma2(v3f Color)
{
	v3f Result = Color;

	Result[0] = sqrt(Result[0]);
	Result[1] = sqrt(Result[1]);
	Result[2] = sqrt(Result[2]);

	return(Result);
}

inline b32
v3f_near_zero(v3f V)
{
	b32 Result = false;

	f32 epsilon = 1e-8;
	if((ABS(V[0]) < epsilon) &&
	   (ABS(V[1]) < epsilon) &&
	   (ABS(V[2]) < epsilon))
	{
		Result = true;
	}
	return(Result);
}

inline v3f
v3f_rand_in_unit_disk()
{
	v3f Result;
	do
	{
		Result = v3f(random_interval(-1.0f, 1.0f), random_interval(-1.0f, 1.0f), 0.0f);
	} while(Result.length_squared() >= 1.0f);

	return(Result);
}

inline s32
s32_rand_interval(s32 min, s32 max)
{
	s32 Result = 0;

	Result = s32(random_interval(min, max + 1));

	return(Result);
}

//
// NOTE(Justin): v2f 
//

class v2f
{
	public:
		v2f() {}
		v2f(f32 e0, f32 e1);

		inline const v2f& operator+() const {return *this; }
		inline f32 operator[](int i) const {return e[i]; }
		inline f32& operator[](int i) {return e[i]; }

		inline v3f& operator+=(const v3f &V);
		inline v3f& operator-=(const v3f &V);

		inline f32 length() const; 
		inline f32 length_squared() const;
		inline void normalize();

		f32 e[2];
};

v2f::v2f(f32 e0, f32 e1)
{
	e[0] = e0;
	e[1] = e1;
}

inline v2f
operator+(const v2f &U, const v2f &V)
{
	return v2f(U.e[0] + V.e[0], U.e[1] + V.e[1]);
}

inline v2f
operator-(const v2f &U, const v2f &V)
{
	return v2f(U.e[0] - V.e[0], U.e[1] - V.e[1]);
}

inline v2f
operator*(f32 t, const v2f &V)
{
	return v2f(t * V.e[0], t * V.e[1]);
}

inline v2f
operator*(const v2f &V, f32 t)
{
	return(t * V);
}

//
// NOTE(Justin): Interval class
//

class interval
{
	public:
		interval() {min = inf; max = -inf;}
		interval(f32 _min, f32 _max);
		interval(const interval &A, const interval &B);
		f32 size() const;
		b32 contains(f32 x) const;
		b32 surrounds(f32 x) const;
		interval expand(f32 delta) const;

		f32 min, max;
};

interval::interval(f32 _min, f32 _max)
{
	min = _min;
	max = _max;
}

interval::interval(const interval &A, const interval &B)
{
	min = MIN(A.min, B.min);
	max = MAX(A.max, B.max);
}

static const interval empty(inf, -inf);
static const interval universe(-inf, inf);

f32
interval::size() const
{
	return(max - min);
}

b32
interval::contains(f32 x) const
{
	b32 Result = false;

	Result = ((min <= x) && (x <= max));

	return(Result);
}

b32
interval::surrounds(f32 x) const
{
	// NOTE(JE): This checks whether or not x is an interior point of the interval.
	b32 Result = false;

	Result = ((min < x) && (x < max));

	return(Result);
}

interval
interval::expand(f32 delta) const
{
	interval Result;

	f32 padding = (delta / 2.0f);

	Result = interval(min - padding, max + padding);

	return(Result);
}

interval
operator+(const interval &I, f32 delta)
{
	interval Result;

	Result = interval(I.min + delta, I.max + delta);

	return(Result);
}

interval
operator+(f32 delta, const interval &I)
{
	interval Result;

	Result = I + delta;

	return(Result);
}

//
// NOTE(Justin): Perlin noise class
//

class perlin
{
	public:
		perlin();
		~perlin();
		s32 *permutation_generate();
		void permute(s32 *indices, u32 permute_count);
		f32 lerp_trilinear(v3f Directions[2][2][2], f32 tx, f32 ty, f32 tz) const;
		f32 noise(const v3f &V) const;
		f32 turbulence(const v3f &V, s32 depth = 7) const;

		// TODO(Justin): Change the name of the count variable.
		u32 v3f_count = 256;
		v3f *v3fs_random;
		s32 *perm_x;
		s32 *perm_y;
		s32 *perm_z;
};

perlin::perlin()
{
	//random_f32s = new f32[f32_count];
	v3fs_random = new v3f[v3f_count];

	for(u32 index = 0; index < v3f_count; index++)
	{
		//random_f32s[f32_index] = random_unit();
		v3fs_random[index] = unit_vector(v3f_rand_interval(-1.0f, 1.0f));
	}

	perm_x = permutation_generate();
	perm_y = permutation_generate();
	perm_z = permutation_generate();
}

perlin::~perlin()
{
	//delete[] random_f32s;
	delete[] v3fs_random;
	delete[] perm_x;
	delete[] perm_y;
	delete[] perm_z;
}

s32 *
perlin::permutation_generate()
{
	// TODO(Justin): Change the name of the count variable.
	s32 *Result = new s32[v3f_count];

	for(u32 i = 0; i < v3f_count; i++)
	{
		Result[i] = i;
	}

	permute(Result, v3f_count);

	return(Result);
}

void
perlin::permute(s32 *indices, u32 permute_count)
{
	for(u32 index = permute_count - 1; index > 0; index--)
	{
		s32 target = s32_rand_interval(0, index);
		s32 tmp = indices[index];
		indices[index] = indices[target];
		indices[target] = tmp;
	}
}

f32
perlin::noise(const v3f &V) const
{
	f32 Result = 0.0f;

	// NOTE(Justin): These are the t parameters of the trilinear interpolation
	f32 tx = V.x() - floor(V.x());
	f32 ty = V.y() - floor(V.y());
	f32 tz = V.z() - floor(V.z());

	// NOTE(Justin) Hermitian smoothing
	tx = SQUARE(tx) * (3.0f - 2.0f * tx);
	ty = SQUARE(ty) * (3.0f - 2.0f * ty);
	tz = SQUARE(tz) * (3.0f - 2.0f * tz);

	// NOTE(Justin): The coordinates of the lattice point associated with V
	s32 lattice_x = (s32)floor(V.x());
	s32 lattice_y = (s32)floor(V.y());
	s32 lattice_z = (s32)floor(V.z());

	//f32 noise_samples[2][2][2];
	v3f Directions[2][2][2];
	for(s32 i = 0; i < 2; i++)
	{
		for(s32 j = 0; j < 2; j++)
		{
			for(s32 k = 0; k < 2; k++)
			{
				s32 index = ((perm_x[(lattice_x + i) & 255]) ^
							 (perm_y[(lattice_y + j) & 255]) ^
							 (perm_z[(lattice_z + k) & 255]));

				Directions[i][j][k] = v3fs_random[index];
			}
		}
	}

	Result = lerp_trilinear(Directions, tx, ty, tz);

	return(Result);
}

f32
perlin::lerp_trilinear(v3f Directions[2][2][2], f32 tx, f32 ty, f32 tz) const
{
	f32 Result = 0.0f;
	for(s32 x = 0; x < 2; x++)
	{
		for(s32 y = 0; y < 2; y++)
		{
			for(s32 z = 0; z < 2; z++)
			{
#if 0
				Result += (x * tx + (1 - x) * (1 - tx)) *
						  (y * ty + (1 - y) * (1 - ty)) *
						  (z * tz + (1 - z) * (1 - tz)) * noise_samples[x][y][z];
#else
				v3f Weight = v3f(tx - x, ty - y, tz - z);
				Result += (x * tx + (1 - x) * (1 - tx)) *
						  (y * ty + (1 - y) * (1 - ty)) *
						  (z * tz + (1 - z) * (1 - tz)) *
						  dot(Directions[x][y][z], Weight);
#endif
			}
		}
	}
	return(Result);
}

f32
perlin::turbulence(const v3f &V, s32 depth) const
{
	f32 Result = 0.0f;
	v3f Tmp = V;
	f32 weight = 1.0f;

	for(u32 index = 0; index < depth; index++)
	{
		Result += weight * noise(Tmp);
		weight *= 0.5f;
		Tmp *= 2.0f;
	}

	Result = ABS(Result);

	return(Result);
}




#define RAYTRACER_MATH_H 
#endif
