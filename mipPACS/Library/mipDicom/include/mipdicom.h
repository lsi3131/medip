#ifndef MIPDICOM_H
#define MIPDICOM_H
#include <string>

#if defined(MIP_DICOM_LIB)
#  define MIP_DICOM_EXPORT  __declspec(dllexport)
# else
#  define MIP_DICOM_EXPORT  __declspec(dllimport)
#endif

class MIP_DICOM_EXPORT mipDicom
{
public:
	mipDicom();
	~mipDicom();
	
	std::string getLibVersion(void);
private:

};

#endif // MIPDICOM_H
