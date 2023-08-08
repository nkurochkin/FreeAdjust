#ifndef CANGLEUNITS_H
#define CANGLEUNITS_H
#include "cunits.h"


class CAngleUnits : public CUnits
{
    double ToUnits(int,int,double);    //  Convert in ti system
    double ToEditUnits(int,int,double);    //  Convert in ti system
public:
    int    vgm(int,int,double,QString &);
    int    vgms(int,int,double,QString &);
    int    vgm(double,QString &);
    int    vgms(double,QString &);

    double ToEditUnits(double);    //  Convert in ti system
    double ToUnits(double);    //  Convert in ti system
    double FromUnits(double);  //  Convert from system
    int    ToViewString(double,QString &);
    int    ToEditString(double,QString &);
    int    ToViewString(int,int,double,QString &);
    int    ToEditString(int,int,double,QString &);
    bool   FromString(QString &,double *);
    unsigned  UnitsControl(unsigned,unsigned,unsigned);

    CAngleUnits();
};

#endif // CANGLEUNITS_H
