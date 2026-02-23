#pragma once

#include <QTreeWidget>

class DataContext;
class mipUsdStage;

class OmniverseStageListWidget : public QTreeWidget
{
public:
	Q_OBJECT

public:
	enum eColumn
	{
		Name = 0, 
		Type = 1, 
		Status = 2, 
	};

public:
	OmniverseStageListWidget(QWidget* parent);

public:
	bool Init(DataContext* pDataContext, mipUsdStage* pUsdStage);

	void Update();

private:
	void updateMaterialInfo();

private:
	DataContext* m_pDataContext;
	mipUsdStage* m_pUsdStage;
};
