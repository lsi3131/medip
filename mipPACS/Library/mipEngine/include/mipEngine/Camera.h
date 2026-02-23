#pragma once
#include "math/math.h"

namespace mip
{
	//class DCAMERA : public TRANSFORM
	//{
	//protected:

	//public:
	//	DCAMERA()
	//	{
	//		
	//	}
	//	~DCAMERA() {}
	//};

	enum VIEWPORT_TYPE
	{
		VT_FRONT,
		VT_BACK,
		VT_LEFT,
		VT_RIGHT,
		VT_TOP,
		VT_BOTTOM,
	};

	class SCAMERA
	{
	protected:
		VECTOR3			m_vPos;
		VECTOR3			m_vAt;
		VECTOR3         m_vUp;
		float			m_fZoom;
		float			m_fZoomMax;
		float			m_near;
		float			m_far;
		int				m_screenCX;
		int				m_screenCY;

		int				m_preScreenX;
		int				m_preScreenY;
		int				m_currentScreenX;
		int				m_currentScreenY;

		MATRIX44		m_matTarget;
		MATRIX44		m_matView;
		MATRIX44		m_matProj;
		PLANE			m_Planes[6];

		bool			needUpdate;
		bool			needUpdateProj;
	public:
		SCAMERA()
		{
			needUpdate = true;
			needUpdateProj = true;
			m_matTarget = MATRIX44::Identity;
			m_matView = MATRIX44::Identity;
			m_matProj = MATRIX44::Identity;

			m_vPos = VECTOR3::Zero;
			m_vAt = VECTOR3::Zero;
			m_vUp = VECTOR3(0, 1, 0);
			m_fZoom = 30.0f;
			m_fZoomMax = 100.0f;
			m_near = 0.01f;
			m_far = 200.0f;

			m_screenCX = 0;
			m_screenCY = 0;

			m_preScreenX = 0;
			m_preScreenY = 0;
			m_currentScreenX = 0;
			m_currentScreenY = 0;
		}
		~SCAMERA() {}

		bool isNeedUpdate() { return needUpdate; }
		bool isNeedUpdateProj() { return needUpdateProj; }

		MATRIX44 getTargetView() { return (m_matView * m_matTarget); }
		MATRIX44 getView() const { return m_matView; }
		MATRIX44 getProj() const { return m_matProj; }

		void setTarget(MATRIX44 & mat) { m_matTarget = mat; }

		void setMovePosition(float x, float y, float z)
		{
			needUpdate = true;
			m_matView._41 = x; m_matView._42 = y; m_matView._43 = z;
		}
		void SetRotatePosition(float x, float y, float z)
		{
			needUpdate = true;
			VECTOR3 v(m_matView._41, m_matView._42, m_matView._43);
			m_matView.set(y, x, z, v);
		}

		void wheelZoom(float dt);

		void setPos(const VECTOR3 & pos)	{ m_vPos = pos; needUpdate = true;}
		void addPos(const VECTOR3 & add)	{ m_vPos += add; needUpdate = true;}
		VECTOR3 getPos()					{ return m_vPos;}
		void setAt(const VECTOR3 & at)		{ m_vAt = at; needUpdate = true; }
		void setUp(const VECTOR3 & up)		{ m_vUp = up; needUpdate = true; }
		void setNear(float fnear)			{ m_near = fnear; needUpdateProj = true;}
		void setFar(float ffar)				{ m_far = ffar; needUpdateProj = true;}
		void setZoom(float zoom)			{ m_fZoom = zoom; needUpdate = true;}
		void setZoomMax(float zoomMax)		{ m_fZoomMax = zoomMax; needUpdate = true;}
		void setNearFar(float fnear, float ffar)	{ m_near = fnear;  m_far = ffar; needUpdateProj = true;}
		void setScreenSize(int cx, int cy) { m_screenCX = cx; m_screenCY = cy; needUpdateProj = true;}
		void setScreenXY(int x, int y)		{ m_currentScreenX = x, m_currentScreenY = y; }
		void setPreScreenXY(int x, int y)	{ m_preScreenX = x, m_preScreenY = y; }

		float getZoomMax()					{ return m_fZoomMax; }
		float getZoom() const				{ return m_fZoom; }
		void getPreScreenXY(int &x, int &y)	const { x = m_preScreenX, y = m_preScreenY; }
		void getScreenXY(int &x, int &y) const { x = m_currentScreenX, y = m_currentScreenY; }
		void getScreenSize(int & cx, int &cy) const { cx = m_screenCX, cy = m_screenCY; }
		VECTOR2 getPreScreenXY() const		{ return VECTOR2(m_preScreenX, m_preScreenY); }
		VECTOR2 getScreenXY() const			{ return VECTOR2(m_currentScreenX, m_currentScreenY); }
		VECTOR2 getScreenSize()	const		{ return VECTOR2(m_screenCX, m_screenCY); }
		

		bool updateLookAtCamera();
		bool updateLookAtCamera(const VECTOR3 & look, const VECTOR3 & at, const VECTOR3 & up);
		mip::RAY getRay(int screenX, int screenY, float ZDepth = 0.0f) const;
		mip::RAY getRay(float ZDepth = 0.0f) const;
		mip::RAY getPreRay(int screenX, int screenY, float ZDepth = 0.0f) const;
		mip::VECTOR3 getWorldPoint(int screenX, int screenY, float ZDepth = 0.0f, mip::MATRIX44 * world = NULL) const;
		mip::VECTOR3 getWorldPoint(float ZDepth = 0.0f, mip::MATRIX44 * world = NULL) const;
		mip::VECTOR3 getScreenPoint(mip::VECTOR3 localPos, mip::MATRIX44 * world) const;
		mip::VECTOR3 getPreWorldPoint(float ZDepth = 0.0f, mip::MATRIX44 * world = NULL) const;
		mip::VECTOR3 getWorldCameraDir() const;
		mip::VECTOR3 getWorldPoint(mip::MATRIX44 & view, mip::MATRIX44 & proj, float ZDepth = 0.0f, mip::MATRIX44 * world = NULL) const;
		mip::VECTOR3 getPreWorldPoint(mip::MATRIX44 & view, mip::MATRIX44 & proj, float ZDepth = 0.0f, mip::MATRIX44 * world = NULL) const;

		mip::VECTOR3 getCameraCoordPoint() const;
		mip::VECTOR3 getPreCameraCoordPoint() const;

		// Project 
		void setOrtho(int cx, int cy, float zoom);
		bool updateOrtho();
		void setPerspectiveFov(float fovy, float aspect);

		mip::PLANE getNearPlane(mip::MATRIX44 * world = NULL);
		mip::PLANE getFarPlane(mip::MATRIX44 * world = NULL);

		// Frustum 
		void updateFrustum();

		bool checkFrustumPoint(VECTOR3 & Pos);
		bool checkFustumAABB(AABB& box);
		bool checkFustumAABB(AARBB & box);

		// Viewport
		void setViewport(VIEWPORT_TYPE type);
	};
};