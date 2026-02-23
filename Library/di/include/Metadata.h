#pragma once
#include"Definitions.h"
#include<vector>
#include<string>

const int SIZE_ID = 65;
const int SIZE_CLASSES = 40;
const int SIZE_DESCRIPTION = 300;

//Everytime a new variable is introduced or added, corresponding version must be noted
//and the new variable must be checked only if the weight is >= that version

enum class MetaType : unsigned int
{
	String,
	VectorString,
	VectorInt,
	VectorFloat,
	VectorVectorInt,
};

struct MetaKey {
	std::string key;
	MetaType type;
	int size;//for char only
	MetaKey(std::string key, MetaType type, int size = 100) {
		this->key = key;
		this->type = type;
		this->size = size;
	}
};

class Metadata {
public:
	std::vector<MetaKey> keys;
	virtual std::vector<std::string> getVS(std::string key) {
		return std::vector<std::string>();
	}
	virtual std::string getS(std::string key) {
		return std::string();
	}
	virtual std::vector<int> getVI(std::string key) {
		return std::vector<int>();
	}
	virtual std::vector<float> getVF(std::string key) {
		return std::vector<float>();
	}
	virtual std::vector< std::vector<int>> getVVI(std::string key) {
		return std::vector< std::vector<int>>();
	}
	virtual void setVS(std::string key, std::vector<std::string> data) {}
	virtual void setS(std::string key, std::string data) {}
	virtual void setVI(std::string key, std::vector<int> data) {}
	virtual void setVF(std::string key, std::vector<float> data) {}
	virtual void setVVI(std::string key, std::vector< std::vector<int>> data) {}

	std::vector<std::string> classes;//n_class x SIZE_CLASSES
	std::string description;//SIZE_DESCRIPTION
	std::string id;//SIZE_ID

	int version = AI_SEGMENTATION_VERSION;
	ClientType type = ClientType::MEDIP;
	Task task;
	int epoch_done = 0;
	int n_class;

	const std::string KEY_ID = "ID";
	const std::string KEY_DESCRIPTION = "DESCRIPTION";
	const std::string KEY_CLASSES = "CLASSES";
	const std::string KEY_METADATA = "METADATA";

	const int META_VERSION = 0;
	const int META_TYPE = 1;
	const int META_TASK = 2;
	const int META_ARCH = 3;
	const int META_EPOCH = 4;
	void reportNotExist(std::string key, bool set) {
		std::cout << "Meta: Key " + key + "  does not exist during "
			<< (set ? "setting" : "getting") << std::endl;
	}
};

class FeatureClassificationMeta : public Metadata {
public:
	void setVS(std::string key, std::vector<std::string> data) {
		if (key == KEY_CLASSES) {
			classes = data;
			n_class = classes.size();
		}
	}
	void setS(std::string key, std::string data) {
		if (key == KEY_ID)
			id = data;
	}
	void setVI(std::string key, std::vector<int> data) {
		if (key == KEY_METADATA) {
			//prepare according to version
			int s = data.size();
			if (s >= META_VERSION)
				version = data[META_VERSION];
			if (s >= META_TYPE)
				type = ClientType(data[META_TYPE]);
			if (s >= META_ARCH)
				arch = FeatureClassificationArch(data[META_ARCH]);
			if (s >= META_TASK)
				task = Task(data[META_TASK]);
			if (s >= META_EPOCH)
				epoch_done = data[META_EPOCH];
			if (s >= META_NORM)
				norm = NormMethod(data[META_NORM]);
		}
		else if (key == KEY_LAYERS) {
			layers = data;
		}
		else if (key == KEY_INDEX_FEATURES) {
			index_features = data;
		}
	}
	void setVF(std::string key, std::vector<float> data) {
		if (key == KEY_MAX_FEATURES) {
			max_features = data;
		} else if (key == KEY_MIN_FEATURES) {
			min_features = data;
		}
	}

	std::vector<std::string> getVS(std::string key) {
		if (key == KEY_CLASSES)
			return classes;
		return std::vector<std::string>();
	}
	std::string getS(std::string key) {
		//id
		if (key == KEY_ID)
			return id;
		return std::string();
	}
	std::vector<int> getVI(std::string key) {
		if (key == KEY_METADATA) {
			std::vector<int> meta(SIZE_METADATA);
			meta[META_TYPE] = (int)type;
			meta[META_VERSION] = version;
			meta[META_ARCH] = (int)arch;
			meta[META_TASK] = (int)task;
			meta[META_EPOCH] = epoch_done;
			meta[META_NORM] = (int)norm;
			/*
			meta[META_BACKGROUND] = (int)background;
			meta[META_INIT] = init;
			meta[META_SOFTMAX] = (int)softmax;
			*/
			return meta;
		}
		else if (key == KEY_LAYERS) {
			return layers;
		}
		else if (key == KEY_INDEX_FEATURES) {
			return index_features;
		}
		return std::vector<int>();
	}
	std::vector<float> getVF(std::string key) {
		if (key == KEY_MAX_FEATURES)
			return max_features;
		else if (key == KEY_MIN_FEATURES)
			return min_features;
		return std::vector<float>();
	}
	FeatureClassificationMeta() {
		keys.push_back(MetaKey(KEY_ID, MetaType::String, SIZE_ID));
		keys.push_back(MetaKey(KEY_CLASSES, MetaType::VectorString, SIZE_CLASSES));
		keys.push_back(MetaKey(KEY_METADATA, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_LAYERS, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_INDEX_FEATURES, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_MAX_FEATURES, MetaType::VectorFloat));
		keys.push_back(MetaKey(KEY_MIN_FEATURES, MetaType::VectorFloat));
	}
	FeatureClassificationMeta& operator=(const FeatureClassificationMeta &other) {
		n_class = other.n_class;
		classes = other.classes;
		n_class = classes.size();

		layers = other.layers;
		index_features = other.index_features;
		max_features = other.max_features;
		min_features = other.min_features;

		id = other.id;
		type = other.type;
		version = other.version;
		arch = other.arch;
		task = other.task;
		epoch_done = other.epoch_done;
		norm = other.norm;
	}
	std::vector<int> layers;//sizes of layers
	std::vector<int> index_features;//index of features (connect with other application)
	std::vector<float> min_features;//min-max values of features
	std::vector<float> max_features;//min-max values of features

	FeatureClassificationArch arch;
	int epoch_done = 0;
	NormMethod norm = NormMethod::LINEAR_WW_WL;

	const std::string KEY_LAYERS = "LAYERS";
	const std::string KEY_INDEX_FEATURES = "INDEX_FEATURES";

	const std::string KEY_MAX_FEATURES = "MAX_FEATURES";
	const std::string KEY_MIN_FEATURES = "MIN_FEATURES";
	
	const int SIZE_METADATA = 6;

	//const std::vector<int> SIZE_METADATA = std::vector<int>({ 6 });
	//const std::vector<int> VERSION_METADATA = std::vector<int>({ 1 });
	//version 1
	const int META_NORM = 5;
};

class TranslationMeta : public Metadata {
public:
	void setVS(std::string key, std::vector<std::string> data) {
		if (key == KEY_CLASSES) {
			classes = data;
			n_class = classes.size();
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setS(std::string key, std::string data) {
		if (key == KEY_ID)
			id = data;
		else if (key == KEY_DESCRIPTION)
			description = data;
		else {
			reportNotExist(key, true);
		}
	}
	void setVI(std::string key, std::vector<int> data) {
		if (key == KEY_METADATA) {
			//if (data.size() >= SIZE_METADATA) {
			int s = data.size();
			if (s > META_VERSION)
				version = data[META_VERSION];
			if (s > META_TYPE)
				type = ClientType(data[META_TYPE]);
			if (s > META_TASK)
				task = Task(data[META_TASK]);
			if (s > META_ARCH)
				arch = TranslationArch(data[META_ARCH]);
			if (s > META_EPOCH)
				epoch_done = data[META_EPOCH];
			if (s > META_NORM)
				norm = NormMethod(data[META_NORM]);
			if (s > META_DENORM)
				denorm = DenormMethod(data[META_DENORM]);
			if (s > META_IMAGE_SIZE)
				image_size = data[META_IMAGE_SIZE];
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setVF(std::string key, std::vector<float> data) {
		if (key == KEY_NORM_VALUES) {
			norm_values = data;
		} else if (key == KEY_DENORM_VALUES) {
			denorm_values = data;
		}
		else {
			reportNotExist(key, true);
		}
	}
	std::vector<std::string> getVS(std::string key) {
		if (key == KEY_CLASSES)
			return classes;
		reportNotExist(key, false); 
		return std::vector<std::string>();
	}
	std::string getS(std::string key) {
		if (key == KEY_ID)
			return id;
		if (key == KEY_DESCRIPTION)
			return description;
		reportNotExist(key, false);
		return std::string();
	}
	std::vector<int> getVI(std::string key) {
		if (key == KEY_METADATA) {
			std::vector<int> meta(SIZE_METADATA);
			meta[META_TYPE] = (int)type;
			meta[META_VERSION] = version;
			meta[META_ARCH] = (int)arch;
			meta[META_TASK] = (int)task;
			meta[META_EPOCH] = epoch_done;
			meta[META_NORM] = (int)norm;
			meta[META_DENORM] = (int)denorm;
			meta[META_IMAGE_SIZE] = image_size;

			return meta;
		}
		reportNotExist(key, false);
		return std::vector<int>();
	}
	std::vector<float> getVF(std::string key) {
		if (key == KEY_NORM_VALUES)
			return norm_values;
		if (key == KEY_DENORM_VALUES)
			return denorm_values;
		reportNotExist(key, false);
		return std::vector<float>();
	}
	TranslationMeta() {
		keys.push_back(MetaKey(KEY_ID, MetaType::String, SIZE_ID));
		keys.push_back(MetaKey(KEY_DESCRIPTION, MetaType::String, SIZE_DESCRIPTION));
		keys.push_back(MetaKey(KEY_CLASSES, MetaType::VectorString, SIZE_CLASSES));
		keys.push_back(MetaKey(KEY_METADATA, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_NORM_VALUES, MetaType::VectorFloat));
		keys.push_back(MetaKey(KEY_DENORM_VALUES, MetaType::VectorFloat));
	}
	TranslationArch arch;

	NormMethod norm = NormMethod::LINEAR_MIN_MAX;
	DenormMethod denorm = DenormMethod::NO_DENORM;
	int image_size;

	std::vector<float> norm_values;//values required to do normalization
	std::vector<float> denorm_values;//values required to do denormalization

	const std::string KEY_NORM_VALUES = "NORM_VALUES";
	const std::string KEY_DENORM_VALUES = "DENORM_VALUES";
	const std::string KEY_METADATA = "METADATA";
	const int SIZE_METADATA = 8;

	const int META_NORM = 5;
	const int META_DENORM = 6;
	const int META_IMAGE_SIZE = 7;
};



class PredictionMeta : public Metadata {
public:
	void setVS(std::string key, std::vector<std::string> data) {
		if (key == KEY_CLASSES) {
			classes = data;
			n_class = classes.size();
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setS(std::string key, std::string data) {
		if (key == KEY_ID)
			id = data;
		else if (key == KEY_DESCRIPTION)
			description = data;
		else {
			reportNotExist(key, true);
		}
	}
	void setVI(std::string key, std::vector<int> data) {
		if (key == KEY_METADATA) {
			//if (data.size() >= SIZE_METADATA) {
			int s = data.size();
			if (s > META_VERSION)
				version = data[META_VERSION];
			if (s > META_TYPE)
				type = ClientType(data[META_TYPE]);
			if (s > META_TASK)
				task = Task(data[META_TASK]);
			if (s > META_ARCH)
				arch = PredictionArch(data[META_ARCH]);
			if (s > META_EPOCH)
				epoch_done = data[META_EPOCH];
			if (s > META_NORM)
				norm = NormMethod(data[META_NORM]);
			if (s > META_DENORM)
				denorm = DenormMethod(data[META_DENORM]);
			if (s > META_IMAGE_SIZE)
				image_size = data[META_IMAGE_SIZE];
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setVF(std::string key, std::vector<float> data) {
		if (key == KEY_NORM_VALUES) {
			norm_values = data;
		}
		else if (key == KEY_DENORM_VALUES) {
			denorm_values = data;
		}
		else {
			reportNotExist(key, true);
		}
	}
	std::vector<std::string> getVS(std::string key) {
		if (key == KEY_CLASSES)
			return classes;
		reportNotExist(key, false);
		return std::vector<std::string>();
	}
	std::string getS(std::string key) {
		if (key == KEY_ID)
			return id;
		if (key == KEY_DESCRIPTION)
			return description;
		reportNotExist(key, false);
		return std::string();
	}
	std::vector<int> getVI(std::string key) {
		if (key == KEY_METADATA) {
			std::vector<int> meta(SIZE_METADATA);
			meta[META_TYPE] = (int)type;
			meta[META_VERSION] = version;
			meta[META_ARCH] = (int)arch;
			meta[META_TASK] = (int)task;
			meta[META_EPOCH] = epoch_done;
			meta[META_NORM] = (int)norm;
			meta[META_DENORM] = (int)denorm;
			meta[META_IMAGE_SIZE] = image_size;

			return meta;
		}
		reportNotExist(key, false);
		return std::vector<int>();
	}
	std::vector<float> getVF(std::string key) {
		if (key == KEY_NORM_VALUES)
			return norm_values;
		if (key == KEY_DENORM_VALUES)
			return denorm_values;
		reportNotExist(key, false);
		return std::vector<float>();
	}
	PredictionMeta() {
		keys.push_back(MetaKey(KEY_ID, MetaType::String, SIZE_ID));
		keys.push_back(MetaKey(KEY_DESCRIPTION, MetaType::String, SIZE_DESCRIPTION));
		keys.push_back(MetaKey(KEY_CLASSES, MetaType::VectorString, SIZE_CLASSES));
		keys.push_back(MetaKey(KEY_METADATA, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_NORM_VALUES, MetaType::VectorFloat));
		keys.push_back(MetaKey(KEY_DENORM_VALUES, MetaType::VectorFloat));
	}
	PredictionArch arch;

	NormMethod norm = NormMethod::LINEAR_MIN_MAX;
	DenormMethod denorm = DenormMethod::NO_DENORM;
	int image_size;

	std::vector<float> norm_values;//values required to do normalization
	std::vector<float> denorm_values;//values required to do denormalization

	const std::string KEY_NORM_VALUES = "NORM_VALUES";
	const std::string KEY_DENORM_VALUES = "DENORM_VALUES";
	const std::string KEY_METADATA = "METADATA";
	const int SIZE_METADATA = 8;

	const int META_NORM = 5;
	const int META_DENORM = 6;
	const int META_IMAGE_SIZE = 7;
};





class ClassificationMeta : public Metadata {
public:
	void setVS(std::string key, std::vector<std::string> data) {
		if (key == KEY_CLASSES) {
			classes = data;
			n_class = classes.size();
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setS(std::string key, std::string data) {
		if (key == KEY_ID)
			id = data;
		else if (key == KEY_DESCRIPTION)
			description = data;
		else if (key == KEY_INPUT_CLASS)
			input_class = data;
		else {
			reportNotExist(key, true);
		}
	}
	void setVI(std::string key, std::vector<int> data) {
		if (key == KEY_METADATA) {
			//if (data.size() >= SIZE_METADATA) {
			int s = data.size();
			if (s > META_VERSION)
				version = data[META_VERSION];
			if (s > META_TYPE)
				type = ClientType(data[META_TYPE]);
			if (s > META_TASK)
				task = Task(data[META_TASK]);
			if (s > META_ARCH)
				arch = ClassificationArch(data[META_ARCH]);
			if (s > META_EPOCH)
				epoch_done = data[META_EPOCH];
			if (s > META_NORM)
				norm = NormMethod(data[META_NORM]);

			if (s > META_ACTIVATION)
				activation = ActivationFunction(data[META_ACTIVATION]);
			if (s > META_LOSS)
				loss = LossFunction(data[META_LOSS]);
			if (s > META_IMAGE_SIZE)
				image_size = data[META_IMAGE_SIZE];
			if (s > META_INPUT_HU)
				input_hu = data[META_INPUT_HU];
			if (s > META_SLICE)
				slice = data[META_SLICE];
			//}
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setVF(std::string key, std::vector<float> data) {
		if (key == KEY_NORM_VALUES) {
			norm_values = data;
		}
		else {
			reportNotExist(key, true);
		}
	}

	std::vector<std::string> getVS(std::string key) {
		if (key == KEY_CLASSES)
			return classes;
		reportNotExist(key, false);
		return std::vector<std::string>();
	}
	std::string getS(std::string key) {
		if (key == KEY_ID)
			return id;
		if (key == KEY_DESCRIPTION)
			return description;
		if (key == KEY_INPUT_CLASS)
			return input_class;
		reportNotExist(key, false);
		return std::string();
	}
	std::vector<int> getVI(std::string key) {
		if (key == KEY_METADATA) {
			std::vector<int> meta(SIZE_METADATA);
			meta[META_TYPE] = (int)type;
			meta[META_VERSION] = version;
			meta[META_ARCH] = (int)arch;
			meta[META_TASK] = (int)task;
			meta[META_EPOCH] = epoch_done;
			meta[META_NORM] = (int)norm;

			meta[META_ACTIVATION] = (int)activation;
			meta[META_LOSS] = (int)loss;
			meta[META_IMAGE_SIZE] = image_size;
			meta[META_INPUT_HU] = input_hu;
			meta[META_SLICE] = slice;
			//META_VOLUME
			//META_CUBE
			//META_STRIDE
			return meta;
		}
		reportNotExist(key, false);
		return std::vector<int>();
	}
	std::vector<float> getVF(std::string key) {
		if (key == KEY_NORM_VALUES)
			return norm_values;
		reportNotExist(key, false);
		return std::vector<float>();
	}
	ClassificationMeta() {
		keys.push_back(MetaKey(KEY_ID, MetaType::String, SIZE_ID));
		keys.push_back(MetaKey(KEY_INPUT_CLASS, MetaType::String, SIZE_CLASSES));
		keys.push_back(MetaKey(KEY_DESCRIPTION, MetaType::String, SIZE_DESCRIPTION));
		keys.push_back(MetaKey(KEY_CLASSES, MetaType::VectorString, SIZE_CLASSES));
		keys.push_back(MetaKey(KEY_METADATA, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_NORM_VALUES, MetaType::VectorFloat));
	}
	bool isSoftmax() const {
		return (loss == LossFunction::Softmax || loss == LossFunction::DiceSoftmax);
	}
	bool isSigmoid() const {
		return (loss == LossFunction::Sigmoid || loss == LossFunction::DiceSigmoid);
	}

	ClassificationArch arch;

	NormMethod norm = NormMethod::LINEAR_WW_WL;

	std::vector<float> norm_values;//values required to do normalization
	std::string input_class;

	const std::string KEY_NORM_VALUES = "NORM_VALUES";
	const std::string KEY_INPUT_CLASS = "INPUT_CLASS";

	ActivationFunction activation = ActivationFunction::LRELU;
	LossFunction loss = LossFunction::Softmax;
	
	int image_size;
	bool input_hu = true;
	int slice = 1;

	const std::string KEY_METADATA = "METADATA";
	const int SIZE_METADATA = 11;

	const int META_NORM = 5;
	const int META_ACTIVATION = 6;
	const int META_LOSS = 7;
	const int META_IMAGE_SIZE = 8;
	const int META_INPUT_HU = 9;	
	const int META_SLICE = 10;
	
	//float gpuTrain = 0;//float gpuPredict = 0;
	//const std::string KEY_METADATAF = "METADATAF";//const int SIZE_METADATAF = 2;
	//keys.push_back(MetaKey(KEY_METADATAF, MetaType::VectorFloat));

	ClassificationMeta& operator=(const ClassificationMeta &other) {
		id = other.id;
		description = other.description;

		type = other.type;
		version = other.version;

		arch = other.arch;
		task = other.task;

		n_class = other.n_class;
		classes = other.classes;
		epoch_done = other.epoch_done;
		norm = other.norm;
		norm_values = other.norm_values;

		//ww, wl = other.ww, other.wl;
		activation = other.activation;
		loss = other.loss;

		input_hu = other.input_hu;
		slice = other.slice;
		
	}
};


class RegressionMeta : public Metadata {
public:
	void setVS(std::string key, std::vector<std::string> data) {
		if (key == KEY_CLASSES) {
			classes = data;
			n_class = classes.size();
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setS(std::string key, std::string data) {
		if (key == KEY_ID)
			id = data;
		else if (key == KEY_DESCRIPTION)
			description = data;
		else {
			reportNotExist(key, true);
		}
	}
	void setVI(std::string key, std::vector<int> data) {
		if (key == KEY_METADATA) {
			//if (data.size() >= SIZE_METADATA) {
			int s = data.size();
			if (s > META_VERSION)
				version = data[META_VERSION];
			if (s > META_TYPE)
				type = ClientType(data[META_TYPE]);
			if (s > META_TASK)
				task = Task(data[META_TASK]);
			if (s > META_ARCH)
				arch = RegressionArch(data[META_ARCH]);
			if (s > META_EPOCH)
				epoch_done = data[META_EPOCH];
			if (s > META_NORM)
				norm = NormMethod(data[META_NORM]);

			if (s > META_SLICE)
				slice = data[META_SLICE];
			if (s > META_SPECIAL)
				special = data[META_SPECIAL];
			if (s > META_IMAGE_SIZE)
				image_size = data[META_IMAGE_SIZE];
			//}
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setVF(std::string key, std::vector<float> data) {
		if (key == KEY_NORM_VALUES) {
			norm_values = data;
		}
		else {
			reportNotExist(key, true);
		}
	}

	std::vector<std::string> getVS(std::string key) {
		if (key == KEY_CLASSES)
			return classes;
		reportNotExist(key, false);
		return std::vector<std::string>();
	}
	std::string getS(std::string key) {
		if (key == KEY_ID)
			return id;
		if (key == KEY_DESCRIPTION)
			return description;
		reportNotExist(key, false);
		return std::string();
	}
	std::vector<int> getVI(std::string key) {
		if (key == KEY_METADATA) {
			std::vector<int> meta(SIZE_METADATA);
			meta[META_TYPE] = (int)type;
			meta[META_VERSION] = version;
			meta[META_ARCH] = (int)arch;
			meta[META_TASK] = (int)task;
			meta[META_EPOCH] = epoch_done;
			meta[META_NORM] = (int)norm;

			meta[META_SLICE] = slice;
			meta[META_SPECIAL] = special;
			meta[META_IMAGE_SIZE] = image_size;
			return meta;
		}
		reportNotExist(key, false);
		return std::vector<int>();
	}
	std::vector<float> getVF(std::string key) {
		if (key == KEY_NORM_VALUES)
			return norm_values;
		reportNotExist(key, false);
		return std::vector<float>();
	}
	RegressionMeta() {
		keys.push_back(MetaKey(KEY_ID, MetaType::String, SIZE_ID));
		keys.push_back(MetaKey(KEY_DESCRIPTION, MetaType::String, SIZE_DESCRIPTION));
		keys.push_back(MetaKey(KEY_CLASSES, MetaType::VectorString, SIZE_CLASSES));
		keys.push_back(MetaKey(KEY_METADATA, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_NORM_VALUES, MetaType::VectorFloat));
	}

	RegressionArch arch;

	NormMethod norm = NormMethod::LINEAR_WW_WL;

	std::vector<float> norm_values;//values required to do normalization

	const std::string KEY_NORM_VALUES = "NORM_VALUES";

	int slice = 1;

	int image_size;
	int special = 0;
	//special = 1, L3 postprocess, multiply output with input height

	const std::string KEY_METADATA = "METADATA";
	const int SIZE_METADATA = 9;

	const int META_NORM = 5;
	const int META_SLICE = 6;
	const int META_SPECIAL = 7;
	const int META_IMAGE_SIZE = 8;
	
	RegressionMeta& operator=(const RegressionMeta &other) {
		id = other.id;
		description = other.description;

		type = other.type;
		version = other.version;

		arch = other.arch;
		task = other.task;

		n_class = other.n_class;
		classes = other.classes;
		epoch_done = other.epoch_done;
		norm = other.norm;
		norm_values = other.norm_values;
		
		slice = other.slice;
		special = other.special;
		image_size = other.image_size;
	}
};


class SegmentationMeta : public Metadata {
public:
	void setVS(std::string key, std::vector<std::string> data) {
		if (key == KEY_CLASSES) {
			classes = data;
			n_class = classes.size();
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setS(std::string key, std::string data) {
		if (key == KEY_ID)
			id = data;
		else if (key == KEY_DESCRIPTION)
			description = data;
		else {
			reportNotExist(key, true);
		}
	}
	void setVI(std::string key, std::vector<int> data) {
		if (key == KEY_METADATA) {
			//if (data.size() >= SIZE_METADATA) {
			int s = data.size();
			if(s > META_VERSION)
				version = data[META_VERSION];
			if (s > META_TYPE)
				type = ClientType(data[META_TYPE]);
			if (s > META_TASK)
				task = Task(data[META_TASK]);
			if (s > META_ARCH)
				arch = SegmentationArch(data[META_ARCH]);
			if (s > META_EPOCH)
				epoch_done = data[META_EPOCH];
			if (s > META_NORM)
				norm = NormMethod(data[META_NORM]);

			if (s > META_ACTIVATION)
				activation = ActivationFunction(data[META_ACTIVATION]);
			if (s > META_LOSS)
				loss = LossFunction(data[META_LOSS]);
			if (s > META_BACKGROUND)
				background = data[META_BACKGROUND];
			if (s > META_INIT)
				init = data[META_INIT];
			if (s > META_SLICE)
				slice = data[META_SLICE];
			if (s > META_WEIGHT_VERSION)
				weight_version = data[META_WEIGHT_VERSION];
			//if (s > META_THREED)
			//	threeD = data[META_THREED];
		}
		else if (key == KEY_PATCH_SIZE) {
			patch_size = data;
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setVF(std::string key, std::vector<float> data) {
		if (key == KEY_NORM_VALUES) {
			norm_values = data;
		}
		else if (key == KEY_TARGET_SPACING) {
			target_spacing = data;
		}
		else {
			reportNotExist(key, true);
		}
	}
	void setVVI(std::string key, std::vector< std::vector<int> > data) {
		if (key == KEY_POOL_KERNEL) {
			pool_kernel_sizes = data;
		}
		else if (key == KEY_CONV_KERNEL) {
			conv_kernel_sizes = data;
		}
		else {
			reportNotExist(key, false);
		}
	}

	std::vector<std::string> getVS(std::string key) {
		if (key == KEY_CLASSES)
			return classes;
		reportNotExist(key, false);
		return std::vector<std::string>();
	}
	std::string getS(std::string key) {
		if (key == KEY_ID)
			return id;
		if (key == KEY_DESCRIPTION)
			return description;
		reportNotExist(key, false);
		return std::string();
	}
	std::vector<int> getVI(std::string key) {
		if (key == KEY_METADATA) {
			std::vector<int> meta(SIZE_METADATA);
			meta[META_TYPE] = (int)type;
			meta[META_VERSION] = version;
			meta[META_ARCH] = (int)arch;
			meta[META_TASK] = (int)task;
			meta[META_EPOCH] = epoch_done;
			meta[META_NORM] = (int)norm;
			
			meta[META_ACTIVATION] = (int) activation;
			meta[META_LOSS] = (int)loss;
			meta[META_BACKGROUND] = (int) background;
			//meta[META_SOFTMAX] = (int)softmax;
			meta[META_INIT] = init;
			meta[META_SLICE] = slice;
			//meta[META_THREED] = threeD;
			//META_VOLUME
			//META_CUBE
			//META_STRIDE

			meta[META_WEIGHT_VERSION] = weight_version;
			return meta;
		}
		else if(key == KEY_PATCH_SIZE){
			return patch_size;
		}
		reportNotExist(key, false);
		return std::vector<int>();
	}
	std::vector<float> getVF(std::string key) {
		if (key == KEY_NORM_VALUES)
			return norm_values;
		else if (key == KEY_TARGET_SPACING)
			return target_spacing;
		reportNotExist(key, false);
		return std::vector<float>();
	}
	std::vector< std::vector<int> > getVVI(std::string key) {
		if (key == KEY_POOL_KERNEL)
			return pool_kernel_sizes;
		else if (key == KEY_CONV_KERNEL)
			return conv_kernel_sizes;
		reportNotExist(key, false);
		std::vector< std::vector<int> >();
	}
	SegmentationMeta() {
		keys.push_back(MetaKey(KEY_ID, MetaType::String, SIZE_ID));
		keys.push_back(MetaKey(KEY_DESCRIPTION, MetaType::String, SIZE_DESCRIPTION));
		keys.push_back(MetaKey(KEY_CLASSES, MetaType::VectorString, SIZE_CLASSES));
		keys.push_back(MetaKey(KEY_METADATA, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_NORM_VALUES, MetaType::VectorFloat));
		//nnunet
		keys.push_back(MetaKey(KEY_POOL_KERNEL, MetaType::VectorVectorInt));
		keys.push_back(MetaKey(KEY_CONV_KERNEL, MetaType::VectorVectorInt));
		keys.push_back(MetaKey(KEY_PATCH_SIZE, MetaType::VectorInt));
		keys.push_back(MetaKey(KEY_TARGET_SPACING, MetaType::VectorFloat));
	}
	bool isSoftmax() const {
		return (loss == LossFunction::Softmax || loss == LossFunction::DiceSoftmax);
	}
	bool isSigmoid() const {
		return (loss == LossFunction::Sigmoid || loss == LossFunction::DiceSigmoid);
	}

	SegmentationArch arch;

	NormMethod norm = NormMethod::LINEAR_WW_WL;

	std::vector<float> norm_values;//values required to do normalization
	std::vector<std::vector<int>> pool_kernel_sizes;
	std::vector<std::vector<int>> conv_kernel_sizes;
	std::vector<int> patch_size;
	std::vector<float> target_spacing;
	
	const std::string KEY_NORM_VALUES = "NORM_VALUES";
	const std::string KEY_POOL_KERNEL = "POOL_KERNEL_SIZES";
	const std::string KEY_CONV_KERNEL = "CONV_KERNEL_SIZES";
	const std::string KEY_PATCH_SIZE = "PATCH_SIZE";
	const std::string KEY_TARGET_SPACING = "TARGET_SPACING";

	ActivationFunction activation = ActivationFunction::RELU;
	LossFunction loss = LossFunction::Sigmoid;

	bool background = true;
	int init = 64;//base_num_features
	int slice = 1;//num_conv_per_stage
	//bool threeD = true;
	//int num_conv_per_stage = 2;

	//bool boxing = false;
	int weight_version = 0;
	
	const std::string KEY_METADATA = "METADATA";
	const int SIZE_METADATA = 12;
	
	const int META_NORM = 5;
	const int META_ACTIVATION = 6;
	const int META_LOSS = 7;
	const int META_BACKGROUND = 8;
	const int META_INIT = 9;//base_num_features
	const int META_SLICE = 10;//num_conv_per_stage
	const int META_WEIGHT_VERSION = 11;
	//const int META_THREED = 12;//nnu-net threeD

	//int ww, wl;
	//const int META_WW = 6, META_WL = 7;
	//const int META_NORM_VALUES = 6;
	//float gpuTrain = 0;//float gpuPredict = 0;
	//const std::string KEY_METADATAF = "METADATAF";//const int SIZE_METADATAF = 2;
	//keys.push_back(MetaKey(KEY_METADATAF, MetaType::VectorFloat));

	SegmentationMeta& operator=(const SegmentationMeta &other) {
		id = other.id;
		description = other.description;

		type = other.type;
		version = other.version;

		arch = other.arch;
		task = other.task;

		n_class = other.n_class;
		classes = other.classes;
		epoch_done = other.epoch_done;
		norm = other.norm;
		norm_values = other.norm_values;

		//ww, wl = other.ww, other.wl;
		activation = other.activation;
		loss = other.loss;

		background = other.background;
		//softmax = other.softmax;
		init = other.init;
		slice = other.slice;
		weight_version = other.weight_version;
	}
};
