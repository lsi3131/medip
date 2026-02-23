#pragma once
#ifndef DEEPINSTHINK_H
#define DEEPINSTHINK_H
//#define PRODUCTION
#define NOMINMAX
//#define LIBTORCH_1_1
#define LIBTORCH_1_7_1

#ifdef DEEPINSTHINK_EXPORTS
#define DEEPINSTHINK_API __declspec(dllexport)
#else
#define DEEPINSTHINK_API __declspec(dllimport)
#endif

//#include <stdio.h>
#include <iostream>
#include <string>
#include <mutex>
#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>


#include "Metadata.h"
class SegmentationNet;
class FeatureClassificationNet;
class TranslationNet;
class ClassificationNet;
class RegressionNet;
class PredictionNet;
//DEEPINSTHINK_API void train_mnist();
using namespace std;

enum Status
{
	IDLE, //아무 일도 하지 않는다
	NORMAL,	//정상적으로 돌아가는 중
	DONE,	//정상적으로 일을 끝났다
	STOPPING,	//메딥에서 시키는 대로 종료 중
	STOPPED,	//메딥에서 시킨 다음에 종료됨
	//EXCEPTION,	//exception 발생, getException()으로 메시지 받을 수 있음, 종료 중
	EXCEPTION_TERMINATED,	//exception 발생한 다음에 종료됨
};
enum AI_Exception
{
	Others/*,
	TrainNotImplemented,
	ParameterMismatch,
	DirectoryError*/
};//NotEnough GPU memory

struct CheckData {
	Status status;
	int progress;
	string exception;
	AI_Exception ex_type;
	CheckData() {
		ex_type = AI_Exception::Others;
	}
};


#ifndef PRODUCTION
DEEPINSTHINK_API void exportSegmentationJson(const string& file_name, const string& new_file_name, bool doEncode = false,
	int version = 1, int weight_version = 1, ClientType type = ClientType::MEDIP, vector<string> classes = { "" },
	vector<vector<int> > salt=vector<vector<int> >());
//editMetadata(file_name, key, value)
//editClasses(filename, classes)
//editNormValues(filename, norm_values)

DEEPINSTHINK_API void exportClassificationJson(const string& file_name, const string& new_file_name, bool doEncode = false,
	int version = 1, ClientType type = ClientType::MEDIP, vector<string> classes = { "" }, std::string input_class = "",
	vector<vector<int> > salt = vector<vector<int> >());

DEEPINSTHINK_API void exportRegressionJson(const string& file_name, const string& new_file_name, bool doEncode = false,
	int version = 1, ClientType type = ClientType::MEDIP, vector<string> classes = { "" },
	vector<vector<int> > salt = vector<vector<int> >());

DEEPINSTHINK_API void exportTranslationJson(const string& file_name, const string& new_file_name, bool doEncode = false,
	int version = 1, ClientType type = ClientType::MEDIP, vector<string> classes = { "" },
	vector<vector<int> > salt = vector<vector<int> >());

DEEPINSTHINK_API void exportTranslationWeight(const string& json_filename, const string& pt_filename, const string& new_file_name, bool doEncode = false,
	int version = 1, ClientType type = ClientType::MEDIP, vector<string> classes = { "" },
	vector<vector<int> > salt = vector<vector<int> >());

DEEPINSTHINK_API void exportPredictionJson(const string& file_name, const string& new_file_name, bool doEncode = false,
	int version = 1, ClientType type = ClientType::MEDIP, vector<string> classes = { "" },
	vector<vector<int> > salt = vector<vector<int> >());

DEEPINSTHINK_API void exportPredictionWeight(const string& json_filename, const string& pt_filename, const string& new_file_name, bool doEncode = false,
	int version = 1, ClientType type = ClientType::MEDIP, vector<string> classes = { "" },
	vector<vector<int> > salt = vector<vector<int> >());
#endif



class DEEPINSTHINK_API AIBase {
public:
	/* Default extension is NONE (user must include extension with filename) */
	static string EXTENSION;
	
	/* Must call setExtension before calling train() or constructor using filename */
	static void setExtension(string ext) {
		ext.erase(std::remove_if(ext.begin(), ext.end(),
			[](auto const& c) -> bool { return !std::isalnum(c); }), ext.end());
		AIBase::EXTENSION = ext;
	}
	
	static void clearCache();
	static bool isCudaAvailable();

	AIBase(){}
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
	CheckData getCheckData() {
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

	CheckData _check;
	mutex _checkMutex;//protect _checkData
	vector< thread > _thread;
};


//#ifndef PRODUCTION
class DEEPINSTHINK_API FeatureClassification : public AIBase {
public:
	FeatureClassification(ClientType type, vector<string> classes = { "" },
		FeatureClassificationArch arch = FeatureClassificationArch::LINEAR_FULLY_CONNECTED,
		NormMethod norm = NormMethod::LINEAR_MIN_MAX,
		vector<int> layer_sizes = vector<int>(),
		vector<float> max_features = vector<float>(),
		vector<float> min_features = vector<float>(),
		vector<int> index_features = vector<int>(),
		vector<vector<int> > salt = vector<vector<int> >());

	/*
	* 저장된 weight 있는 경우 쓰인다.
	*/
	FeatureClassification(string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());

	/* MEDIP용 시작 */

	/* IDLE이나 DONE 상태인 경우 train 시작하고 true를 돌려준다
	* 다른 상태인 경우 train 시작하지 않고 false를 돌려준다
	* target_classes 0-based class
	*/
	bool medipTrain(vector< vector<float>> values, vector<int> target_classes, string save_weight_dir, string save_weight_name,
		bool gpu, int epochs = 1, int batch_size = 4, double lr = 0.01);

	/* IDLE이나 DONE 상태인 경우 predict 시작하고 true를 돌려준다
	* 다른 상태인 경우 predict 시작하지 않고 false를 돌려준다
	*/
	bool medipPredict(vector<float> values, bool gpu = true);

	//Get ROI
	//vector size: number of class x number of slice
	int getResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return _results;
	}
	//미래: get percentage result (weight body etc)

	vector<string> getClasses() {
		return meta.classes;
	}

	vector<int> getIndexFeatures() {
		return meta.index_features;
	}
	/* MEDIP용 끝 */

	void threadMedipTrain(vector< vector<float>> values, vector<int> target_classes,
		string save_weight_dir, string save_weight_name,
		bool gpu, int epochs, int batch_size, double lr);
	void threadMedipPredict(vector<float> values, bool gpu);

	void train(vector< vector<float>> values, vector<int> target_classes,
		string save_weight_dir, string save_weight_name,
		bool gpu, int epochs = 1, int batch_size = 4, double lr = 0.01);

	const static vector<FeatureClassificationArch> _trainable;

private:
	void parameterCheck(bool load = false);

	FeatureClassificationMeta meta;

	int _results;
	mutex _resultsMutex;//protect _result
	shared_ptr<FeatureClassificationNet> _net;
	vector<vector<int> > _salt;
};
//#endif

struct DenormOptions {
	DenormOptions(){ }
	vector<float> _values;
	DenormMethod _denorm;
};


struct NormOptions {
	NormOptions() {}
	vector<float> _values;
	NormMethod _norm;
};

struct WindowNormOptions : NormOptions {
	WindowNormOptions(int ww = 1000, int wl = 0, float target_low = 0, float target_high = 1, NormMethod n = NormMethod::LINEAR_WW_WL) {
		printf("Use Window Norm\n");
		//check NormMethod is correct
		if (n != NormMethod::LINEAR_WW_WL)//may add if new norm appears
			n = NormMethod::LINEAR_WW_WL; _norm = n;
		_values.push_back(ww);
		_values.push_back(wl);
		_values.push_back(target_low);
		_values.push_back(target_high);
	}
};

struct MinMaxNormOptions : NormOptions {
	MinMaxNormOptions(int min, int max, float target_low = 0, float target_high = 1, NormMethod n = NormMethod::LINEAR_MIN_MAX) {
		printf("Use MinMax Norm");
		//check NormMethod is correct
		if (n != NormMethod::LINEAR_MIN_MAX)//may add if new norm appears
			n = NormMethod::LINEAR_MIN_MAX;
		_norm = n;
		_values.push_back(min);
		_values.push_back(max);
		_values.push_back(target_low);
		_values.push_back(target_high);
	}
};

struct CustomNormOptions : NormOptions {
	CustomNormOptions(vector<float> values, vector<float>targets, NormMethod n = NormMethod::LINEAR_CUSTOM) {
		//check NormMethod is correct
		if (n != NormMethod::LINEAR_CUSTOM)//may add if new norm appears
			n = NormMethod::LINEAR_CUSTOM;
		_norm = n;
		_values.insert(std::end(_values), std::begin(values), std::end(values));
		_values.insert(std::end(_values), std::begin(targets), std::end(targets));
	}
};

struct RegressionOptions {
	RegressionOptions() {}
};


class DEEPINSTHINK_API Regression : public AIBase {
public:
	Regression(ClientType type, NormOptions normOptions, int image_size, vector<string> classes = { "" },
		RegressionArch arch = RegressionArch::UNET_REGRESSION32, RegressionOptions options = RegressionOptions(),
		vector<vector<int> > salt = vector<vector<int> >());
	Regression(string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	void parameterCheck(bool load = false);

	/* MEDIP용 시작 */

	static bool readMeta(RegressionMeta &meta, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	static bool updateDescription(string new_description, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());

	bool medipPredict(string dir_predict, bool gpu = true,
		int height = 512, int width = 512);

	vector<float> getResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return _results;
	}

	/* MEDIP용 끝 */

	void threadMedipPredict(string dir_predict, bool gpu, int height, int width);


	const static vector<RegressionArch> _trainable;
	void PrintWeightKeys();
private:
	RegressionMeta meta;

	vector < float > _results;//result class
	mutex _resultsMutex;//protect _results
	shared_ptr<RegressionNet> _net;
	vector<vector<int> > _salt;
};

struct ClassificationOptions {
	ClassificationOptions() {}
};


class DEEPINSTHINK_API Classification : public AIBase {
public:
	Classification(ClientType type, NormOptions normOptions, int image_size, vector<string> classes = { "" },
		ClassificationArch arch = ClassificationArch::RESNET, ClassificationOptions options = ClassificationOptions(),
		vector<vector<int> > salt = vector<vector<int> >());
	Classification(string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	void parameterCheck(bool load = false);

	/* MEDIP용 시작 */
	
	static bool readMeta(ClassificationMeta &meta, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	static bool updateDescription(string new_description, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());

	bool medipPredict(string dir_predict, bool gpu = true,
		int height = 512, int width = 512);

	vector<int> getResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return _results;
	}

	vector<vector <float> > getClassProbability() {
		lock_guard<mutex> guard(_resultsMutex);
		return std::move(_classProbability);
	}

	vector<string> getClasses() {
		return meta.classes;
	}

	vector<vector<uchar> > getVectorResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return std::move(_resultsv);
	}

	/* MEDIP용 끝 */

	void threadMedipPredict(string dir_predict, bool gpu, int height, int width);
	
	
	const static vector<ClassificationArch> _trainable;
	void PrintWeightKeys();
private:
	ClassificationMeta meta;

	vector< int > _results;//Vecotr Size = depth
	vector< vector <float> > _classProbability;//Vector Size = depth * n_class
	vector< vector<uchar> > _resultsv;//Vector Size = depth * n_class

	mutex _resultsMutex;//protect _results
	shared_ptr<ClassificationNet> _net;
	vector<vector<int> > _salt;
};


struct TranslationOptions {
	TranslationOptions() {}
};


class DEEPINSTHINK_API Translation : public AIBase {
public:
	Translation(ClientType type, NormOptions normOptions, int image_size, vector<string> classes = { "" },
		TranslationArch arch = TranslationArch::PIX2PIX, TranslationOptions options = TranslationOptions(),
		vector<vector<int> > salt = vector<vector<int> >());
	//DenormOptions
	Translation(string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	void parameterCheck(bool load = false);

	/* MEDIP용 시작 */
	
	static bool readMeta(TranslationMeta &meta, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	static bool updateDescription(string new_description, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());

	bool medipPredict(string dir_predict, string dir_result, bool gpu = true,
		int height = 512, int width = 512);

	//Get Result
	//vector size: number of class x number of slice
	vector<vector<cv::Mat> > getResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return _results;
	}
	vector<vector<short> > getVectorResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return std::move(_resultsv);
	}

	vector<string> getClasses() {
		return meta.classes;
	}

	/* MEDIP용 끝 */
	
	void threadMedipPredict(string dir_predict, string dir_result, bool gpu, int height, int width);

	const static vector<TranslationArch> _trainable;
	void PrintWeightKeys();
private:
	TranslationMeta meta;

	vector< vector<cv::Mat> > _results;//CV_
	vector< vector<short> > _resultsv;
	mutex _resultsMutex;//protect _results
	shared_ptr<TranslationNet> _net;
	vector<vector<int> > _salt;
};




struct PredictionOptions {
	PredictionOptions() {}
};


class DEEPINSTHINK_API Prediction : public AIBase {
public:
	Prediction(ClientType type, NormOptions normOptions, int image_size, vector<string> classes = { "" },
		PredictionArch arch = PredictionArch::XGBOOST_HCC, PredictionOptions options = PredictionOptions(),
		vector<vector<int> > salt = vector<vector<int> >());
	//DenormOptions
	Prediction(string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	void parameterCheck(bool load = false);

	/* MEDIP용 시작 */

	static bool readMeta(PredictionMeta &meta, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());
	static bool updateDescription(string new_description, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());

	bool medipPredict(
		const float& age,			// "Age" from UI;
		const float& gender,		// "Gender" from UI; NOTE: (Male: 1.f, Female: 0.f)
		const float& agent,			// "Antivirals agent" from UI; NOTE: (Entecavir: 1.f, Tenofovir: 0.f)
		const float& cirr,			// "Cirrhosis, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		const float& hbeag,			// "Presence of HBeAg, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		const float& plt,			// "Platelet, baseline" from UI;
		const float& alb,			// "Albumin, baseline" from UI;
		const float& tb,			// "Total bilirubin, baseline" from UI;
		const float& alt,			// "ALT, baseline" from UI;
		const float& hbv,			// "HBV DNA, baseline" from UI;
		const float& liver,			// "Liver Volume" from DeepCatch;
		const float& spleen);

	//Get Result
	vector<float> getResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return std::move(_results);
	}


	/* MEDIP용 끝 */

	void threadMedipPredict(
		const float& age,			// "Age" from UI;
		const float& gender,		// "Gender" from UI; NOTE: (Male: 1.f, Female: 0.f)
		const float& agent,			// "Antivirals agent" from UI; NOTE: (Entecavir: 1.f, Tenofovir: 0.f)
		const float& cirr,			// "Cirrhosis, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		const float& hbeag,			// "Presence of HBeAg, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		const float& plt,			// "Platelet, baseline" from UI;
		const float& alb,			// "Albumin, baseline" from UI;
		const float& tb,			// "Total bilirubin, baseline" from UI;
		const float& alt,			// "ALT, baseline" from UI;
		const float& hbv,			// "HBV DNA, baseline" from UI;
		const float& liver,			// "Liver Volume" from DeepCatch;
		const float& spleen);

	const static vector<PredictionArch> _trainable;
	void PrintWeightKeys();
private:
	PredictionMeta meta;

	vector<float> _results;//CV_
	mutex _resultsMutex;//protect _results
	shared_ptr<PredictionNet> _net;
	vector<vector<int> > _salt;
};




struct SegmentationOptions {
	SegmentationOptions() {}
	LossFunction _loss = LossFunction::Sigmoid;
	ActivationFunction _activation = ActivationFunction::RELU;

	bool _background = true;
	//NormMethod _norm = NormMethod::LINEAR_WW_WL_ZERO_ONE;
	int _slice = 1;
	int _init = 64;
	int _weight_version = 0;

	auto loss(const LossFunction& t)->decltype(*this) { _loss = t; return *this; }
	auto activation(const ActivationFunction& t)->decltype(*this) { _activation = t; return *this; }

	auto background(const bool& t)->decltype(*this) { _background = t; return *this; }
	//auto norm(const NormMethod& t)->decltype(*this) { _norm = t; return *this; }
	auto slice(const int& t)->decltype(*this) { _slice = t; return *this; }
	auto init(const int& t)->decltype(*this) { _init = t; return *this; }
	auto weight_version(const int& t)->decltype(*this) { _weight_version = t; return *this; }
};
class DEEPINSTHINK_API Segmentation : public AIBase {
public:
	/*
	* 저장된 weight 없는 경우 쓰인다.
	*/
	Segmentation(ClientType type, NormOptions normOptions, vector<string> classes = {""},
		SegmentationArch arch = SegmentationArch::UNET, SegmentationOptions options = SegmentationOptions(),/*bool softmax = false, NormMethod norm = NormMethod::LINEAR_ZERO_ONE,
		bool background = true, int slice = 1, int init = 64,*/
		vector<vector<int> > salt = vector<vector<int> >());
	~Segmentation() {
		cout << "Segmentation destructor.\n";
		//cleanup: make sure that thread is shut down
		if (_thread.size() > 0) {
			if (_thread[0].joinable())
				_thread[0].join();
			_thread.clear();
		}
		//clearNet();
	}
	/*
	* 저장된 weight 있는 경우 쓰인다.
	*/
	Segmentation(string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt=vector<vector<int> >());
	
	/* MEDIP용 시작 */

	//static 기능

	//Weight파일을 열어 Network를 만들지 않으면서 정보를 읽는다
	//weight 파일을 열 때 GPU를 소모하지 않도록 예방
	//reference으로 &meta 출력
	//성공 시 return true, 실패 시 return false (exception 발생 의심)
	static bool readMeta(SegmentationMeta &meta, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());

	//Weight파일을 열어 Network를 만들지 않으면서 description 정보를 바꾼다
	static bool updateDescription(string new_description, string load_weight_dir, string load_weight_name, bool doDecode = false,
		vector<vector<int> > salt = vector<vector<int> >());

	//static 기능 END

	/* IDLE이나 DONE 상태인 경우 train 시작하고 true를 돌려준다
	* 다른 상태인 경우 train 시작하지 않고 false를 돌려준다
	*/
	bool medipTrain(string dir_img, string dir_mask, string save_weight_dir, string save_weight_name,
		bool gpu, int height = 512, int width = 512, int epochs = 1, int batch_size = 4, double lr = 0.01);
	
	/* IDLE이나 DONE 상태인 경우 predict 시작하고 true를 돌려준다
	* 다른 상태인 경우 predict 시작하지 않고 false를 돌려준다
	* dir_result이 있는 경우에는 .raw도 쓰게 한다 (OpenCV Mat도 받을 수 있다)
	* dir_result이 없는 경우에는 .raw 쓰지 않다 (OpenCV Mat으로만 결과를 받을 수 있다)
	*/
	bool medipPredict(string dir_predict, string dir_result, bool gpu = true,
		int height = 512, int width = 512, int depth = 1, int start = 0,
		float x_spacing = 0.9, float y_spacing = 0.9, float z_spacing = 1.5);

	void clearNet();

	//Get ROI
	//vector size: number of class x number of slice
	vector<vector<cv::Mat> > getResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return _results;
	}
	vector<vector<uchar> > getVectorResults() {
		lock_guard<mutex> guard(_resultsMutex);
		return std::move(_resultsv);
	}
	//미래: get percentage result (weight body etc)

	vector<string> getClasses() {
		return meta.classes;
	}
	/* MEDIP용 끝 */

	void threadMedipTrain(string dir_img, string dir_mask, string save_weight_dir, string save_weight_name,
		bool gpu, int height, int width, int epochs, int batch_size, double lr);
	void threadMedipPredict(string dir_predict, string dir_result, bool gpu,
		int height, int width, int depth, int start, float x_spacing, float y_spacing, float z_spacing);
		
	const static vector<SegmentationArch> _trainable;

#ifndef PRODUCTION
	void train(string dir_img, string dir_mask, string save_weight_dir, string save_weight_name, int height = 512, int width = 512, int epochs = 1, double lr = 0.001, int batch_size = 4);

	//after finished, save .raw to dir_result
	//OR can get ROI from function getResults()
	void predictSingle(string dir_predict, string dir_result = "", float threshold = 0.5, int height = 512, int width = 512);
	//todo: predictall
	//TEST용
	void temp();
	void PrintWeightKeys();
	/* Multithreading Test */
	void doThread();
	void createThread();
#endif
private:
	void parameterCheck(bool load = false);

	SegmentationMeta meta;

	vector< vector<cv::Mat> > _results;//CV_8UC1
	vector< vector<uchar> > _resultsv;//Vector Size = depth * n_class
	//return std::move(_resultsv);

	mutex _resultsMutex;//protect _results
	shared_ptr<SegmentationNet> _net;
	vector<vector<int> > _salt;
	//gpu, height_weight
};

#endif