#define _USE_MATH_DEFINES
#include "cangleunits.h"
#include <math.h>
#include <float.h>
#include "convert.h"

CAngleUnits::CAngleUnits(void)
{
    m_Mode=5;
    m_EditPrec=0;
    m_ViewPrec=0;
    m_Empty   =0;
}

double CAngleUnits::ToUnits(double val)
{
    return ToUnits(m_Mode,m_ViewPrec,val);
}

double CAngleUnits::ToUnits(int lmode,int vprec,double val)
{
    if(lmode&1)
    {
        while(val<-M_PI) val+=M_PI*2;
        while(val> M_PI) val-=M_PI*2;
    }else
    {
        while(val<0.    )  val+=M_PI*2;
        if(val>=M_PI*2)
        val=fmod(val,M_PI*2);
    }

    switch(lmode)
    {
    case 0:
    case 1:
    case 8:
        return val*(180./M_PI);
    case 2:
    case 3:
    case 9:
        return rad_gm(val);
    case 4:
    case 5:
    case 10:
        return radgms(val);
    default:
        return val;
    }
}

#define RG (180./M_PI)
double CAngleUnits::ToEditUnits(double val)
{
    return ToEditUnits(m_Mode,m_EditPrec,val);
}

double CAngleUnits::ToEditUnits(int lmode,int vprec,double val)
{
    if(lmode&1)
    {
        while(val<-M_PI) val+=M_PI*2;
        while(val> M_PI) val-=M_PI*2;
    }else
    {
        while(val<0.    )  val+=M_PI*2;
        while(val>=M_PI*2) val-=M_PI*2;
    }

    switch(lmode)
    {
    case 0:
    case 1:
    case 8:
        return val*(180./M_PI);
    case 2:
    case 3:
    case 9:
        {
            double   x,g,m;
            x     = val*RG             ;
            g     = (x-fmod(x,1.))*100.;
            m     = fmod (x,1.)*60.    ;
            double prec=pow(10.,vprec);
            if(val<0)
            {
                m     =-floor(-m*prec+0.5)/prec;
                if(m<=-60.)
                {
                        m=0.;
                        g-=100.;
                }
            }else
            {
                m     = floor(m*prec+0.5)/prec;
                if(m>=60.)
                {
                        m=0.;
                        g+=100.;
                }
            }
            return g+m;
                }//return rad_gm(val);
    case 4:
    case 5:
    case 10:
                {
                      double  g,m,s,gms;

                      g     = val*180./M_PI  ;
                      m     = modf(g,&g)*60. ;
                      s     = modf(m,&m)*60. ;
                      double prec=pow(10.,vprec);
                      if(val<0)
                      {
                          s     =-floor(-s*prec+0.5)/prec;
                          if(s<=-60.)
                          {
                                 s=0.;
                                 m-=1.;
                          }
                          if(m<=-60.)
                          {
                                 m=0.;
                                 g-=1.;
                          }
                      }else
                      {
                          s     = floor(s*prec+0.5)/prec;
                          if(s>=60.)
                          {
                                 s=0.;
                                 m+=1.;
                          }
                          if(m>=60.)
                          {
                                 m=0.;
                                 g+=1.;
                          }
                      }
                      gms   = g*10000.+m*100.+s   ;
                      return gms;
                }
        //return radgms(val);
    default:
        return val;
    }
}

double CAngleUnits::FromUnits(double val)
{
    switch(m_Mode)
    {
    case 0:
    case 1:
    case 8:
        return val*(M_PI/180.);
    case 2:
    case 3:
    case 9:
        return gm_rad(val);
    case 4:
    case 5:
    case 10:
        return gmsrad(val);
    default:
        return val;
    }
}

int    CAngleUnits::vgm(double val,QString &str)
{
    return vgm(m_Mode,m_ViewPrec,val,str);
}

int    CAngleUnits::vgm(int lmode,int vprec,double val,QString &str)
{
    int    g,m,s;
    double dv,fm;
    double prec=pow(10.,vprec);

    val=ToUnits(lmode,vprec,val);
    if(val<0.)
    {
        double v=((double)(__int64)(-val*prec+0.5))/prec;
        fm=modf(v*0.01,&dv)*100.;
        g=dv;
    }else
    {
        double v=((double)(__int64)(val*prec+0.5))/prec;
        fm=modf(v*0.01,&dv)*100.;
        g=dv;
    }
    {
                double sd=modf(fm,&dv)*prec+0.5;
                if(sd>=prec)
                {
                        s=0;
                        m=dv+1.;
                }else
                {
                        s=floor(sd);
                        m=dv;
                }
        }
        if(m>=60)
        {
                m=0;
                g+=1;
        }
    if(val<0.)
    {
        if(vprec)
        {
            str.sprintf("-%d∞%.2u.%.*u'",g,m,(unsigned)vprec,s);
            return str.size();
        }
        str.sprintf("-%d∞%.2u'",g,m);
        return str.size();
    }else
    {
        if(vprec)
        {
            str.sprintf("%d∞%.2u.%.*u'",g,m,(unsigned)vprec,s);
            return str.size();
        }
        str.sprintf("%d∞%.2u'",g,m);
        return str.size();
    }
}

int    CAngleUnits::vgms(double val,QString &str)
{
    return vgms(m_Mode,m_ViewPrec,val,str);
}

int    CAngleUnits::vgms(int lmode,int vprec,double val,QString &str)
{
    int    g,m,is,s;
    double dv,fs;
    double prec=pow(10.,vprec);

    val=ToUnits(lmode,vprec,val);
    if(val<0.)
    {
/*        fs=modf(-val*0.0001,&dv)*100.;
        g=-dv;
                fs=modf(fs,&dv)*100.;
                m=dv;
                {
                        double ds=modf(fs,&dv)*prec+0.5;
                        if(ds>=prec)
                        {
                                is=0;
                                s=dv+1.;
                        }else
                        {
                                is=floor(ds);
                                s=dv;
                        }
                }
                if(s>=60)
                {
                        s=0;
                        m+=1;
                }
                if(m>=60)
                {
                        m=0;
                        g-=1;
                }
                if(g==-360) g=0;*/
        fs=modf(-val*0.0001,&dv)*100.;
        g=dv;
                fs=modf(fs,&dv)*100.;
                m=dv;
                {
                        double ds=modf(fs,&dv)*prec+0.5;
                        if(ds>=prec)
                        {
                                is=0;
                                s=dv+1.;
                        }else
                        {
                                is=floor(ds);
                                s=dv;
                        }
                }
                if(s>=60)
                {
                        s=0;
                        m+=1;
                }
                if(m>=60)
                {
                        m=0;
                        g+=1;
                }
                if(g==360) g=0;

        if(vprec)
        {
            str.sprintf("-%d∞%.2u'%.2u.%.*u\"",g,m,s,(unsigned)vprec,is);
            return str.size();
        }
        str.sprintf("-%d∞%.2u'%.2u\"",g,m,s);
        return str.size();

    }else
    {
        fs=modf(val*0.0001,&dv)*100.;
        g=dv;
                fs=modf(fs,&dv)*100.;
                m=dv;
                {
                        double ds=modf(fs,&dv)*prec+0.5;
                        if(ds>=prec)
                        {
                                is=0;
                                s=dv+1.;
                        }else
                        {
                                is=floor(ds);
                                s=dv;
                        }
                }
                if(s>=60)
                {
                        s=0;
                        m+=1;
                }
                if(m>=60)
                {
                        m=0;
                        g+=1;
                }
                if(g==360) g=0;

        if(vprec)
        {
            str.sprintf("%d∞%.2u'%.2u.%.*u\"",g,m,s,(unsigned)vprec,is);
            return str.size();
        }
        str.sprintf("%d∞%.2u'%.2u\"",g,m,s);
        return str.size();
    }
}


int    CAngleUnits::ToViewString(int lmode,int vprec,double val,QString &str)
{
    switch(lmode)
    {
    case 0:
    case 1:
        str.sprintf("%.*f",(unsigned)vprec,ToUnits(lmode,vprec,val));
        return str.size();
    case 2:
    case 3:
                return vgm(lmode,vprec,val,str);
    case 4:
    case 5:
                return vgms(lmode,vprec,val,str);
        case 8:
                {
                        while(val>M_PI)		val-=M_PI*2;
                        while(val<-M_PI)	val+=M_PI*2;
                        if(val>=0)
                        {
                            if(val<=M_PI/2)
                                str.sprintf("—¬ %.*f",(unsigned)vprec,ToUnits(lmode,vprec,val));
                            else
                                str.sprintf("ﬁ¬ %.*f",(unsigned)vprec,ToUnits(lmode,vprec,M_PI-val));
                        }else
                        {
                            if(val>=-M_PI/2)
                                str.sprintf("—« %.*f",(unsigned)vprec,ToUnits(lmode,vprec,-val));
                            else
                                str.sprintf("ﬁ« %.*f",(unsigned)vprec,ToUnits(lmode,vprec,M_PI+val));
                        }
                        return str.size();
                }
                break;
        case 9:
                {
                        while(val>M_PI)		val-=M_PI*2;
                        while(val<-M_PI)	val+=M_PI*2;
                        if(val>=0)
                        {
                                if(val<=M_PI/2)
                                {
                                    int rv=vgm(lmode,vprec,val,str);
                                    str.insert(0,"—¬ ");
                                    return rv;
                                }else
                                {
                                    int rv=vgm(lmode,vprec,M_PI-val,str);
                                    str.insert(0,"ﬁ¬ ");
                                    return rv;
                                }
                        }else
                        {
                                if(val>=-M_PI/2)
                                {
                                    int rv=vgm(lmode,vprec,-val,str);
                                    str.insert(0,"—« ");
                                    return rv;
                                }else
                                {
                                    int rv=vgm(lmode,vprec,M_PI+val,str);
                                    str.insert(0,"ﬁ« ");
                                    return rv;
                                }
                        }
                }
                break;
        case 10:
                {
                        while(val>M_PI)		val-=M_PI*2;
                        while(val<-M_PI)	val+=M_PI*2;
                        if(val>=0)
                        {
                                if(val<=M_PI/2)
                                {
                                    int rv=vgms(lmode,vprec,val,str);
                                    str.insert(0,"—¬ ");
                                    return rv;
                                }else
                                {
                                    int rv=vgms(lmode,vprec,M_PI-val,str);
                                    str.insert(0,"ﬁ¬ ");
                                    return rv;
                                }
                        }else
                        {
                                if(val>=-M_PI/2)
                                {
                                    int rv=vgms(lmode,vprec,-val,str);
                                    str.insert(0,"—« ");
                                    return rv;
                                }else
                                {
                                    int rv=vgms(lmode,vprec,M_PI+val,str);
                                    str.insert(0,"ﬁ« ");
                                    return rv;
                                }
                        }
                }
                break;
    default:
        str.sprintf("%.*f",(unsigned)vprec,ToUnits(lmode,vprec,val));
        return str.size();
    }
}

int    CAngleUnits::ToViewString(double val,QString &str)
{
        return ToViewString(m_Mode,m_ViewPrec,val,str);
}

int    CAngleUnits::ToEditString(double val,QString &str)
{
        return ToEditString(m_Mode,m_EditPrec,val,str);
}


#define RG (180./M_PI)

int    CAngleUnits::ToEditString(int lmode,int vprec,double val,QString &str)
{
        str.sprintf("%.*f",(unsigned)vprec,ToEditUnits(lmode,vprec,val));
        return str.size();
}

bool   CAngleUnits::FromString(QString &str,double *val)
{
    double var;
    QString text=str;
    text.replace(',','.');
    text.replace('∞',' ');
    text.replace('\'',' ');
    //text.remove_spaces();
    if(text.size())
    {
        var=text.toDouble();
        *val=FromUnits(var);
        return true;
    }
    return false;
}

unsigned CAngleUnits::UnitsControl(unsigned msg,unsigned p1,unsigned p2)
{
    switch(msg)
    {
    case UNC_GETAVAILABLEUNITS: return 11;
    case UNC_GETUNITNAME:
        {
            char *name;
            switch(p1)
            {
            case 0: name="√‡‰ÛÒ˚ 0...360"; break;
            case 1: name="√‡‰ÛÒ˚ -180...180"; break;
            case 2: name="√‡‰ÃËÌ 0...360"; break;
            case 3: name="√‡‰ÃËÌ -180...180"; break;
            case 4: name="√‡‰ÃËÌ—ÂÍ  0...360"; break;
            case 5: name="√‡‰ÃËÌ—ÂÍ  -180...180"; break;
            case 6: name="–‡‰Ë‡Ì  0 ... 6.28"; break;
            case 7: name="–‡‰Ë‡Ì  -3.14 ... 3.14"; break;
            case 8:  name="–ÛÏ·˚ √‡‰ÛÒ˚"; break;
            case 9:  name="–ÛÏ·˚ √‡‰ÃËÌ"; break;
            case 10: name="–ÛÏ·˚ √‡‰ÃËÌ—ÂÍ"; break;
            default: return FALSE;
            }
            strcpy((char *)p2,name);
        }
        return TRUE;
    case UNC_GETUNITSHORTNAME:
        {
            const char *name;
            switch(p1)
            {
            case 0:
            case 1:
                    name="∞"; break;
            case 2:
            case 3:
                    name="∞ '"; break;
            case 4:
            case 5:
                    name="∞ ' \""; break;
            case 6:
            case 7:
                    name="–‡‰"; break;
            case 8:
            case 9:
            case 10:
                    name="–ÛÏ·˚"; break;
            default: return FALSE;
            }
            strcpy((char *)p2,name);
        }
        return TRUE;
    case UNC_GETEXAMPLE:
    case UNC_GETVIEWEXAMPLE:
    case UNC_GETEDITEXAMPLE:
        *(double *)p2=M_PI*1.2614563456789;
        return TRUE;
    case UNC_SETDEFAULTS:
        m_Mode=0;
        m_EditPrec=3;
        m_ViewPrec=3;
        m_Empty   =0;
        return TRUE;
    }
    return false;
}
