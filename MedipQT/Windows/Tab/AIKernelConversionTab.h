#pragma once

#include "CollapseWidget.h"

class AIKernelConversionTab :	public CollapseWidget
{
	Q_OBJECT
public:
	explicit AIKernelConversionTab(QWidget* parent = NULL);

	void SetVisibleInterpolation(bool bVisible);
	int GetSliderValue();

	std::vector<mint16> GetImageData_Origin() const;
	void SetImageData_Origin(const std::vector<mint16>& data);

	std::vector<mint16> GetImageData_Output() const;
	void SetImageData_Output(const std::vector<mint16>& data);

private:
	void applyInterpolationImage(void);

private slots :
	void slot_Predict();
	void slot_SliderRelease();
	void slot_SliderValueChanged();
	void slot_textChanged();
	
private:
	std::vector<mint16> m_image_Origin;
	std::vector<mint16> m_image_Output;

	QRadioButton* m_rdoContinousConversion = nullptr;
	QRadioButton* m_rdoKernelNeutralization = nullptr;
		
	QRadioButton* m_rdoCTQuantization= nullptr;
	

	QPushButton* m_btnPredict = nullptr;

	QGroupBox* m_groupInterpolation = nullptr;
	QSlider* m_sliderInterpolation = nullptr;
	QLineEdit* m_textInterpolation = nullptr;
		
};

