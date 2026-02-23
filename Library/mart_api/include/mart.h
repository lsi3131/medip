#pragma once
#ifndef MART_H
#define MART_H
#define PRODUCTION

#ifdef MART_EXPORTS
#define MART_API __declspec(dllexport)
#else
#define MART_API __declspec(dllimport)
#endif

#include <iostream>
#include <string>
#include <mutex>
#include <opencv2/core/core.hpp>

using namespace std;

class ModuleManager; // External Link

/*
	Define Status & Exception Types
		-ENUM:		StatusType
		-ENUM:		ExceptionType
		-STRUCT:	StatusMonitor
*/

enum StatusType
{
	IDLE,					// DO NOTHING.
	NORMAL,					// WORKING WELL.
	DONE,					// WORK DONE.
	STOPPING,				// STOPPING.
	STOPPED,				// STOPPED.
	//EXCEPTION,			// EXCEPTION OCCURED.
	EXCEPTION_TERMINATED,	// TERMINATED DUE TO AN EXCEPTION.
};
enum ExceptionType
{
	OCCURED					// NOT DEFINED FOR ALL EXCEPTION...
};

struct StatusMonitor {
	StatusType st_type;		// see StatusType
	int progress;			// in [%] unit
	string exception;
	ExceptionType ex_type;	// see ExceptionType
	StatusMonitor() {
		ex_type = ExceptionType::OCCURED;
	}
};

/*
	Define: License, IO & Task Types
		-ENUM CLASS:	LicenseType
		-ENUM CLASS:	InOutType
		-ENUM CLASS:	DimensionType
		-ENUM CLASS:	FrameworkType
		-ENUM CLASS:	ContrastType
		-ENUM CLASS:	KernelType
		-ENUM CLASS:	DoseType
		-STRUCT:		ModelManager
*/

enum class LicenseType : unsigned int
{
	PASS,		// assigned 0	Research + Credit
	RESEARCH,	// assigned 1	Research License.
	CREDIT,		// assigned 2	Credit License.
};

enum class InOutType : unsigned int
{
	IO_SIN_SOUT,		// assigned 0	Single INput, Single OUTput
	IO_SIN_DOUT,		// assigned 1	Single INput, Dual OUTput
	IO_DIN_SOUT,		// assigned 2	Dual INput, Single OUTput
	IO_DIN_DOUT			// assigned 3	Dual INput, Dual OUTput
};


enum class TaskType : unsigned int	// MAIN TASK
{
	EXT_FRAMEWORK,		// assigned 0	Externel Framework: ex NVIDIA-MONAI, ...
	CONTRAST,			// assigned 1	Contrast Synthesis.
	KERNEL,				// assigned 2	Kernel Conversion.
	DOSE,				// assigned 3	Lowdose Reconostruction.
	QUANTIZATION,		// assigned 4	CT Quantization
};

enum class DimensionType : unsigned int
{
	TWO_DIM,			// assigned 0	TWO-DIMensional Operator
	EXTRA_DIM,			// assigned 1	EXTRA-DIMensional Operator
	THREE_DIM,			// assigned 2	THREE-DIMensional Operator
};

enum class FrameworkType : unsigned int	// SUB Category for EXT_FRAMEWORK
{
	IDENTITY,			// assigned 0	DO NOTHING
	MONAI_VB			// assigned 1	NVIDIA::MONAI::Vertebral Bone Segmentation
};

enum class ContrastType : unsigned int	// SUB Category for CONTRAST
{
	IDENTITY,			// assigned 0	DO NOTHING
	NON2CON_ADT,		// assigned 1	ADulT: Non-enhanced to Contrast
	CON2NON_ADT,		// assigned 2	ADulT: Contrast to Non-enhanced
	NON2CON_PED			// assigned 3	PEDiatric: Non-enhanced to Contrast
};

enum class KernelType : unsigned int	// SUB Category for KERNEL
{
	IDENTITY,			// assigned 0	DO NOTHING
	CONT_METHOD,		// assigned 1	CONTinuous Conversion
	NEUT_METHOD			// assigned 2	Kernel NEUTralization
};

enum class DoseType : unsigned int		// SUB Category for DOSE
{
	IDENTITY,			// assigned 0	DO NOTHING
	QD_RECON			// assigned 1	Quater-dose Reconstruction
};

enum class QuantizationType : unsigned int	// SUB Category for QUANTIZATION
{
	IDENTITY,			// assigned 0	DO NOTHING
	PDFF_RECON			// assigned 1	Proton Density Fat Fraction RECONstruction
};


struct ModelManager {
	InOutType io_type;
	TaskType task_type;
	DimensionType dim_type;
	LicenseType lic_type;
	int version;
	ModelManager() {
		lic_type = LicenseType::PASS;
	}
};

/* MART CPP - START */
class MART_API AIBase {
public:

	static void clearCache();
	static bool isCudaAvailable();

	AIBase() {}
	~AIBase() {
		cout << "AIBase Destructor.\n";
		//cleanup: make sure that thread is shut down
		if (_thread.size() > 0) {
			if (_thread[0].joinable())
				_thread[0].join();
			_thread.clear();
		}
	}
	/* NORMAL 상태인 경우:
	* -> STOPPING 상태로 바꾸고, thread이 끝나면 STOPPED으로 바꾸고, true를 돌려준다
	* 다른 상태(IDLE,DONE,STOPPING,STOPPED,EXCEPTION)인 경우:
	* -> STOP signal 보내지 않고 false를 돌려준다
	*/
	bool medipStop();

	//No mutex check inside all get functions yet
	StatusMonitor getStatus() {
		lock_guard<mutex> guard(_checkMutex);
		return _check;
	}

protected:
	bool checkStopSetProgress(int mStart, int mEnd, float progress);
	bool checkStop();

	/* IDLE, DONE, EXCEPTION_TERMINATED, STOPPED 상태인 경우 thread를 clear하고 IDLE 상태로 변한다
	* 다른 상태인 경우 아무것도 하지 않는다
	*/
	void clearThread();
	
	StatusMonitor _check;
	mutex _checkMutex;	//protect _check
	vector< thread > _thread;
};
/* MART CPP - END */

/* MART RECON - START */
class MART_API Reconstruction : public AIBase {
public:

	Reconstruction(string load_weight_dir, string load_weight_name);
	bool medipPredict(vector<short> in_vec1, vector<short> in_vec2, string dir_result, bool gpu = true);
	void threadMedipPredict(vector<short> in_vec1, vector<short> in_vec2, string dir_result, bool gpu);

	vector<vector<short>> getVectorResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return std::move(_resultsv);
	}
	
private:
	
	ModelManager _info;
	vector< vector<short> > _resultsv;
	mutex _resultsMutex;
	std::shared_ptr<ModuleManager> _manager;
};

#endif
/* MART RECON - END */