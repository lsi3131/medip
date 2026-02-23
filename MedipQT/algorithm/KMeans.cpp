/***************************************************************************
Module Name:
	KMeans

History:
	2003/10/16	Fei Wang
	2013 luxiaoxun
***************************************************************************/
#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <assert.h>
#include "KMeans.h"
using namespace std;


KMeans::KMeans(int dimNum, int clusterNum)
{
	m_dimNum = dimNum;
	m_clusterNum = clusterNum;

	m_means = new double*[m_clusterNum];
	for (int i = 0; i < m_clusterNum; i++)
	{
		m_means[i] = new double[m_dimNum];
		memset(m_means[i], 0, sizeof(double) * m_dimNum);
	}

	m_initMode = InitRandom;
	m_maxIterNum = 100;
	m_endError = 0.001;
}

KMeans::~KMeans()
{
	for (int i = 0; i < m_clusterNum; i++)
	{
		delete[] m_means[i];
	}
	delete[] m_means;
}

void KMeans::Cluster(const char* sampleFileName, const char* labelFileName)
{
	// Check the sample file
	ifstream sampleFile(sampleFileName, ios_base::binary);
	assert(sampleFile);

	int size = 0;
	int dim = 0;
	sampleFile.read((char*)&size, sizeof(int));
	sampleFile.read((char*)&dim, sizeof(int));
	assert(size >= m_clusterNum);
	assert(dim == m_dimNum);

	// Initialize model
	Init(sampleFile);

	// Recursion
	double* x = new double[m_dimNum];	// Sample data
	int label = -1;		// Class index
	double iterNum = 0;
	double lastCost = 0;
	double currCost = 0;
	int unchanged = 0;
	bool loop = true;
	int* counts = new int[m_clusterNum];
	double** next_means = new double*[m_clusterNum];	// New model for reestimation
	for (int i = 0; i < m_clusterNum; i++)
	{
		next_means[i] = new double[m_dimNum];
	}

	while (loop)
	{
		memset(counts, 0, sizeof(int) * m_clusterNum);
		for (int i = 0; i < m_clusterNum; i++)
		{
			memset(next_means[i], 0, sizeof(double) * m_dimNum);
		}

		lastCost = currCost;
		currCost = 0;

		sampleFile.clear();
		sampleFile.seekg(sizeof(int) * 2, ios_base::beg);

		// Classification
		for (int i = 0; i < size; i++)
		{
			sampleFile.read((char*)x, sizeof(double) * m_dimNum);
			currCost += GetLabel(x, &label);

			counts[label]++;
			for (int d = 0; d < m_dimNum; d++)
			{
				next_means[label][d] += x[d];
			}
		}
		currCost /= size;

		// Reestimation
		for (int i = 0; i < m_clusterNum; i++)
		{
			if (counts[i] > 0)
			{
				for (int d = 0; d < m_dimNum; d++)
				{
					next_means[i][d] /= counts[i];
				}
				memcpy(m_means[i], next_means[i], sizeof(double) * m_dimNum);
			}
		}

		// Terminal conditions
		iterNum++;
		if (fabs(lastCost - currCost) < m_endError * lastCost)
		{
			unchanged++;
		}
		if (iterNum >= m_maxIterNum || unchanged >= 3)
		{
			loop = false;
		}
		//DEBUG
		//cout << "Iter: " << iterNum << ", Average Cost: " << currCost << std::endl;
	}

	// Output the label file
	ofstream labelFile(labelFileName, ios_base::binary);
	assert(labelFile);

	labelFile.write((char*)&size, sizeof(int));
	sampleFile.clear();
	sampleFile.seekg(sizeof(int) * 2, ios_base::beg);

	for (int i = 0; i < size; i++)
	{
		sampleFile.read((char*)x, sizeof(double) * m_dimNum);
		GetLabel(x, &label);
		labelFile.write((char*)&label, sizeof(int));
	}

	sampleFile.close();
	labelFile.close();

	delete[] counts;
	delete[] x;
	for (int i = 0; i < m_clusterNum; i++)
	{
		delete[] next_means[i];
	}
	delete[] next_means;
}

// N: 입력샘플 개수
// Label: N개(입력샘플) 각각에 Label을 부여하기 위한 공간
void KMeans::Cluster(double *data, int N, int *Label) 
{
	int size = 0;
	size = N;

	assert(size >= m_clusterNum);

	// Initialize model
	Init(data, N);	// 클러스터별 초기 평균값을 정해준다.

	// Recursion
	double* x = new double[m_dimNum];	// Sample data
	int label = -1;		// Class index	
	double iterNum = 0;
	double lastCost = 0;
	double currCost = 0;	
	int unchanged = 0;		
	int* counts = new int[m_clusterNum];	
	double** next_means = new double*[m_clusterNum];	// New model for reestimation
	
	for (int i = 0; i < m_clusterNum; i++)
	{
		next_means[i] = new double[m_dimNum]; // 2차원의 경우, 2차원의 평균(x-y평면의 임의의 지점)을 표현하기 위해선 2개의 값이 필요
	}


	/*cout << "------------------------------------------" << std::endl;
	for (int i = 0; i < m_clusterNum; i++) {
		printf("Mean-%d: %lf\n", i, m_means[i][0]);
	}
	cout << "------------------------------------------" << std::endl;*/
	bool loop = true;
	while (loop)
	{
		memset(counts, 0, sizeof(int) * m_clusterNum);
		for (int i = 0; i < m_clusterNum; i++)
		{
			memset(next_means[i], 0, sizeof(double) * m_dimNum); // 새로운 평균값을 저장
		}

		lastCost = currCost;
		currCost = 0;

		// Classification
		for (int i = 0; i < size; i++)
		{
			for(int j = 0; j < m_dimNum; j++)
				x[j] = data[i*m_dimNum+j];	// 입력샘플 하나를 뽑아서

			currCost += GetLabel(x, &label); // 해당 샘플의 cost와 label을 구하고
			// currCost는 모든 입력샘플의 유클리디안-거리의 합 (모든 입력샘플은 각각 하나의 클러스터에 포함된다.)

			counts[label]++; // 해당 label의 카운터 증가, 새로운 평균값을 구할 때 각 클러스터(label)당 입력샘플의 개수로 사용한다.

			for (int d = 0; d < m_dimNum; d++) 
			{
				next_means[label][d] += x[d]; // 다음 Re-estimation에서 평균값을 다시 구하기 위해, next_means에 label이 구해진 입력샘플을 모두 더한다.
			}
		}
		
		currCost /= size; // 입력샘플의 개수로 나눠서, cost(유클리디안-거리)의 평균을 구한다. 의미가 있나?

		// Reestimation
		for (int i = 0; i < m_clusterNum; i++)
		{
			if (counts[i] > 0)
			{
				for (int d = 0; d < m_dimNum; d++)
				{
					next_means[i][d] /= counts[i]; // 새로운 평균값을 구한다.
				}
				memcpy(m_means[i], next_means[i], sizeof(double) * m_dimNum); // 평균값을 업데이트한다.
			}
		}

		// Terminal conditions
		iterNum++;
		if (fabs(lastCost - currCost) < m_endError * lastCost) // cost의 변화가 거의 없을 경우(아마 아주 적은 갯수의 샘플이동), 카운팅을 해주고 >
		{
			unchanged++;
		}

		if (iterNum >= m_maxIterNum || unchanged >= 3) // 3회 이상이면 종료
		{
			loop = false;
		}

		//DEBUG
		//cout << "Iter: " << iterNum << ", Average Cost: " << currCost << std::endl;
	}

	// Output the label file
	for (int i = 0; i < size; i++) // 모든 입력 샘플의 >
	{
		for(int j = 0; j < m_dimNum; j++)
			x[j] = data[i*m_dimNum+j];

		GetLabel(x, &label); // label을 구해서, 결과로 확정하고 > 
		
		Label[i] = label; // 저장한다.
	}

	// 
	delete[] counts;
	delete[] x;
	for (int i = 0; i < m_clusterNum; i++)
	{
		delete[] next_means[i];
	}
	delete[] next_means;
}

void KMeans::Init(double *data, int N) // N: 입력샘플 개수
{
	int size = N;

	if (m_initMode ==  InitRandom) // 랜덤한 위치에서 입력샘플을 뽑아서 클러스터의 초기 평균값으로 지정.
	{
		int inteval = size / m_clusterNum;
		double* sample = new double[m_dimNum];

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
			for(int j = 0; j < m_dimNum; j++)
				sample[j] = data[select*m_dimNum+j];
			memcpy(m_means[i], sample, sizeof(double) * m_dimNum);
		}

		delete[] sample;
	}
	else if (m_initMode == InitUniform) // 일정 간격을 두고 입력샘플을 뽑아서 클러스터의 초기 평균값으로 지정.
	{
		double* sample = new double[m_dimNum];

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = i * size / m_clusterNum; // 클러스터 개수만큼 나눠서 >
			
			for(int j = 0; j < m_dimNum; j++)
				sample[j] = data[select*m_dimNum+j]; // 나눠진 그룹의 시작이 되는 원소(입력샘플)를 >

			memcpy(m_means[i], sample, sizeof(double) * m_dimNum); // 클러스터의 초기 평균값으로 지정
			
			//cout << m_means[i][0] << std::endl;
		}

		delete[] sample;
	}
	else if (m_initMode == InitManual)
	{
		// Do nothing
	}
}

void KMeans::Init(ifstream& sampleFile)
{
	int size = 0;
	sampleFile.seekg(0, ios_base::beg);
	sampleFile.read((char*)&size, sizeof(int));

	if (m_initMode ==  InitRandom)
	{
		int inteval = size / m_clusterNum;
		double* sample = new double[m_dimNum];

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
			int offset = sizeof(int) * 2 + select * sizeof(double) * m_dimNum;

			sampleFile.seekg(offset, ios_base::beg);
			sampleFile.read((char*)sample, sizeof(double) * m_dimNum);
			memcpy(m_means[i], sample, sizeof(double) * m_dimNum);
		}

		delete[] sample;
	}
	else if (m_initMode == InitUniform)
	{
		double* sample = new double[m_dimNum];

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = i * size / m_clusterNum;
			int offset = sizeof(int) * 2 + select * sizeof(double) * m_dimNum;

			sampleFile.seekg(offset, ios_base::beg);
			sampleFile.read((char*)sample, sizeof(double) * m_dimNum);
			memcpy(m_means[i], sample, sizeof(double) * m_dimNum);
		}

		delete[] sample;
	}
	else if (m_initMode == InitManual)
	{
		// Do nothing
	}
}

double KMeans::GetLabel(const double* sample, int* label)
{
	double dist = -1;
	for (int i = 0; i < m_clusterNum; i++) // 모든 클러스터에 대해 >
	{	
		double temp = CalcDistance(sample, m_means[i], m_dimNum); // 각 평균값과 가장 가까운 클러스터를 찾는다.
		if (temp < dist || dist == -1)
		{
			dist = temp;
			*label = i; // 가장 가까운 label을 저장하고, >
		}
	}
	return dist; // cost인 유클리디안-거리는 return으로 넘겨준다.
}

// Euclidean distance
double KMeans::CalcDistance(const double* x, const double* u, int dimNum)
{
	double temp = 0;
	for (int d = 0; d < dimNum; d++) // 다차원일 경우 각각의 거리의 합을 제곱근 
	{
		temp += (x[d] - u[d]) * (x[d] - u[d]);
	}
	return sqrt(temp);
}

ostream& operator<<(ostream& out, KMeans& kmeans)
{
	out << "<KMeans>" << std::endl;
	out << "<DimNum> " << kmeans.m_dimNum << " </DimNum>" << std::endl;
	out << "<ClusterNum> " << kmeans.m_clusterNum << " </CluterNum>" << std::endl;

	out << "<Mean>" << std::endl;
	for (int i = 0; i < kmeans.m_clusterNum; i++)
	{
		for (int d = 0; d < kmeans.m_dimNum; d++)
		{
			out << kmeans.m_means[i][d] << " ";
		}
		out << std::endl;
	}
	out << "</Mean>" << std::endl;

	out << "</KMeans>" << std::endl;
	return out;
}
