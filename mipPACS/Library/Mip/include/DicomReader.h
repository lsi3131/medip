#pragma once
#include <string>
#include <vector>
#include <map>
#include "Math/Math.h"

typedef unsigned char uint8_t;
class QProgressDialog;
class DcmDataset;

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
	SERIES_SEARCH_INFO(std::string _strSeriseNumber, std::string _strRow, std::string _strColumn)
	{
		strSeriseNumber = _strSeriseNumber;
		strRow = _strRow;
		strColumn = _strColumn;
	}
	bool operator<(const SERIES_SEARCH_INFO& other) const
	{
		if (strSeriseNumber == other.strSeriseNumber)
		{
			if (strRow == other.strRow)
			{				
				return strColumn < other.strColumn;				
			}
			else
			{
				return strRow < other.strRow;
			}
		}
		else
		{
			return strSeriseNumber < other.strSeriseNumber;
		}

		return false;
	}
	bool operator==(const SERIES_SEARCH_INFO& other) const
	{
		if (strSeriseNumber == other.strSeriseNumber)
		{
			if (strRow == other.strRow)
			{
				if (strColumn == other.strColumn)
					return true;
			}			
		}	

		return false;
	}

	std::string strSeriseNumber;
	std::string strRow;
	std::string strColumn;
};
struct multikey_less : public std::binary_function<SERIES_SEARCH_INFO, SERIES_SEARCH_INFO, bool>
{
	bool operator()(const SERIES_SEARCH_INFO &mkey1, const SERIES_SEARCH_INFO &mkey2) const
	{
		return mkey1.operator<(mkey2); // mkey1 < mkey2;
	}
};
struct DcmtkSeriesInfo
{
	std::string strSeriesUID;            ///< globally unique series identifier
	std::string strAcquisitionNum;		// Acquisition Number
	std::string patientsName_;   ///< patient name
	std::string patientId_;      ///< patient identifier
	std::string studyDate_;      ///< date the study was taken
	std::string studyTime_;      ///< time the study was tacken
	std::string modality_;       ///< e.g. CT, MR, US
	std::string numImages_;      ///< number of images in the series
	std::string description_;    ///< SeriesDescription
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
	std::string xySpacing;
	std::string zSpacing;
	std::string convolutionkernel;
	std::string manufacturerModel;
	std::string seriesDate;

	std::string studyDescription;
	std::string Manufacturer;
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
};

struct SliceInfo
{
	std::wstring		filename;
	float				depth;
	int					instance_number;
};

class VData
{
public:
	virtual ~VData() { }

	int size;
	virtual void * Get(int index) = 0;
	virtual short Get16(int index) = 0;
	virtual short * GetDatas() = 0;

	virtual void SetData(int index, int value) = 0;
};

class Modality {
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
	class DcmtkVolumeReader
	{
	public:
		DcmtkVolumeReader();
		virtual ~DcmtkVolumeReader();
		DcmtkVolumeReader* create() const;


		std::string getClassName() const { return "DcmtkVolumeReader"; }
		std::string getFormatDescription() const { return "DICOM format, using DCMTK library"; }

		void Reset();

		static	void UnloadDicomFiles();

		static	bool LoadSeriesInfoList(const std::wstring& fileName, std::vector<DcmtkSeriesInfo> & result, bool keepDecomp=false);

		static	bool LoadAcquisionInfoList(const std::wstring& fileName, const DcmtkSeriesInfo &cSeriesInfo, std::vector<DcmtkSeriesInfo> &vecAcquisitionInfo);
		
		static	bool LoadSeriesInfo(const std::wstring& fileName, DcmtkSeriesInfo & result, bool keepDecomp=false);

		static	bool LoadSortedDicomFilesList(const std::wstring &fileName, const DcmtkSeriesInfo &dcmSeriesInfo, int nFilterAcquisitionNumber, std::vector<SliceInfo> & slices, std::wstring & path, DicomVolumeInfo & info);

		static 	bool LoadVolumeData(std::vector<SliceInfo> & slices, std::wstring & path, DicomVolumeInfo & volume_info,
			float scale = 0.1f, unsigned int samplerate = 1, void * data = NULL, DicomVolumeCropInfo * cropInfo = NULL,
			QProgressDialog * progress = NULL, int nMaxProgress = 100, mip::MATRIX44 * localMatrix = NULL);

		static	bool LoadDicomData(const std::wstring& fileName, int size, int bitsStored, void * data, DicomVolumeInfo & volume_info);

		static	bool ApplyHUOffset(mint16 * buffers, int size, DicomVolumeInfo & volume_info);

		static  int AddSlice(DicomVolumeInfo & info, const std::wstring& fileName, void * data, unsigned int offset, int samplerate = 1, DicomVolumeCropInfo * cropInfo = NULL);

		void setForceDownScaleLoading(bool value) { m_forceDownScaleLoading = value; }
		void setDownScaleLimit(unsigned int x, unsigned int y, unsigned int z)
		{
			m_forceDownScaleLimitX = x;
			m_forceDownScaleLimitY = y;
			m_forceDownScaleLimitZ = z;
		}
		unsigned int getDownScaledCnt() { return m_DownScaledCnt; }

	private:
		static 	bool LoadVolumeInfo(DcmDataset *dataset, DicomVolumeInfo & info);

		static 	bool LoadSeriesInfo(DcmDataset *dataSet, DcmtkSeriesInfo & result);

		static const std::string loggerCat_;
		std::vector<std::string> extensions_;
		std::vector<std::string> filenames_;
		std::vector<std::string> protocols_;

		static bool keepDecomp_;

		static std::string getRealsopuid(const std::string &sopuid);

	public:
		DCM_MODAL_TYPE modality_mode;

		unsigned char * scalars_;

		mip::VECTOR3 xOrientation_;
		mip::VECTOR3 yOrientation_;
		mip::VECTOR3 normal_;
		mip::VECTOR3 offset_;
		int dx_, dy_, dz_;
		int airdx_, airdy_, airdz_;
		int graftdx_, graftdy_, graftdz_;
		float xSize, ySize, zSize;
		float airxSize, airySize, airzSize;
		float graftXSize, graftYSize, graftZSize;
		std::vector<float>	sliceVolume;

		int preDx_, preDy_, preDz_;
		int loadrate_;
		bool bBigData;


		int bitsStored_;
		int rescaleIntercept_; // 2017.09.28 ¿ÃµŒ»Ò ∆¿¿Â √ﬂ∞°
		int samplesPerPixel_;
		int bytesPerVoxel_;
		Modality modality_;
		int pixelRepresentation;
		int windowWidth;
		int windowCenter;

		float x_spacing;
		float y_spacing;
		float z_spacing; // For the resulting Volume

		VData *		pData;
		VData *		pAirwayData;
		VData *		pBoneGraftData;
		//	DcmtkSecurityOptions security_;

		/////////////////////////////////
		struct sPatientInfomation
		{
			std::string ContentDate;
			std::string Modality;
			std::string Name;
			std::string ID;
			std::string BirthDay;
			std::string Age;
			std::string Sex;
			std::string KVP;
			std::string Series;
			std::string mA;

			sPatientInfomation()
			{
				ContentDate = "NULL";
				Modality = "NULL";
				Name = "NULL";
				ID = "NULL";
				BirthDay = "NULL";
				Age = "NULL";
				Sex = "NULL";
				KVP = "NULL";
				Series = "NULL";
				mA = "NULL";
			}

			void reset()
			{
				ContentDate = "NULL";
				Modality = "NULL";
				Name = "NULL";
				ID = "NULL";
				BirthDay = "NULL";
				Age = "NULL";
				Sex = "NULL";
				KVP = "NULL";
				Series = "NULL";
				mA = "NULL";
			}
		} m_PatientInfo;

		bool							m_bCrop;
		bool							m_bHiddenOpen1;
		bool							m_bHiddenOpen2;
		bool							m_forceDownScaleLoading;
		unsigned int					m_forceDownScaleLimitX;
		unsigned int					m_forceDownScaleLimitY;
		unsigned int					m_forceDownScaleLimitZ;
		unsigned int					m_DownScaledCnt;

		float							m_fCropRatioAxial[2];
		float							m_fCropRatioSagittal[2];
		float							m_fCropRatioCoronal[2];

		int								m_nCropXlen;
		int								m_nCropYlen;
		int								m_nCropZlen;
		float							m_fCropWidth;
		float							m_fCropHeight;
		float							m_fCropDepth;
	};
};
