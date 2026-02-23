#pragma once

#ifndef TASUBCLASSRESULTCTRL_H
#define TASUBCLASSRESULTCTRL_H

#include "define.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <QString>

using namespace std;

typedef void(*progUpdatefunc)(int value, void * data);

//namespace TA
//{
	

	class TASubClassResultCtrl
	{
	public:
		explicit TASubClassResultCtrl();
		~TASubClassResultCtrl();

		//static TASubClassResultCtrl * getSingleton();

		//void setProgress(progUpdatefunc updater, void* data = NULL);
		int getMajorClassSize(void);
		QString getMajorClassName(int nNum);

		int getMiddleClassSize(int nMajorNum);
		QString getMiddleClassName(int nMajorNum, int nNum);

		int getMinorClassSize(int nMajorNum, int nMiddleNum);
		QString getMinorClassName(int nMajorNum, int nMiddleNum, int nNum);

		bool setFeatureValue(const QString &strMajor, const QString &strMiddle, const QString &strMinor, double dVal);
		double getFeatureValue(const QString &strMajor, const QString &strMiddle, const QString &strMinor);
		double getFeatureValue(int nMajorNum, int nMiddleNum, int nNum);


	private:
// 		int width, height, depth;
// 		short *m_pDataset;
// 		unsigned char* m_pMaskset;
// 		//unsigned char mVal;
// 
// 
// 		void*			m_pdtProg;
// 		progUpdatefunc	pFuncUpdater;

		vector<pair<QString, vector<pair<QString, vector<pair<QString, double>>>>>> m_vecClassify;

	};
//}

#endif