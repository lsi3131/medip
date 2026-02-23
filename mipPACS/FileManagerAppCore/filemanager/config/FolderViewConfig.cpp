#include "stdafx.h"
#include "FolderViewConfig.h"
#include <qdir>

namespace fm
{
	const wchar_t* FolderViewConfig::FAVORITES_FOLDER_LIST = L"FavoritesFolderList";
	const wchar_t* FolderViewConfig::FAVORITES_FOLDER = L"FavoritesFolder";
	const wchar_t* FolderViewConfig::FAVORITES_FOLDER_ATTR_ORDER = L"Order";
	const wchar_t* FolderViewConfig::FAVORITES_FOLDER_ATTR_DIRECTORY = L"DirectoryPath";
	const wchar_t* FolderViewConfig::FAVORITES_FOLDER_ATTR_NAME = L"Name";

	FolderViewConfig::FolderViewConfig()
	{
		m_pXmlConnection = new XmlConnection();
	}

	FolderViewConfig::~FolderViewConfig()
	{
		delete m_pXmlConnection;
	}

	bool FolderViewConfig::LoadXml(std::wstring filepath)
	{
		if (filepath.empty())
		{
			qWarning() << "filepath is empty";
			return false;
		}

		/* Default XML 파일 작성 */
		if (XmlConnection::IsXmlFileExist(filepath) == false)
		{
			if (saveNewDefaultXmlFile(filepath) == false)
			{
				return false;
			}
		}

		if (m_pXmlConnection->LoadXml(filepath) == false)
		{
			qWarning() << Q_FUNC_INFO << "fail to load Xml File.(=" << QString::fromStdWString(filepath) << "). create new default format.";

			if (saveNewDefaultXmlFile_And_RetryLoadXml(filepath) == false)
			{
				return false;
			}
		}

		CreateDefaultElement();
		return Update();
	}

	bool FolderViewConfig::SaveXml()
	{
		return m_pXmlConnection->SaveXml();
	}

	bool FolderViewConfig::AddFavoritesFolderList(std::vector<FavoritesFolder>& favoritesFolder)
	{
		QDomElement listNode;
		if (FindRootChildNode(listNode, FAVORITES_FOLDER_LIST) == false)
		{
			return false;
		}
		for (FavoritesFolder& folder : favoritesFolder)
		{
			if (IsFavoritesFolderListHasDirectoryPath(folder.DirectoryPath) == false)
			{
				folder.Order = 0;

				QDomNode node = CreateFavoritesFolderNode(folder);
				listNode.appendChild(node);
			}
		}
		return m_pXmlConnection->SaveXml();
	}

	bool FolderViewConfig::ModifyFavoritesFolder(FavoritesFolder& favoritesFolder)
	{
		QDomElement listNode;
		if (FindRootChildNode(listNode, FAVORITES_FOLDER_LIST) == false)
		{
			return false;
		}

		QDomElement foundNode;
		if (FindFavoritesFolderNodeByDirectoryPath(foundNode, favoritesFolder.DirectoryPath))
		{
			SetNodeByFavoritesFolder(foundNode, favoritesFolder);
		}

		Update();
		return m_pXmlConnection->SaveXml();
	}

	bool FolderViewConfig::DeleteFavoritesFolderByDirectoryPath(QString dirpath)
	{
		QDomElement listNode;
		if (FindRootChildNode(listNode, FAVORITES_FOLDER_LIST) == false)
		{
			return false;
		}

		QDomNode node;
		if (FindFavoritesFolderNodeByDirectoryPath(node, dirpath) == false)
		{
			return false;
		}

		listNode.removeChild(node);
		Update();
		return m_pXmlConnection->SaveXml();
	}

	std::vector<FavoritesFolder> FolderViewConfig::GetFavoritesFolderList()
	{
		return m_favoritesFolderList;
	}

	bool FolderViewConfig::CreateDefaultElement()
	{
		QDomNode rootNode = RootNode();
		QDomNode node;
		if (FindRootChildNode(node, FAVORITES_FOLDER_LIST) == false)
		{
			rootNode.appendChild(CreateDefaultFavoritesFolderList());
		}

		return m_pXmlConnection->SaveXml();
	}

	QDomElement FolderViewConfig::CreateDefaultFavoritesFolderList()
	{
		QDomElement el = m_pXmlConnection->XmlDoc()->createElement(QString::fromStdWString(FAVORITES_FOLDER_LIST));
		return el;
	}

	QDomNode FolderViewConfig::RootNode()
	{
		return m_pXmlConnection->XmlDoc()->firstChildElement();
	}

	bool FolderViewConfig::FindRootChildNode(QDomNode& node, std::wstring nodeName)
	{
		QDomNode rootNode = RootNode();

		QDomNodeList rootNodeList = rootNode.childNodes();

		for (int i = 0; i < rootNodeList.size(); ++i)
		{
			if (rootNodeList.at(i).nodeName() == QString::fromStdWString(nodeName))
			{
				node = rootNodeList.at(i);
				/* Found */
				return true;
			}
		}

		/* Not Found */
		return false;
	}

	bool FolderViewConfig::FindFavoritesFolderNodeByDirectoryPath(QDomNode& foundNode, QString foundDirpath)
	{
		QDomNode rootNode;
		if (FindRootChildNode(rootNode, FAVORITES_FOLDER_LIST) == false)
		{
			return false;
		}
		QDomNodeList elementList = rootNode.childNodes();

		for (int i = 0; i < elementList.size(); ++i)
		{
			FavoritesFolder folder;
			if (GetFavoritesByNode(folder, elementList.at(i).toElement()))
			{
				QDir dirCurrent(folder.DirectoryPath);
				QDir dirFound(foundDirpath);

				if (dirCurrent == dirFound)
				{
					foundNode = elementList.at(i);
					return true;
				}
			}
		}
		return false;
	}

	bool FolderViewConfig::IsFavoritesFolderListHasDirectoryPath(QString dirpath)
	{
		QDomNode node;
		return FindFavoritesFolderNodeByDirectoryPath(node, dirpath);
	}

	void FolderViewConfig::SetNodeText(QDomNode& el, QString data)
	{
		if (el.hasChildNodes())
		{
			el.firstChild().setNodeValue(data);
		}
		else
		{
			QDomText newData = m_pXmlConnection->XmlDoc()->createTextNode(data);
			el.appendChild(newData);
		}
	}

	bool FolderViewConfig::SetNodeByFavoritesFolder(QDomElement& el, const FavoritesFolder& data)
	{
		el.setAttribute(QString::fromStdWString(FAVORITES_FOLDER_ATTR_ORDER), data.Order);
		el.setAttribute(QString::fromStdWString(FAVORITES_FOLDER_ATTR_DIRECTORY), data.DirectoryPath);
		el.setAttribute(QString::fromStdWString(FAVORITES_FOLDER_ATTR_NAME), data.Name);

		return true;
	}

	bool FolderViewConfig::GetFavoritesListByNode(std::vector<FavoritesFolder>& outDataList, QDomElement& el)
	{
		QDomNodeList elementList = el.childNodes();
		for (int i = 0; i < elementList.size(); ++i)
		{
			QDomElement el = elementList.at(i).toElement();
			FavoritesFolder folder;
			if (GetFavoritesByNode(folder, el))
			{
				outDataList.push_back(folder);
			}
		}

		return !outDataList.empty();
	}

	bool FolderViewConfig::GetFavoritesByNode(FavoritesFolder& outFolder, QDomElement& el)
	{
		QDomNamedNodeMap attributeMap = el.attributes();

		outFolder.Order = attributeMap.namedItem(QString::fromStdWString(FAVORITES_FOLDER_ATTR_ORDER)).nodeValue().toInt();
		outFolder.DirectoryPath = attributeMap.namedItem(QString::fromStdWString(FAVORITES_FOLDER_ATTR_DIRECTORY)).nodeValue();
		outFolder.Name = attributeMap.namedItem(QString::fromStdWString(FAVORITES_FOLDER_ATTR_NAME)).nodeValue();

		return true;
	}

	bool FolderViewConfig::Update()
	{
		m_favoritesFolderList.clear();

		QDomNode rootNode = m_pXmlConnection->XmlDoc()->firstChildElement();
		QDomNodeList configNodeList = rootNode.childNodes();

		for (int i = 0; i < configNodeList.size(); ++i)
		{
			QDomElement el = configNodeList.at(i).toElement();

			if (el.nodeName() == QString::fromStdWString(FAVORITES_FOLDER_LIST))
			{
				GetFavoritesListByNode(m_favoritesFolderList, el);
			}
		}
		return true;
	}

	QDomElement FolderViewConfig::CreateFavoritesFolderNode(FavoritesFolder& data)
	{
		QDomElement el = m_pXmlConnection->XmlDoc()->createElement(QString::fromStdWString(FAVORITES_FOLDER));
		SetNodeByFavoritesFolder(el, data);

		return el;
	}

	bool FolderViewConfig::saveNewDefaultXmlFile(const std::wstring& filepath) const
	{
		const QString defaultXmlData = QString(
			"<FolderViewConfig>\r\n"
			"</FolderViewConfig>");

		if (XmlConnection::CreateNewXml(filepath, defaultXmlData.toStdString()) == false)
		{
			qCritical() << Q_FUNC_INFO << "fail to create XML file." << QString::fromStdWString(filepath);
			return false;
		}

		return true;
	}

	bool FolderViewConfig::saveNewDefaultXmlFile_And_RetryLoadXml(const std::wstring& filepath)
	{
		if (saveNewDefaultXmlFile(filepath) == false)
		{
			return false;
		}

		/* 새로 config를 생성했는데도 불구하고 Load의 실패하면 Fatal Issue 이다. */
		if (m_pXmlConnection->LoadXml(filepath) == false)
		{
			qCritical() << Q_FUNC_INFO << "fail to load new config file. " << QString::fromStdWString(filepath);
			return false;
		}

		return true;
	}
}
