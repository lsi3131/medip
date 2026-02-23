#pragma once

#include <string>
//#include "QString"

template <typename T>
class ImageArray {

private:
	const int MAX_INT = ((int)0x7fffffff);

public:
	int sx;
	int sy;
	int sz;

	T *ndarray;

	int nDim;	

public:
	int nzX() {
		return nz(sx);
	}
	int nzY() {
		return nz(sy);
	}
	int nzZ() {
		return nz(sz);
	}

	void allocArray(int sx, int sy = 0, int sz = 0) {
		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

		nDim = 0;
		sx > 0 ? this->nDim++ : this->nDim;
		sy > 0 ? this->nDim++ : this->nDim;
		sz > 0 ? this->nDim++ : this->nDim;

		int size = getSize();
		ndarray = new T[size];

		clearArray();
	}

	void deallocArray() {
		delete[] ndarray;
	}

	void clearArray() {
		memset(ndarray, 0, getSize() * sizeof(T));
	}

	void setElement(int x, int y, int z, T value) {
		int index = getArrayIndex(x, y, z);
		ndarray[index] = value;
	}

	T getElement(int x, int y=0, int z=0) {
		int index = getArrayIndex(x, y, z);
		return ndarray[index];
	}

	int getArrayIndex(int x, int y, int z) {
		int index = z*sy*sx + y*sx + x;
		return index;
	}

	/*const char* getArrayInfoString() {
		std::string retString;
		retString = "";
		return retString.c_str();
	}*/
	const char* getArrayInfoString() {
		return mergeArrayInfo().toUtf8().constData();
	}

	void printInfo(std::string name) {
		//qDebug("\n");
		//qDebug("----------------------------------------------");
		//qDebug("\t# %s : \n%s", name.c_str(), this->getArrayInfoString());
		//qDebug("----------------------------------------------");
		//qDebug("\n");
	}
	

public:	
	int getSize() {
		int temp = sx * nz(sy) * nz(sz);
		return temp;
	}

	double getSum() {
		double temp = 0;
		for (int z = 0; z < nz(sz); z++) {
			for (int y = 0; y < nz(sy); y++) {
				for (int x = 0; x < sx; x++) {

					//int index = getArrayIndex(x, y, z);
					temp += getElement(x, y, z);

				}
			}
		}
		return temp;
	}

	double getMin() {
		double temp = MAX_INT;

		for (int z = 0; z < nz(sz); z++) {
			for (int y = 0; y < nz(sy); y++) {
				for (int x = 0; x < sx; x++) {
					if (getElement(x, y, z) < temp)
						temp = getElement(x, y, z);
				}
			}
		}

		return temp;
	}

	double getMax() {
		double temp = -MAX_INT;

		for (int z = 0; z < nz(sz); z++) {
			for (int y = 0; y < nz(sy); y++) {
				for (int x = 0; x < sx; x++) {
					if (getElement(x, y, z) > temp)
						temp = getElement(x, y, z);
				}
			}
		}

		return temp;
	}

	double getMean() {
		double temp = 0;
		for (int z = 0; z < nz(sz); z++) {
			for (int y = 0; y < nz(sy); y++) {
				for (int x = 0; x < sx; x++) {
					temp += getElement(x, y, z);
				}
			}
		}
		temp = temp / getSize();
		return temp;
	}

	double getVariance() {
		double temp = 0;
		double mean = getMean();
		for (int z = 0; z < nz(sz); z++) {
			for (int y = 0; y < nz(sy); y++) {
				for (int x = 0; x < sx; x++) {
					temp += pow(mean - getElement(x, y, z), 2);
				}
			}
		}
		temp = temp / getSize();
		return temp;
	}

	double getStd() {
		double temp = 0;
		temp = sqrt(getVariance());
		return temp;
	}

private:		
	int nz(int value) {
		return value == 0 ? 1 : value;
	}

	//////////////////////////////////////////////////////////////////////////
public:

	/*

	QString mergeArrayInfo() {
		QStringList arrayInfo;
		arrayInfo.append("\t shape: " + getShapeString());
		arrayInfo.append("\t size: " + getSizeString());
		arrayInfo.append("\t dtype: " + getDtypeString());
		arrayInfo.append("\t strides: " + getStridesString());
		arrayInfo.append("\t nbytes: " + getNbytesString());
		arrayInfo.append("\t sum: " + getSumString());
		arrayInfo.append("\t min: " + getMinString());
		arrayInfo.append("\t max: " + getMaxString());
		arrayInfo.append("\t mean: " + getMeanString());
		arrayInfo.append("\t variance: " + getVarianceString());
		arrayInfo.append("\t standard deviation: " + getStdString());

		//
		QString outputContents;
		outputContents.sprintf("%s", arrayInfo.join("\n").toUtf8().constData());
		//outputContents.replace('|', '\n');

		return outputContents;
	}


	//
	QString getShapeString() {
		QString contents;
		contents.sprintf("(z:%d, y:%d, x:%d)", sz, sy, sx);
		return contents;
	}

	QString getSizeString() {
		QString contents;
		contents.append(QString::number(getSize()));
		return contents;
	}

	QString getDtypeString() {
		QString contents;
		contents.append(QString::number(sizeof(T) * 8) + " bits");
		return contents;
	}

	QString getStridesString() {
		QString contents;
		int elementByte = sizeof(T);
		contents.sprintf("(%d, %d, %d)", elementByte*sx*sy, elementByte*sx, elementByte);
		return contents;
	}

	QString getNbytesString() {
		QString contents;
		contents.sprintf("%d", getSize() * sizeof(T));
		return contents;
	}

	QString getSumString() {
		QString contents;
		//contents.append(QString::number(getSum(), 'd'));
		contents.sprintf("%0.10lf", getSum());
		return contents;
	}

	QString getMinString() {
		QString contents;
		//contents.append(QString::number(getMin(), 'd'));
		contents.sprintf("%0.10lf", getMin());
		return contents;
	}

	QString getMaxString() {
		QString contents;
		//contents.append(QString::number(getMax(), 'd'));
		contents.sprintf("%0.10lf", getMax());
		return contents;
	}

	QString getMeanString() {
		QString contents;
		//contents.append(QString::number(getMean(), 'd'));
		contents.sprintf("%0.15lf", getMean());
		return contents;
	}

	QString getVarianceString() {
		QString contents;
		//contents.append(QString::number(getVariance(), 'd'));
		contents.sprintf("%0.15lf", getVariance());
		return contents;
	}

	QString getStdString() {
		QString contents;
		//contents.append(QString::number(getStd(), 'd'));
		contents.sprintf("%0.15lf", getStd());
		return contents;
	}
	*/

};

//
template <typename T>
void destroyImageArray(ImageArray<T> *imageArray)
{
	imageArray->deallocArray();
	delete imageArray;
}

