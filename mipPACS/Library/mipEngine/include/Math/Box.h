#pragma once
#include "mip/core.h"

namespace mip
{
	class MIP_LIB AARBB
	{
	public:
		mip::VECTOR3		center;
		float					size;
	public:
		AARBB(){ size = 0.0f; center.x = 0.0f; center.y = 0.0f; center.z = 0.0f; }
		AARBB(float x, float y, float z, float length){ set(x, y, z, length); }
		void set(float x, float y, float z, float length)
		{
			center.x = x; center.y = y; center.z = z;
			size = length;
		}
		bool checkPoint(const VECTOR3 & v)
		{
			float hsize = size * 0.5f;
			if (v.x < (center.x - hsize)) return false;
			if (v.x > (center.x + hsize)) return false;
			if (v.y < (center.y - hsize)) return false;
			if (v.y > (center.y + hsize)) return false;
			if (v.z < (center.z - hsize)) return false;
			if (v.z > (center.z + hsize)) return false;

			return true;
		}
	};
	
	class MIP_LIB AABB
	{
	public:
		enum {
			POINT_MAX = 8,
		};
	public:
		mip::VECTOR3		min;
		mip::VECTOR3		max;
	public:
		AABB() { reset(); };
		AABB(const AABB & aabb) { min = aabb.min; max = aabb.max; }
		AABB(const AARBB & aarbb) { set(aarbb); }
		AABB(float x, float y, float z, VECTOR3 & length) {	set(x,y,z,length);}
		void set(const AARBB & aarbb)
		{
			min.x = aarbb.center.x - aarbb.size*0.5f; min.y = aarbb.center.y - aarbb.size*0.5f; min.z = aarbb.center.z - aarbb.size*0.5f;
			max.x = aarbb.center.x + aarbb.size*0.5f; max.y = aarbb.center.y + aarbb.size*0.5f; max.z = aarbb.center.z + aarbb.size*0.5f;
		}
		void set(const AABB &aabb) {
			min = aabb.min;
			max = aabb.max;
		}
		void set(float x, float y, float z, VECTOR3 & length)
		{
			min.x = x - length.x*0.5f; min.y = y - length.y*0.5f; min.z = z - length.z*0.5f;
			max.x = x + length.x*0.5f; max.y = y + length.y*0.5f; max.z = z + length.z*0.5f;
		}

		AABB& transform(const MATRIX44& mat);
		AABB& transform(const QUATERNION& q, const VECTOR3& v);
		AABB toTransform(const MATRIX44& mat) const;
		AABB toTransform(const QUATERNION& q, const VECTOR3& v) const;

		float xLength() const { return abs(max.x - min.x); }
		float yLength() const { return abs(max.y - min.y); }
		float zLength() const { return abs(max.z - min.z); }
		float xDim() const { return (max.x - min.x); }
		float yDim() const { return (max.y - min.y); }
		float zDim() const { return (max.z - min.z); }
		bool isNull() const { return (min.x == 0.0f && max.x == 0 && min.y == 0 && max.y == 0 && min.z == 0 && max.z == 0) ? true : false; }
		VECTOR3 getSize() const
		{
			return (max - min);
		}
		SPHERE getSphere() const
		{
			return SPHERE(getCenter(), getExtent().length());
		}
		VECTOR3 getCenter() const
		{
			return (min + max) * 0.5f;
		}
		VECTOR3 getExtent() const
		{
			return 0.5f * (max - min);
		}
		float getVolume() const
		{
			return ((max.x - min.x) * (max.y - min.y) * (max.z - min.z));
		}
		bool checkInside(const VECTOR3 & v) const
		{
			return ((v.x > min.x) && (v.x < max.x) && (v.y > min.y) && (v.y < max.y) && (v.z > min.z) && (v.z < max.z));
		}
		bool checkInside(const AABB & box) const
		{
			return (checkInside(box.min) && checkInside(box.max));
		}
		VECTOR3 point(const int i) const 
		{
			return VECTOR3(
				min.x + (i % 2) * xDim(),
				min.y + ((i / 2) % 2) * yDim(),
				min.z + (i>3)* zDim()
				);
		}
		void reset()
		{
			min = VECTOR3::Zero;
			max = VECTOR3::Zero;
		}
		bool checkPoint(const VECTOR3 & v) const
		{
			if (v.x >= min.x && v.x <= max.x &&
				v.y >= min.y && v.y <= max.y &&
				v.z >= min.z && v.z <= max.z)
				return true;
			else
				return false;
		}
		void expand(float w)
		{
			min -= VECTOR3(w, w, w);
			max += VECTOR3(w, w, w);
		}
		void expand(const VECTOR3 &e)
		{
			min -= e;
			max += e;
		}
		void shift(const VECTOR3 &offset)
		{
			min += offset;
			max += offset;
		}
		void moveto(const VECTOR3 &dest)
		{
			const VECTOR3 Offset = dest - getCenter();
			min += Offset;
			max += Offset;
		}

		VECTOR3 getClosestPoint(const VECTOR3 & Point) const;

		bool getPlaneList(std::vector<PLANE> & planeList);
		bool getLineList(std::vector<VECTOR3> & lineList);
	};

	class MIP_LIB OBB_CONV
	{
	public:
		VECTOR3					verts[8];
		PLANE					planes[6];
	private:
		bool					bPlane;

	public:
		static unsigned short tri_count;
		static unsigned short indeces[];

	public:
		OBB_CONV();
		OBB_CONV(const AABB & box);
		OBB_CONV(const AABB & box, const MATRIX44 & mat);
		void set(const AABB & box);
		void set(const AABB & box, const MATRIX44 & mat);
		void set(const VECTOR3 &center, float size);
		void set(const VECTOR3 &center, const VECTOR3 &size);
		void setProjMinMax(const VECTOR3& proj_min, const VECTOR3& proj_max);

		void transform(const MATRIX44& mat);
		void transform(const QUATERNION& q);
		void buildPlanes();
		bool isPlanes()			{ return bPlane; }
		void clearPlane()		{ bPlane = true; }
		void reset();

		bool getLineList(std::vector<VECTOR3> & lineList);
	};

	class MIP_LIB RECTi
	{
	public:
		muint32  left;
		muint32  top;
		muint32  right;
		muint32  bottom;

		RECTi()
		{
			top = 0;
			bottom = 0;
			left = 0;
			right = 0;
		}
		RECTi(muint32 _left, muint32 _top, muint32 _right, muint32 _bottom)
		{
			top = _top;
			bottom = _bottom;
			left = _left;
			right = _right;
		}
	};
};

