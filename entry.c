#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <immintrin.h>

/**
 * The function to optimise as part of the coursework.
 *
 * l0, l1, l2 and l3 record the amount of time spent in each loop
 * and should not be optimised out. :)
 */

void compute()
{

	double t0, t1;
	int vec_size = (N/4)*4;

	// Loop 0.
	t0 = wtime();
	
	memset(az, 0.0f, sizeof(float)*N);
	memset(ax, 0.0f, sizeof(float)*N);
	memset(ay, 0.0f, sizeof(float)*N);
	
	t1 = wtime();
	l0 += (t1 - t0);

	// Loop 1.
	t0 = wtime();

	int i;
	float rx, ry, rz;
	float r2, r2inv, r6inv, s;
	int j = 0;
	__m128 vec_eps = _mm_set1_ps(eps);

#pragma omp parallel for schedule(static) shared(N)
	for (i=0; i<vec_size; i+=4)
	{
		__m128 vec_x_i = _mm_load_ps(i+x), vec_y_i = _mm_load_ps(i+y), vec_z_i = _mm_load_ps(i+z);
		for (j=0;j<N;j++)
		{
			__m128 vec_rx = _mm_sub_ps(_mm_set1_ps(x[j]), vec_x_i);
			__m128 vec_ry = _mm_sub_ps(_mm_set1_ps(y[j]), vec_y_i);
			__m128 vec_rz = _mm_sub_ps(_mm_set1_ps(z[j]), vec_z_i);
		
			__m128 vec_r2inv = _mm_rsqrt_ps(_mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(vec_rx, vec_rx), _mm_mul_ps(vec_ry, vec_ry)), _mm_mul_ps(vec_rz, vec_rz)), vec_eps));
			__m128 vec_s = _mm_mul_ps(_mm_set1_ps(m[j]), _mm_mul_ps(vec_r2inv, _mm_mul_ps(vec_r2inv, vec_r2inv)));
			__m128 vec_ax_add = _mm_add_ps(_mm_mul_ps(vec_s, vec_rx), _mm_load_ps(i + ax));
			_mm_store_ps(ax + i, vec_ax_add);

			__m128 v_ry_mul = _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(m[j]), _mm_mul_ps(vec_r2inv, _mm_mul_ps(vec_r2inv, vec_r2inv))), vec_ry);
			__m128 vec_ay_add = _mm_add_ps(v_ry_mul, _mm_load_ps(ay + i));
			_mm_store_ps(ay + i, vec_ay_add);

			__m128 vec_rz_mul = _mm_mul_ps(vec_s, vec_rz);
			__m128 vec_az_add = _mm_add_ps(vec_rz_mul, _mm_load_ps(az + i));
			_mm_store_ps(az + i, vec_az_add);
		}
	}
	for (; i < N; i++)
	{
		for (; j < N; j++)
		{
			rx = x[j] - x[i];
			ry = y[j] - y[i];
			rz = z[j] - z[i];

			r2 = rx * rx + ry * ry + rz * rz + eps;
			r2inv = 1.0f / sqrt(r2);
			r6inv = r2inv * r2inv * r2inv;
			s = m[j] * r6inv;
			
			ax[i] += s * rx;
			ay[i] += s * ry;
			az[i] += s * rz;
		}
	}

	t1 = wtime();
	l1 += (t1 - t0);

	// Loop 2.
	t0 = wtime();

	float dt_dmp = dt * dmp;
	__m128 vec_dt_dmp = _mm_set1_ps(dt_dmp);
	for (i=0; i<vec_size; i+=4)
	{
		_mm_store_ps(vx + i, _mm_add_ps(_mm_mul_ps(vec_dt_dmp, _mm_load_ps(ax + i)), _mm_load_ps(vx + i)));
	}
	for (i=0; i<vec_size; i+=4)
	{
		_mm_store_ps(vy + i, _mm_add_ps(_mm_mul_ps(vec_dt_dmp, _mm_load_ps(ay + i)), _mm_load_ps(vy + i)));
	}
	for (i = 0; i < vec_size; i += 4)
	{
		_mm_store_ps(vz + i, _mm_add_ps(_mm_mul_ps(vec_dt_dmp, _mm_load_ps(az + i)), _mm_load_ps(vz + i)));
	}
	for (; i < N; i++)
	{
		vx[i] += dmp * (dt * ax[i]);
		vy[i] += dmp * (dt * ay[i]);
		vz[i] += dmp * (dt * az[i]);
	}

	t1 = wtime();
	l2 += (t1 - t0);

	// Loop 3.
	t0 = wtime();

	for (int i = 0; i < N; i++)
	{
		x[i] += dt * vx[i];
		y[i] += dt * vy[i];
		z[i] += dt * vz[i];
		if (x[i] >= 1.0f || x[i] <= -1.0f)
			vx[i] *= -1.0f;
		if (y[i] >= 1.0f || y[i] <= -1.0f)
			vy[i] *= -1.0f;
		if (z[i] >= 1.0f || z[i] <= -1.0f)
			vz[i] *= -1.0f;
	}
	t1 = wtime();
	l3 += (t1 - t0);
}
