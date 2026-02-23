#include "stdafx.h"
#include "ActionImageFeatureExtractor.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "QClipboard"
#include "Windows/Tabwindow.h"
#include "algorithm/FeatureExtractor.h"
#include "algorithm/MagicCut.h"

void WorkImageFeatureExtractor::updateProgress(int val, void* data)
{
	WorkImageFeatureExtractor* worker = (WorkImageFeatureExtractor*)data;
	worker->setProgressValue(val);
}

void WorkImageFeatureExtractor::setProgressValue(int value, bool init)
{
	static int val = 0;
	
	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkImageFeatureExtractor::threadRun()
{
	setProgressValue(0, true);
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	mint16 HuMin = _volumData->getHuMin();
	int width = dataCX;
	int height = dataCY;
	int slice = dataCZ;
	
#ifndef DEPRECATED_VOLUMEDATA
	mip::TAResult result(dataCX, dataCY, dataCZ, HuMin, _volumData->getHUDataPoint(), _volumData->getMaskDataPoint(_maskIndex), _mask, _maskIndex);
#else
	mip::TAResult result(dataCX, dataCY, dataCZ, HuMin, _volumData->getHUDataPoint(), _volumData->getMaskDataPoint(_maskIndex), _mask,
		_volumData->getSpaceX(true), _volumData->getSpaceY(true), _volumData->getSpaceZ(true));
#endif

	result.setProgress(updateProgress, this);
	result.startTA(m_textureFeatureVals);
	
	// classify ·Î ¹­À½
	_str->clear();
	//_str->append(QString((QChar*)str.data()));

	emit finished();

}

ActionFeatureAdd::ActionFeatureAdd(int uid, QString result, VOLUME_DATA * volumData, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_FEATUREEXTRACTOR_ADD;

	m_id = s_id++;
	m_uid = uid;
	m_result = result;
	m_first = true;
	m_preInfo = -1;

	if (WIN_MANAGER->GetTab())
	{
		AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

		if (tab)
			m_preInfo = tab->existResult(m_uid, true);
	}

	m_volumData = volumData;

	
}

void ActionFeatureAdd::undo()
{
	AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

	if (-1 != m_preInfo)
	{
		QDir dir(STRING_MANAGER->cacheFilePath);
		if (dir.exists())
		{
			//QString filename = (STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			//QFile file(filename);
			//if (file.exists() && file.open(QIODevice::ReadOnly))
			//{
			//	mint16 huMin, huMax;
			//	int pointCount;
			//	
			//	file.read((char*)&huMin, sizeof(mint16));
			//	file.read((char*)&huMax, sizeof(mint16));
			//	file.read((char*)&pointCount, sizeof(int));
			//	QVector<QPointF> points(pointCount);
			//	file.read((char*)points.data(), sizeof(QPointF) *(pointCount));
			//	file.read((char*)&pointCount, sizeof(int));
			//	QVector<QPointF> logPoints(pointCount);
			//	file.read((char*)logPoints.data(), sizeof(QPointF) *(pointCount));

			//	// Read GMM QTable

			//	// Read GMM Border QTable

			//	// Read Tree Data

			//	file.close();

			//	if (tab)
			//		tab->SetResult(m_uid, m_preResult, huMin, huMax, points, logPoints, m_id);
			//}
		}
	}
	else
	{
		makeRedoFile(tab);

		if (tab)
		{
			tab->DeleteResult(m_uid, true);
		}
	}

	m_volumData->setTAState(m_uid, false);
}


void ActionFeatureAdd::redo()
{
	AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

	mint16 huMin, huMax;
	int pointCount;
	QVector<QPointF> points, logPoints;
	if (m_first)
	{
		m_first = false;
		QDir dir(STRING_MANAGER->cacheFilePath);
		if (dir.exists())
		{
			QString filename = (STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			QFile file(filename);

			if (-1 != m_preInfo)
			{
				if (file.open(QIODevice::WriteOnly))
				{
					if (tab)
						tab->GetResult(huMin, huMax, m_preResult, points, logPoints, m_uid, true);
					file.write((char*)&huMin, sizeof(mint16));
					file.write((char*)&huMax, sizeof(mint16));
					pointCount = points.count();
					file.write((char*)&pointCount, sizeof(int));
					file.write((char*)points.toStdVector().data(), sizeof(QPointF) *(pointCount));
					pointCount = logPoints.count();
					file.write((char*)&pointCount, sizeof(int));
					file.write((char*)logPoints.toStdVector().data(), sizeof(QPointF) *(pointCount));
					pointCount = m_preResult.length();
					file.write((char*)&pointCount, sizeof(int));
					file.write((char *)m_preResult.constData(), sizeof(QChar)*m_preResult.size());

					file.close();
				}
			}

			// init
			if (tab)
				tab->SetResult(m_uid, m_result, m_id);
		}
	}
	else
	{
		QDir dir(STRING_MANAGER->cacheFilePath);
		if (dir.exists())
		{
			readRedoFile(tab);
		}
	}

	m_volumData->setTAState(m_uid, true);
}

void ActionFeatureAdd::makeRedoFile(QWidget* pTab)
{
	QString filename = (STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	QFile  file(filename);

	if (file.open(QIODevice::WriteOnly))
	{
		mint16 huMin, huMax;
		int pointCount;
		QVector<QPointF> points, logPoints;

		if (pTab)
		{
			((AnalysisTab*)pTab)->GetResult(huMin, huMax, m_result, points, logPoints, m_uid, true);
		}

		file.write((char*)&huMin, sizeof(mint16));
		file.write((char*)&huMax, sizeof(mint16));
		pointCount = points.count();
		file.write((char*)&pointCount, sizeof(int));
		file.write((char*)points.toStdVector().data(), sizeof(QPointF) * (pointCount));
		pointCount = logPoints.count();
		file.write((char*)&pointCount, sizeof(int));
		file.write((char*)logPoints.toStdVector().data(), sizeof(QPointF) * (pointCount));
		pointCount = m_result.length();
		file.write((char*)&pointCount, sizeof(int));
		file.write((char*)m_result.constData(), sizeof(QChar) * m_result.size());

		// Save QTable data
		if (pTab)
		{
			if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
			{
				std::vector<std::vector<double>> vecGMMTable;
				std::vector<std::vector<double>> vecBorderTable;
				((AnalysisTab*)pTab)->GetQTableResult(vecGMMTable, vecBorderTable, m_uid, true);

				// Save GMM
				int rows = (int)vecGMMTable.size();
				file.write((char*)&rows, sizeof(int));
				for (int i = 0; i < vecGMMTable.size(); ++i)
				{
					int cols = (int)vecGMMTable[i].size();
					file.write((char*)&cols, sizeof(int));
					for (int j = 0; j < vecGMMTable[i].size(); ++j)
					{
						file.write((char*)&vecGMMTable[i][j], sizeof(double));
					}
				}

				// Save GMM Border QTable
				rows = (int)vecBorderTable.size();
				file.write((char*)&rows, sizeof(int));
				for (int i = 0; i < rows; ++i)
				{
					int cols = (int)vecBorderTable[i].size();
					file.write((char*)&cols, sizeof(int));
					for (int j = 0; j < cols; ++j)
					{
						file.write((char*)&vecBorderTable[i][j], sizeof(double));
					}
				}
			}

			// Save Tree Data(Shape2D / 3D)
			QTreeWidget* treeWidget = ((AnalysisTab*)pTab)->getTreeWidget(m_uid, true);
			
			int topLevelItemCount = (int)treeWidget->topLevelItemCount();
			file.write((char*)&topLevelItemCount, sizeof(int));

			for (int i = 0; i < topLevelItemCount; ++i)
			{
				QTreeWidgetItem* treeItem = treeWidget->topLevelItem(i);

				treeItem->setCheckState(0, Qt::CheckState::Unchecked);

				int treeItemColumnCount = (int)treeItem->columnCount();
				file.write((char*)&treeItemColumnCount, sizeof(int));
				for (int j = 0; j < treeItemColumnCount; ++j)
				{
					QString str = treeItem->text(j);
					int     str_len = str.length() + 1;

					file.write((char*)&str_len, sizeof(int));
					if (str_len > 0)
					{
						file.write(str.toLocal8Bit().toStdString().c_str(), sizeof(char) * str_len);
					}
				}

				int treeItemChildCount = (int)treeItem->childCount();
				file.write((char*)&treeItemChildCount, sizeof(int));

				for (int j = 0; j < treeItemChildCount; ++j)
				{
					QTreeWidgetItem* childItem = treeItem->child(j);
					int childItemColumnCount = (int)childItem->columnCount();
					file.write((char*)&childItemColumnCount, sizeof(int));

					childItem->setCheckState(0, Qt::CheckState::Unchecked);

					for (int k = 0; k < childItemColumnCount; ++k)
					{
						QString str1 = childItem->text(k);
						int     str1_len = str1.length() + 1;

						file.write((char*)&str1_len, sizeof(int));
						if (str1_len > 0)
						{
							file.write(str1.toLocal8Bit().toStdString().c_str(), sizeof(char) * str1_len);
						}
					}

					int childItemChildCount = (int)childItem->childCount();
					file.write((char*)&childItemChildCount, sizeof(int));

					for (int l = 0; l < childItemChildCount; ++l)
					{
						QTreeWidgetItem* childItem2 = childItem->child(l);
						int childItem2ColumnCount = (int)childItem2->columnCount();
						file.write((char*)&childItem2ColumnCount, sizeof(int));

						childItem2->setCheckState(0, Qt::CheckState::Unchecked);

						for (int m = 0; m < childItem2ColumnCount; ++m)
						{
							QString str2 = childItem2->text(m);
							int     str2_len = str2.length() + 1;

							file.write((char*)&str2_len, sizeof(int));
							if (str2_len > 0)
							{
								file.write(str2.toLocal8Bit().toStdString().c_str(), sizeof(char) * str2_len);
							}
						}
					}
				}
			}
		}

		file.close();
	}
}

void ActionFeatureAdd::readRedoFile(QWidget* pTab)
{
	QString filename = (STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	QFile file(filename);
	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		mint16 huMin, huMax;
		int pointCount;
		QVector<QPointF> points, logPoints;

		file.read((char*)&huMin, sizeof(mint16));
		file.read((char*)&huMax, sizeof(mint16));
		file.read((char*)&pointCount, sizeof(int));
		points.resize(pointCount);
		file.read((char*)points.data(), sizeof(QPointF) * (pointCount));
		file.read((char*)&pointCount, sizeof(int));
		logPoints.resize(pointCount);
		file.read((char*)logPoints.data(), sizeof(QPointF) * (pointCount));
		file.read((char*)&pointCount, sizeof(int));
		file.read((char*)m_result.constData(), sizeof(QChar) * pointCount);

		if (pTab)
		{
			((AnalysisTab*)pTab)->SetResult(m_uid, m_result, huMin, huMax, points, logPoints, m_id);

			std::vector<std::vector<double>> vecGMMTable;
			std::vector<std::vector<double>> vecBorderTable;

			if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
			{
				// Read GMM QTable data
				int rows = -INT_MAX;
				file.read((char*)&rows, sizeof(int));
				vecGMMTable.resize(rows);
				for (int i = 0; i < rows; ++i)
				{
					int cols = -INT_MAX;
					file.read((char*)&cols, sizeof(int));
					vecGMMTable[i].resize(cols);
					for (int j = 0; j < cols; ++j)
					{
						file.read((char*)&vecGMMTable[i][j], sizeof(double));
					}
				}

				// Read GMM Border QTable data
				rows = -INT_MAX;
				file.read((char*)&rows, sizeof(int));
				vecBorderTable.resize(rows);
				for (int i = 0; i < rows; ++i)
				{
					int cols = -INT_MAX;
					file.read((char*)&cols, sizeof(int));
					vecBorderTable[i].resize(cols);
					for (int j = 0; j < cols; ++j)
					{
						file.read((char*)&vecBorderTable[i][j], sizeof(double));
					}
				}

				((AnalysisTab*)pTab)->SetQTableResult(vecGMMTable, vecBorderTable, m_uid, true);
			}

			// Read Tree Data(Shape2D / 3D)
			((AnalysisTab*)pTab)->InitTreeWidget(m_uid, true);
			QTreeWidget* treeWidget = ((AnalysisTab*)pTab)->getTreeWidget(m_uid, true);

			int topLevelItemCount = -INT_MAX;
			file.read((char*)&topLevelItemCount, sizeof(int));

#ifdef _DEBUG
			assert(topLevelItemCount == treeWidget->topLevelItemCount());
#endif

			for (int i = 0; i < topLevelItemCount; ++i)
			{
				QTreeWidgetItem* treeItem = treeWidget->topLevelItem(i);

				int treeItemColumnCount;
				file.read((char*)&treeItemColumnCount, sizeof(int));

#ifdef _DEBUG
				assert(treeItemColumnCount == (int)treeItem->columnCount());
#endif

				for (int j = 0; j < treeItemColumnCount; ++j)
				{
					int len;
					file.read((char*)&len, sizeof(int));

					QString str;
					if (len > 0)
					{
						std::vector<char> data(len);
						file.read(data.data(), sizeof(char) * len);

						str = QString(data.data());
					}

					treeItem->setText(j, str);
				}

				int treeItemChildCount;
				file.read((char*)&treeItemChildCount, sizeof(int));

#ifdef _DEBUG
				assert(treeItemChildCount == treeItem->childCount());
#endif
				for (int j = 0; j < treeItemChildCount; ++j)
				{
					QTreeWidgetItem* childItem = treeItem->child(j);

					int childItemColumnCount;
					file.read((char*)&childItemColumnCount, sizeof(int));

#ifdef _DEBUG
					assert(childItemColumnCount == childItem->columnCount());
#endif
					for (int k = 0; k < childItemColumnCount; ++k)
					{
						int len;
						file.read((char*)&len, sizeof(int));

						QString str;
						if (len > 0)
						{
							std::vector<char> data(len);
							file.read(data.data(), sizeof(char) * len);

							str = QString(data.data());
						}

						childItem->setText(k, str);
					}

					int childItemChildCount;
					file.read((char*)&childItemChildCount, sizeof(int));

#ifdef _DEBUG
					assert(childItemChildCount == childItem->childCount());
#endif

					for (int l = 0; l < childItemChildCount; ++l)
					{
						QTreeWidgetItem* childItem2 = childItem->child(l);

						int childItem2ColumnCount;
						file.read((char*)&childItem2ColumnCount, sizeof(int));

#ifdef _DEBUG
						assert(childItem2ColumnCount == childItem2->columnCount());
#endif

						for (int m = 0; m < childItem2ColumnCount; ++m)
						{
							int len;
							file.read((char*)&len, sizeof(int));

							QString str;
							if (len > 0)
							{
								std::vector<char> data(len);
								file.read(data.data(), sizeof(char) * len);

								str = QString(data.data());
							}

							childItem2->setText(m, str);
						}
					}
				}
			}
		}

		file.close();
	}
}

bool ActionFeatureAdd::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionFeatureDel::ActionFeatureDel(int uid, VOLUME_DATA * volumData, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_FEATUREEXTRACTOR_DEL;

	m_id = s_id++;
	m_uid = uid;
	m_first = true;
	m_volumData = volumData;
	m_state = m_volumData->getTAState(m_uid);
}

void ActionFeatureDel::undo()
{
	AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = (STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			mint16 huMin, huMax;
			int pointCount;

			file.read((char*)&huMin, sizeof(mint16));
			file.read((char*)&huMax, sizeof(mint16));
			file.read((char*)&pointCount, sizeof(int));
			QVector<QPointF> points(pointCount);
			file.read((char*)points.data(), sizeof(QPointF) *(pointCount));
			file.read((char*)&pointCount, sizeof(int));
			QVector<QPointF> logPoints(pointCount);
			file.read((char*)logPoints.data(), sizeof(QPointF) *(pointCount));

			file.close();

			if (tab)
				tab->SetResult(m_uid, m_result, huMin, huMax, points, logPoints, m_id);
		}
	}

	m_volumData->setTAState(m_uid, m_state);
}

void ActionFeatureDel::redo()
{
	AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

	mint16 huMin, huMax;
	int pointCount;
	QVector<QPointF> points, logPoints;

	if (m_first)
	{
		m_first = false;
		QDir dir(STRING_MANAGER->cacheFilePath);
		if (dir.exists())
		{
			QString filename = (STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			QFile file(filename);

			if (file.open(QIODevice::WriteOnly))
			{
				if (tab)
					tab->GetResult(huMin, huMax, m_result, points, logPoints, m_uid, true);
				file.write((char*)&huMin, sizeof(mint16));
				file.write((char*)&huMax, sizeof(mint16));
				pointCount = points.count();
				file.write((char*)&pointCount, sizeof(int));
				file.write((char*)points.toStdVector().data(), sizeof(QPointF) *(pointCount));
				pointCount = logPoints.count();
				file.write((char*)&pointCount, sizeof(int));
				file.write((char*)logPoints.toStdVector().data(), sizeof(QPointF) *(pointCount));

				file.close();
			}
		}
	}

	if (tab)
		tab->DeleteResult(m_uid, true);

	m_volumData->setTAState(m_uid, false);

}

bool ActionFeatureDel::mergeWith(const QUndoCommand * command)
{
	return false;
}

