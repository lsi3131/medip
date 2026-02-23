#pragma once

#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H

#include "define.h"
#include "color.h"
#include "Windows/WindowBase.h"
#include "BoundingBox.h"
#include "Math/Math.h"
#include "MaskInfo.h"
#include "MeshInfo.h"
#include "MeshLayerData.h"
#include "MeshLayerDataOmniverse.h"
#include "DeletedMaskInfo.h"
#include "qmap.h"
#include <queue>
#include <utility>

//#define MESHINFO_TEXT_LENGTH_MAX	256

//#define MASKINFO_NOT_CUSTOM_TEXT
class vtkDataArray;

struct UIDMask
{
	muint32 uid;
	mask _m;
};

namespace mip
{
	class MeshTopology;
};

class VOLUME_DATA
{
public:
	VOLUME_DATA();
	~VOLUME_DATA();

	void clear(bool delMask = true, bool delMaskList = true);
	void clearTempData();
	void clearCacheData();
	void clearOnly3DHU();
	bool setHUData(mint16* data, muint32 cx, muint32 cy, muint32 cz);
	bool createData(mint16* data, muint32 cx, muint32 cy, muint32 cz, float x, float y, float z, bool delMask = true);
	bool createData(muint32 cx, muint32 cy, muint32 cz, float x, float y, float z, bool delMask = true, bool delMaskList = true);
	bool create3DHUData(muint32 cx, muint32 cy, muint32 cz, float pixelSpacingX, float pixelSpacingY, float pixelSpacingZ);
	bool createMaskData(mask* data, int mI = 0);
	void createTestTexture();
	void updateVolumeMinMax();

	void updateMPRData();
	void updateColorTable();
	bool connectCheckAll();
	bool visibleCheckAll(bool type = false); /*0:layer, 1:surface*/

	void applyTempMask(int Maskindex = 0);
	void applyTempMaskBitAdd(mask _m, int Maskindex = 0);
	int SplitTempMaskBit(int _uid, mask _m, int _mI = 0);
	int applyTempMaskBitOutset(const std::vector<unsigned char>& vClassData, mask maskBit, int nMaskByteIdx, unsigned char nThreshold);
	void applyTempMaskBitChange(mask _m, int Maskindex = 0, mask _chm = 0, int tempMaskIndex = 0, bool _del = true);
	void applyTempMaskBitChangeLoadROI(mask _m, QString& filename, int Maskindex = 0);
	void applyTempMaskBitChangeLoadTXT(unsigned char* mask_buffer, mask _m, QString& filename, int Maskindex = 0);
	void applyTempMaskBitDel(mask _m, int Maskindex = 0, mask _chm = 0, int tempMaskIndex = 0, bool _del = true);

	bool createTempMaskData(bool init = true, int newdatalen = -1, int mI = 0);
	void setTempMaskBitFromData(mask _srcm, int _srcmI = 0, mask _dstm = 1, int _dstmI = 0);
	mint16* getHUDataPoint() { return pData3D_HU; }
	mint16* getHUFlipDataPoint();

	mask* getMaskDataPoint(int n3DMaskByteIdx = 0);
	mask** getAllMaskDataPoint(void);
	mask* getMaskTempDataPoint(int _mI = 0);
	bool createTempHUData2(bool init = true);
	bool createTempHUData(bool init = true, int newdatalen = -1);

	void setData(muint32 index, mint16 data);
	void setData(muint32 x, muint32 y, muint32 z, mint16 data);
	void setSpace(float x, float y, float z) { spaceX = x; spaceY = y; spaceZ = z; }
	void setImgPos(float x, float y, float z) { imgPos = mip::VECTOR3(x, y, z); };
	float getImgPosX() { return imgPos.x; }
	float getImgPosY() { return imgPos.y; }
	float getImgPosZ() { return imgPos.z; }
	mip::VECTOR3 getImgPos() { return imgPos; }

	float getSpaceX(bool isMM = false);
	float getSpaceY(bool isMM = false);
	float getSpaceZ(bool isMM = false);
	float getSizeX(bool isMM = false);
	float getSizeY(bool isMM = false);
	float getSizeZ(bool isMM = false);
	float getSpace3D(bool isMM = false);
	float getSpace2D(bool isMM = false);

	float getMinSpace(bool isMM = false);

	muint32 getCX() { return dataCX; }
	muint32 getCY() { return dataCY; }
	muint32 getCZ() { return dataCZ; }
	muint32 getVolumeDataLength() { return dataLenth; }
	mint16 getHuMin() { return HUMin; }
	mint16 getHuMax() { return HUMax; }
	void setHuMin(mint16 min) { m_bCreate = false; HUMin = min; }
	void setHuMax(mint16 max) { m_bCreate = false; HUMax = max; }

	bool isValidate() { return (pData3D_HU != 0) ? true : false; }
	static BoundingBoxI getBoxForScreen(WINDOW_TYPE type, const BoundingBoxI& box);
	bool getLengthForScreen(WINDOW_TYPE type, muint32& cx, muint32& cy, muint32& cz);
	bool getSpacingForScreen(WINDOW_TYPE type, float& c_x, float& c_y, float& c_z, bool isMM = false);
	BoundingBoxI getLayerBoundingBoxForScreen(WINDOW_TYPE type, int uid, bool isDrawcut = false);
	BoundingBoxI getBoundingBoxForScreen(WINDOW_TYPE type);
	mint16 getData(muint32 x, muint32 y, muint32 z);
	mint16 getData(muint32 index);
	COLOR getGrayColorData(muint32 x, muint32 y, muint32 z, mint32 window_level, mint32 window_width, bool color_mode, bool invert_mode);

	bool  checkMaskUsed(mask _mask, int maskIndex = 0);
	void  setMaskBit(muint32 x, muint32 y, muint32 z, mask data, int Maskindex = 0);
	void  setMaskBitList(WINDOW_TYPE type, muint32 x, muint32 y, muint32 z, std::vector<QPoint>* list, mask data, int Maskindex = 0);
	void  setMaskBitList(WINDOW_TYPE type, std::vector<QPoint>* list, muint32 depth, mask mask, int Maskindex = 0);
	void  delMaskBit(muint32 x, muint32 y, muint32 z, mask data, int Maskindex = 0);
	void  delMaskBitList(WINDOW_TYPE type, muint32 x, muint32 y, muint32 z, std::vector<QPoint>* list, mask data, int Maskindex = 0);
	void  delMaskBitList(WINDOW_TYPE type, std::vector<QPoint>* list, muint32 depth, mask data, int Maskindex = 0);
	mask  getMaskData(muint32 x, muint32 y, muint32 z, int MaskByteIndex = 0);
	mask  getMaskData(mint32 index, int MaskByteIndex = 0);

	//	bool  isMaskOnlyBit(muint32 x, muint32 y, muint32 z, mask data, int Maskindex = 0, bool isFirst=true);
	bool  isMaskBit(muint32 index, mask data, int Maskindex = 0);
	bool  isMaskBit(muint32 x, muint32 y, muint32 z, mask data, int Maskindex = 0);
	bool  isMaskBit(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data, int Maskindex = 0);
	void  applyMaskCopy(mask source, mask dest, int Maskindex = 0);
	void  applyMaskAdd(mask source, mask dest, int Maskindex = 0, int destMaskindex = 0);

	muint32 createTempMaskSlice(WINDOW_TYPE type);
	void	clearTempMaskSlice();
	void	applyTempMaskSliceAdd(WINDOW_TYPE type, mask _m, int Maskindex, muint32 slicesize);
	void	applyTempMaskSliceDel(WINDOW_TYPE type, mask _m, int Maskindex, muint32 slicesize);
	void	applyTempMaskSlice(int Maskindex, muint32 slicesize);

	muint32 getVolumeIndex(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth);
	void  setTempMaskBit(muint32 x, muint32 y, muint32 z, mask data);
	void  delTempMaskBit(muint32 x, muint32 y, muint32 z, mask data);
	void  setTempMaskBitAuto(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data);
	void  delTempMaskBitAuto(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data);
	void  setTempMaskBitList(WINDOW_TYPE type, std::vector<QPoint>* list, muint32 depth, mask mask);
	void  setTempMaskBit(WINDOW_TYPE type, muint32 x, muint32 y, muint32 depth, mask data);
	void  moveMaskToTempMask(mask dest_mask, mask source_mask, int dest_Maskindex = 0, int source_Maskindex = 0);

	void  setMaskData(muint32 x, muint32 y, muint32 z, mask value, int Maskindex = 0);
	void  setMaskData(muint32 index, mask value, int Maskindex = 0);

	void AutoWindowing(int iNumBins, float fAutoScaleSlope, float fRescaleIntercept, double& dWindowWidth, double& dWindowLevel); // 2017.08.24 이두희 팀장 추가

	BoundingBoxI getBoundingBox() { return m_boundingBox; }
	void setBoundingBox(BoundingBoxI& box) { m_boundingBox = box; }
	BoundingBoxI getPreBoundingBox() { return m_preBoundingBox; }
	void setPreBoundingBox() { m_preBoundingBox = m_boundingBox; }
	mip::AABB getAABB();
	mip::AABB getBoundingBoxAABB();
	mip::AABB getAABB(BoundingBoxI& box);

	BoundingBoxI getBoundingBox3D() { return m_boundingBox3D; }
	void setBoundingBox3D(BoundingBoxI& box) { m_boundingBox3D = box; }
	mip::AABB getBoundingBox3DAABB();
	void init3DPlanes();
	bool getAnal3DPlanes(WINDOW_TYPE type, mip::VECTOR3* point, bool init = false);
	std::vector<mip::VECTOR3> getAnal3DPlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, bool init = false, bool _getInit = false);

	bool GetZeroBasePlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point);

	bool checkRotatePlanes(WINDOW_TYPE type, bool isSlider = true);
	void setAnal3DPlanes(WINDOW_TYPE type);
	void setCurrentMaskIndex(muint32 index);
	MaskInfo* getCurrentMaskInfo();
	int	getCurrentMaskIndex();
	muint32 getMaskInfoListCnt() { return  m_vecMaskInfoList.size(); }
	muint32 getCurrentMaskInfoID() { return m_currentMaskInfoIndex; }
	bool isEmptyMaskVoxel(int index, bool seed = false, bool bUID = false);
	void setMaskName(QString maskName, int index, bool bUID = false);
	bool isDuplicateMaskName(QString changedMaskName);									// jhc[2020.10.29] mask name 중복 할당 방지
	QString makeUniqueMaskNameWithSuffixNumber(QString basicMaskName, int curSufficNumber = 0);			// jhc[2020.10.29] mask name 중복 할당 방지
	QString makeUniqueMaskNameWithCopyText(QString basicMaskName);
	QString getMaskName(int index, bool bUID = false);
	MaskInfo* getMaskInfo(muint32 index, bool bUID = false);	// bUID : 용도 중간에 있는 ROI mask를 삭제했을 때 index로 못찾음
	MaskInfo* getMaskInfoByIndex(muint32 layerIndex);
	MaskInfo* getMaskInfoByUID(muint32 uid);
	mask getMask(muint32 uid);
	int GetMaskByteIndex(muint32 uid);
	MaskInfo* getAtLastMaskInfo(int* pOutIndex = nullptr);
	MaskInfo* findMaskInfo(int _mI, mask _m);
	MaskInfo* findMaskInfo(QString strMaskName, bool isSearchOrder = false);			// false:foreward, true:backward
	//std::vector<MaskInfo*> findMultiMaskInfo(mask maskBit);
	void setMultiSelectUIDByMaskBit(std::vector<std::pair<int, mask8 >> vecMultiMask);
	void setMultiSelectUID(const std::vector<muint32>& vecMaskUID);
	std::vector<muint32> getMultiSelectMaskUID(void) { return m_vecMultiSelectedMaskUID; }

	bool isMaskEditable(void);
	bool isMaskEditable(int nIdx);

	int getMaskIndex(int UID);
	int getMaskIndexCnt(void);
	int getMaskIndexByMaskInfoPtr(MaskInfo* pMaskInfo);

	void clearMaskInfo();
	bool createMaskInfo(bool isCopy = false, bool b_update_ui = true);
	bool createMaskInfoWithName(const QString& name, bool isCopy = false, bool b_update_ui = true);

	void clearMaskData(mask m_, int Maskindex = 0);
	void clearMaskDataByInfo(MaskInfo* pInfo);
	bool createMaskInfoFromCopyByIndex(muint32 layerIndex);
	bool createMaskInfoFromCopyByUID(int UID);
	bool createMaskInfoFromCopyByInfo(const MaskInfo* info);
	bool createMaskInfoFromCopy_2(muint32 copy_id);

	bool insertMaskInfo(muint32 index, const MaskInfo& info);
	bool insertNewMaskInfo(muint32 index);

	bool resetMaskInfo(muint32 index, const MaskInfo& info);

	bool delMaskInfo(muint32 index, DeletedMaskInfo* pOutDeleteMaskResultInfo = nullptr);
	bool delMaskInfoList(std::vector<int> layerIndexList, std::vector<DeletedMaskInfo>* pOutDeleteMaskResultInfoList = nullptr);
	bool delMaskInfos(std::vector<muint32>& list, std::vector<UIDMask>& mv_vector, bool& existSurface, bool bDeleteAll = false);
	void deleteMaskInfoAndData(MaskInfo* pDeleteInfo);
	bool takMaskInfo(muint32 index, MaskInfo& info/*out*/);
	void moveMaskInfoAndData(MaskInfo* pFrom, MaskInfo* pTo);
	bool moveMaskInfo(muint32 source_index, muint32 dest_index);
	bool moveMaskBitData(int oldMaskByteIndex, int newMaskByteIndex, mask oldMaskBitFlag, mask newMaskBitFlag);
	bool moveMaskInfo_ex(MaskInfo* pFrom, MaskInfo* pTo);
	muint32 indexGenForMask();
	//mint32	generateIndexForMesh();
	//mint32	VisualPrint_indexGenForMesh();
	COLOR colorGen(muint32 index);
	QColor getMaskColor(muint32 index, bool bUID = false);

	muint32 getDownScaledCnt() { return downScaledCnt; }
	void setDownScaledCnt(muint32 value) { downScaledCnt = value; }


	void	setImgOrientation(bool, mip::VECTOR3, bool init = false);
	bool	getImgOrientation(bool, float*);

	bool getPlaneLine(WINDOW_TYPE type, std::vector<mip::VECTOR3>& list);
	bool getPlaneSurface(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord,
		mip::MATRIX44* trasform = 0, bool reverse = false);
	bool getPlaneSurfaceFromPlane(const std::vector<mip::VECTOR3>& plane, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord);
	bool getPlaneSurfaceFromPlaneList(const std::vector<mip::VECTOR3>& plane, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord, float depth = 0.0f);
	bool getPlaneSurfaceFromPlaneList2D(const std::vector<mip::VECTOR3>& plane, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, bool flip = false, float depth = 0.0f);
	bool getAllThickness(float _thick, float dt, WINDOW_TYPE _type, bool isSlider = false, bool sliderSub = false);
	muint32 getPlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, mip::MATRIX44* trasform = 0);
	bool getCullingPlaneSurface(std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, float size, mip::MATRIX44* trasform = 0);
	mip::VECTOR3 point2texcoord(const mip::VECTOR3& point);
	mip::VECTOR2 point2texcoord2D(const mip::VECTOR3& point);

	void setDepth(WINDOW_TYPE winType, const int depth);
	int  getDepth(WINDOW_TYPE winType) const;
	void setPrevDepth(WINDOW_TYPE winType, const int depth);
	int  getPrevDepth(WINDOW_TYPE winType) const;

	muint32 getMPRPPlanes(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, mip::MATRIX44* trasform = 0);
	bool getMPRPlaneSurface(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord, mip::MATRIX44* trasform = 0);
	bool getMPRPlaneSurfaceList(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR3>& texcoord, mip::MATRIX44* trasform = 0, float depth = 0.0f);

	bool getMPRPlaneSurface2D(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, mip::MATRIX44* trasform = 0);
	bool getMPRPlaneSurfaceList2D(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, mip::MATRIX44* trasform = 0, float depth = 0.0f);

	std::vector<mip::VECTOR3> GetMPRPPlanesByDepth(WINDOW_TYPE type, int depth, mip::MATRIX44* trasform = 0);

	bool getSegmentationPlaneSurfaceList2D(WINDOW_TYPE type, std::vector<mip::VECTOR3>& point, std::vector<mip::VECTOR2>& texcoord, float depth = 0.0f);

	void getAllMaskVoxel(bool withBox = true);
	//	void getAllMaskBoundingBox(bool withVoxel=false);
	void updateUIDBoundingBox(int index, bool isUID = false, bool seed = false);
	void updateBoundingBoxByUID(int uid);

	bool compareBoundingBox(int index, bool seed = false);

	void forceUpdateVolume();
	void forceUpdateMaskVolume();
	bool getUpdateVolume() { return bUpdateVolume; }
	bool getUpdateMaskVolume() { return bRedrawMask; }
	void clearUpdateVolume() { bUpdateVolume = false; }
	void clearUpdateMaskVolume() { bRedrawMask = false; }
	void forceUpdateMPR() { bUpdateMPR = true; }
	bool getUpdateMPR() { return bUpdateMPR; }
	void clearUpdateMPR() { bUpdateMPR = false; }

	QColor get2Dcolor(int index);

	void updateVoxelCount(int uid, mask maskBitFlag, int maskByteIndex);
	void setVoxelCount(int uid, int count, bool total = true, bool seed = false);
	int getVoxelCount(int uid, bool seed = false);

	float getBurdenVal(int uid);
	void setBurdenVal(int uid, int value);

	void setBoundingBox(int uid, BoundingBoxI box = BoundingBoxI(), bool reset = false, bool seed = false);
	BoundingBoxI getBoundingBox(int uid, bool seed = false);

	void setTAState(int uid, bool val, bool _drawcut = false);
	bool getTAState(int uid, bool _drawcut = false);

	int GetMaskPointCount();

	mip::VECTOR3 toWorldPosition(mip::VECTOR3& pos);
	bool IsAIMaskByIndex(int index, int* outValue = nullptr);
	bool IsAIMaskByUID(int UID, int* outValue = nullptr);
	bool FindAIDataIndexByUID(int* pOutIndex, int UID);
	void DeleteAIDataByUID(int uid);
	bool SetAIDataByUID(int uid, const std::vector<unsigned char>& AIResult, int AIOutset);

	QVector<mask> GetMaskBitFlagList(int maskByteIndex);
	mask* GetMaskDataPtr(int maskByteIndex);
	muint32 GetUsedMaskUIDBitArray();

	bool IsMaskUIDUsed(int uid);

	int GetMaskByteIndex(MaskInfo* pInfo);
	mask GetMaskBitFlag(MaskInfo* pInfo);

	std::vector<unsigned char> GetAIResultByUID(int uid);
	/**
		return
			success : 0 ~ 255
			fail :  -1
	*/
	int GetAIOutsetByUID(int uid);

	mip::VECTOR3 GetOffsetCenter();

private:
	void addMaskBitFlagToList(MaskInfo* pInfo);

	bool createNewMaskInfoAndInitData(const QString& layerName, bool isCopy, bool b_update_ui);
	bool insertNewMaskInfoAndInitData(int index, const QString& layerName);

	bool CheckDataPoint(int Maskindex = 0, bool CheckOnly = false); //data point check access
	void changeUID(mint32 pre_mask_uid, mint32 mask_uid);

	void deleteMaskBitFlagFromList(int maskByteIndex, mask maskBitFlag);
	void setUsedMaskUIDBitOn(int uid);
	void setUsedMaskUIDBitOff(int uid);

	void clearVoxelCountAndBoundingBox(int uid);

	void eraseMaskInfoByIndexAndSetCurrentIndex(int index);
public:
	bool				threadStop;
	int					threadResult;
	mint16* pData3D_HU_Temp;
	double* pData3D_HU_Temp_D1;
	double* pData3D_HU_Temp_D2;
	mask* pData3D_Mask_Temp;
	mask* pData3D_Mask_Temp_2[3];
	mint16* pData3D_HU_Flip;

	muint32* pData2D_Mask_Temp_SliceIndex;
	mask* pData2D_Mask_Temp_SliceData;

	int					fillMaskCount; // 졸라 쓸모없는 변수(임시변수)-삭제 예정

	mip::VECTOR3		axialPPlane[4];
	mip::VECTOR3		saggitalPPlane[4];
	mip::VECTOR3		coronalPPlane[4];
	mip::VECTOR3		axialInitPlane[4];
	mip::VECTOR3		saggitalInitPlane[4];
	mip::VECTOR3		coronalInitPlane[4];

	mip::VECTOR3		axialPPlane_Base[4];
	mip::VECTOR3		saggitalPPlane_Base[4];
	mip::VECTOR3		coronalPPlane_Base[4];

	int					axialDepth;
	int					coronalDepth;
	int					sagittalDepth;

	int					axialPrevDepth;
	int					coronalPrevDepth;
	int					sagittalPrevDepth;

	std::map<WINDOW_TYPE, mip::VECTOR3[4]> planeVertex;

	mint16* axialPlaneData[3];
	mint16* coronalPlaneData[3];
	mint16* sagittalPlaneData[3];

	//////// deepcatch thread 에서 class이름 저장 용도
	//std::vector<std::string>	m_vecTmpMaskName;

	// AI data
	// int: ROI uid, unsigned char : 0~255 image data

	std::vector<pair<int, std::vector<unsigned char>>> m_vecAIResultData;
	std::vector<pair<int, int>> m_vecAIOutset;

private:
	mint16* pData3D_HU;
	mask* pData3D_Mask[4];
	muint32 dataCX;
	muint32 dataCY;
	muint32 dataCZ;
	muint32 dataLenth;
	mint16 HUMin;
	mint16 HUMax;
	bool m_bCreate;
	muint32 downScaledCnt;

	float spaceX;
	float spaceY;
	float spaceZ;
	mip::VECTOR3 imgPos = mip::VECTOR3(0, 0, 0);

	mip::VECTOR3	xAxis;//image orientation
	mip::VECTOR3	yAxis;//image orientation

	BoundingBoxI m_boundingBox;
	BoundingBoxI m_preBoundingBox;
	BoundingBoxI m_boundingBox3D;

	mip::VECTOR3 pointsZPlane[4];
	mip::VECTOR3 pointsYPlane[4];
	mip::VECTOR3 pointsXPlane[4];
	QVector<mask> m_maskIDList[4];
	QVector<QColor> color2DTable;

	std::vector<MaskInfo*> m_vecMaskInfoList;
	muint32 m_useMaskInfoBit;

	muint32 m_currentMaskInfoIndex;
	//muint8 m_currentMeshInfoIndex;
	//muint8 m_muint8VisualPrint_currentMeshInfoIndex;
	// multi select 관련 코드
	std::vector<muint32> m_vecMultiSelectedMaskUID;


	bool bRedrawMask;
	bool bUpdateVolume;
	bool bUpdateMPR;

	bool bLatestTA[MASK_MAX]; //false(default) : need to update ta
	int m_voxelCount[MASK_MAX];
	float m_dBrudenValue[MASK_MAX];
	BoundingBoxI m_boundingBoxROI[MASK_MAX];
	int m_seedVoxel[2]; /*0 : backseed, 1: foreseed*/
	BoundingBoxI seedBoundingbox[2]; /*0 : backseed, 1: foreseed*/
};
#endif