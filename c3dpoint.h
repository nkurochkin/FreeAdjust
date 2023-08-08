#ifndef C3DPOINT_H
#define C3DPOINT_H

class C3DPoint
{
public:
    double x,y,z;

    C3DPoint() {}
    C3DPoint(double init) { x=y=z=init; }
    C3DPoint(double _x, double _y, double _z)
          { x=_x; y=_y; z=_z; }
    C3DPoint(const C3DPoint& pt) { *this=pt; }

 C3DPoint& operator=(double init) { x=y=z=init; return *this; }
 C3DPoint& operator=(const C3DPoint&);

 friend C3DPoint operator+(const  C3DPoint& pt1,const  C3DPoint& pt );
 friend C3DPoint operator+(const  C3DPoint& pt,const  double nOffset );
 friend C3DPoint operator+(const  double nOffset,const  C3DPoint& pt );
 C3DPoint& operator+=(const C3DPoint&);
 C3DPoint& operator+=(const double);
 C3DPoint& operator+=(const int);

 friend C3DPoint operator-(const  C3DPoint& pt1,const  C3DPoint& pt );
 friend C3DPoint operator-(const  C3DPoint& pt,const  double nOffset );
 friend C3DPoint operator-(const  C3DPoint& pt,const  int nOffset );
 C3DPoint& operator-=(const C3DPoint&);
 C3DPoint& operator-=(const double);
 C3DPoint& operator-=(const int);
 C3DPoint  operator- ();

 friend C3DPoint operator*(const  C3DPoint& pt1,const  C3DPoint& pt );
 friend C3DPoint operator*(const  C3DPoint& pt,const  double nOffset );
 friend C3DPoint operator*(const  double nOffset,const  C3DPoint& pt );
 friend C3DPoint operator*(const  C3DPoint& pt,const  int nOffset );
 C3DPoint& operator*=(const C3DPoint&);
 C3DPoint& operator*=(const double);
 C3DPoint& operator*=(const int);

 friend C3DPoint operator/(const  C3DPoint& pt1,const  C3DPoint& pt );
 friend C3DPoint operator/(const  C3DPoint& pt,const  double nOffset );
 friend C3DPoint operator/(const  double nOffset,const  C3DPoint& pt );
 friend C3DPoint operator/(const  C3DPoint& pt,const  int nOffset );
 C3DPoint& operator/=(const C3DPoint&);
 C3DPoint& operator/=(const double);
 C3DPoint& operator/=(const int);

 int  operator>(const C3DPoint&);
 int  operator<(const C3DPoint&);
 int  operator==(const C3DPoint&);
 int  operator==(const double);
 int  operator!=(const C3DPoint&);

// void* operator new (size_t sz);
// void  operator delete (void* n);

 double GetX(){ return x; }
 double GetY(){ return y; }
 double GetZ(){ return z; }

 double Angle(C3DPoint& v);
 double AngleX();
 double Cos(C3DPoint& v);
 double DotProd(C3DPoint& v);
 double Det(C3DPoint& row2, C3DPoint& row3);
 double DirectionXY(C3DPoint &);
 double DirectionYX(C3DPoint &);
 double Distance(C3DPoint &);
 double Distance(C3DPoint &,C3DPoint &);
 double Distance(C3DPoint &,C3DPoint &,C3DPoint &);
// double Distance(CStraightLine &);
// double Distance(CPlane &);
 double DistanceXY(C3DPoint &);
 double DistanceXY(double x,double y);
 double Dist2XY(C3DPoint &);
 double Dist2XY(double x,double y);
 double Dist2XYZ(C3DPoint &);
 double DistSeg(C3DPoint &p1, C3DPoint &p2,C3DPoint &ip);
 double DistSeg(C3DPoint &p1, C3DPoint &p2);
   bool IsInsideSeg(C3DPoint &s1,C3DPoint &s2);
   bool IsInsideSeg(C3DPoint &s1,C3DPoint &s2,double prec);
//    void InitNormal(CTriangle &);
    void InitNormal(C3DPoint &,C3DPoint &,C3DPoint &);
    bool IsValid();
    void NANCorrect(double);
    void Normalize();
    void Round(C3DPoint& prec);
    bool HaveZero();
    bool IsZero();
    void Show();
  double SquaresSum();
  double SquaresSumXY();
  double ScalarProd(C3DPoint& pt1);
    void VectProd(C3DPoint& pt2, C3DPoint& outPt);
  double Length();
  double LengthXY();
  double TriArea(C3DPoint& pt2, C3DPoint& pt3);
  double TriAreaPlane(C3DPoint& pt2, C3DPoint& pt3);
  double TetraArea(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4);
  double TetraAreaPlane(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4);
  double PentaArea(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4, C3DPoint& pt5);
  double PentaAreaPlane(C3DPoint& pt2, C3DPoint& pt3, C3DPoint& pt4, C3DPoint& pt5);
    void Set(double,double,double);
 //            void RotateZ(double);
 //DllExport   void RotateZ(double,C3DPoint &);
 //            void RotateZ(double,double);
 //            void RotateZ(double,double,C3DPoint &);

 void RotateX(double angle);
 void RotateX(double _sin, double _cos);
 void RotateY(double angle);
 void RotateY(double _sin, double _cos);
 void RotateZ(double angle);
 void RotateZ(double _sin, double _cos);
 void RotateX(double angle, C3DPoint& pt);
 void RotateX(double _sin, double _cos, C3DPoint& pt);
 void RotateY(double angle, C3DPoint& pt);
 void RotateY(double _sin, double _cos, C3DPoint& pt);
 void RotateZ(double angle, C3DPoint& pt);
 void RotateZ(double _sin, double _cos, C3DPoint& pt);

 //----------------------------
 void TurnPoint(C3DPoint& Po, double alfa, C3DPoint& Pe);
 void Pressure(C3DPoint& Po, double dcompress, C3DPoint& Pe);
 //----------------------------

 void GetSCV(double angle, C3DPoint& s, C3DPoint& c);
 void Rotate(C3DPoint& angle);
 void Rotate(C3DPoint& angle, C3DPoint& o_pt);
 void RotateSC_XYZ(C3DPoint& angle_sin, C3DPoint& angle_cos);
 void RotateSC_ZYX(C3DPoint& angle_sin, C3DPoint& angle_cos);
 void RotateSC(C3DPoint& angle_sin, C3DPoint& angle_cos, C3DPoint& o_pt);
 void RotateSCV(C3DPoint& s, C3DPoint& c);
 void Zero();
 void Max();

};

#endif // C3DPOINT_H
