#include "c3dpoint.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>


__declspec(naked) double __fastcall RoundF(double num)
{
    __asm
    {
        fld    qword ptr [esp+4]
        lea    ecx,[esp-8]
        sub    esp,16        ; allocate frame
        and    ecx,-8        ; align pointer on boundary of 8
        fistp  qword ptr[ecx]
        fild   qword ptr[ecx]
        add    esp,16
        ret    8
    }
}


C3DPoint& C3DPoint::operator=(const C3DPoint& pt)
{
 //memcpy(this,&pt,sizeof(C3DPoint));
 x=pt.x;
 y=pt.y;
 z=pt.z;
 return *this;
}

C3DPoint operator+(const C3DPoint& pt1,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt1;
 ptTemp.x += pt.x;
 ptTemp.y += pt.y;
 ptTemp.z += pt.z;
 return ptTemp;
}

C3DPoint operator+(const C3DPoint& pt,const  double nOffset)
{
 C3DPoint ptTemp = pt;
 ptTemp.x += nOffset;
 ptTemp.y += nOffset;
 ptTemp.z += nOffset;
 return ptTemp;
}

C3DPoint operator+(const double nOffset,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt;
 ptTemp.x += nOffset;
 ptTemp.y += nOffset;
 ptTemp.z += nOffset;
 return ptTemp;
}

C3DPoint& C3DPoint::operator+=(const C3DPoint& pt)
{
 x+=pt.x;
 y+=pt.y;
 z+=pt.z;
 return *this;
}

C3DPoint& C3DPoint::operator+=(const double nOffset)
{
 x+=nOffset;
 y+=nOffset;
 z+=nOffset;
 return *this;
}

C3DPoint& C3DPoint::operator+=(const int nOffset)
{
 x+=(double)nOffset;
 y+=(double)nOffset;
 z+=(double)nOffset;
 return *this;
}

// Handle C3DPoint - C3DPoint expression.
C3DPoint operator-(const C3DPoint& pt1,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt1;
 ptTemp.x -= pt.x;
 ptTemp.y -= pt.y;
 ptTemp.z -= pt.z;
 return ptTemp;
}

// Handle C3DPoint - double expression.
C3DPoint operator-(const C3DPoint& pt,const  double nOffset)
{
 C3DPoint ptTemp = pt;
 ptTemp.x -= nOffset;
 ptTemp.y -= nOffset;
 ptTemp.z -= nOffset;
 return ptTemp;
}

C3DPoint operator-(const double nOffset,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt;
 ptTemp.x = nOffset-ptTemp.x;
 ptTemp.y = nOffset-ptTemp.y;
 ptTemp.z = nOffset-ptTemp.z;
 return ptTemp;
}

C3DPoint operator-(const C3DPoint& pt,const  int nOffset)
{
 C3DPoint ptTemp = pt;
 ptTemp.x -= (double)nOffset;
 ptTemp.y -= (double)nOffset;
 ptTemp.z -= (double)nOffset;
 return ptTemp;
}

C3DPoint C3DPoint::operator- ()
{
 C3DPoint ptTemp;
 ptTemp.x=-x;
 ptTemp.y=-y;
 ptTemp.z=-z;
 return ptTemp;
}

C3DPoint& C3DPoint::operator-=(const C3DPoint& pt)
{
 x-=pt.x;
 y-=pt.y;
 z-=pt.z;
 return *this;
};

C3DPoint& C3DPoint::operator-=(const double nOffset)
{
 x-=nOffset;
 y-=nOffset;
 z-=nOffset;
 return *this;
}

C3DPoint& C3DPoint::operator-=(const int nOffset)
{
 x-=(double)nOffset;
 y-=(double)nOffset;
 z-=(double)nOffset;
 return *this;
}

C3DPoint operator*(const C3DPoint& pt1,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt1;
 ptTemp.x *= pt.x;
 ptTemp.y *= pt.y;
 ptTemp.z *= pt.z;
 return ptTemp;
}

C3DPoint operator*(const C3DPoint& pt,const  double nOffset)
{
 C3DPoint ptTemp = pt;
 ptTemp.x *= nOffset;
 ptTemp.y *= nOffset;
 ptTemp.z *= nOffset;
 return ptTemp;
}

C3DPoint operator*(const double nOffset,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt;
 ptTemp.x *= nOffset;
 ptTemp.y *= nOffset;
 ptTemp.z *= nOffset;
 return ptTemp;
}

C3DPoint operator*(const C3DPoint& pt,const  int nOffset)
{
 C3DPoint ptTemp = pt;
 ptTemp.x *= (double)nOffset;
 ptTemp.y *= (double)nOffset;
 ptTemp.z *= (double)nOffset;
 return ptTemp;
}

C3DPoint& C3DPoint::operator*=(const C3DPoint& pt)
{
 x*=pt.x;
 y*=pt.y;
 z*=pt.z;
 return *this;
}

C3DPoint& C3DPoint::operator*=(const double nOffset)
{
 x*=nOffset;
 y*=nOffset;
 z*=nOffset;
 return *this;
}

C3DPoint& C3DPoint::operator*=(const int nOffset)
{
 x*=(double)nOffset;
 y*=(double)nOffset;
 z*=(double)nOffset;
 return *this;
}

C3DPoint operator/(const C3DPoint& pt1,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt1;
 ptTemp.x /= pt.x;
 ptTemp.y /= pt.y;
 ptTemp.z /= pt.z;
 return ptTemp;
}

C3DPoint operator/(const C3DPoint& pt,const  double nOffset)
{
 C3DPoint ptTemp = pt;
 ptTemp.x /= nOffset;
 ptTemp.y /= nOffset;
 ptTemp.z /= nOffset;
 return ptTemp;
}

C3DPoint operator/(const double nOffset,const  C3DPoint& pt)
{
 C3DPoint ptTemp = pt;
 ptTemp.x = nOffset/ptTemp.x;
 ptTemp.y = nOffset/ptTemp.y;
 ptTemp.z = nOffset/ptTemp.z;
 return ptTemp;
}

C3DPoint operator/(const C3DPoint& pt,const  int nOffset)
{
 C3DPoint ptTemp = pt;
 ptTemp.x /= (double)nOffset;
 ptTemp.y /= (double)nOffset;
 ptTemp.z /= (double)nOffset;
 return ptTemp;
}

C3DPoint& C3DPoint::operator/=(const C3DPoint& pt)
{
 x/=pt.x;
 y/=pt.y;
 z/=pt.z;
 return *this;
}

C3DPoint& C3DPoint::operator/=(const double nOffset)
{
 x/=nOffset;
 y/=nOffset;
 z/=nOffset;
 return *this;
}

C3DPoint& C3DPoint::operator/=(const int nOffset)
{
 x/=(double)nOffset;
 y/=(double)nOffset;
 z/=(double)nOffset;
 return *this;
}

int C3DPoint::operator>(const C3DPoint& pt)
{
 return x>pt.x || y>pt.y || z>pt.z;
}

int C3DPoint::operator<(const C3DPoint& pt)
{
 return x<pt.x || y<pt.y || z<pt.z;
}

int C3DPoint::operator==(const C3DPoint& pt)
{
 return x==pt.x && y==pt.y && z==pt.z;
}

int C3DPoint::operator==(const double val)
{
 return x==val && y==val && z==val;
}

int C3DPoint::operator!=(const C3DPoint& pt)
{
 return x!=pt.x || y!=pt.y || z!=pt.z;
}

//void* C3DPoint::operator new (size_t sz)
//{
// return pGeneralHeap->Allocate(sz);
//}

//void C3DPoint::operator delete (void* p)
//{
// pGeneralHeap->Free(p);
//}

/*
BOOL CALLBACK C3DPoint::dEdit(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
   case WM_INITDIALOG : {
                         char s[32];
                         sprintf(s,"%7.2f",x);
                         SendDlgItemMessage(hDlg,IDC_EDIT_X,WM_SETTEXT,0,(LPARAM)s);
                         sprintf(s,"%7.2f",y);
                         SendDlgItemMessage(hDlg,IDC_EDIT_Y,WM_SETTEXT,0,(LPARAM)s);
                         sprintf(s,"%7.2f",z);
                         SendDlgItemMessage(hDlg,IDC_EDIT_Z,WM_SETTEXT,0,(LPARAM)s);
                         return TRUE;
                        }
   case WM_COMMAND :
   switch(LOWORD(wParam))
   {
    case IDOK : EndDialog(hDlg,TRUE); return TRUE;
    case IDCANCEL : EndDialog(hDlg,FALSE); return TRUE;
   }

  }
  return 0L;
}

/*
void C3DPoint::Edit(HWND hWnd)
{
  DialogBox(hInst,"VERT_COORDS",hWnd,(DLGPROC)dEdit);
}
*/

double C3DPoint::Angle(C3DPoint& v)
{
        double cos=Cos(v);
        double angle=acos(cos);
        if((x*v.y-y*v.x)<0)
                angle=-angle;
        return angle;
}

double C3DPoint::AngleX()
{
        double len=sqrt(x*x+y*y);
        if(len==0.)
            return 0.;
        double angle=acos(x/len);
        if(y<0)
            return -angle;
        return angle;
}

double C3DPoint::Cos(C3DPoint& v)
{
    double znam=sqrt(x*x+y*y+z*z)*sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
    if(znam==0.)
        return 0.;
    return (x*v.x+y*v.y+z*v.z)/znam;
}

double C3DPoint::DotProd(C3DPoint& v)
{
    return x*v.x+y*v.y+z*v.z;
}

double C3DPoint::Det(C3DPoint& row2, C3DPoint& row3)
{
 return x*(row2.y*row3.z-row3.y*row2.z)+
        y*(row2.z*row3.x-row3.z*row2.x)+
        z*(row2.x*row3.y-row3.x*row2.y);
}

double C3DPoint::DirectionXY(C3DPoint &p1)
{
    double dx=p1.x-x;
    double dy=p1.y-y;
    if(dx==0. && dy==0) return 0;
    return atan2(dy,dx);
}

double C3DPoint::DirectionYX(C3DPoint &p1)
{
    double dx=p1.x-x;
    double dy=p1.y-y;
    if(dx==0. && dy==0) return 0;
    return atan2(dx,dy);
}


double C3DPoint::Distance(C3DPoint &p1)
{
    double dx=p1.x-x;
    double dy=p1.y-y;
    double dz=p1.z-z;
    return sqrt(dx*dx+dy*dy+dz*dz);
}

double C3DPoint::DistanceXY(C3DPoint &p1)
{
    double dx=p1.x-x;
    double dy=p1.y-y;
    return sqrt(dx*dx+dy*dy);
}

double C3DPoint::DistanceXY(double px,double py)
{
    double dx=px-x;
    double dy=py-y;
    return sqrt(dx*dx+dy*dy);
}

double C3DPoint::Dist2XY(C3DPoint &p1)
{
    double dx=p1.x-x;
    double dy=p1.y-y;
    return dx*dx+dy*dy;
}

double C3DPoint::Dist2XY(double px,double py)
{
    double dx=px-x;
    double dy=py-y;
    return dx*dx+dy*dy;
}

double C3DPoint::Dist2XYZ(C3DPoint &p1)
{
    double dx=p1.x-x;
    double dy=p1.y-y;
    double dz=p1.y-y;
    return dx*dx+dy*dy+dz*dz;
}

/*
double C3DPoint::DistSeg(C3DPoint &p1, C3DPoint &p2,C3DPoint &ip)
{
        double ret=0;
        CStraightLine line(p1,p2);
        C3DPoint pp;
        line.PerpIntersectionPoint(*this,pp);
        double t;
        if(line.dirV.x!=0)
                t=(pp.x-line.M.x)/line.dirV.x;
        else
        {
                if(line.dirV.y!=0)
                        t=(pp.y-line.M.y)/line.dirV.y;
                else
                {
                        if(line.dirV.z!=0)
                                t=(pp.z-line.M.z)/line.dirV.z;
                        else
                                t=0;
                }
        }
        if(t<=0.)
        {
            ip=p1;
                ret=Distance(p1);
        }else
        {
                if(t>=1.)
                {
                ip=p2;
                        ret=Distance(p2);
                }else
                {
                ip=pp;
                        ret=Distance(pp);
            }
        }
        return ret;
}

double C3DPoint::DistSeg(C3DPoint &p1,C3DPoint &p2)
{
        double ret=0;
        CStraightLine line(p1,p2);
        C3DPoint pp;
        line.PerpIntersectionPoint(*this,pp);
        double t;
        if(line.dirV.x!=0)
                t=(x-line.M.x)/line.dirV.x;
        else
        {
                if(line.dirV.y!=0)
                        t=(y-line.M.y)/line.dirV.y;
                else
                {
                        if(line.dirV.z!=0)
                                t=(z-line.M.z)/line.dirV.z;
                        else
                                t=0;
                }
        }
        if(t<=0.)
                ret=Distance(p1);
        else
        {
                if(t>=1.)
                        ret=Distance(p2);
                else
                        ret=Distance(pp);
        }
        return ret;
}
*/
//
// Определяет находится ли точка ip внутри отрезка s1 s2 точка должна лежать на одной прямой с ними
//   т.к. алгоритм работает с веторами, возможно что он определит находится ли точка между
//   двух плоскостей перпендиклярных прямой s1 s2 и проходящих через эти точки
//
bool C3DPoint::IsInsideSeg(C3DPoint &s1,C3DPoint &s2)
{
    if(*this==s1)
        return true;
    if(*this==s2)
        return true;
    C3DPoint dd1=*this-s1;
    C3DPoint dd2=*this-s2;
    dd1.Normalize();
    dd2.Normalize();
    if(dd1.DotProd(dd2)<=0)
        return true;
    return false;
}

bool C3DPoint::IsInsideSeg(C3DPoint &s1,C3DPoint &s2,double prec)
{
    C3DPoint dd1=*this-s1;
    C3DPoint dd2=*this-s2;
    if(dd1.Length()<=prec)
        return true;
    if(dd2.Length()<=prec)
        return true;
    dd1.Normalize();
    dd2.Normalize();
    if(dd1.DotProd(dd2)<=0)
        return true;
    return false;
}

/*
double C3DPoint::Distance(C3DPoint &p1,C3DPoint &p2)
{
    CStraightLine line(p1,p2);
    return line.Distance(*this);
}

double C3DPoint::Distance(CStraightLine &line)
{
    return line.Distance(*this);
}

double C3DPoint::Distance(C3DPoint &p1,C3DPoint &p2,C3DPoint &p3)
{
    CPlane pl(p1,p2,p3);
    return pl.Distance(*this);
}

double C3DPoint::Distance(CPlane &pl)
{
    return pl.Distance(*this);
}

void C3DPoint::InitNormal(CTriangle &tri)
{
 x=(tri.B->Node().y-tri.A->Node().y)*(tri.C->Node().z-tri.A->Node().z)-(tri.B->Node().z-tri.A->Node().z)*(tri.C->Node().y-tri.A->Node().y);
 y=(tri.B->Node().z-tri.A->Node().z)*(tri.C->Node().x-tri.A->Node().x)-(tri.B->Node().x-tri.A->Node().x)*(tri.C->Node().z-tri.A->Node().z);
 z=(tri.B->Node().x-tri.A->Node().x)*(tri.C->Node().y-tri.A->Node().y)-(tri.B->Node().y-tri.A->Node().y)*(tri.C->Node().x-tri.A->Node().x);
}
*/

void C3DPoint::InitNormal(C3DPoint& pt0, C3DPoint& pt1, C3DPoint& pt2)
{
 x=(pt1.y-pt0.y)*(pt2.z-pt0.z)-(pt1.z-pt0.z)*(pt2.y-pt0.y);
 y=(pt1.z-pt0.z)*(pt2.x-pt0.x)-(pt1.x-pt0.x)*(pt2.z-pt0.z);
 z=(pt1.x-pt0.x)*(pt2.y-pt0.y)-(pt1.y-pt0.y)*(pt2.x-pt0.x);
}

bool C3DPoint::IsValid()
{
    if(_isnan(x) || _isnan(y) || _isnan(z))
        return false;
    return true;
}

void C3DPoint::NANCorrect(double val)
{
    if(_isnan(x))
        x=val;
    if(_isnan(y))
        y=val;
    if(_isnan(z))
        z=val;
}

void C3DPoint::Normalize()
{
    double len=x*x+y*y+z*z;
    if(len==0.)
    {
        x=1.;
        y=0.;
        z=0.;
    }else
    {
        len=1./sqrt(len);
        x*=len;
        y*=len;
        z*=len;
    }
}

void C3DPoint::Round(C3DPoint& prec)
{
        if(prec.x!=0.)
        {
                x/=prec.x;
                x=RoundF(x);
                x*=prec.x;
        }
        if(prec.y!=0.)
        {
                y/=prec.y;
                y=RoundF(y);
                y*=prec.y;
        }
        if(prec.z!=0.)
        {
                z/=prec.z;
                z=RoundF(z);
                z*=prec.z;
        }
}

double C3DPoint::SquaresSum()
{
        return x*x+y*y+z*z;
}

double C3DPoint::SquaresSumXY()
{
        return x*x+y*y;
}

double C3DPoint::Length()
{
        return sqrt(x*x+y*y+z*z);
}

double C3DPoint::LengthXY()
{
        return sqrt(x*x+y*y);
}

void C3DPoint::VectProd(C3DPoint& pt2, C3DPoint& outPt)
{
        outPt.x=y*pt2.z-pt2.y*z;
        outPt.y=z*pt2.x-pt2.z*x;
        outPt.z=x*pt2.y-pt2.x*y;
}

double C3DPoint::ScalarProd(C3DPoint& pt1)
{
    return x*pt1.x+y*pt1.y+z*pt1.z;
}


void C3DPoint::Set(double px,double py,double pz)
{
    x=px; y=py; z=pz;
}

void C3DPoint::Zero()
{
    x=y=z=0;
}

void C3DPoint::Max()
{
    x=y=z=DBL_MAX;
}

bool C3DPoint::HaveZero()
{
        if(x==0. || y==0. || z==0.)
                return true;
        return false;
}

bool C3DPoint::IsZero()
{
        if(x==0. && y==0. && z==0.)
                return true;
        return false;
}

double C3DPoint::TriArea(C3DPoint& pt2, C3DPoint& pt3)
{
        C3DPoint v01(pt2-*this);
        C3DPoint v02(pt3-*this);
        double res=v01.y*v02.z-v02.y*v01.z;
        res*=res;
        double coord=v01.z*v02.x-v02.z*v01.x;
        res+=coord*coord;
        coord=v01.x*v02.y-v02.x*v01.y;
        res+=coord*coord;
        res=sqrt(res)*0.5;
        return res;
}

double C3DPoint::TriAreaPlane(C3DPoint& pt2, C3DPoint& pt3)
{
        C3DPoint v01(pt2-*this);
        C3DPoint v02(pt3-*this);
        double a=v01.x*v02.y-v02.x*v01.y;
        return sqrt(a*a)*0.5;
}

__inline double Plane3Area(double x0, double y0,
                           double x1, double y1,
                           double x2, double y2)
{
        double a=(x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);
        return sqrt(a*a)*0.5;
}

double C3DPoint::TetraArea(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4)
{
        return 0;
}

double C3DPoint::TetraAreaPlane(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4)
{
        C3DPoint v1(pt2-*this);
        C3DPoint v2(pt3-*this);
        C3DPoint v3(pt4-*this);
        double a1=v1.x*v2.y-v2.x*v1.y;
        a1=sqrt(a1*a1)*0.5;
        double a2=v2.x*v3.y-v3.x*v2.y;
        a2=sqrt(a2*a2)*0.5;
        return a1+a2;
}

__inline double Plane4Area(double x0, double y0,
                           double x1, double y1,
                           double x2, double y2,
                           double x3, double y3)
{
        double a1=(x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);
        double a2=(x2-x0)*(y3-y0)-(y2-y0)*(x3-x0);
        return (sqrt(a1*a1)+sqrt(a2*a2))*0.5;
}

double C3DPoint::PentaArea(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4, C3DPoint& pt5)
{
        return 0;
}

double C3DPoint::PentaAreaPlane(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4, C3DPoint& pt5)
{
        C3DPoint v1(pt2-*this);
        C3DPoint v2(pt3-*this);
        C3DPoint v3(pt4-*this);
        C3DPoint v4(pt5-*this);
        double a1=v1.x*v2.y-v2.x*v1.y;
        a1=sqrt(a1*a1)*0.5;
        double a2=v2.x*v3.y-v3.x*v2.y;
        a2=sqrt(a2*a2)*0.5;
        double a3=v3.x*v4.y-v4.x*v3.y;
        a3=sqrt(a3*a3)*0.5;
        return a1+a2+a3;
}

__inline double Plane5Area(double x0, double y0,
                           double x1, double y1,
                           double x2, double y2,
                           double x3, double y3,
                           double x4, double y4)
{
        double a1=(x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);
        double a2=(x2-x0)*(y3-y0)-(y2-y0)*(x3-x0);
        double a3=(x3-x0)*(y4-y0)-(y3-y0)*(x4-x0);
        return (sqrt(a1*a1)+sqrt(a2*a2)+sqrt(a3*a3))*0.5;
}

__inline double Plane6Area(double x0, double y0,
                           double x1, double y1,
                           double x2, double y2,
                           double x3, double y3,
                           double x4, double y4,
                           double x5, double y5)
{
        double a1=(x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);
        double a2=(x2-x0)*(y3-y0)-(y2-y0)*(x3-x0);
        double a3=(x3-x0)*(y4-y0)-(y3-y0)*(x4-x0);
        double a4=(x4-x0)*(y5-y0)-(y4-y0)*(x5-x0);
        return (sqrt(a1*a1)+sqrt(a2*a2)+sqrt(a3*a3)+sqrt(a4*a4))*0.5;
}

__inline double Plane7Area(double x0, double y0,
                           double x1, double y1,
                           double x2, double y2,
                           double x3, double y3,
                           double x4, double y4,
                           double x5, double y5,
                           double x6, double y6)
{
        double a1=(x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);
        double a2=(x2-x0)*(y3-y0)-(y2-y0)*(x3-x0);
        double a3=(x3-x0)*(y4-y0)-(y3-y0)*(x4-x0);
        double a4=(x4-x0)*(y5-y0)-(y4-y0)*(x5-x0);
        double a5=(x5-x0)*(y6-y0)-(y5-y0)*(x6-x0);
        return (sqrt(a1*a1)+sqrt(a2*a2)+sqrt(a3*a3)+sqrt(a4*a4)+sqrt(a5*a5))*0.5;
}

////------------------------------------------------------------------------------
//void C3DPoint::RotateZ(double angle)
//{
//	RotateZ(sin(angle),cos(angle));
//}
////------------------------------------------------------------------------------
//void C3DPoint::RotateZ(double angle, C3DPoint& pt)
//{
//	RotateZ(sin(angle),cos(angle),pt);
//}
////------------------------------------------------------------------------------
//void C3DPoint::RotateZ(double _sin, double _cos)
//{
//	double _x=x;
//	double _y=y;
//	x=_x*_cos-_y*_sin;
//	y=_x*_sin+_y*_cos;
//}
////------------------------------------------------------------------------------
//void C3DPoint::RotateZ(double _sin, double _cos, C3DPoint& pt)
//{
//	pt.x=x*_cos-y*_sin;
//	pt.y=x*_sin+y*_cos;
//	pt.z=z;
//}


//------------------------------------------------------------------------------
void C3DPoint::RotateX(double angle)
{
        RotateX(sin(angle),cos(angle));
}
//------------------------------------------------------------------------------
void C3DPoint::RotateX(double angle, C3DPoint& pt)
{
        RotateX(sin(angle),cos(angle),pt);
}
//------------------------------------------------------------------------------
void C3DPoint::RotateX(double _sin, double _cos)
{
        double _y=y;
        double _z=z;
        y=_y*_cos-_z*_sin;
        z=_y*_sin+_z*_cos;
}
//------------------------------------------------------------------------------
void C3DPoint::RotateX(double _sin, double _cos, C3DPoint& pt)
{
        pt.x=x;
        pt.y=y*_cos-z*_sin;
        pt.z=y*_sin+z*_cos;
}
//------------------------------------------------------------------------------
void C3DPoint::RotateY(double angle)
{
        RotateY(sin(angle),cos(angle));
}
//------------------------------------------------------------------------------
void C3DPoint::RotateY(double angle, C3DPoint& pt)
{
        RotateY(sin(angle),cos(angle),pt);
}
//------------------------------------------------------------------------------
void C3DPoint::RotateY(double _sin, double _cos)
{
        double _x=x;
        double _z=z;
        x=_x*_cos+_z*_sin;
        z=-_x*_sin+_z*_cos;
}
//------------------------------------------------------------------------------
void C3DPoint::RotateY(double _sin, double _cos, C3DPoint& pt)
{
        pt.x=x*_cos+z*_sin;
        pt.y=y;
        pt.z=-x*_sin+z*_cos;
}
//------------------------------------------------------------------------------
void C3DPoint::RotateZ(double angle)
{
        RotateZ(sin(angle),cos(angle));
}
//------------------------------------------------------------------------------
void C3DPoint::RotateZ(double angle, C3DPoint& pt)
{
        RotateZ(sin(angle),cos(angle),pt);
}
//------------------------------------------------------------------------------
void C3DPoint::RotateZ(double _sin, double _cos)
{
        double _x=x;
        double _y=y;
        x=_x*_cos-_y*_sin;
        y=_x*_sin+_y*_cos;
}
//------------------------------------------------------------------------------
void C3DPoint::RotateZ(double _sin, double _cos, C3DPoint& pt)
{
        pt.x=x*_cos-y*_sin;
        pt.y=x*_sin+y*_cos;
        pt.z=z;
}
//-----------------------------------------------------------------------------
void C3DPoint::TurnPoint(C3DPoint& Po, double alfa, C3DPoint& Pe)
{
     // Отображение, описывающее поворот относительно произвольной точки О
     // "Введение в теорию фракталов", А.Д.Морозов, стр.56
     // ==================================================================
     // ПЕРЕДАЮ
     // Po - координаты x и y точки Po, которую будем поворачивать
     // O - координаты неподвижной точки О - центра вращения
     // alfa - угол поворота в радианах
     // ПОЛУЧАЮ
     // Pe - новые координаты точки Ро после поворота

     double cos_a = cos(alfa);
     double sin_a = sin(alfa);
     Pe.x = (Po.x-x)*cos_a - (Po.y-y)*sin_a + x;
     Pe.y = (Po.x-x)*sin_a + (Po.y-y)*cos_a + y;
}
//------------------------------------------------------------------------------
void C3DPoint::Pressure(C3DPoint& Po, double dcompress, C3DPoint& Pe)
{
     // Сжатие (растяжение) связано с изменением масштаба. Перемасштабирование или
     // центральное расширение характеризуется центром и показателем сжатия "compress".
     // Центральное расширение (сжатие) относительно точки О выражается этими формулами:
     // "Введение в теорию фракталов", А.Д.Морозов, стр.58
     //
     //================================================================================
     // ПЕРЕДАЮ
     // Po - координаты x и y начальной точки Po
     // O - координаты неподвижной точки О
     // compress - коэффициент сжатия (если <1) или растяжения (если >1)
     // ПОЛУЧАЮ
     // Pe - новые координаты точки Ро после сжатия (растяжения)

     double tx = dcompress*(Po.x - x) + x;
     double ty = dcompress*(Po.y - y) + y;
     Pe.x = tx;
     Pe.y = ty;
}
//------------------------------------------------------------------------------
void C3DPoint::Rotate(C3DPoint& angle)
{
        RotateX(angle.x);
        RotateY(angle.y);
        RotateZ(angle.z);
}
//------------------------------------------------------------------------------
void C3DPoint::RotateSC_XYZ(C3DPoint& angle_sin, C3DPoint& angle_cos)
{
        RotateX(angle_sin.x,angle_cos.x);
        RotateY(angle_sin.y,angle_cos.y);
        RotateZ(angle_sin.z,angle_cos.z);
}
//------------------------------------------------------------------------------
void C3DPoint::RotateSC_ZYX(C3DPoint& angle_sin, C3DPoint& angle_cos)
{
        RotateZ(angle_sin.x,angle_cos.x);
        RotateY(angle_sin.y,angle_cos.y);
        RotateX(angle_sin.z,angle_cos.z);
}
//------------------------------------------------------------------------------
void C3DPoint::RotateSC(C3DPoint& angle_sin, C3DPoint& angle_cos, C3DPoint& o_pt)
{
        RotateX(angle_sin.x,angle_cos.x,o_pt);
        o_pt.RotateY(angle_sin.y,angle_cos.y);
        o_pt.RotateZ(angle_sin.z,angle_cos.z);
}
//------------------------------------------------------------------------------
void C3DPoint::GetSCV(double angle, C3DPoint& s, C3DPoint& c)
{
        s.x=sin(angle);
        c.x=cos(angle);
        c.y=z/sqrt(x*x+y*y+z*z);
        s.y=sin(acos(c.y));
        double theta;
        if(x>0)
                theta=atan(y/x);
        else
                if(x<0)
                        theta=M_PI+atan(y/x);
                else
                        if(y>=0)
                                theta=M_PI_2;
                        else
                                theta=M_PI_2*3;
        s.z=sin(theta);
        c.z=cos(theta);
}
//------------------------------------------------------------------------------
void C3DPoint::RotateSCV(C3DPoint& s, C3DPoint& c)
{
        double _x=x;
        double _y=y;
        double _z=z;
        x=_x*(c.z*c.z*c.y*c.y*c.x+c.z*c.z*s.y*s.y+s.z*s.z*c.x)
         +_y*(-c.z*s.z*c.y*c.y*c.x-s.z*s.z*c.y*s.x-c.z*s.z*s.y*s.y-c.z*c.z*c.y*s.x+s.z*c.z*c.x)
         +_z*(c.z*c.y*s.y*c.x+s.z*s.y*s.x-c.z*s.y*c.y);
        y=_x*(-s.z*c.z*c.y*c.y*c.x+c.z*c.z*c.y*s.x+s.z*c.z*s.y*s.y+s.z*s.z*c.y*s.x+s.z*c.z*c.x)
         +_y*(s.z*s.z*c.y*c.y*c.x+s.z*s.z*s.y*s.y+c.z*c.z*c.x)
         +_z*(-s.z*c.y*s.y*c.x+c.z*s.y*s.x+s.z*c.y*s.y);
        z=_x*(c.z*s.y*c.y*c.x-c.z*s.y*c.y-s.z*s.y*s.x)
         +_y*(-s.z*s.y*c.y*c.x+s.z*s.y*c.y-c.z*s.y*s.x)
         +_z*(s.y*s.y*c.x+c.y*c.y);
}
//------------------------------------------------------------------------------
void C3DPoint::Rotate(C3DPoint& angle, C3DPoint& o_pt)
{
        RotateX(angle.x,o_pt);
        o_pt.RotateY(angle.y);
        o_pt.RotateZ(angle.z);
}
