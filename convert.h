#ifndef _CONVERT_H
#define _CONVERT_H

#include <QString>

double  rad_gm(double r);
double  gm_rad(double gm);
int     rad_strgm(double r,QString &str);
int     rad_gms(double rad,double *secnd);
double  gmsrad(double gms);
double  radgms(double r);

#endif
