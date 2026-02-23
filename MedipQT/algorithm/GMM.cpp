/***************************************************************************

Module Name:

	Gaussian Mixture Model with Diagonal Covariance Matrix

History:

	2003/11/01	Fei Wang
	2013 luxiaoxun
***************************************************************************/
#include "stdafx.h"
#include <math.h>
#include <iostream>
#include <assert.h>
#include "GMM.h"
#include "KMeans.h"
using namespace std;

#define max(a, b) (a > b ? a : b)

//double M_PI=3.14159;

GMM::GMM(int dimNum, int mixNum)
{	
	m_dimNum = dimNum;	// 입력 X의 차원 (ex. gray-scale[1차원], RGB[3차원])
	m_mixNum = mixNum;	// 구하고자 하는 정규분포(standard distribution)의 개수

	m_maxIterNum = 100; // 반복 100번
	m_endError = 0.001;

	Allocate();

	for (int i = 0; i < m_mixNum; i++)
	{
		m_priors[i] = 1.0 / m_mixNum; // 초기값은 1/n

		for (int d = 0; d < m_dimNum; d++)
		{
			m_means[i][d] = 0; // 평균 0
			m_vars[i][d] = 1; // 분산 1 // 표준정규분포로 시작
		}
	}

	countHuIndex = 0;
	countWholeHuValue = 0;
}

GMM::~GMM()
{
	Dispose();
}

void GMM::Allocate()
{
	m_priors = new double[m_mixNum];
	m_means = new double*[m_mixNum];
	m_vars = new double*[m_mixNum];

	for (int i = 0; i < m_mixNum; i++) // K개의 정규분포
	{
		m_means[i] = new double[m_dimNum]; // 차원별 평균?
		m_vars[i] = new double[m_dimNum];
	}

	m_minVars = new double[m_dimNum];
}

void GMM::Dispose()
{
	delete[] m_priors;

	for (int i = 0; i < m_mixNum; i++)
	{
		delete[] m_means[i];
		delete[] m_vars[i];
	}
	delete[] m_means;
	delete[] m_vars;

	delete[] m_minVars;
}

void GMM::Copy(GMM* gmm)
{
	assert(m_mixNum == gmm->m_mixNum && m_dimNum == gmm->m_dimNum);

	for (int i = 0; i < m_mixNum; i++)
	{
		m_priors[i] = gmm->Prior(i);
		memcpy(m_means[i], gmm->Mean(i), sizeof(double) * m_dimNum);
		memcpy(m_vars[i], gmm->Variance(i), sizeof(double) * m_dimNum);
	}
	memcpy(m_minVars, gmm->m_minVars, sizeof(double) * m_dimNum);
}

double GMM::GetProbability(const double* sample)
{
	double p = 0;
	for (int i = 0; i < m_mixNum; i++)
	{
		p += m_priors[i] * GetProbability(sample, i);
	}
	return p;
}

double GMM::GetProbability(const double* x, int j)
{
	double p = 1;
	for (int d = 0; d < m_dimNum; d++)
	{
		p *= 1 / sqrt(2 * 3.14159 * m_vars[j][d]);
		p *= exp(-0.5 * (x[d] - m_means[j][d]) * (x[d] - m_means[j][d]) / m_vars[j][d]);
	}
	return p;
}

void GMM::Train(const char* sampleFileName)
{
	//DumpSampleFile(sampleFileName);
	Init(sampleFileName);

	ifstream sampleFile(sampleFileName, ios_base::binary);
	assert(sampleFile);

	int size = 0;
	sampleFile.seekg(0, ios_base::beg);
	sampleFile.read((char*)&size, sizeof(int));

	// Reestimation
	bool loop = true;
	double iterNum = 0;
	double lastL = 0;
	double currL = 0;
	int unchanged = 0;
	double* x = new double[m_dimNum];	// Sample data
	double* next_priors = new double[m_mixNum];
	double** next_vars = new double*[m_mixNum];
	double** next_means = new double*[m_mixNum];

	for (int i = 0; i < m_mixNum; i++)
	{
		next_means[i] = new double[m_dimNum];
		next_vars[i] = new double[m_dimNum];
	}

	while (loop)
	{
		// Clear buffer for reestimation
		memset(next_priors, 0, sizeof(double) * m_mixNum);
		for (int i = 0; i < m_mixNum; i++)
		{
			memset(next_vars[i], 0, sizeof(double) * m_dimNum);
			memset(next_means[i], 0, sizeof(double) * m_dimNum);
		}

		lastL = currL;
		currL = 0;

		// Predict
		sampleFile.seekg(2 * sizeof(int), ios_base::beg);
		for (int k = 0; k < size; k++)
		{
			sampleFile.read((char*)x, sizeof(double) * m_dimNum);
			double p = GetProbability(x);

			for (int j = 0; j < m_mixNum; j++)
			{
				double pj = GetProbability(x, j) * m_priors[j] / p;
				next_priors[j] += pj;
				for (int d = 0; d < m_dimNum; d++)
				{
					next_means[j][d] += pj * x[d];
					next_vars[j][d] += pj* x[d] * x[d];
				}
			}
			currL += (p > 1E-20) ? log10(p) : -20;
		}
		currL /= size;

		// Reestimation: generate new priors, means and variances.
		for (int j = 0; j < m_mixNum; j++)
		{
			m_priors[j] = next_priors[j] / size;

			if (m_priors[j] > 0)
			{
				for (int d = 0; d < m_dimNum; d++)
				{
					m_means[j][d] = next_means[j][d] / next_priors[j];
					m_vars[j][d] = next_vars[j][d] / next_priors[j] - m_means[j][d] * m_means[j][d];
					if (m_vars[j][d] < m_minVars[d])
					{
						m_vars[j][d] = m_minVars[d];
					}
				}
			}
		}

		// Terminal conditions
		iterNum++;
		if (fabs(currL - lastL) < m_endError * fabs(lastL))
		{
			unchanged++;
		}
		if (iterNum >= m_maxIterNum || unchanged >= 3)
		{
			loop = false;
		}
		//--- Debug ---
		//std::cout << "Iter: " << iterNum << ", Average Log-Probability: " << currL << std::endl;
	}

	sampleFile.close();
	delete[] next_priors;
	for (int i = 0; i < m_mixNum; i++)
	{
		delete[] next_means[i];
		delete[] next_vars[i];
	}
	delete[] next_means;
	delete[] next_vars;
	delete[] x;
}

void GMM::Train(double *data, int N) // N : 입력샘플 개수
{
	Init(data, N);

	int size = N;

	// Reestimation
	bool loop = true;
	double iterNum = 0;
	double lastL = 0;
	double currL = 0;
	int unchanged = 0;
	double* x = new double[m_dimNum];	// Sample data
	double* next_priors = new double[m_mixNum];
	double** next_vars = new double*[m_mixNum];
	double** next_means = new double*[m_mixNum];

	for (int i = 0; i < m_mixNum; i++)
	{
		next_means[i] = new double[m_dimNum];
		next_vars[i] = new double[m_dimNum];
	}

	while (loop)
	{
		// Clear buffer for reestimation
		memset(next_priors, 0, sizeof(double) * m_mixNum);
		for (int i = 0; i < m_mixNum; i++)
		{
			memset(next_vars[i], 0, sizeof(double) * m_dimNum);
			memset(next_means[i], 0, sizeof(double) * m_dimNum);
		}

		lastL = currL;
		currL = 0;

		// Predict
		for (int k = 0; k < size; k++)
		{
			for(int j=0;j<m_dimNum;j++)
				x[j]=data[k*m_dimNum+j];
			double p = GetProbability(x);

			for (int j = 0; j < m_mixNum; j++)
			{
				double pj = GetProbability(x, j) * m_priors[j] / p;

				next_priors[j] += pj;

				for (int d = 0; d < m_dimNum; d++)
				{
					next_means[j][d] += pj * x[d];
					next_vars[j][d] += pj* x[d] * x[d];
				}
			}

			currL += (p > 1E-20) ? log10(p) : -20;
		}
		currL /= size;

		// Reestimation: generate new priors, means and variances.
		for (int j = 0; j < m_mixNum; j++)
		{
			m_priors[j] = next_priors[j] / size;

			if (m_priors[j] > 0)
			{
				for (int d = 0; d < m_dimNum; d++)
				{
					m_means[j][d] = next_means[j][d] / next_priors[j];
					m_vars[j][d] = next_vars[j][d] / next_priors[j] - m_means[j][d] * m_means[j][d];
					if (m_vars[j][d] < m_minVars[d])
					{
						m_vars[j][d] = m_minVars[d];
					}
				}
			}
		}

		// Terminal conditions
		iterNum++;
		if (fabs(currL - lastL) < m_endError * fabs(lastL))
		{
			unchanged++;
		}
		if (iterNum >= m_maxIterNum || unchanged >= 3)
		{
			loop = false;
		}

		//--- Debug ---
		//std::cout << "Iter: " << iterNum << ", Average Log-Probability: " << currL << std::endl;
	}
	delete[] next_priors;
	for (int i = 0; i < m_mixNum; i++)
	{
		delete[] next_means[i];
		delete[] next_vars[i];
	}
	delete[] next_means;
	delete[] next_vars;
	delete[] x;
}

void GMM::Init(double *data, int N) // N: 입력샘플 개수
{
	const double MIN_VAR = 1E-10;

	KMeans* kmeans = new KMeans(m_dimNum, m_mixNum); //
	kmeans->SetInitMode(KMeans::InitUniform);
	int *Label;
	Label = new int[N]; // 입력샘플 각각에 클러스터링 레이블 부여
	kmeans->Cluster(data, N, Label); //k-means를 통해서 모든 입력샘플에 대해 클러스터 Label를 구한다.

	
	int* counts = new int[m_mixNum]; // 각 분포의 입력샘플 개수저장할 듯 // m_mixNum: 구하고자하는 정규분포 개수
	double* overMeans = new double[m_dimNum]; // 각 차원의 전체 평균? // Overall mean of training data
	for (int i = 0; i < m_mixNum; i++)
	{
		counts[i] = 0;
		m_priors[i] = 0; // 해당 분포의 확률
		memcpy(m_means[i], kmeans->GetMean(i), sizeof(double) * m_dimNum); // kmean로 얻은 평균값을 초기값으로 사용
		memset(m_vars[i], 0, sizeof(double) * m_dimNum); // 분산의 0으로 초기화
	}
	memset(overMeans, 0, sizeof(double) * m_dimNum);
	memset(m_minVars, 0, sizeof(double) * m_dimNum);

	int size = 0;
	size = N; // 입력샘플 개수 

	double* x = new double[m_dimNum]; // 입력샘플 하나를 뽑아내기 위한 변수
	int label = -1;

	/*std::cout << "---------------------------" << std::endl;	
	for (int i = 0; i < N; i++) {
		printf("%2.2lf(%d) ", data[i], Label[i]);	
	}
	std::cout << std::endl;
	std::cout << "---------------------------" << std::endl;*/


	for (int i = 0; i < size; i++) // 모든 입력샘플에 대해
	{
		for(int j=0;j<m_dimNum;j++)
			x[j] = data[i*m_dimNum+j]; // 입력 샘플과 > 
		label = Label[i]; // 입력샘플의 k-means로 구한 Label

		// Count each Gaussian
		counts[label]++; // 
		double* m = kmeans->GetMean(label);
		for (int d = 0; d < m_dimNum; d++)
		{
			m_vars[label][d] += (x[d] - m[d]) * (x[d] - m[d]); // 입력샘플에 대해 해당 입력샘플이 포함된 분포의 평균값과 분산(편차의 제곱)을 구함
		}

		// Count the overall mean and variance.
		for (int d = 0; d < m_dimNum; d++) // 모든 차원에 대해 > 
		{
			overMeans[d] += x[d]; // 각 차원마다 입력을 저장
			m_minVars[d] += x[d] * x[d]; // 0을 기준으로 한다면 이게 분산이 맞는데.. 아래서 평균을 빼주기 때문에 상관없는듯
		}
	}

	// 분산 구하는 법
	// 평균값의 선형성으로부터 다음과 같은 식을 얻을 수 있다. >> 이 내용을 참고
	// E[X^2] - (E[X])^2
	// https://ko.wikipedia.org/wiki/%EB%B6%84%EC%82%B0

	// Compute the overall variance (* 0.01) as the minimum variance.
	for (int d = 0; d < m_dimNum; d++) // 
	{
		overMeans[d] /= size; // 각 차원마다 평균을 구함
		m_minVars[d] = max(MIN_VAR, 0.01 * (m_minVars[d] / size - overMeans[d] * overMeans[d])); // 각 차원의 전체 분산의 1퍼센트 값?
		
		// E[] : 평균을 표현
		// m_minVars[d] / size = E[X^2]																							
		// overMeans[d] * overMeans[d] = (E[X])^2
	}

	// Initialize each Gaussian.
	for (int i = 0; i < m_mixNum; i++)
	{
		m_priors[i] = 1.0 * counts[i] / size; // 분포가 차지하는 입력샘플의 비중. 즉, 입력샘플들이 분포에 포함될 확률.

		if (m_priors[i] > 0) // k-means로 클러스터가 구성이 됐다면, 이 값은 존재
		{
			for (int d = 0; d < m_dimNum; d++)
			{
				m_vars[i][d] = m_vars[i][d] / counts[i];

				// A minimum variance for each dimension is required.
				if (m_vars[i][d] < m_minVars[d])
				{
					m_vars[i][d] = m_minVars[d];
				}
			}
		}
		else
		{
			memcpy(m_vars[i], m_minVars, sizeof(double) * m_dimNum);
			std::cout << "[WARNING] Gaussian " << i << " of GMM is not used!\n";
		}
	}
	delete kmeans;
	delete[] x;
	delete[] counts;
	delete[] overMeans;
	delete[] Label;

}

void GMM::Init(const char* sampleFileName)
{
	const double MIN_VAR = 1E-10;

	KMeans* kmeans = new KMeans(m_dimNum, m_mixNum);
	kmeans->SetInitMode(KMeans::InitUniform);
	kmeans->Cluster(sampleFileName, "gmm_init.tmp");

	int* counts = new int[m_mixNum];
	double* overMeans = new double[m_dimNum];	// Overall mean of training data
	for (int i = 0; i < m_mixNum; i++)
	{
		counts[i] = 0;
		m_priors[i] = 0;
		memcpy(m_means[i], kmeans->GetMean(i), sizeof(double) * m_dimNum);
		memset(m_vars[i], 0, sizeof(double) * m_dimNum);
	}
	memset(overMeans, 0, sizeof(double) * m_dimNum);
	memset(m_minVars, 0, sizeof(double) * m_dimNum);

	// Open the sample and label file to initialize the model
	ifstream sampleFile(sampleFileName, ios_base::binary);
	assert(sampleFile);

	ifstream labelFile("gmm_init.tmp", ios_base::binary);
	assert(labelFile);

	int size = 0;
	sampleFile.read((char*)&size, sizeof(int));
	sampleFile.seekg(2 * sizeof(int), ios_base::beg);
	labelFile.seekg(sizeof(int), ios_base::beg);

	double* x = new double[m_dimNum];
	int label = -1;

	for (int i = 0; i < size; i++)
	{
		sampleFile.read((char*)x, sizeof(double) * m_dimNum);
		labelFile.read((char*)&label, sizeof(int));

		// Count each Gaussian
		counts[label]++;
		double* m = kmeans->GetMean(label);
		for (int d = 0; d < m_dimNum; d++)
		{
			m_vars[label][d] += (x[d] - m[d]) * (x[d] - m[d]);
		}

		// Count the overall mean and variance.
		for (int d = 0; d < m_dimNum; d++)
		{
			overMeans[d] += x[d];
			m_minVars[d] += x[d] * x[d];
		}
	}

	// Compute the overall variance (* 0.01) as the minimum variance.
	for (int d = 0; d < m_dimNum; d++)
	{
		overMeans[d] /= size;
		m_minVars[d] = max(MIN_VAR, 0.01 * (m_minVars[d] / size - overMeans[d] * overMeans[d]));
	}

	// Initialize each Gaussian.
	for (int i = 0; i < m_mixNum; i++)
	{
		m_priors[i] = 1.0 * counts[i] / size;

		if (m_priors[i] > 0)
		{
			for (int d = 0; d < m_dimNum; d++)
			{
				m_vars[i][d] = m_vars[i][d] / counts[i];

				// A minimum variance for each dimension is required.
				if (m_vars[i][d] < m_minVars[d])
				{
					m_vars[i][d] = m_minVars[d];
				}
			}
		}
		else
		{
			memcpy(m_vars[i], m_minVars, sizeof(double) * m_dimNum);
			std::cout << "[WARNING] Gaussian " << i << " of GMM is not used!\n";
		}
	}

	delete kmeans;
	delete[] x;
	delete[] counts;
	delete[] overMeans;

	sampleFile.close();
	labelFile.close();
}

void GMM::DumpSampleFile(const char* fileName)
{
	ifstream sampleFile(fileName, ios_base::binary);
	assert(sampleFile);

	int size = 0;
	sampleFile.read((char*)&size, sizeof(int));
	std::cout << size << std::endl;

	int dim = 0;
	sampleFile.read((char*)&dim, sizeof(int));
	std::cout << dim << std::endl;

	double* f = new double[dim];
	for (int i = 0; i < size; i++)
	{
		sampleFile.read((char*)f, sizeof(double) * dim);

		std::cout << i << ":";
		for (int j = 0; j < dim; j++)
		{
			std::cout << " " << f[j];
		}
		std::cout << std::endl;
	}

	delete[] f;
	sampleFile.close();
}

ostream& operator<<(ostream& out, GMM& gmm)
{
	/*out << "<GMM>" << std::endl;
	out << "<DimNum> " << gmm.m_dimNum << " </DimNum>" << std::endl;
	out << "<MixNum> " << gmm.m_mixNum << " </MixNum>" << std::endl;*/

	out << "<HU Index Count> " << std::endl;
	out << gmm.countHuIndex << std::endl;
	out << std::endl;
	//out << "</HU Index Count>" << std::endl;

	out << "<HU Overall Value Count>" << std::endl;
	out << gmm.countWholeHuValue << std::endl;
	out << std::endl;
	//out << "</HU Overall Value Count>" << std::endl;

	out << fixed;
	out.precision(6);

	out << "<Prior> " << std::endl;
	for (int i = 0; i < gmm.m_mixNum; i++)
	{
		out << gmm.m_priors[i] << std::endl;
	}
	out << std::endl;
	//out << "</Prior>" << std::endl;

	out.precision(4);

	out << "<Mean>" << std::endl;
	for (int i = 0; i < gmm.m_mixNum; i++)
	{
		for (int d = 0; d < gmm.m_dimNum; d++)
		{
			out << gmm.m_means[i][d] << " ";
		}
		out << std::endl;
	}
	out << std::endl;
	//out << "</Mean>" << std::endl;

	out << "<Standard Deviation>" << std::endl;
	for (int i = 0; i < gmm.m_mixNum; i++)
	{
		for (int d = 0; d < gmm.m_dimNum; d++)
		{
			out << sqrt(gmm.m_vars[i][d]) << " ";
		}
		out << std::endl;
	}
	out << std::endl;
	//out << "</Standard Deviation>" << std::endl;

	out << "<Variance>" << std::endl;
	for (int i = 0; i < gmm.m_mixNum; i++)
	{
		for (int d = 0; d < gmm.m_dimNum; d++)
		{
			out << gmm.m_vars[i][d] << " ";
		}
		out << std::endl;
	}
	//out << std::endl;
	//out << "</Variance>" << std::endl;

	//out << "</GMM>" << std::endl;

	return out;
}

istream& operator>>(istream& in, GMM& gmm)
{
	char label[50];
	in >> label; // "<GMM>"
	assert(strcmp(label, "<GMM>") == 0);

	gmm.Dispose();

	in >> label >> gmm.m_dimNum >> label; // "<DimNum>"
	in >> label >> gmm.m_mixNum >> label; // "<MixNum>"

	gmm.Allocate();

	in >> label; // "<Prior>"
	for (int i = 0; i < gmm.m_mixNum; i++)
	{
		in >> gmm.m_priors[i];
	}
	in >> label;

	in >> label; // "<Mean>"
	for (int i = 0; i < gmm.m_mixNum; i++)
	{
		for (int d = 0; d < gmm.m_dimNum; d++)
		{
			in >> gmm.m_means[i][d];
		}
	}
	in >> label;

	in >> label; // "<Variance>"
	for (int i = 0; i < gmm.m_mixNum; i++)
	{
		for (int d = 0; d < gmm.m_dimNum; d++)
		{
			in >> gmm.m_vars[i][d];
		}
	}
	in >> label;

	in >> label; // "</GMM>"
	return in;
}

