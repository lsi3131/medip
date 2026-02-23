#pragma once


#include "filemanager/export.h"
#include "filemanager/dicom/dicom_defines.h"
#include "filemanager/connection/XmlConnection.h"
#include <string>
#include <vector>
#include <qdom.h>

class QDomNode;
class QDomNodeList;

namespace fm
{
	class XmlConnection;

	struct FavoritesFolder
	{
		int Order;
		QString DirectoryPath;
		QString Name;
	};

	class FM_CORE_EXPORT FolderViewConfig
	{
	public:
		struct MainWindow
		{
			int Width;
			int Height;
			bool Maximized;
		};

		struct Column
		{
			QString ID;
			int Width;
			bool Show;
		};

		struct CenterWindow
		{
			std::vector<Column> ColumnList;
			void SetValue(const wchar_t* id, bool show, int width)
			{
				for (auto& col : ColumnList)
				{
					if (QString::fromStdWString(id) == col.ID)
					{
						col.Show = show;
						col.Width = width;
						break;
					}
				}
			}

			const Column* GetValue(const wchar_t* id) const
			{
				for (auto& col : ColumnList)
				{
					if (QString::fromStdWString(id) == col.ID)
					{
						return &col;
					}
				}
				return nullptr;
			}
		};

		struct SubWindow
		{
			QString ID;
			int Height;
			bool Show;
			SubWindow() {};
			SubWindow(QString id, int height, bool show) :
				ID(id), Height(height), Show(show)
			{}
		};

		struct SubWindowContainer
		{
			std::vector<SubWindow> SubWindowList;
			void SetValue(const wchar_t* id, bool show, int height)
			{
				for (auto& win : SubWindowList)
				{
					if (QString::fromStdWString(id) == win.ID)
					{
						win.Show = show;
						win.Height = height;
						break;
					}
				}
			}

			const SubWindow* GetValue(const wchar_t* id) const
			{
				for (auto& win : SubWindowList)
				{
					if (QString::fromStdWString(id) == win.ID)
					{
						return &win;
					}
				}
				return nullptr;
			}
		};

	public:
		static const wchar_t* FAVORITES_FOLDER_LIST;
		static const wchar_t* FAVORITES_FOLDER;
		static const wchar_t* FAVORITES_FOLDER_ATTR_ORDER;
		static const wchar_t* FAVORITES_FOLDER_ATTR_DIRECTORY;
		static const wchar_t* FAVORITES_FOLDER_ATTR_NAME;

	public:
		FolderViewConfig();
		~FolderViewConfig();

	public:
		bool LoadXml(std::wstring filepath);
		bool SaveXml();
		bool Update();

	public:
		bool AddFavoritesFolderList(std::vector<FavoritesFolder>& favoritesFolder);
		bool ModifyFavoritesFolder(FavoritesFolder& favoritesFolder);
		bool DeleteFavoritesFolderByDirectoryPath(QString dirpath);
		std::vector<FavoritesFolder> GetFavoritesFolderList();

	private:
		bool CreateDefaultElement();
		QDomElement CreateDefaultFavoritesFolderList();

		QDomNode RootNode();
		bool FindRootChildNode(QDomNode& node, std::wstring nodeName);
		bool FindFavoritesFolderNodeByDirectoryPath(QDomNode& foundNode, QString foundDirpath);
		bool IsFavoritesFolderListHasDirectoryPath(QString dirpath);

		void SetNodeText(QDomNode& el, QString data);
		bool SetNodeByFavoritesFolder(QDomElement& el, const FavoritesFolder& data);

		bool GetFavoritesListByNode(std::vector<FavoritesFolder>& outDataList, QDomElement& el);
		bool GetFavoritesByNode(FavoritesFolder& outData, QDomElement& el);

		QDomElement CreateFavoritesFolderNode(FavoritesFolder& data);

		bool saveNewDefaultXmlFile(const std::wstring& filepath) const;
		bool saveNewDefaultXmlFile_And_RetryLoadXml(const std::wstring& filepath);

	private:
		XmlConnection* m_pXmlConnection;
		std::vector<FavoritesFolder> m_favoritesFolderList;
	};
};
