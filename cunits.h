#ifndef CUNITS_H
#define CUNITS_H

#include <QString>

enum EUnitsControl
{
    UNC_GETAVAILABLEUNITS  =1,
    UNC_GETUNITNAME        ,
    UNC_GETUNITSHORTNAME   ,
    UNC_GETEXAMPLE,
    UNC_GETVIEWEXAMPLE,
    UNC_GETEDITEXAMPLE,
    UNC_SETDEFAULTS,
    UNC_LOADREGISTRY,
    UNC_SAVEREGISTRY
};


class CUnits
{
public:
    unsigned short m_Mode;
    unsigned short m_EditPrec;
    unsigned short m_ViewPrec;
    unsigned short m_Empty;

//    void   ToEdit(HWND,DWORD,double);
//    BOOL   FromEdit(HWND,DWORD,double *);

    virtual double ToUnits(double)=0;    //  Convert in ti system
    virtual double FromUnits(double)=0;  //  Convert from system
    virtual int    ToViewString(double,QString &)=0;
    virtual int    ToEditString(double,QString &)=0;
    virtual bool   FromString(QString &,double *)=0;
    virtual unsigned  UnitsControl(unsigned,unsigned,unsigned)=0;

//    void RegistryLoad(CRegistry &,LPTSTR,LPTSTR);
//    void RegistrySave(CRegistry &,LPTSTR,LPTSTR);
    CUnits(void){};
    ~CUnits(void){};
};

#endif // CUNITS_H
