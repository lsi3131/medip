#include "stdafx.h"
#include "graphics/CRSpline.h"
 
CRSpline::CRSpline()
: vp(), delta_t(0)
{
}

CRSpline::CRSpline(const CRSpline& s)
{
    for (int i = 0; i < (int)s.vp.size(); i++)
        vp.push_back(s.vp[i]);
    delta_t = s.delta_t;
}

CRSpline::~CRSpline()
{}

// Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4)
QVector3D CRSpline::Eq(float t, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, const QVector3D& p4)
{
    float t2 = t * t;
    float t3 = t2 * t;

    float b1 = .5 * (  -t3 + 2*t2 - t);
    float b2 = .5 * ( 3*t3 - 5*t2 + 2);
    float b3 = .5 * (-3*t3 + 4*t2 + t);
    float b4 = .5 * (   t3 -   t2    );

    return (p1*b1 + p2*b2 + p3*b3 + p4*b4); 
}

void CRSpline::AddSplinePoint(const QVector3D& v)
{
    vp.push_back(v);
    delta_t = (float)1 / (float)vp.size();
}

QVector3D CRSpline::GetInterpolatedSplinePoint(float t, int &nPoint, bool &isContinue) //nearPoint
{
    // Find out in which interval we are on the spline
    int p = (int)(t / delta_t);
    // Compute local control point indices
#define BOUNDS(pp) { if (pp < 0) pp = 0; else if (pp > vp.size()-1) pp = vp.size() - 1; }
    int p0 = p - 1;     BOUNDS(p0);
    int p1 = p;         BOUNDS(p1);
    int p2 = p + 1;     BOUNDS(p2);
	nPoint = p2;
    int p3 = p + 2;     BOUNDS(p3);
    // Relative (local) time 
	float lt = (t - delta_t*(float)p) / delta_t;
	
	// Interpolate
	QVector3D vec = CRSpline::Eq(lt, vp[p0], vp[p1], vp[p2], vp[p3]);

	if((p1==p2) || (vec == vp.back()))
		isContinue = false;

    return vec;
}

int CRSpline::GetNumPoints()
{
	return vp.size();
}

QVector3D& CRSpline::GetNthPoint(int n)
{
	return vp[n];
}


