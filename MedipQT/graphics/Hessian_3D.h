#pragma once

#ifndef HESSIAN_3D_H
#define HESSIAN_3D_H

#include <math.h>

#ifdef MAX
#undef MAX
#endif

#define MAX(a, b) ((a)>(b)?(a):(b))

#define ndim 3

static double hypot2(double x, double y) {
	return sqrt(x*x + y*y);
}

// Symmetric Householder reduction to tridiagonal form.

static void tred2(double V[ndim][ndim], double d[ndim], double e[ndim]) {

	//  This is derived from the Algol procedures tred2 by
	//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
	//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
	//  Fortran subroutine in EISPACK.

	for (int j = 0; j < ndim; j++) {
		d[j] = V[ndim - 1][j];
	}

	// Householder reduction to tridiagonal form.

	for (int i = ndim - 1; i > 0; i--) {

		// Scale to avoid under/overflow.

		double scale = 0.0;
		double h = 0.0;
		for (int k = 0; k < i; k++) {
			scale = scale + fabs(d[k]);
		}
		if (scale == 0.0) {
			e[i] = d[i - 1];
			for (int j = 0; j < i; j++) {
				d[j] = V[i - 1][j];
				V[i][j] = 0.0;
				V[j][i] = 0.0;
			}
		}
		else {

			// Generate Householder vector.

			for (int k = 0; k < i; k++) {
				d[k] /= scale;
				h += d[k] * d[k];
			}
			double f = d[i - 1];
			double g = sqrt(h);
			if (f > 0) {
				g = -g;
			}
			e[i] = scale * g;
			h = h - f * g;
			d[i - 1] = f - g;
			for (int j = 0; j < i; j++) {
				e[j] = 0.0;
			}

			// Apply similarity transformation to remaining columns.

			for (int j = 0; j < i; j++) {
				f = d[j];
				V[j][i] = f;
				g = e[j] + V[j][j] * f;
				for (int k = j + 1; k <= i - 1; k++) {
					g += V[k][j] * d[k];
					e[k] += V[k][j] * f;
				}
				e[j] = g;
			}
			f = 0.0;
			for (int j = 0; j < i; j++) {
				e[j] /= h;
				f += e[j] * d[j];
			}
			double hh = f / (h + h);
			for (int j = 0; j < i; j++) {
				e[j] -= hh * d[j];
			}
			for (int j = 0; j < i; j++) {
				f = d[j];
				g = e[j];
				for (int k = j; k <= i - 1; k++) {
					V[k][j] -= (f * e[k] + g * d[k]);
				}
				d[j] = V[i - 1][j];
				V[i][j] = 0.0;
			}
		}
		d[i] = h;
	}

	// Accumulate transformations.

	for (int i = 0; i < ndim - 1; i++) {
		V[ndim - 1][i] = V[i][i];
		V[i][i] = 1.0;
		double h = d[i + 1];
		if (h != 0.0) {
			for (int k = 0; k <= i; k++) {
				d[k] = V[k][i + 1] / h;
			}
			for (int j = 0; j <= i; j++) {
				double g = 0.0;
				for (int k = 0; k <= i; k++) {
					g += V[k][i + 1] * V[k][j];
				}
				for (int k = 0; k <= i; k++) {
					V[k][j] -= g * d[k];
				}
			}
		}
		for (int k = 0; k <= i; k++) {
			V[k][i + 1] = 0.0;
		}
	}
	for (int j = 0; j < ndim; j++) {
		d[j] = V[ndim - 1][j];
		V[ndim - 1][j] = 0.0;
	}
	V[ndim - 1][ndim - 1] = 1.0;
	e[0] = 0.0;
}

// Symmetric tridiagonal QL algorithm.

static void tql2(double V[ndim][ndim], double d[ndim], double e[ndim]) {

	//  This is derived from the Algol procedures tql2, by
	//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
	//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
	//  Fortran subroutine in EISPACK.

	for (int i = 1; i < ndim; i++) {
		e[i - 1] = e[i];
	}
	e[ndim - 1] = 0.0;

	double f = 0.0;
	double tst1 = 0.0;
	double eps = pow(2.0, -52.0);
	for (int l = 0; l < ndim; l++) {

		// Find small subdiagonal element

		tst1 = MAX(tst1, fabs(d[l]) + fabs(e[l]));
		int m = l;
		while (m < ndim) {
			if (fabs(e[m]) <= eps*tst1) {
				break;
			}
			m++;
		}

		// If m == l, d[l] is an eigenvalue,
		// otherwise, iterate.

		if (m > l) {
			int iter = 0;
			do {
				iter = iter + 1;  // (Could check iteration count here.)

								  // Compute implicit shift

				double g = d[l];
				double p = (d[l + 1] - g) / (2.0 * e[l]);
				double r = hypot2(p, 1.0);
				if (p < 0) {
					r = -r;
				}
				d[l] = e[l] / (p + r);
				d[l + 1] = e[l] * (p + r);
				double dl1 = d[l + 1];
				double h = g - d[l];
				for (int i = l + 2; i < ndim; i++) {
					d[i] -= h;
				}
				f = f + h;

				// Implicit QL transformation.

				p = d[m];
				double c = 1.0;
				double c2 = c;
				double c3 = c;
				double el1 = e[l + 1];
				double s = 0.0;
				double s2 = 0.0;
				for (int i = m - 1; i >= l; i--) {
					c3 = c2;
					c2 = c;
					s2 = s;
					g = c * e[i];
					h = c * p;
					r = hypot2(p, e[i]);
					e[i + 1] = s * r;
					s = e[i] / r;
					c = p / r;
					p = c * d[i] - s * g;
					d[i + 1] = h + s * (c * g + s * d[i]);

					// Accumulate transformation.

					for (int k = 0; k < ndim; k++) {
						h = V[k][i + 1];
						V[k][i + 1] = s * V[k][i] + c * h;
						V[k][i] = c * V[k][i] - s * h;
					}
				}
				p = -s * s2 * c3 * el1 * e[l] / dl1;
				e[l] = s * p;
				d[l] = c * p;

				// Check for convergence.

			} while (fabs(e[l]) > eps*tst1);
		}
		d[l] = d[l] + f;
		e[l] = 0.0;
	}

	// Sort eigenvalues and corresponding vectors.

	for (int i = 0; i < ndim - 1; i++) {
		int k = i;
		double p = d[i];
		for (int j = i + 1; j < ndim; j++) {
			if (d[j] < p) {
				k = j;
				p = d[j];
			}
		}
		if (k != i) {
			d[k] = d[i];
			d[i] = p;
			for (int j = 0; j < ndim; j++) {
				p = V[j][i];
				V[j][i] = V[j][k];
				V[j][k] = p;
			}
		}
	}
}

static void eigen_decomposition(double A[ndim][ndim], double V[ndim][ndim], double d[ndim]) {
	double e[ndim];
	for (int i = 0; i < ndim; i++) {
		for (int j = 0; j < ndim; j++) {
			V[i][j] = A[i][j];
		}
	}
	tred2(V, d, e);
	tql2(V, d, e);

}


struct eigenvector
{
	double x;
	double y;
	double z;



};

struct eigenvalue
{
	double lamda1;
	double lamda2;
	double lamda3;

};

struct struct_FF3D {
	double FrangiScaleRange_min;
	double FrangiScaleRange_max;
	double FrangiScaleRatio;
	double FrangiAlpha;
	double FrangiBeta;
	int FrangiC;
	bool verbose;
	bool BlackWhite;
};

static void CalcEigenvalues(double m[3][3], double *eigenvalue)
{


	double PI = 3.141592654;
	if (m[0][0] == 0 && m[1][1] == 0 && m[2][2] == 0)
	{
		eigenvalue[0] = 0;
		eigenvalue[1] = 0;
		eigenvalue[2] = 0;
	}
	else
	{
		float p = -(m[0][0] + m[1][1] + m[2][2]);
		float q = (m[0][0] * m[1][1] + m[0][0] * m[2][2] + m[1][1] * m[2][2]
			- m[1][2] * m[2][1] - m[0][1] * m[1][0] - m[0][2] * m[2][0]);
		float r = (-m[0][0] * m[1][1] * m[2][2] + m[0][0] * m[1][2] * m[2][1]
			+ m[0][1] * m[1][0] * m[2][2] - m[0][1] * m[1][2] * m[2][0]
			- m[0][2] * m[1][0] * m[2][1] + m[0][2] * m[1][1] * m[2][0]);

		float a = q - (p*p) / 3;
		float b = (2 * p*p*p) / 27 - (p*q) / 3 + r;

		float theta2;
		float tempAcos;
		// Because of Numerical error, we need to use epsillon
		// to check if the term is 1 or not
		//    if (fabs(0.25*b*b+a*a*a/27.0)<epsillon || a==0) theta2 = 0 ;
		//    else theta2 = acos(-(b/2)/sqrt(-a*a*a/27)) ;

		if ((tempAcos = -(b / 2) / sqrt(-a*a*a / 27)) > 1.0)
		{
			tempAcos = 1.0;
			theta2 = acos(tempAcos);
		}
		else
		{
			theta2 = acos(tempAcos);
		}

		eigenvalue[0] = 2 * sqrt(-a / 3)*cos(theta2 / 3) - p / 3;
		eigenvalue[1] = -2 * sqrt(-a / 3)*cos((theta2 + PI) / 3) - p / 3;
		eigenvalue[2] = -2 * sqrt(-a / 3)*cos((theta2 - PI) / 3) - p / 3;


		// 		CString asdf;
		// 		asdf.Format(_T("e0: %f e1: %f e2: %f"), eigenvalue[0], eigenvalue[1],eigenvalue[2]);
		// 		AfxMessageBox(asdf);
		//    cerr << (Matrix3D) *this ;
		//    cerr << "\nLambda\n" << lambda[0] << "\t" << lambda[1] 
		//	 << "\t" << lambda[2] << endl ;
	}

}

static double Fx(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = input[z * height * width + y * width + x - 1];
	double fr = input[z * height * width + y * width + x + 1];
	return (fr - fl) / (2.0);
}

static double Fy(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = input[z * height * width + (y - 1) * width + x];
	double fr = input[z * height * width + (y + 1) * width + x];
	return (fr - fl) / (2.0);
}

static double Fz(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = input[(z - 1) * height * width + (y)* width + x];
	double fr = input[(z + 1) * height * width + (y)* width + x];
	return (fr - fl) / (2.0);
}

static double Fxx(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fx(input, x - 1, y, z, width, height, slice);
	double fr = Fx(input, x + 1, y, z, width, height, slice);
	return (fr - fl) / (2.0);
}


static double Fxy(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fx(input, x, y - 1, z, width, height, slice);
	double fr = Fx(input, x, y + 1, z, width, height, slice);
	return (fr - fl) / (2.0);
}



static double Fyx(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fy(input, x - 1, y, z, width, height, slice);
	double fr = Fy(input, x + 1, y, z, width, height, slice);
	return (fr - fl) / (2.0);
}


static double Fzx(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fz(input, x - 1, y, z, width, height, slice);
	double fr = Fz(input, x + 1, y, z, width, height, slice);
	return (fr - fl) / (2.0);
}

static double Fxz(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fx(input, x, y, z - 1, width, height, slice);
	double fr = Fx(input, x, y, z + 1, width, height, slice);
	return (fr - fl) / (2.0);
}

static double Fyz(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fy(input, x, y, z - 1, width, height, slice);
	double fr = Fy(input, x, y, z + 1, width, height, slice);
	return (fr - fl) / (2.0);
}

static double Fzy(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fz(input, x, y - 1, z, width, height, slice);
	double fr = Fz(input, x, y + 1, z, width, height, slice);
	return (fr - fl) / (2.0);
}

static double Fyy(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fy(input, x, y - 1, z, width, height, slice);
	double fr = Fy(input, x, y + 1, z, width, height, slice);
	return (fr - fl) / (2.0);
}

static double Fzz(short *input, int x, int y, int z, int width, int height, int slice)
{

	double fl = Fz(input, x, y, z - 1, width, height, slice);
	double fr = Fz(input, x, y, z + 1, width, height, slice);
	return (fr - fl) / (2.0);
}


static void HessianMatrix(short *input, unsigned char *layer, unsigned char *output, int width, int height, int slice, int extractor_mode)
{

	
	eigenvalue *e_value = new eigenvalue[width * height * slice];
	eigenvector *e_vector = new eigenvector[width * height * slice];

	int i, j, k;

	for (i = 2; i < slice - 2; i++)
	{
		for (j = 2; j < height - 2; j++)
		{
			for (k = 2; k < width - 2; k++)
			{
				float dxy, dyz, dxz, dyx, dzy, dzx, dxx, dyy, dzz;
				int originPos = i*width*height + j*width + k;
				e_value[originPos].lamda1 = 0;
				e_value[originPos].lamda2 = 0;
				e_value[originPos].lamda3 = 0;


				if (layer[originPos] != 0)
				{

					double hessianM[3][3] = { 0 };


					dxx = Fxx(input, k, j, i, width, height, slice);
					dxy = Fxy(input, k, j, i, width, height, slice);
					dxz = Fxz(input, k, j, i, width, height, slice);
					dyx = Fyx(input, k, j, i, width, height, slice);
					dyy = Fyy(input, k, j, i, width, height, slice);
					dyz = Fyz(input, k, j, i, width, height, slice);
					dzx = Fzx(input, k, j, i, width, height, slice);
					dzy = Fzy(input, k, j, i, width, height, slice);
					dzz = Fzz(input, k, j, i, width, height, slice);


					// Create Hessian
					hessianM[0][0] = dxx;		hessianM[0][1] = dxy;		hessianM[0][2] = dxz;
					hessianM[1][0] = dyx;		hessianM[1][1] = dyy;		hessianM[1][2] = dyz;
					hessianM[2][0] = dzx;		hessianM[2][1] = dzy;		hessianM[2][2] = dzz;




					double temp_value[3];
					double temp_vector[3][3];


					eigen_decomposition(hessianM, temp_vector, temp_value);


					e_value[originPos].lamda1 = temp_value[0];
					e_value[originPos].lamda2 = temp_value[1];
					e_value[originPos].lamda3 = temp_value[2];

					e_vector[originPos].x = temp_vector[0][0];
					e_vector[originPos].y = temp_vector[1][0];
					e_vector[originPos].z = temp_vector[2][0];



				}


			}
		}
	}


	struct_FF3D defaultoptions = { 1, 3, 1, 0.5, 0.5, 500, true, false };


	for (int z = 0; z < slice; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (layer[z * height * width + y * width + x] != 0)
				{
					double Lambda1 = e_value[z * height * width + y * width + x].lamda3;
					double Lambda2 = e_value[z * height * width + y * width + x].lamda2;
					double Lambda3 = e_value[z * height * width + y * width + x].lamda1;

					double LambdaAbs1 = abs(Lambda1);
					double LambdaAbs2 = abs(Lambda2);
					double LambdaAbs3 = abs(Lambda3);


					if (extractor_mode == HESSIAN_VESSEL)
					{
						// The Vesselness Features
						double Ra = LambdaAbs2 / LambdaAbs3;
						double Rb = LambdaAbs1 / sqrt(LambdaAbs2 * LambdaAbs3);



						double s = sqrt(pow(LambdaAbs1, 2) + pow(LambdaAbs2, 2) + pow(LambdaAbs3, 2));

						double A = 2 * pow(defaultoptions.FrangiAlpha, 2);
						double B = 2 * pow(defaultoptions.FrangiBeta, 2);
						double C = 2 * pow(defaultoptions.FrangiC, 2);



						//Compute Vesselness function
						double expRa = (1 - exp(-(pow(Ra, 2) / A)));
						double expRb = exp(-(pow(Rb, 2) / B));
						double expS = (1 - exp(-(pow(s, 2) / C)));

						//Compute Vesselness function
						double Voxel_data = expRa * expRb * expS;

						// 

						if (defaultoptions.BlackWhite) // white object
//						if (!defaultoptions.BlackWhite) // black object
						{
							if (Lambda2 < 0 || Lambda3 < 0)
							{
								Voxel_data = 0;
							}
							else
							{
								Voxel_data = Voxel_data;
							}
						}
						else
						{
							if (Lambda2 > 0 || Lambda3 > 0)
							{
								Voxel_data = 0;
							}
							else
							{
								Voxel_data = Voxel_data;
							}
						}

						// Remove NaN values
						if (!std::isfinite(Voxel_data))
						{
							Voxel_data = 0;
						}
						double thr = 0.01; // white object
//						double thr = 0.00; // black object
						if (Voxel_data > thr)
							output[z * height * width + y * width + x] = 255;
							
						else output[z * height * width + y * width + x] = 0;
					}
					else if (extractor_mode == HESSIAN_FISSURE)
					{
 						double thr=3.2;
 						if (LambdaAbs3 / LambdaAbs1 > thr && LambdaAbs3 / LambdaAbs2 > thr)
// 						double thr = 0.3;
// 						if ( (LambdaAbs3 - LambdaAbs2) / LambdaAbs3 > thr)// && input[z * height * width + y * width + x] >= -800 && input[z * height * width + y * width + x] <= -400)
							output[z * height * width + y * width + x] = 255;
							
							else output[z * height * width + y * width + x] = 0;
					}
				}
				else
					output[z * height * width + y * width + x] = 0;

			}
		}
	}

	delete[] e_value;
	delete[] e_vector;
}



static void ImageDownSampling(short *input, short *output, int width, int height, int slice)
{
	
	for (int z = 0;z < slice ;z++)
	{
		for (int y = 0;y < height ;y++)
		{
			for (int x = 0;x < width ;x++)
			{
				if(x==0 || y==0 || z==0 || x==width-1 || y==height-1 || z==slice-1)
					input[z  * width*height + y  * width + x] = 0;
				
				else output[z / 2 * width / 2 * height / 2 + y / 2 * width / 2 + x / 2] =

					(double)input[(z - 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z - 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
					(double)input[(z - 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
					(double)input[(z - 1)  * width*height + y  * width + x - 1] * 2. / 60. +
					(double)input[(z - 1)  * width*height + y  * width + x] * 3. / 60. +
					(double)input[(z - 1)  * width*height + y  * width + x + 1] * 2. / 60. +
					(double)input[(z - 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z - 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
					(double)input[(z - 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60. +

					(double)input[z  * width*height + (y - 1)  * width + x - 1] * 2. / 60. +
					(double)input[z  * width*height + (y - 1)  * width + x] * 3. / 60. +
					(double)input[z  * width*height + (y - 1)  * width + x + 1] * 2. / 60. +
					(double)input[z  * width*height + y  * width + x - 1] * 3. / 60. +
					(double)input[z  * width*height + y  * width + x] * 10. / 60. +
					(double)input[z  * width*height + y  * width + x + 1] * 3. / 60. +
					(double)input[z  * width*height + (y + 1)  * width + x - 1] * 2. / 60. +
					(double)input[z  * width*height + (y + 1)  * width + x] * 3. / 60. +
					(double)input[z  * width*height + (y + 1)  * width + x + 1] * 2. / 60. +

					(double)input[(z + 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z + 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
					(double)input[(z + 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
					(double)input[(z + 1)  * width*height + y  * width + x - 1] * 2. / 60. +
					(double)input[(z + 1)  * width*height + y  * width + x] * 3. / 60. +
					(double)input[(z + 1)  * width*height + y  * width + x + 1] * 2. / 60. +
					(double)input[(z + 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z + 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
					(double)input[(z + 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60.;

				x++;
			}
			y++;
		}
		z++;
	}
}


static void ImageDownSampling(unsigned char *input, unsigned char *output, int width, int height, int slice)
{
	memset(output, 0, width /2 * height /2 * slice /2 );
	for (int z = 1;z < slice-1 ;z++)
	{
		for (int y = 1;y < height-1 ;y++)
		{
			for (int x = 1;x < width-1 ;x++)
			{
// 				if (x == 0 || y == 0 || z == 0 || x == width - 1 || y == height - 1 || z == slice - 1)
// 					output[z / 2 * width / 2 * height / 2 + y / 2 * width / 2 + x / 2] = 0;
// 
// 					else 
					if( 128 < (int)
					(
					(double)input[(z - 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z - 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
					(double)input[(z - 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
					(double)input[(z - 1)  * width*height + y  * width + x - 1] * 2. / 60. +
					(double)input[(z - 1)  * width*height + y  * width + x] * 3. / 60. +
					(double)input[(z - 1)  * width*height + y  * width + x + 1] * 2. / 60. +
					(double)input[(z - 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z - 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
					(double)input[(z - 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60. +

					(double)input[z  * width*height + (y - 1)  * width + x - 1] * 2. / 60. +
					(double)input[z  * width*height + (y - 1)  * width + x] * 3. / 60. +
					(double)input[z  * width*height + (y - 1)  * width + x + 1] * 2. / 60. +
					(double)input[z  * width*height + y  * width + x - 1] * 3. / 60. +
					(double)input[z  * width*height + y  * width + x] * 10. / 60. +
					(double)input[z  * width*height + y  * width + x + 1] * 3. / 60. +
					(double)input[z  * width*height + (y + 1)  * width + x - 1] * 2. / 60. +
					(double)input[z  * width*height + (y + 1)  * width + x] * 3. / 60. +
					(double)input[z  * width*height + (y + 1)  * width + x + 1] * 2. / 60. +

					(double)input[(z + 1)  * width*height + (y - 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z + 1)  * width*height + (y - 1)  * width + x] * 2. / 60. +
					(double)input[(z + 1)  * width*height + (y - 1)  * width + x + 1] * 1. / 60. +
					(double)input[(z + 1)  * width*height + y  * width + x - 1] * 2. / 60. +
					(double)input[(z + 1)  * width*height + y  * width + x] * 3. / 60. +
					(double)input[(z + 1)  * width*height + y  * width + x + 1] * 2. / 60. +
					(double)input[(z + 1)  * width*height + (y + 1)  * width + x - 1] * 1. / 60. +
					(double)input[(z + 1)  * width*height + (y + 1)  * width + x] * 2. / 60. +
					(double)input[(z + 1)  * width*height + (y + 1)  * width + x + 1] * 1. / 60.))


					output[z / 2 * width / 2 * height / 2 + y / 2 * width / 2 + x / 2] = 255;
				x++;
			}
			y++;
		}
		z++;
	}
}



static void ImageGaussianFilter(unsigned char *input, int width, int height, int slice)
{
	unsigned char *output= new unsigned char[width*height*slice];
	memset(output, 0, width  * height  * slice );
	for (int z = 1;z < slice-1;z++)
	{
		for (int y = 1;y < height-1;y++)
		{
			for (int x = 1;x < width-1;x++)
			{				
			int count=0;
				if (input[(z - 1)  * width*height + (y - 1)  * width + x - 1] != 0) count++;
				if(input[(z - 1)  * width*height + (y - 1)  * width + x] != 0)count++;
				if (input[(z - 1)  * width*height + (y - 1)  * width + x + 1] != 0)count++;
				if (input[(z - 1)  * width*height + y  * width + x - 1] != 0)count++;
				if (input[(z - 1)  * width*height + y  * width + x] != 0)count++;
				if (input[(z - 1)  * width*height + y  * width + x + 1] != 0)count++;
				if (input[(z - 1)  * width*height + (y + 1)  * width + x - 1] != 0)count++;
				if (input[(z - 1)  * width*height + (y + 1)  * width + x] != 0)count++;
				if (input[(z - 1)  * width*height + (y + 1)  * width + x + 1] != 0)count++;

				if (input[z  * width*height + (y - 1)  * width + x - 1] != 0)count++;
				if (input[z  * width*height + (y - 1)  * width + x] != 0)count++;
				if (input[z  * width*height + (y - 1)  * width + x + 1] != 0)count++;
				if (input[z  * width*height + y  * width + x - 1] != 0)count++;
				if (input[z  * width*height + y  * width + x] != 0)count++;
				if (input[z  * width*height + y  * width + x + 1] != 0)count++;
				if (input[z  * width*height + (y + 1)  * width + x - 1] != 0)count++;
				if (input[z  * width*height + (y + 1)  * width + x] != 0)count++;
				if (input[z  * width*height + (y + 1)  * width + x + 1] != 0)count++;

				if (input[(z + 1)  * width*height + (y - 1)  * width + x - 1] != 0)count++;
				if (input[(z + 1)  * width*height + (y - 1)  * width + x] != 0)count++;
				if (input[(z + 1)  * width*height + (y - 1)  * width + x + 1] != 0)count++;
				if (input[(z + 1)  * width*height + y  * width + x - 1] != 0)count++;
				if (input[(z + 1)  * width*height + y  * width + x] != 0)count++;
				if (input[(z + 1)  * width*height + y  * width + x + 1] != 0)count++;
				if (input[(z + 1)  * width*height + (y + 1)  * width + x - 1] != 0)count++;
				if (input[(z + 1)  * width*height + (y + 1)  * width + x] != 0)count++;
				if (input[(z + 1)  * width*height + (y + 1)  * width + x + 1] != 0)count++;


				if(count==27) output[z  * width  * height  + y  * width  + x ] = 255;
				
			}
			
		}
		
	}
	delete []output;
}
static void ImageUpSampling_trilinear(unsigned char *input, unsigned char *output, int width, int height, int slice)
{
	
	memset(output, 0, (width * 2 )* (height * 2 ) * (slice * 2 ));

	//확대
	for (int z = 1; z < slice-1; z++)
	{
		for (int y = 1; y < height-1; y++)
		{
			for (int x = 1; x < width-1; x++)
			{
				int xx = x * 2;
				int yy = y * 2;
				int zz = z * 2;
 				if(input[z*width*height + y*width + x] != 0) 
				{
					
// 					output[((zz-1)* width * 2 * height * 2) + ((yy)* width * 2) + xx - 1] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy)* width * 2) + xx] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy)* width * 2) + xx + 1] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx - 1] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx + 1] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx - 1] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx] = 255;
// 					output[((zz - 1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx + 1] = 255;
// 					
// 					output[((zz )* width * 2 * height * 2) + ((yy)* width * 2) + xx-1] = 255;
 					output[((zz )* width * 2 * height * 2) + ((yy)* width * 2) + xx] = 255;
// 					output[((zz )* width * 2 * height * 2) + ((yy)* width * 2) + xx+1] = 255;
// 					output[((zz )* width * 2 * height * 2) + ((yy-1)* width * 2) + xx - 1] = 255;
//					output[((zz )* width * 2 * height * 2) + ((yy-1)* width * 2) + xx] = 255;
// 					output[((zz )* width * 2 * height * 2) + ((yy-1)* width * 2) + xx + 1] = 255;
// 					output[((zz )* width * 2 * height * 2) + ((yy+1)* width * 2) + xx - 1] = 255;
// 					output[((zz )* width * 2 * height * 2) + ((yy+1)* width * 2) + xx] = 255;
// 					output[((zz )* width * 2 * height * 2) + ((yy+1)* width * 2) + xx + 1] = 255;
// 
// 					output[((zz+1)* width * 2 * height * 2) + ((yy)* width * 2) + xx - 1] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy)* width * 2) + xx] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy)* width * 2) + xx + 1] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx - 1] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy - 1)* width * 2) + xx + 1] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx - 1] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx] = 255;
// 					output[((zz+1)* width * 2 * height * 2) + ((yy + 1)* width * 2) + xx + 1] = 255;
				}
			}
		}
	}
// 
// 	width = width * 2;
// 	height = height * 2;
// 	slice = slice * 2;
// 
// 	//x축방향
// 	for (int z = 0; z < slice - 1; z++)
// 	{
// 		for (int y = 0; y < height - 1; y++)
// 		{
// 			for (int x = 0; x < width - 1; x++)
// 			{
// 				if (z % 2 == 0)
// 				{
// 					if (y % 2 == 0)
// 					{
// 						if (x % 2 == 1)
// 						{
// 							short Q1 = output[(z*width*height) + (y*width) + (x - 1)];
// 							short Q2 = output[(z*width*height) + (y*width) + (x + 1)];
// 
// 							if((Q1 + Q2) / 2 > 128)
// 							output[z*width*height + y*width + x] = 255;
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// 
// 
// 	//y축방향
// 	for (int z = 0; z < slice - 1; z++)
// 	{
// 		for (int y = 0; y < height - 1; y++)
// 		{
// 			for (int x = 0; x < width - 1; x++)
// 			{
// 				if (z % 2 == 0)
// 				{
// 					if (y % 2 == 1)
// 					{
// 						short Q1 = output[(z*width*height) + ((y - 1)*width) + x];
// 						short Q2 = output[(z*width*height) + ((y + 1)*width) + x];
// 
// 						if ((Q1 + Q2) / 2 > 128)
// 							output[z*width*height + y*width + x] = 255;
// 					}
// 
// 				}
// 			}
// 		}
// 	}
// 
// 	//z축방향
// 	for (int z = 1; z < slice - 1; z++)
// 	{
// 		for (int y = 1; y < height - 1; y++)
// 		{
// 			for (int x = 1; x < width - 1; x++)
// 			{
// 				if (z % 2 == 1)
// 				{
// 					short Q1 = output[(z - 1)*width*height + y*width + x];
// 					short Q2 = output[(z + 1)*width*height + y*width + x];
// 
// 					if ((Q1 + Q2) / 2 > 128)
// 						output[z*width*height + y*width + x] = 255;
// 				}
// 			}
// 		}
// 	}
}


static void ImageUpSampling_trilinear(short *input, short *output, int width, int height, int slice)
{

	memset(output, 0, width * 2 * height * 2 * slice * 2 * sizeof(short));

	//확대
	for (int z = 0; z < slice; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int xx = x * 2;
				int yy = y * 2;
				int zz = z * 2;
				output[(zz * width * 2 * height * 2) + (yy * width * 2) + xx] = input[z*width*height + y*width + x];
			}
		}
	}

	width = width * 2;
	height = height * 2;
	slice = slice * 2;

	//x축방향
	for (int z = 0; z < slice - 1; z++)
	{
		for (int y = 0; y < height - 1; y++)
		{
			for (int x = 0; x < width - 1; x++)
			{
				if (z % 2 == 0)
				{
					if (y % 2 == 0)
					{
						if (x % 2 == 1)
						{
							short Q1 = output[(z*width*height) + (y*width) + (x - 1)];
							short Q2 = output[(z*width*height) + (y*width) + (x + 1)];

							output[z*width*height + y*width + x] = (Q1 + Q2) / 2;
						}
					}
				}
			}
		}
	}


	//y축방향
	for (int z = 0; z < slice - 1; z++)
	{
		for (int y = 0; y < height - 1; y++)
		{
			for (int x = 0; x < width - 1; x++)
			{
				if (z % 2 == 0)
				{
					if (y % 2 == 1)
					{
						short Q1 = output[(z*width*height) + ((y - 1)*width) + x];
						short Q2 = output[(z*width*height) + ((y + 1)*width) + x];

						output[z*width*height + y*width + x] = (Q1 + Q2) / 2;
					}

				}
			}
		}
	}

	//z축방향
	for (int z = 1; z < slice - 1; z++)
	{
		for (int y = 1; y < height - 1; y++)
		{
			for (int x = 1; x < width - 1; x++)
			{
				if (z % 2 == 1)
				{
					short Q1 = output[(z - 1)*width*height + y*width + x];
					short Q2 = output[(z + 1)*width*height + y*width + x];

					output[z*width*height + y*width + x] = (Q1 + Q2) / 2;
				}
			}
		}
	}
}
#endif