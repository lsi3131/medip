#pragma once

namespace mip
{
	class ShaderMeshData
	{
	public:
		ShaderMeshData() : pickPoint(mip::VECTOR3(0, 0, 0)), radius(0.0f), screenHeight(0), screenWidth(0), wireFrame(0.0f)
		{
			textureIdList.clear();
		}

		void ShaderMeshData::init(mip::VECTOR4& _pickPoint, float _radius)
		{
			this->pickPoint = _pickPoint;
			this->radius = _radius;
		};

		float			getRadius() { return radius; };
		mip::VECTOR3	getPickPt() { return pickPoint; };
		int				getScreenH() { return screenHeight; };
		int				getScreenW() { return screenWidth; };

		void setRadius(float _fVal) { radius = _fVal; };
		void setPickPt(mip::VECTOR3 _vec) { pickPoint = _vec; };
		void setScreenH(int _nVal) { screenHeight = _nVal; };
		void setScreenW(int _nVal) { screenWidth = _nVal; };
		void setWireFrame(float _fVal) { wireFrame = _fVal; };

	public:
		mip::VECTOR3	pickPoint;
		float			radius;
		int				screenHeight;
		int				screenWidth;
		float			wireFrame;
		std::vector<muint32> textureIdList;
	};
};
