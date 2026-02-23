#pragma once
#include <string>
#include <vector>
#include <map>
#include "Math/Math.h"
#include "mipdicom.h"

typedef unsigned char uint8_t;
class QProgressDialog;
class QDir;
class DcmDataset;
class DcmFileFormat;
class OFString;

namespace gdcm
{
	class Pixmap;
	class File;
	class Image;
	class PixmapReader;
}

namespace mip
{
	class vec3
	{
	public: float x, y, z;
	public:
		vec3(float _x, float _y, float _z);
		vec3();
	};

	vec3 cross(vec3 a, vec3 b);
	float dot(vec3 a, vec3 b);

	class DcmtkVolumeReader;
};

enum DCM_VOLUME_DATA_TYPE
{
	DCM_VOLUME_DATA_TYPE_NORMAL,
	DCM_VOLUME_DATA_TYPE_PET_SUV,
};

enum DCM_MODAL_TYPE
{
	DCM_CT = 0,
	DCM_UK,
	DCM_MR,
	DCM_RF,
	DCM_US,
	DCM_XA,
	DCM_XRAY,
	DCM_CR,
	DCM_MCT,
	DCM_PT,
};

struct SERIES_SEARCH_INFO
{
	SERIES_SEARCH_INFO() {}
	SERIES_SEARCH_INFO(
		std::string _strSeriseNumber,
		std::string _strFrameReferenceUID,
		std::string _strRow,
		std::string _strColumn,
		std::string _strSpacingXY)
		: strSeriseNumber(_strSeriseNumber)
		, strFrameReferenceUID(_strFrameReferenceUID)
		, strRow(_strRow)
		, strColumn(_strColumn)
		, strSpacingXY(_strSpacingXY)
	{

	}
	bool operator<(const SERIES_SEARCH_INFO& other) const
	{
		bool isCompare;

		if (strSeriseNumber == other.strSeriseNumber)
		{
			if (strFrameReferenceUID == other.strFrameReferenceUID)
			{
				if (strRow == other.strRow)
				{
					if (strColumn == other.strColumn)
					{
						isCompare = strSpacingXY < other.strSpacingXY;
					}
					else
					{
						isCompare = strColumn < other.strColumn;
					}
				}
				else
				{
					isCompare = strRow < other.strRow;
				}
			}
			else
			{
				isCompare = strFrameReferenceUID < other.strFrameReferenceUID;
			}
		}
		else
		{
			isCompare = strSeriseNumber < other.strSeriseNumber;
		}

		return isCompare;
	}

	bool operator==(const SERIES_SEARCH_INFO& other) const
	{
		return this->GetFullInfo() == other.GetFullInfo();
	}

	const std::string GetFullInfo() const
	{
		std::string fullInfo = strSeriseNumber;
		fullInfo += strFrameReferenceUID;
		fullInfo += strRow;
		fullInfo += strColumn;
		fullInfo += strSpacingXY;

		return fullInfo;
	}

	std::string strSeriseNumber;
	std::string strFrameReferenceUID;
	std::string strRow;
	std::string strColumn;
	std::string strSpacingXY;
};

struct multikey_less : public std::binary_function<SERIES_SEARCH_INFO, SERIES_SEARCH_INFO, bool>
{
	bool operator()(const SERIES_SEARCH_INFO& mkey1, const SERIES_SEARCH_INFO& mkey2) const
	{
		return mkey1.operator<(mkey2); // mkey1 < mkey2;
	}
};

struct DcmtkSeriesInfo
{
	std::string strSeriesUID;           ///< globally unique series identifier
	std::string strStudyUID;            ///< globally unique study identifier
	std::string strAcquisitionNum;		// Acquisition Number
	std::string patientsName_;			///< patient name
	std::string patientId_;				///< patient identifier
	std::string patientSize_;			///< patient height
	std::string patientWeight_;			///< patient weight
	std::string studyDate_;				///< date the study was taken
	std::string studyTime_;				///< time the study was tacken
	std::string modality_;				///< e.g. CT, MR, US
	std::string numImages_;				///< number of images in the series
	std::string description_;			///< SeriesDescription
	std::string birthday_;
	std::string age_;
	std::string kvp_;
	std::string ma_;
	std::string sex_;
	std::string sopuid_;

	std::string sliceThickness;
	std::string width;
	std::string height;
	std::string imagePosition;
	std::string imageOrientation;
	std::string xySpacing;
	std::string zSpacing;
	std::string convolutionkernel;
	std::string manufacturerModel;
	std::string seriesDate;

	std::string studyDescription;
	std::string Manufacturer;

	std::string units_;
	std::string radionuclideTotalDose_;
	std::string radionuclideHalfLife;
	std::string radiopharmaceuticalStartTime_;
	std::string acquisitionTime_;

	std::string frameOfReferenceUID_;
	std::string examID;		//Study UID
	std::string accessionNumber;

	std::string echoTime;
	std::string repetitionTime;
	std::string magneticFieldStrength;
	std::string flipAngle;

	std::string sopClassUid;
};

struct DicomVolumeCropInfo
{
	unsigned sx, ex;
	unsigned sy, ey;
	unsigned sz, ez;
};

struct DicomVolumeInfo
{
	int				dx;
	int				dy;
	int				dz;
	float			x_spacing;
	float			y_spacing;
	float			z_spacing;

	int				scaled_dx;
	int				scaled_dy;
	int				scaled_dz;
	float			scaled_x_spacing;
	float			scaled_y_spacing;
	float			scaled_z_spacing;

	int				bitsStored;
	int				rescaleIntercept;
	float			rescaleSlose;
	int				samplesPerPixel;
	int				pixelRepresentation;
	int				bytesPerVoxel;

	int				window_width;
	int				window_level;

	int				instance_number;

	mip::VECTOR3	xAxis;
	mip::VECTOR3	yAxis;
	mip::VECTOR3	normal;
	mip::VECTOR3	offset;

	DCM_MODAL_TYPE	modality_mode;

	std::string photometricInterpretation;
};

struct SliceInfo
{
	std::wstring filepath;
	float depth;
	int instance_number;
	mip::VECTOR3	patientImagePostion;
};

class MIP_DICOM_EXPORT VData
{
public:
	virtual ~VData() { }

	int size;
	virtual void* Get(int index) = 0;
	virtual short Get16(int index) = 0;
	virtual short* GetDatas() = 0;

	virtual void SetData(int index, int value) = 0;
};

class MIP_DICOM_EXPORT Modality {
public:
	Modality(const std::string& name = "");

	/// Two modalities are equal if their names are equal
	bool operator== (const Modality& m) const { return m.name_ == name_; }
	bool operator!= (const Modality& m) const { return m.name_ != name_; }
	friend std::ostream& operator<<(std::ostream& os, const Modality& m);

	std::string getName() const { return name_; }
	bool isUnknown() const { return name_ == MODALITY_UNKNOWN.name_; }

	static const std::vector<Modality*> getModalities() { return modalities(); }
	static const std::vector<std::string>& getModalityNames() { return modalityNames_(); }

	// all known modalities
	static const Modality MODALITY_UNKNOWN;
	static const Modality MODALITY_ANY;
	static const Modality MODALITY_CT;
	static const Modality MODALITY_PET;
	static const Modality MODALITY_MR;
	static const Modality MODALITY_US;
	static const Modality MODALITY_SEGMENTATION;
	static const Modality MODALITY_MASKING;
	static const Modality MODALITY_LENSEVOLUME;
	static const Modality MODALITY_AMBIENTOCCLUSION;
	static const Modality MODALITY_DYNAMICAMBIENTOCCLUSION;
	static const Modality MODALITY_DIRECTION_X;
	static const Modality MODALITY_DIRECTION_Y;
	static const Modality MODALITY_DIRECTION_Z;
	static const Modality MODALITY_DIRECTIONS;
	static const Modality MODALITY_NORMALS;
	static const Modality MODALITY_GRADIENTS;
	static const Modality MODALITY_GRADIENT_MAGNITUDES;
	static const Modality MODALITY_2ND_DERIVATIVES;
	static const Modality MODALITY_FLOW;
	static const Modality MODALITY_INDEX_VOLUME;
	static const Modality MODALITY_BRICKED_VOLUME;
	static const Modality MODALITY_EEP_VOLUME;

protected:
	std::string name_;

	static std::vector<std::string>& modalityNames_() {
		static std::vector<std::string> modalityNames;
		return modalityNames;
	}

	static std::vector<Modality*>& modalities() {
		static std::vector<Modality*> modalities;
		return modalities;
	}
};

namespace mip
{
	struct DicomVolumeData
	{
		DicomVolumeInfo VolumeInfo;
		void* HUData;
	};

	class MIP_DICOM_EXPORT DcmtkVolumeReader
	{
	public:
		const static int DEFAULT_MAX_FILE_SIZE;

	public:
		static void SetCheckMaxFileSize(bool value);
		static bool IsCheckMaxFileSize();

		static void SetLoadMaxFileSize(mint64 value);
		static mint64 GetLoadMaxFileSize();

		static void SetEnableCP246Support(bool value);
		static bool GetEnableCP246Support();

		static	void UnloadDicomFiles();

		static	bool LoadSeriesInfoList(const std::wstring& filePath, std::vector<DcmtkSeriesInfo>& result, bool keepDecomp = false);

		static	bool LoadSeriesInfo(const std::wstring& filePath, DcmtkSeriesInfo& outDcmSeriesInfo, bool keepDecomp = false);

		static	bool LoadAcquisionInfoList(const std::wstring& filePath, const DcmtkSeriesInfo& cSeriesInfo, std::vector<DcmtkSeriesInfo>& vecAcquisitionInfo);

		static  void ClearTemporaryDirectoryPath(const std::wstring& temporaryFolderName);

		static  bool GetTemporaryDirectroyPathFromDcmtkSeriesInfo(
			const std::wstring& filePath,
			const DcmtkSeriesInfo& dcmSeriesInfo,
			const std::wstring& temporaryDirectoryPath,
			std::wstring& outDirectoryPath);

		static	bool LoadSortedDicomFilesList(
			const std::wstring& filePath,
			const DcmtkSeriesInfo& dcmSeriesInfo,
			int nFilterAcquisitionNumber,
			std::vector<SliceInfo>& outSliceInfoList,
			std::wstring& outDirectoryPath,
			DicomVolumeInfo& outDcmVolumeInfo);

		static	bool LoadDicomSliceAndVolumeInfo(const std::wstring& filePath, const DcmtkSeriesInfo& dcmSeriesInfo, SliceInfo& outSliceInfoList, DicomVolumeInfo& outDcmVolumeInfo);

		static bool LoadVolumeData(
			const std::vector<SliceInfo>& slices,
			const std::wstring& dirpath,
			DicomVolumeInfo& volume_info,
			DCM_VOLUME_DATA_TYPE volumeDataType = DCM_VOLUME_DATA_TYPE_NORMAL,
			float scale = 0.1f,
			unsigned int samplerate = 1,
			void* data = NULL,
			const DicomVolumeCropInfo* cropInfo = NULL,
			QProgressDialog* progress = NULL,
			int nMaxProgress = 100,
			mip::MATRIX44* localMatrix = NULL);

		static bool LoadVolumeData(
			DicomVolumeData* pOutVolumeData,
			const std::vector<SliceInfo>& slices,
			const std::wstring& dirpath,
			const DicomVolumeInfo& volumeInfo,
			DCM_VOLUME_DATA_TYPE volumeDataType = DCM_VOLUME_DATA_TYPE_NORMAL,
			float scale = 0.1f,
			unsigned int samplerate = 1,
			const DicomVolumeCropInfo* cropInfo = NULL,
			QProgressDialog* progress = NULL,
			int nMaxProgress = 100,
			mip::MATRIX44* localMatrix = NULL);

		static std::wstring GetTemporaryDirectoryPath(const std::wstring& temporaryFolderName);
		static std::wstring GetDecompressedDirectoryPath();

		static std::string getClassName();
		static std::string getFormatDescription();

	private:
		static mint64 m_loadMaxFileSize;
		static bool m_isCheckMaxFileSize;

	public:
		DcmtkVolumeReader();
		virtual ~DcmtkVolumeReader();

	private:
		bool DoLoadSeriesInfoList(const std::wstring& filePath, std::vector<DcmtkSeriesInfo>& outDcmSeriesInfoList, bool keepDecomp);
		bool DoLoadSeriesInfo(const std::wstring& filePath, DcmtkSeriesInfo& outDcmSeriesInfo, bool keepDecomp);

		bool DoGetTemporaryDirectroyPathFromDcmtkSeriesInfo(
			const std::wstring& filePath, 
			const DcmtkSeriesInfo& dcmSeriesInfo, 
			const std::wstring& temporaryFolderName,
			std::wstring& outDirectoryPath);

		bool DoLoadSortedDicomFilesList(
			const std::wstring& filePath,
			const DcmtkSeriesInfo& dcmSeriesInfo,
			int nFilterAcquisitionNumber,
			std::vector<SliceInfo>& outSliceInfoList,
			std::wstring& outDirectoryPath,
			DicomVolumeInfo& outDcmVolumeInfo);

		bool DoLoadDicomSliceAndVolumeInfo(
			const std::wstring& filePath,
			const DcmtkSeriesInfo& dcmSeriesInfo,
			SliceInfo& outSliceInfo,
			DicomVolumeInfo& outDcmVolumeInfo);

		bool DoLoadVolumeData(
			DicomVolumeData* pOutVolumeData,
			const std::vector<SliceInfo>& slices,
			const DicomVolumeInfo& volumeInfo,
			DCM_VOLUME_DATA_TYPE volumeDataType,
			float scale,
			unsigned int samplerate,
			const DicomVolumeCropInfo* cropInfo,
			QProgressDialog* progress,
			int nMaxProgress,
			mip::MATRIX44* localMatrix);

	private:
		bool LoadDicomFileFormat(const std::wstring& dcmFilePath, DcmFileFormat& outDcmFileFormat);
		bool LoadDicomFile_GDCM(gdcm::PixmapReader* outPixmapReader, const std::wstring& dcmFilePath);
		bool SaveDecompressDicomFile_GDCM(const std::wstring& dcmFilePath, const gdcm::Pixmap& pmap, const gdcm::File& file);
		bool SaveDecompressDicomStream_GDCM(const gdcm::Pixmap& pmap, const gdcm::File& file, DcmFileFormat& outDcmFileFormat);

		bool TryGetDirectory(QDir& dir, const std::wstring& filePath);
		bool CheckFileSize(int fileSize);
		void LoadSeriesInfoListFromDirectory(const QDir& dir);
		void UpdateSeriesInfoListDataByDataset(DcmDataset* dcmDataset);
		bool TryGetSeriesSearchInfoByDataset(SERIES_SEARCH_INFO& outSeriesSearchInfo, DcmDataset* dcmDataset);
		void UpdateSeriesImageCountMap(const SERIES_SEARCH_INFO& seriesSearchInfo, bool& outIsNewSeriesInserted);
		void RemoveDuplicateSeriesImageCountMap();
		void MappingImageCount_To_SeriesInfoList(std::vector<DcmtkSeriesInfo>& outDcmSeriesInfoList);

		void InitDicomVolumeInfo(DicomVolumeInfo& outDcmVolumeInfo);
		bool Load_And_SaveDecompressedDicomFileList(const QDir& dcmDirectory, const std::wstring& dcmDecompressDirectoryPath, const DcmtkSeriesInfo& dcmSeriesInfo);
		bool LoadDicomVolumeInfoFromDirectory(DicomVolumeInfo& outDcmVolumeInfo, const QDir& dcmDirectory, const DcmtkSeriesInfo& dcmSeriesInfo);
		bool LoadDicomVolumeInfoFile(DicomVolumeInfo& outDcmVolumeInfo, const std::wstring& filepath, const DcmtkSeriesInfo& dcmSeriesInfo);

		bool LoadSliceInfoListFromDirectory(std::vector<SliceInfo>& outSliceInfoList, const DicomVolumeInfo& dcmVolumeInfo, const QDir& dcmDirectory, const DcmtkSeriesInfo& dcmSeriesInfo, int nFilterAcquisitionNumber);
		bool LoadSliceInfoFromFile(SliceInfo& outSliceInfo, const std::wstring& filepath, const DcmtkSeriesInfo& dcmSeriesInfo);
		bool SortSliceInfoListAndGetSpacingZ(std::vector<SliceInfo>& outSliceInfoList, float& outSpacingZ, const DicomVolumeInfo& outDcmVolumeInfo);
		bool GetSpacingZFromSliceInfoList(const std::vector<SliceInfo>& outSliceInfoList, float& outSpacingZ);
		bool RemoveNotMatchedSliceInfoFromList(std::vector<SliceInfo>& outSliceInfoList, std::vector<SliceInfo>::const_iterator itSliceInfo, const DicomVolumeInfo& dcmVolumeInfo);

		bool SetDicomVolumeInfoBySliceInfoList(DicomVolumeInfo& outDcmVolumeInfo, const std::vector<SliceInfo>& sliceInfoList);
		bool SetDicomVolumeInfoBySliceInfo(DicomVolumeInfo& outDcmVolumeInfo, const SliceInfo& sliceInfo);
		bool SetDicomVolumeInfoBytesPerVoxel(DicomVolumeInfo& outDcmVolumeInfo);
		bool IsSupportedDicomVolumeInfoBytesPerVoxel(const DicomVolumeInfo& dcmVolumeInfo);

		int AddSlice(const DicomVolumeInfo& volume_info, const std::wstring& filePath, void* data, unsigned int offset, int samplerate, const DicomVolumeCropInfo* cropInfo);
		bool ApplyHUOffset(mint16* buffers, int size, const DicomVolumeInfo& volume_info);

		bool SetVolumeInfoByDataset(DcmDataset* dataset, DicomVolumeInfo& info);

		bool SetSeriesInfoByDataset(DcmDataset* dataSet, DcmtkSeriesInfo& outDcmSeriesInfo);

		bool SaveDecompressedDicomFile(DcmFileFormat* dcmfileformat, const std::wstring& loadFilePath, const std::wstring& decompressedFilePath);
		bool Load_And_SaveDecompressedDicomFile(const std::wstring& loadFilepath, const std::wstring& saveFilepath, const DcmtkSeriesInfo& dcmSeriesInfo);
		bool Decompress_GDCMPixmap(gdcm::Pixmap* outPixmap);
		bool IsDicomDatasetMatchToSeriesInfo(DcmDataset* dataset, const DcmtkSeriesInfo& dcmSeriesInfo);
		bool IsDicomDatasetMatchToSeriesInfoWithAcquisitionNumber(DcmDataset* dataset, const DcmtkSeriesInfo& dcmSeriesInfo, int acquisitionNumber);
		std::wstring GetDirectoryPath(const std::wstring& filepath);
		void InitTemporaryDirectoryPath(const std::wstring& temporaryFolderName);
		void InitDecompressedDirectoryPath();

		bool SetSliceInfo(SliceInfo& outSliceInfo, DcmDataset* dataset, const std::wstring& filepath, const VECTOR3& normal, bool& outNeedSort);
		bool SetSliceInfo(SliceInfo& outSliceInfo, DcmDataset* dataset, const std::wstring& filepath);

		void InsertSeriesSearchInfo_Or_AddImageCount(std::map<SERIES_SEARCH_INFO, int>& mapSeriesSearchInfo, const SERIES_SEARCH_INFO& seriesSearchInfo, bool& isSeriesInfoInserted);

		bool GetSeriesSearchInfoByDataset(SERIES_SEARCH_INFO& seriesSearchInfo, DcmDataset& dcmDataset);
		void DoClearTemporaryDirectoryPath(const std::wstring& temporaryFolderName);
		void ClearDepressedDirectory();

		void SetTemporayDirectoryAndGetPath(const std::wstring& temporaryFolderName, std::wstring& outTemporaryFolderName);

		float StringToFloat(const OFString& str, float defaultValue = 0.0f);

	private:
		bool m_keepDecompress;

		std::vector<DcmtkSeriesInfo> m_tempDcmSeriseInfoList;
		std::map<SERIES_SEARCH_INFO, int> m_mapSeriesImageCount;

		bool m_isNeedSortSliceInfoList;
		DCM_VOLUME_DATA_TYPE m_volumeDataType;
	};
};
