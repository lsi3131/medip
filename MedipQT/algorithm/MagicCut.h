#pragma once
//#include "Common.h"

#include <vector>
#include <list>
//#include <math.h>
#include <QString>

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define VM_MASK0		(1)

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifdef MAX
#undef MAX
#endif

#define MAX(a, b) ((a)>(b)?(a):(b))

#define ndim 3




typedef void(*progUpdatefunc)(int value, void * data);

typedef int                 BOOL;

typedef unsigned char muint8;
typedef unsigned short muint16;
typedef unsigned int muint32;
typedef unsigned long long muint64;

typedef char mint8;
typedef short mint16;
typedef int mint32;
typedef long long mint64;
typedef unsigned char mask;

typedef struct tagNeighborhood3D26C
{
	double index_1;
	double index_2;
	double index_3;
	double index_4;
	double index_5;
	double index_6;
	double index_7;
	double index_8;
	double index_9;
	double index_10;
	double index_11;
	double index_12;
	double index_13;
	double index_14;
	double index_15;
	double index_16;
	double index_17;
	double index_18;
	double index_19;
	double index_20;
	double index_21;
	double index_22;
	double index_23;
	double index_24;
	double index_25;
	double index_26;
} Neighbor3D26C;


namespace mip
{
	struct BoundingBoxSimple
	{
		mint32 maxX;
		mint32 maxY;
		mint32 maxZ;
		mint32 minX;
		mint32 minY;
		mint32 minZ;
	};
	struct Position3D
	{
		int x;
		int y;
		int z;
	};
}


namespace mip
{
	void CreateNLink3D26C_by_spacing_targeted(unsigned char *previewVolume, unsigned short *m_pRawVolume, Neighbor3D26C ***m_ppNLink3D26C, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, double xy_spacing, double z_spacing, double m_nLambda, progUpdatefunc update = 0, void * data = 0);
	int InitialCut3D26C_targeted(unsigned char *previewVolume, unsigned char *_outpudata, unsigned char *_volumData, Neighbor3D26C ***m_ppNLink3D26C, unsigned short *m_pRawVolume, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, unsigned char current_mask, progUpdatefunc update = 0, void * data = 0);
	
	void CreateNLink3D26C_by_spacing(unsigned short *m_pRawVolume, Neighbor3D26C ***m_ppNLink3D26C, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, double xy_spacing, double z_spacing, double m_nLambda, progUpdatefunc update = 0, void * data = 0);
	int InitialCut3D26C(unsigned char *_outpudata, unsigned char *_volumData, Neighbor3D26C ***m_ppNLink3D26C, unsigned short *m_pRawVolume, int width, int height, int slice, int x_min, int x_max, int y_min, int y_max, int z_min, int z_max, unsigned char current_mask, progUpdatefunc update = 0, void * data = 0);
	void writeRawFile(unsigned char *image, int size, QString filename);
	void writeRawFile(short *image, int size, QString filename);
	void writeRawFile(double *image, int size, QString filename);
	void SortLabelingMask(int *input, int *output, int width, int height, int slice, int m_num_component);
	void Rotation3D(unsigned char *input, unsigned char *output, int width, int height, int slice, float xdegree, float ydegree, float zdegree, int blank);
	int LabelImage(unsigned short width, unsigned short height, unsigned short slice, unsigned char* input, int* output);
	class RG
	{
	public:
		explicit RG(unsigned char *exist_mask, unsigned char *result_mask, int w, int h, int d, unsigned char chk_mask,
			BoundingBoxSimple boundingbox, std::vector<Position3D> *points, bool *threadStop);

		void setRangeMode(short* org, int lower, int upper);
		void setResultVal(unsigned char val);
		void setZeroCheck(bool val) { mZero = val; }; //only for 3d split
		void setProgress(progUpdatefunc updater, void* data = NULL);
		void setProgressRange(int start, int end);
		int	getCount() { return mCount; }

		void expandConnectivity(bool val);
		void SetRollingBall(bool val);
		bool startRG();


	private:
		void	setProgressValue(int);

	private:
		short*			mData;
		bool			mRollingBall;
		unsigned char*			mOrigin;
		unsigned char*			mResult;
		unsigned char			mResultVal;
		unsigned char			mOriginVal;
		int				mWidth;
		int				mHeight;
		int				mSlice;

		int				mLower;
		int				mUpper;

		bool			mMode;
		bool			mExpand;
		BoundingBoxSimple	mBox;
		bool			*mStop;
		int				mProg;
		int				mEndProg;

		bool			mZero;

		int				mCount; //selected voxel count
		std::list<Position3D> mSeeds;

		void*			mProgress; //
		progUpdatefunc	updater;
		//	QProgressDialog*	mProgDlg;
	};

	class Threshold {
	public:
		explicit Threshold(int depth, int width, int height, int lower, int upper,
			short *dataset, unsigned char *result, unsigned char mVal, mip::BoundingBoxSimple box, bool *stop);

		void setLayermode(unsigned char *maskset);
		void setProgress(progUpdatefunc updater, void* data = NULL);


		bool startThre();

	private:
		void	setProgressValue(int);

	private:
		int mDepth, mWidth, mHeight;

		//BoundingBoxI mBox;
		BoundingBoxSimple mBox;

		short *mDataset;

		int mLower, mUpper;

		unsigned char *mResult;

		bool *mStop;

		bool mWithin;

		unsigned char *mMaskset;

		unsigned char mMask;
		void*			mProgress;
		progUpdatefunc	mUpdater;
		bool res;
	};

	class HoleFilling
	{
	public:
		enum Mode {
			Mode3D = 0, // default
			Mode2DPlaneSagittal,
			Mode2DPlaneCoronal,
			Mode2DPlaneAxial,
			Mode2DPlaneWhole
		};
	public:
		explicit HoleFilling(int h, int w, int d, mip::BoundingBoxSimple box, int lowerHU, int upperHU, unsigned char maskValue,
			short *voxelData, unsigned char *maskedVolume, unsigned char *resultVolume, bool *stopEvent);
		explicit HoleFilling(int h, int w, int d, mip::BoundingBoxSimple box, int lowerHU, int upperHU, unsigned char maskValue,
			short *voxelData, unsigned char *maskedVolume, unsigned char *resultVolume, bool *stopEvent, Mode fillingMode);

		bool startFilling();
		void setProgress(progUpdatefunc updater, void* data = NULL);
	private:
		void	setProgressValue(int);
		bool performOptionalFilling(Mode mode, BoundingBoxSimple basicBox);
	private:		
		int				mHeight;
		int				mWidth;
		int				mDepth;
		//BoundingBoxI	mBasicBox;
		mip::BoundingBoxSimple	mBasicBox;
		int				mLowerHU;
		int				mUpperHU;
		unsigned char	mMaskValue;
		short			*mVoxel;

		unsigned char	*mMaskedVolume;
		unsigned char	*mResultVolume;
		bool			*mStopEvent;
		void*			mProgress;
		Mode			mMode;

		progUpdatefunc	mUpdater;
	};

	struct ScaleHeader
	{
		mint32 orgX;
		mint32 orgY;
		mint32 orgZ;
		float	orgSpX;
		float	orgSpY;
		float	orgSpZ;

		mint32 newX;
		mint32 newY;
		mint32 newZ;
		float	newSpX;
		float	newSpY;
		float	newSpZ;
	};
	
	class Downscaling
	{
	public:
		explicit Downscaling(std::string cachePath, int x, int y, int z, float spX, float spY, float spZ,
			short *dataset, int lastIndex, bool *stop);
		void setMaskPoint(int i, unsigned char* maskset);

		int startScaling();
		void setProgress(progUpdatefunc updater, void* data = NULL);
	private:
		void	setProgressValue(int);

	private:
		ScaleHeader shInfo;
		unsigned char* mMaskset[4];
		short* mDataset;
		bool *mStop;
		int mLastIndex; //mask last index
		std::string mCachePath;
		void*			mProgress;

		progUpdatefunc	mUpdater;
	};


	//Volume processing
	namespace VP
	{
		int riciandenoise3(double *u, const double *f, int M, int N, int P, double sigma, double lambda,
			double Tol, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z, bool *stop, progUpdatefunc updater, void* data = NULL);
		int riciandenoise2(double *u, const double *f, int M, int N, double sigma, double lambda, double Tol, int min_x, int max_x, int min_y, int max_y, bool *stop, progUpdatefunc updater, void* data = NULL);

		int ConvolutionFiltering(short *output, short *input, int kernel_size, double *kernel, int width, int height, int slice, int minX, int maxX, int minY, int maxY, int minZ, int maxZ, bool *stop, progUpdatefunc updater, void* data, bool is_mask);
		void getGaussian(int width, int height, int slice, double sigma, double* kernel, bool *stop, progUpdatefunc updater, void* data = NULL);
		void getLaplacian(int width, int height, int slice, double sigma, double* kernel, bool *stop, progUpdatefunc updater, void* data = NULL);
	};

	//Image processing
	namespace IP
	{
		void Dilation(int cx, int cy, int cz, unsigned char* output, unsigned char * maskset, unsigned char _m, unsigned char _direction, bool *stop, progUpdatefunc updater, void* data = NULL);
		void Erosion(int cx, int cy, int cz, unsigned char* output, unsigned char * maskset, unsigned char _m, unsigned char direction, bool *stop, progUpdatefunc updater, void* data = NULL);
	}


	//////////////////////////////////////////////////////////////////////////
	// TA
	//////////////////////////////////////////////////////////////////////////

#define DEPRECATED_VOLUMEDATA
#define DEPRECATED_VOLUMEDATA_GETROUNDNESS
#define DEPRECATED_VOLUMEDATA_GETGLCM
#define DEPRECATED_VOLUMEDATA_GETTEXTURE
#define DEPRECATED_VOLUMEDATA_GETWAVELET
#define DEPRECATED_VOLUMEDATA_GETGLN

	namespace TA
	{
		typedef struct
		{
			int x;
			int y;
			int z;
			short value;
		} PointHU;

		enum {
			L, R, U, D, UL, UR, DL, DR,
			B, BL, BR, BU, BD, BUL, BUR, BDL, BDR,
			A, AL, AR, AU, AD, AUL, AUR, ADL, ADR,
			Connected_4, Connected_8,	// 4, 8은 2차원에서
			Connected_6, Connected_26
		}; // 각각의 약자는 Left, Right, Up, Down, Behind, After

		struct TextureFeatureValues
		{
			short min_value;
			short max_value;
			int count;
			double volume;
			double avg;
			double stdev;
			double var;
			double entropy;
			double homogeneity;
		};

		typedef struct {
			int perc1;
			int perc5;
			int perc10;
			int perc15;
			int perc20;
			int perc25;
			int perc30;
			int perc35;
			int perc40;
			int perc45;
			int perc50;
			int perc55;
			int perc60;
			int perc65;
			int perc70;
			int perc75;
			int perc80;
			int perc85;
			int perc90;
			int perc95;
			int perc99;
		} HistoPerc;

		void generateAverageMap(unsigned char *maskVolume,			short *voxelData,  float *output,			int width, int height, int cnt,			unsigned char _maskBitValue);
		double CalcContrast(int *src, int data_size, mint16 HuMin);
		
		double CalcIDM(int *src, int data_size, mint16 HuMin);
		double CalcHomo(int *src, int data_size, mint16 HuMin);
		double CalcEntropy(int *src, int data_size, mint16 HuMin);
		double CalcUniformity(int *src, int data_size, mint16 HuMin);
		double CalcAvg(int *src, int data_size);
		double CalcPositiveAvg(int *src, int data_size);
		double CalcVariance(int *src, int data_size);
		double CalcMoments(int *src, int data_size, mint16 HuMin);
		double CalcStddev(int *src, int data_size);
		double CalcSkewness(int *src, int data_size, double a_L1_Avg, double a_L1_Stdev);
		double CalcKurtosis(int *src, int data_size, double a_L1_Avg, double a_L1_Stdev);
		double distance(int x1, int y1, int z1, int  x2, int  y2, int z2);
		double distance(int x1, int y1, int z1, int  x2, int  y2, int z2, double xy_spacing, double z_spacing);

#ifndef DEPRECATED_VOLUMEDATA_GETROUNDNESS
		double GetRoundness(VOLUME_DATA *_volumData, int width, int height, int cnt, mask _mask, int _mI);
#else
		double GetRoundness(mask *maskset, int width, int height, int cnt, mask _mask);
#endif

		void GLCM_2D(short **input, double **output, int width, int height, unsigned short mode, unsigned short GLCM_max_value);
		void GLCM_2D(unsigned char *mask, unsigned short **input, double **output, int width, int height, unsigned short mode, unsigned short GLCM_max_value, int noise_thr);
		void GLCM_2D_3x3(unsigned short **input, double **output, int width, int height, int ref_x, int ref_y, unsigned short mode, int GLCM_max_value, int noise_thr);

		double GLCM_PATENT_VALUE(double **input, int matrix_size);
		double GLCM_ASM(double **input, int matrix_size);

		double GLCM_IDM(double **input, int matrix_size);
		double GLCM_Contrast(double **input, int matrix_size);
		double GLCM_Entropy(double **input, int matrix_size);

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
		void GLCM_3D(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short mode, unsigned short GLCM_max_value, int noise_thr);
#else
		void GLCM_3D(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short mode, unsigned short GLCM_max_value, int noise_thr);
#endif

		double GLCM_3D_PATENT(unsigned short ***input, double **output, int width, int height, int slice, unsigned short mode, unsigned short GLCM_max_value, int noise_thr, int x, int y, int z);

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
		void GLCM_3D_EW(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr);
		void GLCM_3D_4C(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr);
		void GLCM_3D_NS(VOLUME_DATA *_volumData, mask _mask, int _mI, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr);
		void GLRL(unsigned short *input, VOLUME_DATA *_volumData, mask _mask, int _mI, int *output, int width, int height, int cnt, unsigned short mode, unsigned short HU_max_value, int run_lenght_max);
#else
		void GLCM_3D_EW(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr);
		void GLCM_3D_4C(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr);
		void GLCM_3D_NS(mask *maskset, mask _mask, unsigned short ***input, double **output, int width, int height, int slice, unsigned short GLCM_max_value, int noise_thr);
		void GLRL(unsigned short *input, mask *maskset, mask _mask, int *output, int width, int height, int cnt, unsigned short mode, unsigned short HU_max_value, int run_lenght_max);
#endif

		void ImageWidthDownSampling(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		void ImageHeightDownSampling(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);

		void ImageSliceDownSampling(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		void HighPassFiltering(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);

		void LowPassFiltering(double *input, double *output, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		double waveletHLH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		double waveletHLL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);

		double waveletHHH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		double waveletHHL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);

		double waveletLLH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		double waveletLLL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		double waveletLHL(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);
		double waveletLHH(short *image, unsigned char *roi_mask, int width, int height, int cnt, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z);

#ifndef DEPRECATED_VOLUMEDATA_GETGLCM
		void OnGetGLCM(VOLUME_DATA *_volumData, mask _mask, int _mI, int width, int height, int cnt, mint16 HuMin, double &EW_GLCMASM,
			double &EW_GLCMIDM,
			double &EW_GLCMcontrast,
			double &EW_GLCMentropy,
			double &NS_GLCMASM,
			double &NS_GLCMIDM,
			double &NS_GLCMcontrast,
			double &NS_GLCMentropy,
			double &CROSS_GLCMASM,
			double &CROSS_GLCMIDM,
			double &CROSS_GLCMcontrast,
			double &CROSS_GLCMentropy,
			double &SIX_GLCMASM,
			double &SIX_GLCMIDM,
			double &SIX_GLCMcontrast,
			double &SIX_GLCMentropy);
#else 
		void OnGetGLCM(mask *maskset, short *voxelData, mask _mask, int width, int height, int cnt, mint16 HuMin, double &EW_GLCMASM,
			double &EW_GLCMIDM,
			double &EW_GLCMcontrast,
			double &EW_GLCMentropy,
			double &NS_GLCMASM,
			double &NS_GLCMIDM,
			double &NS_GLCMcontrast,
			double &NS_GLCMentropy,
			double &CROSS_GLCMASM,
			double &CROSS_GLCMIDM,
			double &CROSS_GLCMcontrast,
			double &CROSS_GLCMentropy,
			double &SIX_GLCMASM,
			double &SIX_GLCMIDM,
			double &SIX_GLCMcontrast,
			double &SIX_GLCMentropy);
#endif

#ifndef DEPRECATED_VOLUMEDATA_GETTEXTURE
		double OnGetTexture(VOLUME_DATA *_volumData, int width, int height, int cnt, mask _mask, int _mI, double &texture_energy, double &texture_compactness1, double &texture_compactness2);
#else 
		double OnGetTexture(mask *maskset, short *voxelData, int width, int height, int cnt, mask _mask, double &texture_energy, double &texture_compactness1, double &texture_compactness2, float spaceX, float spaceY, float spaceZ);
#endif

#ifndef DEPRECATED_VOLUMEDATA_GETWAVELET
		void OnGetWavelet(VOLUME_DATA *_volumData, int width, int height, int cnt, mask _mask, int _mI, double	&Wavelet_HHH, double	&Wavelet_HHL, double	&Wavelet_HLH, double	&Wavelet_HLL, double	&Wavelet_LHH, double	&Wavelet_LHL, double	&Wavelet_LLH, double	&Wavelet_LLL);
#else 
		void OnGetWavelet(mask *maskset, short *voxelData, int width, int height, int cnt, mask _mask, double	&Wavelet_HHH, double	&Wavelet_HHL, double	&Wavelet_HLH, double	&Wavelet_HLL, double	&Wavelet_LHH, double	&Wavelet_LHL, double	&Wavelet_LLH, double	&Wavelet_LLL);
#endif

		void Floodfill_2D(unsigned char *sub_Img, int x, int y, int z, int numRows, int numCols, int numSlice, int newValue, int bgValue, int fgValue);
		float EuclideanDistance_by_Spacing(int x1, int y1, int z1, int x2, int y2, int z2, float xy_spacing, float z_spacing);
		void MakeSphere(unsigned char *sphere, int radius, int width, int height, int slice, int seed_x, int seed_y, int seed_z, float xy_spacing, float z_spacing);
		void MakeEllipsoid(unsigned char *ellipsoid, int x_radius, int y_radius, int z_radius);
		void Bresenham3D(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char *output, int symbol, int width, int height, int slice);
		float EuclideanDistance(int x1, int y1, int z1, int x2, int y2, int z2);

		int otsu(int nrows, int ncols, short *x, mint16 HuMin);
		bool getHistogram(const unsigned char *image, int width, int height, int slice, unsigned int *histo, int histoSize);
		bool equalizeHistogram(const unsigned char *in, int width, int height, int slice, unsigned char *out);
		bool getSumHistogram(const unsigned char *image, int width, int height, int slice, unsigned int *sumHisto, int histoSize);

		float GetDiceSimilarityCoefficient(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt);
		bool GetHausdorffDistance(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt, float x_spacing, float y_spacing, float z_spacing, double &m_hausdorff_euclidean, double &m_hausdorff_spacing);
		bool GetBoundary(unsigned char *input, unsigned char *output, int width, int height, int cnt, bool is2D);
		float GetTanimotoCoefficient(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt);
		float GetVolumeOverlapPercentage(unsigned char *mask1, unsigned char *mask2, int width, int height, int cnt);
	};

	class TAResult
	{
	public:
#ifndef DEPRECATED_VOLUMEDATA
		explicit TAResult(int w, int h, int d, short HUMin, short *dataset, unsigned char* maskset, unsigned char mVal, int mIndex);
#else
		explicit TAResult(int w, int h, int d, short HUMin, short *dataset, unsigned char* maskset, unsigned char mVal, float spaceX, float spaceY, float spaceZ);
#endif
		void setProgress(progUpdatefunc updater, void* data = NULL);

		void startTA(mip::TA::TextureFeatureValues*);

	private:
		void setProgressValue(int);

	private:
		int m_width;
		int m_height;
		int m_depth;
		short *m_pDataset;
		unsigned char* m_maskset;
		unsigned char m_maskVal;
#ifndef DEPRECATED_VOLUMEDATA
		int mIndex;
#else
		float m_spaceX;
		float m_spaceY;
		float m_spaceZ;
#endif
		short m_minVal;

		void* m_dtProg;
		progUpdatefunc m_updater;

	};


	//////////////////////////////////////////////////////////////////////////
	// Isotropic
	//////////////////////////////////////////////////////////////////////////

	class Isotropic
	{
	public:
		explicit Isotropic(int width, int height, int depth, float spaceX, float spaceZ, bool *stop);

		void setShortIsotropic(short *dataset);
		void setUCharIsotropic(unsigned char * maskset, unsigned char mVal);

		void setProgress(progUpdatefunc updater, void* data = NULL);

		int getIsoCount();

		void startIsotropic(void *result, int kernelType = 0); //return new count

	private:
		void	setProgressValue(int);

	private:
		int mWidth, mHeight, mDepth;
		float mSpaceX, mSpaceZ;

		bool *mStop;
		bool mShort;

		void *mDataset;

		unsigned char mVal;

		void*			mProgress;
		progUpdatefunc	mUpdater;

	};

	float KernelBSpline(double x);
	float KernelCubic(double t);
	float KernelLinear(double t);
	float KernelGaussian(double x);	


	//////////////////////////////////////////////////////////////////////////
	// Hessian
	//////////////////////////////////////////////////////////////////////////

	namespace Hessian
	{
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

		double hypot2(double x, double y);
		void tred2(double V[ndim][ndim], double d[ndim], double e[ndim]);
		void tql2(double V[ndim][ndim], double d[ndim], double e[ndim]);
		void eigen_decomposition(double A[ndim][ndim], double V[ndim][ndim], double d[ndim]);
		void CalcEigenvalues(double m[3][3], double *eigenvalue);
		double Fx(short *input, int x, int y, int z, int width, int height, int slice);
		double Fy(short *input, int x, int y, int z, int width, int height, int slice);
		double Fz(short *input, int x, int y, int z, int width, int height, int slice);
		double Fxx(short *input, int x, int y, int z, int width, int height, int slice);
		double Fxy(short *input, int x, int y, int z, int width, int height, int slice);
		double Fyx(short *input, int x, int y, int z, int width, int height, int slice);
		double Fzx(short *input, int x, int y, int z, int width, int height, int slice);
		double Fxz(short *input, int x, int y, int z, int width, int height, int slice);
		double Fyz(short *input, int x, int y, int z, int width, int height, int slice);
		double Fzy(short *input, int x, int y, int z, int width, int height, int slice);
		double Fyy(short *input, int x, int y, int z, int width, int height, int slice);
		double Fzz(short *input, int x, int y, int z, int width, int height, int slice);

		void HessianMatrix(short *input, unsigned char *layer, unsigned char *output, int width, int height, int slice, int extractor_mode);
		void ImageDownSampling(short *input, short *output, int width, int height, int slice);
		void ImageDownSampling(unsigned char *input, unsigned char *output, int width, int height, int slice);
		void ImageGaussianFilter(unsigned char *input, int width, int height, int slice);
		void ImageUpSampling_trilinear(unsigned char *input, unsigned char *output, int width, int height, int slice);
		void ImageUpSampling_trilinear(short *input, short *output, int width, int height, int slice);

	};
}

//////////////////////////////////////////////////////////////////////////
// 
namespace mmip {

	class Isotropic
	{
	public:
		explicit Isotropic(int width, int height, int depth, float spaceX, float spaceY, float spaceZ, bool *stop);

		void setShortIsotropic(short *dataset);
		void setUCharIsotropic(unsigned char * maskset, unsigned char mVal);

		void setProgress(progUpdatefunc updater, void* data = NULL);

		int getIsoCount(int type);

		void startIsotropic(void *result, int kernelType = 0); //return new count

	private:
		void	setProgressValue(int);

	private:

		float mSpaceXYResizing;

		int mWidth, mHeight, mDepth;
		float mSpaceX, mSpaceY, mSpaceZ;

		bool *mStop;
		bool mShort;

		void *mDataset; // volumn pointer : _volumData->getHUDataPoint()

		unsigned char mVal;

		void*			mProgress;
		progUpdatefunc	mUpdater;


	};

	float KernelBSpline(double x);
	float KernelCubic(double t);
	float KernelGaussian(double x);

}

