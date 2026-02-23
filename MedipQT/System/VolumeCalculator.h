#pragma once

#ifndef VOLUME_CALCULATOR_H
#define VOLUME_CALCULATOR_H

#include "define.h"
#include "defineMEDIP.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <QMap>

struct MaskInfo;
class VOLUME_DATA;

/*
	작성자 : 이상일
	목적 :
	MaskInfo구조체에서 mask index, mask ID 등을 추출하는 Utility 함수.
	Mask Module 구조 개선 전까지 임시로 사용함.
*/
class MaskUtil
{
public:
	/**
		Volume Data에서 전체 신규 MaskList 복사본을 생성한다.
	*/
	static bool CreateMaskInfoList(std::vector<MaskInfo>& outNewMaskInfos, VOLUME_DATA* pVolumeData);

	/**
		Volume Data에서 Mask 정보를 가져온다.
	*/
	static bool GetAllMaskInfos(std::vector<MaskInfo*>& outMaskInfos, VOLUME_DATA* pVolumeData);
	static bool GetMaskInfos(std::vector<MaskInfo*>& outMaskInfos, VOLUME_DATA* pVolumeData, std::vector<muint32>& maskUIDs);

	/**
		MaskArray의 Index를 가져온다.
	*/
	static int GetMaskIndex(MaskInfo* pMaskInfo);

	/**
		MaskID는 "defineMEDIP.h의 " mask8과 동일. MaskData, MaskID 등으로 표현된다.
	*/
	static unsigned char GetMaskID(MaskInfo* pMaskInfo);
};

/*
	작성자 : 이상일
	목적 :
	MaskInfo에 해당하는 ROI에서 HU값을 계산 후 Volume에 반영하는 Class
*/

#define VOLUME_CALCULATOR VolumeCalculator::GetInstance()

class VolumeCalculator
{
public:
	static VolumeCalculator* GetInstance()
	{
		static VolumeCalculator instance;
		return &instance;
	}

private:
	VolumeCalculator();
	~VolumeCalculator();

public:
	bool Initialize(VOLUME_DATA* pVolumeData);
	bool IsIntialized();
	bool CanCalculate(VOLUME_DATA* pVolumeData);

	void SetCalcParameter(IMAGE_CALCULATOR_OPERATOR op, double value, std::vector<MaskInfo*>& maskInfos);

	bool CalcHU_Volume(VOLUME_DATA* pVolumeData);
	bool CalcHU_Plane(
		VOLUME_DATA* pVolumeData,
		WINDOW_TYPE type,
		int depth
	);

	bool Reset(VOLUME_DATA* pVolumeData);

	mint16* OriginHUVolumeData() { return m_pOriginHUVolumeData; }
	IMAGE_CALCULATOR_OPERATOR Operator() { return m_operator; }
	double Value() { return m_value; }
	std::vector<MaskInfo*>& MaskInfos() { return m_maskInfos; }

	void SetPreivewON(bool value) { m_isPreviewON = value; }
	bool IsPreviewON() { return m_isPreviewON; }

private:
	void CalcHU_Range(VOLUME_DATA* pVolumeData, bool* dirtVolume, MaskInfo* pMaskInfo, double value, IMAGE_CALCULATOR_OPERATOR op,
		int xMin, int xMax,
		int yMin, int yMax,
		int zMin, int zMax
	);

	bool* CreateDirtVolume(VOLUME_DATA* pVolumeData);

private:
	mint16* m_pOriginHUVolumeData;

	int m_cx;
	int m_cy;
	int m_cz;

	IMAGE_CALCULATOR_OPERATOR m_operator;
	double m_value;
	std::vector<MaskInfo*> m_maskInfos;

	bool m_isPreviewON;
};

#endif