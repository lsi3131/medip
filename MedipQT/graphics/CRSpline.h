
#ifndef CRSPLINE_H
#define CRSPLINE_H

#include <qvector.h>
#include <QVector3D>

class CRSpline
{
public:

    // Constructors and destructor
    CRSpline();
    CRSpline(const CRSpline&);
    ~CRSpline();

    // Operations
    void AddSplinePoint(const QVector3D& v);
	QVector3D GetInterpolatedSplinePoint(float t, int &nPoint, bool &isContinue);   // t = 0...1; 0=vp[0] ... 1=vp[max]
	int GetNumPoints();
	QVector3D& GetNthPoint(int n);

    // Static method for computing the Catmull-Rom parametric equation
    // given a time (t) and a vector quadruple (p1,p2,p3,p4).
    static QVector3D Eq(float t, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, const QVector3D& p4);

private:
    QVector<QVector3D> vp;
    float delta_t;
};

#endif