#include "stdafx.h"
#include "IconManager.h"
#include <qdir>
#include <qfileinfo>
#include <qtwin>

namespace fm
{
	QIcon IconManager::GetFileIcon(QString filepath)
	{
		QFileInfo fileInfo(filepath);

		QIcon icon;
		//HRESULT result = SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICON | SHGSI_LARGEICON | SHGSI_SMALLICON | SHGFI_OPENICON, &info);
		//HRESULT result = SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICON, &info);
		//HRESULT result = SHGetStockIconInfo(SIID_FOLDEROPEN, SHGSI_ICON, &info);
		//HRESULT result = SHGetStockIconInfo(SIID_FOLDERFRONT, SHGSI_ICON, &info);
		//HRESULT result = SHGetStockIconInfo(SIID_FOLDERBACK, SHGSI_ICON, &info);
		std::wstring wstrFilePath = QDir::toNativeSeparators(filepath).toStdWString();
		DWORD fileAttr = FILE_ATTRIBUTE_DIRECTORY;

		if (fileInfo.isDir())
		{
			fileAttr = FILE_ATTRIBUTE_DIRECTORY;
		}
		else if (fileInfo.isFile())
		{
			fileAttr = FILE_ATTRIBUTE_NORMAL;
		}
		SHFILEINFOW info;
		memset(&info, 0, sizeof(info));
		int result = SHGetFileInfoW(wstrFilePath.c_str(), fileAttr, &info, sizeof(info), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
		if (result != 0)
		{
			QPixmap pmap = QtWin::fromHICON(info.hIcon);
			pmap.save(QDir::currentPath() + "icon.png");
			icon.addPixmap(pmap);
		}
		return icon;
	}
}
