#pragma once

#ifndef CMESHFONT_MANAGER_H
#define CMESHFONT_MANAGER_H

#include <QPixmap>
#include <QChar>
#include <QFont>
#include <QByteArray>
#include <QImage>
#include <QBitArray>

#include "mipEngine/Camera.h"
#include "Renderer/MeshTopology.h"

struct Model3DData
{
	unsigned int m_triangleCount;
	unsigned int m_pointCount;
	unsigned int* m_pointIndices;
	float* m_pointCoords;
};

class Model3D : private Model3DData
{
public:
	Model3D();
	virtual ~Model3D();

public:
	bool IsValid() const;
	void Release();
	Model3D& operator=(const Model3D& modelSrc);
	Model3D& Swap(Model3D& modelSrc);
	void Assign(Model3DData& modelData);
	bool Load(const QString& sFilePath);
	bool Save(const QString& sFilePath) const;

public:
	unsigned int GetTriangleCount() const { return m_triangleCount; };
	unsigned int GetPointCount() const { return m_pointCount; };
	const unsigned int* GetIndexes() const { return m_pointIndices; };
	const float* GetPoints() const { return m_pointCoords; };
};


class	CMesh3DFontManager : public QObject
{
	//Q_OBJECT

public:
	//CMeshFontManager();
	CMesh3DFontManager(const QImage& image, Model3D& model);
	~CMesh3DFontManager();

public:
	static void Create2DImage(QImage& image,
		const QString& sText,
		const QFont& font,
		unsigned short dimension);

	void Create2DFontPlane(
		std::vector<mip::VECTOR3>& _PointXYZ,
		std::vector<float>& outPoint,
		std::vector<int>& outIndex,
		float size = 10.0f
	);

	void GetPixelCount();
	void CreatePixelData();
	void CreateBinaryData(
		std::vector<mip::VECTOR3>& _PointXYZ,
		std::vector<float>& _Point,
		std::vector<int>& _Index,
		float size
	);

	class PixelData
	{
	public:
		enum
		{
			eWrongIndex = 0xFFFFFFFF
		};
		enum ESide
		{
			eLeft,
			eTop,
			eRight,
			eBottom,

			eSideCount
		};
		enum ECorner
		{
			eTopLeft,
			eTopRight,
			eBottomRight,
			eBottomLeft,

			eCornerCount
		};

	public:
		PixelData();
		~PixelData();

	public:
		unsigned int    m_x;
		unsigned int    m_y;
		bool            m_edge[eSideCount];
		unsigned int    m_pointIndex[eCornerCount];
		bool            m_corner[eCornerCount];
	};

private:
	const QImage	m_image;
	Model3D			m_model;
	QBitArray       m_pixelSet;
	unsigned int    m_pixelCount;
	PixelData* m_pixelData;
	unsigned int    m_pointCount;
	QBitArray       m_cornerSet;
	unsigned int    m_cornerCount;
	unsigned int    m_edgeCount;

};


class FontController
{
public:
	FontController();
	virtual ~FontController();

public:
	mip::MeshTopology* pRotateController;

private:
	bool				m_bVisible;
	bool				m_bHover;
	bool				m_bPick;

private:
	std::vector<mip::TVert> m_orgRotCtrlPos;

public:
	void InitOrgData();

	void CreateRotateContorller(mip::MeshTopology* pMT);
	bool MoveCtrlToMeshSurface(
		mip::MeshTopology* pTarget,
		const mip::VECTOR3 pickPoint,
		const int pickFaceId
	);

	bool BackToOrgPos();

#if 0  //Stamp_Rotate_contollor_Test
	bool isPicking(mip::MeshTopology* pTarget, mip::VECTOR3 pickPoint,
		mip::VECTOR3 rayPos, mip::VECTOR3 rayDir, const int pickFaceId, std::vector<mip::VECTOR3>& DrawList);
#else
	bool isPicking(mip::MeshTopology* pTarget, mip::VECTOR3 pickPoint,
		mip::VECTOR3 rayPos, mip::VECTOR3 rayDir, const int pickFaceId, mip::VECTOR3& outPt);
#endif

	void setVisible(bool _bVal) { m_bVisible = _bVal; };
	void setHover(bool _bHover) { m_bHover = _bHover; };
	void setPick(bool _bPick) { m_bPick = _bPick; };
	bool getVisible() { return m_bVisible; };
	bool getHover() { return m_bHover; };
	bool getPick() { return m_bPick; };

};
#endif