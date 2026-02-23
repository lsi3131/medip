#include "stdafx.h"
#include "MeshControl.h"
#include "windowManager.h"
#include "CMeshFontManager.h"
#include "CMeshModelViewManager.h"

#include <QPainter>
#include <QtGlobal>
#include <QRectF>

#define _3D_TEXT_DEPTH   -0.2f

CMesh3DFontManager::CMesh3DFontManager(const QImage& image,
	Model3D& model)
	: m_image(image)
	, m_model(model)
	, m_pixelCount(0)
	, m_pixelData(0)
	, m_pointCount(0)
	, m_cornerCount(0)
	, m_edgeCount(0)
{
}


CMesh3DFontManager::~CMesh3DFontManager()
{
}


/*
@brief
*/
void CMesh3DFontManager::Create2DImage(QImage& image,
	const QString& sText,
	const QFont& font,
	unsigned short dimension)
{
	double dd = static_cast<qreal>(QFontMetrics(font).height());
	double dsd = static_cast<qreal>(dimension);
	double ds3d = font.pointSizeF();

	qreal coeff = (static_cast<qreal>(QFontMetrics(font).height()) / static_cast<qreal>(dimension));
	qreal newFontPointSize = (font.pointSizeF() / coeff);

	QFont fontScaled(font);
	fontScaled.setPointSizeF(newFontPointSize);

	int width = (3 + QFontMetrics(fontScaled).boundingRect(sText).width());

	QImage imageOutput(width, dimension, QImage::Format_Mono);
	imageOutput.fill((uint)1);
	{
		QPainter painter(&imageOutput);
		painter.setFont(fontScaled);
		painter.setPen(QColor(Qt::black));

		const QRect rectDraw(
			QPoint(0, 0), QSize(width, dimension));
		painter.drawText(rectDraw, (Qt::AlignLeft | Qt::AlignTop), sText);
	}

	image = imageOutput;
}

void CMesh3DFontManager::Create2DFontPlane(
	std::vector<mip::VECTOR3>& _PointXYZ,
	std::vector<float>& outPoint,
	std::vector<int>& outIndex,
	float size
)
{
	GetPixelCount();
	CreatePixelData();
	CreateBinaryData(_PointXYZ, outPoint, outIndex, size);
}


void CMesh3DFontManager::GetPixelCount()
{
	m_pixelSet.fill(false, m_image.height() * m_image.width());
	m_pixelCount = 0;
	unsigned int pixel = 0;
	for (int y = 0; y < m_image.height(); ++y)
	{
		const uchar* dataPtr = m_image.constScanLine(y);
		uchar data = 0;
		for (int x = 0; x < m_image.width(); ++x, ++pixel)
		{
			if (0 == (x % (sizeof(uchar) * 8)))
			{
				data = (*dataPtr);
				++dataPtr;
			}
			else
			{
				data = (data << 1);
			}
			if (0 == (data & 0x80))
			{
				m_pixelSet.setBit(pixel);
				++m_pixelCount;
			}
		}
	}
}

void CMesh3DFontManager::CreatePixelData()
{
	delete[] m_pixelData;
	m_pixelData = new PixelData[m_pixelCount];

	const int h = m_image.height();
	const int w = m_image.width();

	unsigned int pixel = 0;
	unsigned int pixelIndex = 0;
	QVector<unsigned int> pointIndexesTop((1 + w), PixelData::eWrongIndex);
	m_pointCount = 0;
	for (int y = 0; y < h; ++y)
	{
		QVector<unsigned int> pointIndexesBottom((1 + w), PixelData::eWrongIndex);
		for (int x = 0; x < w; ++x, ++pixel)
		{
			if (m_pixelSet.testBit(pixel))
			{
				PixelData& pixelData = m_pixelData[pixelIndex++];
				pixelData.m_edge[PixelData::eLeft] =
					((0 == x) ? true : !m_pixelSet.testBit(w * y + x - 1));
				pixelData.m_edge[PixelData::eTop] =
					((0 == y) ? true : !m_pixelSet.testBit(w * (y - 1) + x));
				pixelData.m_edge[PixelData::eRight] =
					((w == (1 + x)) ? true : !m_pixelSet.testBit(w * y + x + 1));
				pixelData.m_edge[PixelData::eBottom] =
					((h == (1 + y)) ? true : !m_pixelSet.testBit(w * (y + 1) + x));
				pixelData.m_x = x;
				pixelData.m_y = y;

				pixelData.m_corner[PixelData::eTopLeft] = (pixelData.m_edge[PixelData::eLeft] || pixelData.m_edge[PixelData::eTop]);
				pixelData.m_corner[PixelData::eTopRight] = (pixelData.m_edge[PixelData::eRight] || pixelData.m_edge[PixelData::eTop]);
				pixelData.m_corner[PixelData::eBottomRight] = (pixelData.m_edge[PixelData::eRight] || pixelData.m_edge[PixelData::eBottom]);
				pixelData.m_corner[PixelData::eBottomLeft] = (pixelData.m_edge[PixelData::eLeft] || pixelData.m_edge[PixelData::eBottom]);

				pixelData.m_pointIndex[PixelData::eTopLeft] = ((PixelData::eWrongIndex != pointIndexesTop[x])
					? pointIndexesTop[x]
					: (m_pointCount++));

				pixelData.m_pointIndex[PixelData::eTopRight] = ((PixelData::eWrongIndex != pointIndexesTop[1 + x])
					? pointIndexesTop[1 + x]
					: (m_pointCount++));
				pointIndexesTop[1 + x] = pixelData.m_pointIndex[PixelData::eTopRight];

				pixelData.m_pointIndex[PixelData::eBottomLeft] = ((PixelData::eWrongIndex != pointIndexesBottom[x])
					? pointIndexesBottom[x]
					: (m_pointCount++));
				pointIndexesBottom[x] = pixelData.m_pointIndex[PixelData::eBottomLeft];

				pixelData.m_pointIndex[PixelData::eBottomRight] =
					(m_pointCount++);
				pointIndexesBottom[1 + x] = pixelData.m_pointIndex[PixelData::eBottomRight];
			}
		}

		pointIndexesTop = pointIndexesBottom;
	}

	m_cornerSet.fill(false, m_pointCount);
	m_edgeCount = 0;
	for (unsigned int pixelIndex = 0; pixelIndex < m_pixelCount; ++pixelIndex)
	{
		const PixelData& pixelData = m_pixelData[pixelIndex];

		for (unsigned int corner = 0; corner < PixelData::eCornerCount; ++corner)
		{
			if (pixelData.m_corner[corner])
			{
				m_cornerSet.setBit(pixelData.m_pointIndex[corner]);
			}
		}

		for (unsigned int side = 0; side < PixelData::eSideCount; ++side)
		{
			if (pixelData.m_edge[side])
			{
				++m_edgeCount;
			}
		}
	}

	m_cornerCount = m_cornerSet.count(true);
}

void CMesh3DFontManager::CreateBinaryData(
	std::vector<mip::VECTOR3>& out_PointXYZ,
	std::vector<float>& outPoint,
	std::vector<int>& outIndex,
	float size)
{
	Model3DData modelData =
	{
		(2 * ((2 * m_pixelCount) + m_edgeCount)),
		(2 * m_pointCount),
		0,
		new float[2 * 3 * m_pointCount]
	};
	//PHS
	std::vector<mip::VECTOR3> _PointXYZ; _PointXYZ.resize(m_pointCount, mip::VECTOR3(-1, -1, -1));
	modelData.m_pointIndices = new unsigned int[3 * modelData.m_triangleCount];

	float offset = 0.1f;
	double stepOne = (1.0 / static_cast<double>(m_image.height()));
	stepOne = stepOne * (size * offset);
	printf_s("\n m_image.height() - %d / stepOne - %f \n", m_image.height(), stepOne);

	unsigned int* pointIndices = modelData.m_pointIndices;
	for (unsigned int pixelIndex = 0; pixelIndex < m_pixelCount; ++pixelIndex)
	{
		const PixelData& pixelData = m_pixelData[pixelIndex];

		(*pointIndices) = pixelData.m_pointIndex[PixelData::eBottomLeft];
		pointIndices++;
		(*pointIndices) = pixelData.m_pointIndex[PixelData::eTopLeft];
		pointIndices++;
		(*pointIndices) = pixelData.m_pointIndex[PixelData::eTopRight];
		pointIndices++;

		(*pointIndices) = pixelData.m_pointIndex[PixelData::eTopRight];
		pointIndices++;
		(*pointIndices) = pixelData.m_pointIndex[PixelData::eBottomRight];
		pointIndices++;
		(*pointIndices) = pixelData.m_pointIndex[PixelData::eBottomLeft];
		pointIndices++;

		for (unsigned int corner = 0; corner < PixelData::eCornerCount; ++corner)
		{
			bool right = ((PixelData::eTopRight == corner) || (PixelData::eBottomRight == corner));
			bool bottom = ((PixelData::eBottomLeft == corner) || (PixelData::eBottomRight == corner));
			float* buffer = modelData.m_pointCoords + (3 * pixelData.m_pointIndex[corner]);
			buffer[0] = static_cast<float>(stepOne * (pixelData.m_x + (right ? 1 : 0)));
			buffer[1] = static_cast<float>(stepOne * (m_image.height() - (pixelData.m_y + (bottom ? 1 : 0))));
			buffer[2] = 0.0f;

			//PHS
			int index = pixelData.m_pointIndex[corner];
			if (_PointXYZ[index].x < 0)
			{
				_PointXYZ[index].x = buffer[0];
				_PointXYZ[index].y = buffer[1];
				_PointXYZ[index].z = buffer[2];
			}
		}
	}

	Q_ASSERT(pointIndices == modelData.m_pointIndices + 6 * m_pixelCount);

	//2021_11_05_byPHS 2D font Vertex and Index
	std::vector<float> _Point(modelData.m_pointCoords, (modelData.m_pointCoords + 3 * m_pointCount));
	std::vector<int> _Index(modelData.m_pointIndices, modelData.m_pointIndices + (6 * m_pixelCount));
	out_PointXYZ.swap(_PointXYZ);
	outPoint.swap(_Point);
	outIndex.swap(_Index);

	return;

	unsigned int* pointIndicesSrc = modelData.m_pointIndices;
	unsigned int* pointIndicesDest = (modelData.m_pointIndices + 6 * m_pixelCount);
	for (unsigned int pointIndex = 0; pointIndex < (m_pixelCount * 6); ++pointIndex)
	{
		(*(pointIndicesDest++)) = (*(pointIndicesSrc++)) + m_pointCount;
	}

	const float* pointCoordSrc = modelData.m_pointCoords;
	float* pointCoordDest = (modelData.m_pointCoords + 3 * m_pointCount);
	for (unsigned int point = 0; point < m_pointCount; ++point)
	{
		(*(pointCoordDest++)) = (*(pointCoordSrc++));
		(*(pointCoordDest++)) = (*(pointCoordSrc++));
		(*(pointCoordDest++)) = _3D_TEXT_DEPTH;
		++pointCoordSrc;
	}

	unsigned int edgeIndex = 0;
	unsigned int* point = (modelData.m_pointIndices + (3 * 2 * 2 * m_pixelCount));
	for (unsigned int pixelIndex = 0; pixelIndex < m_pixelCount; ++pixelIndex)
	{
		const PixelData& pixelData = m_pixelData[pixelIndex];
		for (unsigned int side = 0; side < PixelData::eSideCount; ++side)
		{
			if (pixelData.m_edge[side])
			{
				PixelData::ECorner corners[2] =
				{
					static_cast<PixelData::ECorner>(side),
					((side == PixelData::eLeft)
					? PixelData::eBottomLeft
						: static_cast<PixelData::ECorner>(side - 1))
				};
				unsigned int pointIndexes[4] =
				{
					pixelData.m_pointIndex[corners[0]],
					pixelData.m_pointIndex[corners[1]],
					pixelData.m_pointIndex[corners[1]] + m_pointCount,
					pixelData.m_pointIndex[corners[0]] + m_pointCount
				};
				(*(point++)) = pointIndexes[0];
				(*(point++)) = pointIndexes[1];
				(*(point++)) = pointIndexes[2];
				(*(point++)) = pointIndexes[2];
				(*(point++)) = pointIndexes[3];
				(*(point++)) = pointIndexes[0];

				++edgeIndex;
			}
		}
	}

	std::vector<int> destIndex(modelData.m_pointIndices, modelData.m_pointIndices + (3 * 2 * 2 * m_pixelCount));

	std::vector<float> destPoint(modelData.m_pointCoords, (modelData.m_pointCoords + 3 * m_pointCount));

	Q_ASSERT(edgeIndex == m_edgeCount);

	m_model.Assign(modelData);
}


CMesh3DFontManager::PixelData::PixelData()
	: m_x(0)
	, m_y(0)
{
	for (unsigned int side = 0; side < eSideCount; ++side)
	{
		m_edge[side] = false;
	}
	for (unsigned int corner = 0; corner < eCornerCount; ++corner)
	{
		m_pointIndex[corner] = eWrongIndex;
		m_corner[corner] = true;
	}
}

CMesh3DFontManager::PixelData::~PixelData()
{
}

//

Model3D::Model3D()
{
	m_triangleCount = 0;
	m_pointCount = 0;
	m_pointIndices = 0;
	m_pointCoords = 0;
}

Model3D::~Model3D()
{
	Release();
}

bool Model3D::IsValid() const
{
	return ((0 != m_triangleCount) &&
		(0 != m_pointCount) &&
		(0 != m_pointIndices) &&
		(0 != m_pointCoords));
}

void Model3D::Release()
{
	m_triangleCount = 0;
	m_pointCount = 0;

	delete[] m_pointIndices;
	m_pointIndices = 0;
	delete[] m_pointCoords;
	m_pointCoords = 0;
}

Model3D& Model3D::operator=(const Model3D& modelSrc)
{
	Release();

	m_pointIndices = new unsigned int[3 * m_triangleCount];
	m_pointCoords = new float[3 * m_pointCount];

	if ((0 == m_pointIndices) || (0 == m_pointCoords))
	{
		Release();
		return (*this);
	}

	memcpy(m_pointIndices, modelSrc.m_pointIndices,
		(sizeof(unsigned int) * 3 * m_triangleCount));
	memcpy(m_pointCoords, modelSrc.m_pointCoords,
		(sizeof(float) * 3 * m_pointCount));
	return (*this);
}

Model3D& Model3D::Swap(Model3D& modelSrc)
{
	unsigned int triangleCount = m_triangleCount;
	unsigned int pointCount = m_pointCount;
	unsigned int* pointIndices = m_pointIndices;
	float* pointCoords = m_pointCoords;

	m_triangleCount = modelSrc.m_triangleCount;
	m_pointCount = modelSrc.m_pointCount;
	m_pointIndices = modelSrc.m_pointIndices;
	m_pointCoords = modelSrc.m_pointCoords;

	modelSrc.m_triangleCount = triangleCount;
	modelSrc.m_pointCount = pointCount;
	modelSrc.m_pointIndices = pointIndices;
	modelSrc.m_pointCoords = pointCoords;

	return (*this);
}

void Model3D::Assign(Model3DData& modelData)
{
	Release();

	(*dynamic_cast<Model3DData*>(this)) = modelData;

	modelData.m_triangleCount = 0;
	modelData.m_pointCount = 0;
	modelData.m_pointIndices = 0;
	modelData.m_pointCoords = 0;
}

bool Model3D::Load(const QString& sFilePath)
{
	Release();

	QFile file(sFilePath);
	if (file.open(QIODevice::ReadOnly))
	{
		if (file.size() > 2 * sizeof(unsigned int))
		{
			file.read(
				static_cast<char*>(static_cast<void*>(&m_triangleCount)),
				sizeof(unsigned int));
			file.read(
				static_cast<char*>(static_cast<void*>(&m_pointCount)),
				sizeof(unsigned int));
			if (file.size() == ((2 * sizeof(unsigned int)) +
				sizeof(unsigned int) * m_triangleCount * 3 +
				sizeof(float) * m_pointCount * 3))
			{
				m_pointIndices = new unsigned int[m_triangleCount * 3];
				file.read(
					static_cast<char*>(static_cast<void*>(m_pointIndices)),
					sizeof(unsigned int) * m_triangleCount * 3);
				m_pointCoords = new float[m_pointCount * 3];
				file.read(
					static_cast<char*>(static_cast<void*>(m_pointCoords)),
					sizeof(float) * m_pointCount * 3);

				file.close();

				std::vector<int> destIndex(m_pointIndices, m_pointIndices + (m_triangleCount * 3));
				std::vector<float> destPoint(m_pointCoords, m_pointCoords + (3 * m_pointCount));
				return true;
			}
		}
		file.close();
	}

	return false;
}

bool Model3D::Save(const QString& sFilePath) const
{
	QFile file(sFilePath);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(
			static_cast<const char*>(static_cast<const void*>(&m_triangleCount)),
			sizeof(unsigned int));
		file.write(
			static_cast<const char*>(static_cast<const void*>(&m_pointCount)),
			sizeof(unsigned int));
		file.write(
			static_cast<const char*>(static_cast<const void*>(m_pointIndices)),
			sizeof(unsigned int) * m_triangleCount * 3);
		file.write(
			static_cast<const char*>(static_cast<const void*>(m_pointCoords)),
			sizeof(float) * m_pointCount * 3);

		file.close();
		return true;
	}

	return false;
}

FontController::FontController()
	: pRotateController(nullptr)
	, m_bVisible(false)
	, m_bHover(false)
	, m_bPick(false)
{
}

FontController::~FontController()
{
	//SAFE_DELETES(pRotateController);	
}

void FontController::InitOrgData()
{
	std::vector<mip::TVert> trash;
	m_orgRotCtrlPos.swap(trash);
}

void FontController::CreateRotateContorller(mip::MeshTopology* pMT)
{
	if (pMT == nullptr) return;

	std::vector<mip::TTri>			m_ttris;
	std::vector<mip::TVert>			m_tverts;
	std::vector<mip::THEdge>		m_tVHedges;

	std::vector<muint32>			trash_tris;
	std::vector<mip::VECTOR3>		trash_vert;
	std::vector<mip::VECTOR3>		trash_Normals;

	pMT->m_ttris.swap(m_ttris);
	pMT->m_tverts.swap(m_tverts);
	pMT->m_tVHedges.swap(m_tVHedges);

	pMT->m_tris.swap(trash_tris);
	pMT->m_verts.swap(trash_vert);
	pMT->m_normals.swap(trash_Normals);

	int RotCtrlCnt = 108;
	pMT->m_tris.resize(RotCtrlCnt, 0);
	pMT->m_verts.reserve(RotCtrlCnt);
	pMT->m_normals.resize(RotCtrlCnt, mip::VECTOR3(0.00000000, 0.00000000, -1.00000000));

	for (int ii = 0; ii < RotCtrlCnt; ii++)
	{
		pMT->m_tris[ii] = ii;
	}

	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0527856834, -0.00930754002, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0535999909, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0535999909, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0527856834, 0.00930754002, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0527856834, 0.00930754002, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0503675155, 0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0503675155, 0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0464189537, 0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0464189537, 0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0410599746, 0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0410599746, 0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0344534107, 0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0344534107, 0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0267999955, 0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0267999955, 0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0183322784, 0.0503675155, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0183322784, 0.0503675155, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00930754002, 0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00930754002, 0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-9.84615831e-18, 0.0535999909, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-9.84615831e-18, 0.0535999909, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.00930754002, 0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.00930754002, 0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0183322784, 0.0503675155, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0183322784, 0.0503675155, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0267999955, 0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0267999955, 0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0344534107, 0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0344534107, 0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0410599746, 0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0410599746, 0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0464189537, 0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0464189537, 0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0503675155, 0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0503675155, 0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0527856834, 0.00930754002, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0527856834, 0.00930754002, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0535999909, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0535999909, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0527856834, -0.00930754002, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0527856834, -0.00930754002, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0503675155, -0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0503675155, -0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0464189537, -0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0464189537, -0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0410599746, -0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0410599746, -0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0344534107, -0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0344534107, -0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0267999955, -0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0267999955, -0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0183322784, -0.0503675230, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.0183322784, -0.0503675230, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.00930754002, -0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(-0.00930754002, -0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(3.28205291e-18, -0.0535999797, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(3.28205291e-18, -0.0535999797, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00930754002, -0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00930754002, -0.0527856834, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0183322784, -0.0503675230, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0183322784, -0.0503675230, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0267999955, -0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0267999955, -0.0464189537, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0344534107, -0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0344534107, -0.0410599709, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0410599746, -0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0410599746, -0.0344534181, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0464189537, -0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0464189537, -0.0267999899, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0503675155, -0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.00000000, -0.00000000, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0503675155, -0.0183322784, 0.00000000));
	pMT->m_verts.push_back(mip::VECTOR3(0.0527856834, -0.00930754002, 0.00000000));

	mip::VECTOR4 color(1, 255, 1, 80);
	pMT->mergingVertex();
	pMT->buildTopologyHEdge();
	pMT->updateVertex();

	//mip::mesh_control::SubDivision(pMT);

	pMT->buildTree();

	//m_tree = new OcTree(mip::OCTREE, m_tris.size() / 100, m_boundingBox.xLength() / 6);

	//m_tree->buildTree(&m_tris, &m_verts, m_boundingBox);
	//pMT->updateColor(color);
	pMT->m_baseColor = color;
	pMT->enableAlpha(true);
	pMT->initShader(mip::SHADERTYPE::SHADER_BASIC);

	m_orgRotCtrlPos.assign(pMT->m_tverts.size(), mip::VECTOR3(0, 0, 0));
	std::copy(pMT->m_tverts.begin(), pMT->m_tverts.end(), m_orgRotCtrlPos.begin());


	pMT->setAlpha(80.0f);
}


bool FontController::MoveCtrlToMeshSurface(
	mip::MeshTopology* pTarget,
	const mip::VECTOR3 pickPoint,
	const int pickFaceId
)
{
	if (pTarget == nullptr || pRotateController == nullptr || pickFaceId < 0)
	{
		return false;
	}

	//rotate
	int v0 = pTarget->m_ttris[pickFaceId].vi[0];
	int v1 = pTarget->m_ttris[pickFaceId].vi[1];
	int v2 = pTarget->m_ttris[pickFaceId].vi[2];

	mip::VECTOR3 pickFaceNormal(0, 0, 0);
	pickFaceNormal = (pTarget->m_tverts[v1].pos - pTarget->m_tverts[v0].pos).cross((pTarget->m_tverts[v2].pos - pTarget->m_tverts[v0].pos));
	pickFaceNormal.normalize();
	mip::VECTOR3 _pickPoint = pickPoint;
	mip::VECTOR3 vDest = pickFaceNormal; mip::VECTOR3 vStart = mip::VECTOR3(0, 0, 1); //font 노멀		


	//vStart에서 vDest로 향하는 쿼터니언 생성
	const mip::QUATERNION quat = mip::mesh_control::RotationBetween_Vectors(vStart, vDest);
	for (int ii = 0; ii < pRotateController->m_tverts.size(); ii++)
	{
		pRotateController->m_tverts[ii].pos = quat * pRotateController->m_tverts[ii].pos;
	}

	//translate
	const mip::VECTOR3 tranVector = pickFaceNormal * 0.001;
	for (int ii = 0; ii < pRotateController->m_tverts.size(); ii++)
	{
		pRotateController->m_tverts[ii].pos = pRotateController->m_tverts[ii].pos + tranVector;
	}

	pRotateController->updateVertex();

	WIN_MANAGER->makeCurrent();
	pRotateController->buildRenderBufferTopology();
	WIN_MANAGER->doneCurrent();

	return true;
}

bool FontController::BackToOrgPos()
{
	if (m_orgRotCtrlPos.size() != pRotateController->m_tverts.size()) return false;

	for (int ii = 0; ii < pRotateController->m_tverts.size(); ii++)
	{
		pRotateController->m_tverts[ii].pos = m_orgRotCtrlPos[ii].pos;
	}
	return true;
}

#if 0 //Stamp_Rotate_contollor_Test
bool FontController::isPicking(mip::MeshTopology* pTarget, mip::VECTOR3 pickPoint,
	mip::VECTOR3 rayPos, mip::VECTOR3 rayDir, const int pickFaceId, std::vector<mip::VECTOR3>& DrawList)
#else
bool FontController::isPicking(mip::MeshTopology* pTarget, mip::VECTOR3 pickPoint,
	mip::VECTOR3 rayPos, mip::VECTOR3 rayDir, const int pickFaceId, mip::VECTOR3& outPt)
#endif

{
	if (pTarget == nullptr || pRotateController == nullptr || pickFaceId < 0) return false;
	BackToOrgPos();

	//rotate
	int v0 = pTarget->m_ttris[pickFaceId].vi[0];
	int v1 = pTarget->m_ttris[pickFaceId].vi[1];
	int v2 = pTarget->m_ttris[pickFaceId].vi[2];

	mip::VECTOR3 pickFaceNormal(0, 0, 0);
	pickFaceNormal = (pTarget->m_tverts[v1].pos - pTarget->m_tverts[v0].pos).cross((pTarget->m_tverts[v2].pos - pTarget->m_tverts[v0].pos));
	pickFaceNormal.normalize();
	mip::VECTOR3 _pickPoint = pickPoint;
	mip::VECTOR3 vDest = pickFaceNormal; mip::VECTOR3 vStart = mip::VECTOR3(0, 0, 1); //font 노멀

	//vStart에서 vDest로 향하는 쿼터니언 생성
	const mip::QUATERNION quat = mip::mesh_control::RotationBetween_Vectors(vStart, vDest);

	bool b_clicked = false;
	mip::MeshTopology* pMT = pRotateController;
	mip::VECTOR3 offset = pickPoint;

#if 1
	for (int ii = 0; ii < pMT->m_ttris.size(); ii++)
	{
		int v0 = pMT->m_ttris[ii].vi[0];
		int v1 = pMT->m_ttris[ii].vi[1];
		int v2 = pMT->m_ttris[ii].vi[2];

		mip::VECTOR3 vec0 = pMT->m_tverts[v0].pos;
		mip::VECTOR3 vec1 = pMT->m_tverts[v1].pos;
		mip::VECTOR3 vec2 = pMT->m_tverts[v2].pos;

		vec0 = quat * vec0;
		vec1 = quat * vec1;
		vec2 = quat * vec2;

		vec0 = vec0 + offset;
		vec1 = vec1 + offset;
		vec2 = vec2 + offset;

#if 0
		DrawList.push_back(vec0);
		DrawList.push_back(vec1);
		DrawList.push_back(vec1);
		DrawList.push_back(vec2);
		DrawList.push_back(vec2);
		DrawList.push_back(vec0);
#endif

		rayDir.normalize();
		mip::VECTOR3 out;
		if (mip::mesh_control::getPickPtOfPlane(vec0, vec1, vec2, rayPos, rayDir, out))
		{
			outPt = out;
			b_clicked = true;
			return true;
		}
	}
#endif 
	return b_clicked;
}
