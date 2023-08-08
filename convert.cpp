#define _USE_MATH_DEFINES

#include <math.h>
#include "convert.h"

#define RG (180./M_PI)

double  rad_gm(double r)
{
double   x,g,m;

            x     = r*RG               ;
            g     = (x-fmod(x,1.))*100.;
            m     = fmod (x,1.)*60.    ;
            return g+m;
}

double  gm_rad(double gm)
{
double  x,g,m;
            x     = gm/100.            ;
            g     = x-fmod(x,1.)       ;
            m     = (gm-g*100.)/60.    ;
            return (g+m)/RG;
}

int  rad_strgm(double r,QString &str)
{
    double   x,g,m;
    unsigned long im,ifm;

    if(r<0.) r+=M_PI*2;
    x     = r*RG          ;
    g     = (x-fmod(x,1.));
    m     = fmod (x,1.)*60.;
    ifm   = floor((modf(m,&x))*1000.);
    im    = x;
    str.sprintf("%.0f°%.2u'%.3u",g,im,ifm);
    return str.size();
}

int rad_gms(double rad,double *secnd)
{

   double gr,min,sec;

   sec  = modf(modf(RG*fabs(rad),&gr)*60.,&min)*60.;

   if(sec<60.)*secnd=sec;
   else { *secnd=sec-60.; min=min+1.; }

   if(min<60.);
   else { min=min-60.; gr=gr+1.; }

   if(rad<0.) return((int)min|((int)gr<<6)|0x8000);
   else       return((int)min|((int)gr<<6));
}

/*
double rad_gms(double rad)
{

   double gr,min,sec;

   sec  = modf(modf(RG*fabs(rad),&gr)*60.,&min)*60.;

   if(sec<60.)*secnd=sec;
   else { *secnd=sec-60.; min=min+1.; }

   if(min<60.);
   else { min=min-60.; gr=gr+1.; }

   if(rad<0.) return -(gr*10000.+min*100.+sec);
   else       return   gr*10000.+min*100.+sec;
}
*/
double  gmsrad(double gms)
{
      double  g,m,s;

      m     = modf(gms/10000,&g)*100.;
      s     = modf(m,&m)      ;
          if(s>=0.6)
          {
                if(s>0.9999) s=0.;
                else		 s-=0.6;
                m+=1.;
          }else
          if(s<=-0.6)
          {
                if(s<-0.9999) s=0.;
                else		  s+=0.6;
                m-=1.;
          }
          if(m>=60)
          {
                if(m>99.99) m=0;
                else		m-=60.;
                g+=1.;
          }else
          if(m<=-60)
          {
                if(m<-99.99) m=0;
                else		 m+=60;
                g-=1;
          }
      return  (g+m/60+s/36)/RG;
}

double  radgms(double r)
{
      double  g,m,s,gms;

      g     = r*180./M_PI    ;
      m     = modf(g,&g)*60. ;
      s     = modf(m,&m)*60. ;
      gms   = g*10000.+m*100.+s   ;
      //if(gms >= 3600000.) gms = 0.;
      return gms;
}

