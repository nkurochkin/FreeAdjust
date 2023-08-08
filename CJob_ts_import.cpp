#include "CJob.h"
#include <QFile>
#include <QTextStream>
#include "convert.h"
#include <QMessageBox>

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <stdio.h>

namespace jobnet
{

class CDevImport
{
public:
    CJob *m_job;
    CHClass<CStation> m_st;
    QAtomicInt        val;
    union{
        unsigned m_uFlags;
        struct{
            unsigned m_fLAngles:1;
        };
    };

    virtual int scan_line(QString &)=0;
    int scan_file(QTextStream &in);

    CDevImport(CJob *pjob)
    {
        m_job=pjob;
        m_uFlags=0;
    }
};

class CDevImportNikon : public CDevImport
{
public:
    int scan_line(QString &);
    CDevImportNikon(CJob *pjob):CDevImport(pjob)
    {

    }
};

class CDevImportSokkia : public CDevImport
{
public:
    int m_format;
    int m_zan;
    double m_ih;
    double m_th;

    int scan_line(QString &);
    CDevImportSokkia(CJob *pjob):CDevImport(pjob)
    {
        m_format =0;
        m_zan    =1;
        m_ih=m_th=0.;
    }
};

class CDevImportTrimble : public CDevImport
{
public:
    int m_format;
    double m_ih;
    double m_th;
    double m_z;
    QString  m_st_id;

    void getM5Input(QString &);
    double getM5Value(QString &,unsigned n);
    int scan_line_m5(QString &);
    int scan_line(QString &);
    CDevImportTrimble(CJob *pjob):CDevImport(pjob)
    {
        m_format =0;
        m_ih=m_th=0.;
        m_z      =0.;
        //m_fLAngles=1;
    }
};

int CJob::ImportDeviceFile(QString &fname,int mode)
{
    mode=-1;
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return -1;
    int rv=-1;
    QTextStream in(&file);
    {
    // Авто определение типа файла
        QString line = in.readLine();
        if(line.indexOf("Nikon RAW data format")!=-1)
            mode=ImpDev_Nikon;
        else
        if(line.indexOf("00NMSDR")!=-1)
            mode=ImpDev_Sokkia;
        else
        if(line.indexOf("For M5")!=-1 || line.indexOf("For_M5")!=-1)
            mode=ImpDev_Trimble;
    }
    in.seek(0);
    switch(mode)
    {
    case ImpDev_Nikon:
        {
            CDevImportNikon imp(this);
            rv=imp.scan_file(in);
        }
        break;
    case ImpDev_Sokkia:
        {
            CDevImportSokkia imp(this);
            rv=imp.scan_file(in);
        }
        break;
    case ImpDev_Trimble:
        {
            CDevImportTrimble imp(this);
            rv=imp.scan_file(in);
        }
        break;
    default:
        {
            QMessageBox msgBox;
            msgBox.setText("Неизвестный формат файла или\nне удалось автоматически распознать тип !");
            msgBox.exec();
        }
        break;
    }
    update_node_stats();
    return rv;
}

int CDevImport::scan_file(QTextStream &in)
{
    while (!in.atEnd())
    {
        QString line = in.readLine();
        scan_line(line);
    }
    return 0;
}

int CDevImportNikon::scan_line(QString &line)
{
    unsigned size=line.size();
    unsigned i=0;
    if(size>2)
    {
        switch(line[0].unicode())
        {
        case 'C':
            if(line[1].unicode()=='O')
            { // Comment lines

            }
            break;
        case 'S':
            switch(line[1].unicode())
            {
            case 'T': // station
                {
                    i=2;
                    // find second tag
                    while(i<size && line[i].unicode()!=',')
                        i++;
                    i++;
                    if(i>=size)
                        return -1;
                    {   // read station id
                        QString st_id;
                        while(i<size && line[i].unicode()!=',')
                        {
                            st_id+=line[i].unicode();
                            i++;
                        }
                        m_st=new CStation();
                        CHClass<CNode> node;
                        if(!st_id.size()) // unnamed station
                        { // add new unnamed node
                            node=new CNode();
                            node.Lock();
                            m_job->m_nodes.append(node);
                        }else
                        { // Look for node
                            int j;
                            for(j=0;j<m_job->m_nodes.size();j++)
                            {
                                if(m_job->m_nodes[j]->m_id==st_id)
                                {
                                    node=m_job->m_nodes[j];
                                    break;
                                }
                            }
                            if(!node.Exist())
                            { // add new node
                                node=new CNode();
                                node->m_id=st_id;
                                node.Lock();
                                m_job->m_nodes.append(node);
                            }
                        }
                        m_st->m_node=node;
                        m_st.Lock();
                        m_job->m_stations.append(m_st);
                    }
                    i++;
                    if(i>=size)
                        return 0;
                    while(i<size && line[i].unicode()!=',')
                        i++;
                    i++;
                    if(i>=size)
                        return 0;
                    while(i<size && line[i].unicode()!=',')
                        i++;
                    i++;
                    if(i>=size)
                        return 0;
                    while(i<size && line[i].unicode()!=',')
                        i++;
                    i++;
                    if(i>=size)
                        return 0;
                    {   // read station height
                        QString text;
                        while(i<size && line[i].unicode()!=',')
                        {
                            text+=line[i].unicode();
                            i++;
                        }
                        bool ok;
                        double val=text.toDouble(&ok);
                        if(ok)
                            m_st->m_height=val;
                    }
                }
                break;
            case 'S': // measurement
                if(m_st.Exist())
                {
                    i=2;
                    // find second tag
                    while(i<size && line[i].unicode()!=',')
                        i++;
                    i++;
                    if(i>=size)
                        return -1;
                    CHClass<CStationNode> obs;
                    double  height=0.;
                    { // Node name
                        QString text;
                        while(i<size && line[i].unicode()!=',')
                        {
                            text+=line[i].unicode();
                            i++;
                        }
                        obs=m_st->GiveNode(text,*m_job);
                    }
                    i++;
                    if(i>=size)
                        return 0;
                    {   // read target height
                        QString text;
                        while(i<size && line[i].unicode()!=',')
                        {
                            text+=line[i].unicode();
                            i++;
                        }
                        bool ok;
                        double val=text.toDouble(&ok);
                        if(ok)
                            height=val;
                    }
                    i++;
                    double ha,va,sd;
                    bool   fha=false;
                    bool   fsd=false;
                    bool   fva=false;

                    if(i>=size)
                        return 0;
                    {   // read m1
                        QString text;
                        while(i<size && line[i].unicode()!=',')
                        {
                            text+=line[i].unicode();
                            i++;
                        }
                        bool ok;
                        double val=text.toDouble(&ok);
                        if(ok)
                        {
                            sd=val;
                            fsd=true;
                        }
                    }
                    i++;
                    if(i>=size)
                        return 0;
                    {   // read m2
                        QString text;
                        while(i<size && line[i].unicode()!=',')
                        {
                            text+=line[i].unicode();
                            i++;
                        }
                        bool ok;
                        double val=text.toDouble(&ok);
                        if(ok)
                        {
                            ha=val*(M_PI/180.);
                            if(m_fLAngles)
                                ha=M_PI*2-ha;
                            fha=true;
                        }
                    }
                    i++;
                    if(i>=size)
                        return 0;
                    {   // read m3
                        QString text;
                        while(i<size && line[i].unicode()!=',')
                        {
                            text+=line[i].unicode();
                            i++;
                        }
                        bool ok;
                        double val=text.toDouble(&ok);
                        if(ok)
                        {
                            va=val*(M_PI/180.);
                            fva=true;
                            // Приводим зенитные углы к горизонту
                            va=(M_PI*0.5)-va;
                            fva=true;

                            //obs->m_vaq=m_job->m_zenith_angle_stdev;

                            if(va>(M_PI*0.5) || va<-(M_PI*0.5))
                            { // Обратный круг
                                if(va<0.)
                                    va+=M_PI*2;
                                va=M_PI-va;
                                if(fha)
                                    ha-=M_PI;
                            }
                        }
                    }
                    i++;
                    if(i>=size)
                        return 0;
                    {   // read time
                        QString text;
                        while(i<size && line[i].unicode()!=',')
                        {
                            text+=line[i].unicode();
                            i++;
                        }
                    }
                    // Добавить все актуальные измерения
                    if(fsd)
                    {
                        CHClass<CStationObservation> sto;
                        sto=new CStationObservation(ST_OBS_SD,sd,m_job->m_distance_stdev,height);
                        sto.Lock();
                        obs->m_list.append(sto);
                    }
                    if(fha)
                    {
                        CHClass<CStationObservation> sto;
                        sto=new CStationObservation(ST_OBS_HA,ha,m_job->m_angle_stdev,height);
                        sto.Lock();
                        obs->m_list.append(sto);
                    }
                    if(fva)
                    {
                        CHClass<CStationObservation> sto;
                        sto=new CStationObservation(ST_OBS_VA,va,m_job->m_zenith_angle_stdev,height);
                        sto.Lock();
                        obs->m_list.append(sto);
                    }
                }
            }
            break;
        }
    }
    return -1;
}


int CDevImportSokkia::scan_line(QString &line)
{
    unsigned size=line.size();
    if(size>4)
    {
        switch(line[0].unicode())
        {
        case '0':
            if(line[1].unicode()=='0')
            { // Comment lines
                if(line[2].unicode()=='N' && line[3].unicode()=='M')
                {
                    bool ok=false;
                    unsigned ver=0;
                    QStringRef tver(&line,7,line.size()-4);
                    ver=tver.toString().toInt(&ok);
                    if(ok)
                    {
                        if(ver<30)
                            m_format=1; // Старый формат
                    }
                }
            }else
            if(line[1].unicode()=='9' && line[2].unicode()=='F' &&
            ( line[3].unicode()=='1' || line[3].unicode()=='2' ) && size>37)
            {
                QString id;
                QString ids;
                double ha,va,sd;
                bool   fha=false;
                bool   fsd=false;
                bool   fva=false;
                switch(m_format)
                {
                default:
                    //if(size<=69)
                    if(size<84)
                        return -1;
                    { // Получить значения измерений
                        bool ok=false;
                        QStringRef ts(&line,68,16);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                        {
                            ha=val;
                            fha=1;
                        }
                    }
                    {
                        bool ok=false;
                        QStringRef ts(&line,52,16);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                        {
                            va=val;
                            fva=1;
                        }
                    }
                    {
                        bool ok=false;
                        QStringRef ts(&line,36,16);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                        {
                            sd=val;
                            fsd=1;
                        }
                    }
                    id=QStringRef(&line,20,16).toString().trimmed();
                    ids=QStringRef(&line,4,16).toString().trimmed();
                    break;
                case 1:
                    if(size<42)
                        return -1;

                    { // Получить значения измерений
                        bool ok=false;
                        QStringRef ts(&line,32,10);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                        {
                            ha=val;
                            fha=1;
                        }
                    }
                    {
                        bool ok=false;
                        QStringRef ts(&line,22,10);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                        {
                            va=val;
                            fva=1;
                        }
                    }
                    {
                        bool ok=false;
                        QStringRef ts(&line,12,10);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                        {
                            sd=val;
                            fsd=1;
                        }
                    }
                    id=QStringRef(&line,8,4).toString().trimmed();
                    ids=QStringRef(&line,4,4).toString().trimmed();
                    break;
                }
                // Корректровка значений углов
                if(line[3].unicode()=='1')
                {
                    if(fha)
                    {
                        ha=ha*M_PI/180;
                        if(m_fLAngles)
                            ha=M_PI*2-ha;
                    }
                    if(fva)
                    {
                        if(m_zan)
                            va=(90.-va)*M_PI/180.;
                        else
                            va=va*M_PI/180.;
                    }
                }else
                {
                    if(fha)
                    {
                        ha+=180.;
                        if(ha>=360.)
                            ha-=360.;
                        ha=ha*M_PI/180.;
                        if(m_fLAngles)
                            ha=M_PI*2-ha;
                    }
                    if(fva)
                    {
                        if(m_zan)
                            va=(va-270.)*M_PI/180.;
                        else
                            va=(180-va)*M_PI/180.;
                    }
                }
                if(m_st.Exist())
                {
                    CHClass<CStationNode> obs;
                    obs=m_st->GiveNode(id,*m_job);
                    // Добавить все актуальные измерения
                    if(fsd)
                    {
                        CHClass<CStationObservation> sto;
                        sto=new CStationObservation(ST_OBS_SD,sd,m_job->m_distance_stdev,m_th);
                        sto.Lock();
                        obs->m_list.append(sto);
                    }
                    if(fha)
                    {
                        CHClass<CStationObservation> sto;
                        sto=new CStationObservation(ST_OBS_HA,ha,m_job->m_angle_stdev,m_th);
                        sto.Lock();
                        obs->m_list.append(sto);
                    }
                    if(fva)
                    {
                        CHClass<CStationObservation> sto;
                        //sto=new CStationObservation(ST_OBS_VA,va,m_job->m_zenith_angle_stdev,m_th);
                        sto=new CStationObservation(ST_OBS_ZA,M_PI_2-va,m_job->m_zenith_angle_stdev,m_th);
                        sto.Lock();
                        obs->m_list.append(sto);
                    }
                }
            }else
            if((line[1].unicode()=='2' && line[2].unicode()=='T' && line[3].unicode()=='P') ||
               (line[1].unicode()=='2' && line[2].unicode()=='K' && line[3].unicode()=='I') ||
               (line[1].unicode()=='2' && line[2].unicode()=='N' && line[3].unicode()=='M')
            )
            { // Установка станции
                QString ids;
                m_ih=0.;
                m_st=0;
                switch(m_format)
                {
                default:
                    ids=QStringRef(&line,4,16).toString().trimmed();
                    if(size<=68)
                        return -1;
                    {
                        bool ok=false;
                        QStringRef ts(&line,68,16);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                        {
                            m_ih=val;
                        }
                    }
                    break;
                case 1:
                    ids=QStringRef(&line,4,4).toString().trimmed();
                    if(size<=38)
                        return -1;
                    {
                        bool ok=false;
                        QStringRef ts(&line,38,10);
                        double val=ts.toString().toDouble(&ok);
                        if(ok)
                            m_ih=val;
                    }
                    break;
                }
                if(ids.size())
                {
                    m_st=new CStation();
                    CHClass<CNode> node;
                    node=m_job->GiveNode(ids);
                    m_st->m_node=node;
                    m_st->m_height=m_ih;
                    m_st.Lock();
                    m_job->m_stations.append(m_st);
                }
            }else
            if(line[1].unicode()=='3' && line[2].unicode()=='N' && line[3].unicode()=='M' && size>4)
            {
                bool ok=false;
                QStringRef ts(&line,4,m_format?10:16);
                double val=ts.toString().toDouble(&ok);
                if(ok)
                    m_th=val;
            }
            break;
        }
    }
    return -1;
}

//======================================================
//
//  Trimble
//

void CDevImportTrimble::getM5Input(QString &line)
{
    unsigned offs=49;
    unsigned i;
    for(i=0;i<3;i++)
    {
        QStringRef lt(&line,offs,2);
        if(lt.toString()=="th")
        {
            m_th=getM5Value(line,i);
        }else
        if(lt.toString()=="ih")
        {
            m_ih=getM5Value(line,i);
            if(m_st.Exist())
                m_st->m_height=m_ih;
        }
        offs+=23;
    }
}

double CDevImportTrimble::getM5Value(QString &line,unsigned n)
{
    bool ok=false;
    QStringRef lv(&line,52+n*23,14);
    //double val=lv.toString().trimmed().toDouble(&ok);
    double val=lv.toString().toDouble(&ok);
    if(!ok)
        return 0.;

    QStringRef vt(&line,67+n*23,3);
    if(vt.toString()=="m  ")
        return val;
    if(vt.toString()=="ft ")
        return val*0.3048;
    if(vt.toString()=="DMS")
        return gmsrad(val*10000.);
    if(vt.toString()=="DEG")
        return val*(M_PI/180.);
    if(vt.toString()=="grad")
        return val*(M_PI/200.);
    if(vt.toString()=="%  ")
        return atan(val*0.01);
    return val;
}


int CDevImportTrimble::scan_line_m5(QString &line)
{
    QStringRef ts(&line,17,2);
    if(ts.toString()=="TI")
    {
        QStringRef f1(&line,21,5);
        if(f1.toString()[0].toAscii()==' ' || f1.toString().indexOf("INPUT")==0)
        {
            getM5Input(line);
        }//else
        //if(f1.toString().indexOf("Hz=0")==0)
        //{
        //    QStringRef f2(&line,49+23,2);
        //    if(f2.toString()=="Hz")
        //    {
        //        //state.m_Hz=getM5Value(line,1);
        //    }
        //}
    }else
    if(ts.toString()=="TG")
    {
        getM5Input(line);
    }
    if(ts.toString()=="PI")
    {
        QStringRef t1(&line,49,2);
        QStringRef t2(&line,49+23,2);
        QStringRef t3(&line,49+23*2,2);
        if(line[21].toAscii()=='S' && (t3.toString()=="Z " || t3.toString()=="h "))
        {
            m_z=getM5Value(line,2);
        }else
        {
            QString id;
            if(m_format==1)
            {
                QStringRef f2(&line,21,22);
                id=f2.toString().trimmed();
            }else
            {
                m_st_id=QStringRef(&line,21,15).toString().trimmed();
                id=QStringRef(&line,36,12).toString().trimmed();
            }
            if(id.size()==0)
                return -1;

            if(t1.toString()=="HD")
            {
                double hd=getM5Value(line,0);
                if(t2.toString()=="Hz")
                {
                    double ha=getM5Value(line,1);
                    if(m_fLAngles)
                        ha=M_PI*2-ha;
                    if(t3.toString()=="h " || t3.toString()=="Z ")
                    {
                        double z=getM5Value(line,2);
                        double h=z-m_ih+m_th;
                        if(t3.toString()=="Z ")
                            h-=m_z;

                        if(m_st.Exist())
                        {
                            CHClass<CStationNode> obs=m_st->GiveNode(id,*m_job);
                            obs->add_observation(ST_OBS_HD,hd,0.,m_th);
                            obs->add_observation(ST_OBS_HA,ha,0.,m_th);
                            obs->add_observation(ST_OBS_VD,h,0.,m_th);
                            /*
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_HD,hd,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_HA,ha,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_VD,h,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            */
                        }
                    }
                }
            }else
            if(t1.toString()=="SD")
            {
                double sd=getM5Value(line,0);
                if(t2.toString()=="Hz")
                {
                    double ha=getM5Value(line,1);
                    if(m_fLAngles)
                        ha=M_PI*2-ha;
                    if(t3.toString()=="V1")
                    { // Зенитный угол
                        double va=getM5Value(line,2);
                        if(m_st.Exist())
                        {
                            CHClass<CStationNode> obs=m_st->GiveNode(id,*m_job);
                            obs->add_observation(ST_OBS_SD,sd,0.,m_th);
                            obs->add_observation(ST_OBS_HA,ha,0.,m_th);
                            obs->add_observation(ST_OBS_ZA,va,0.,m_th);
                            /*
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_SD,sd,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_HA,ha,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_ZA,va,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            */
                        }
                        //pd.CorrectLR();
                    }else
                    if(t3.toString()=="V2" || t3.toString()=="V3" || t3.toString()=="V4")
                    {
                        double va=getM5Value(line,2);
                        if(m_st.Exist())
                        {
                            CHClass<CStationNode> obs=m_st->GiveNode(id,*m_job);
                            obs->add_observation(ST_OBS_SD,sd,0.,m_th);
                            obs->add_observation(ST_OBS_HA,ha,0.,m_th);
                            obs->add_observation(ST_OBS_VA,va,0.,m_th);
                            /*
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_SD,sd,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_HA,ha,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_VA,va,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            */
                        }
                        //pd.CorrectLR();
                    }
                }
            }else
            if(t1.toString()=="  ")
            {
                if(t2.toString()=="Hz")
                {
                    double ha=getM5Value(line,1);
                    if(m_fLAngles)
                        ha=M_PI*2-ha;
                    if(t3.toString()=="V1")
                    {
                        double va=getM5Value(line,2);
                        if(m_st.Exist())
                        {
                            CHClass<CStationNode> obs=m_st->GiveNode(id,*m_job);
                            obs->add_observation(ST_OBS_HA,ha,0.,m_th);
                            obs->add_observation(ST_OBS_ZA,va,0.,m_th);
                            /*
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_HA,ha,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_ZA,va,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            */
                        }
                        //pd.CorrectLR();
                    }else
                    if(t3.toString()=="V2" || t3.toString()=="V3" || t3.toString()=="V4")
                    {
                        double va=getM5Value(line,2);
                        if(m_st.Exist())
                        {
                            CHClass<CStationNode> obs=m_st->GiveNode(id,*m_job);
                            obs->add_observation(ST_OBS_HA,ha,0.,m_th);
                            obs->add_observation(ST_OBS_VA,va,0.,m_th);
                            /*
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_HA,ha,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            {
                                CHClass<CStationObservation> sto;
                                sto=new CStationObservation(ST_OBS_VA,va,0.,m_th);
                                sto.Lock();
                                obs->m_list.append(sto);
                            }
                            */
                        }
                        //pd.CorrectLR();
                    }
                }else
                if(m_format==1 && line[118].toAscii()=='O')
                {
                    m_st_id=id;
                }
            }/*else
            if(!memcmp(buff+49,"x  ",3))
            {
                double x=GetM5Value(buff,0);
                if(!memcmp(buff+49+23,"y  ",3))
                {
                    double y=GetM5Value(buff,1);
                    if(!memcmp(buff+49+23*2,"h  ",3))
                    {
                        double hd=sqrt(x*x+y*y);
                        double hz=0.;
                        if(hd>0.)
                            hz=atan2(y,x);
                        double z=GetM5Value(buff,2);
                        double h=z-state.m_z;
                        pd.type=0;
                        pd.num =rt++;
                        pd.kod =0;
                        pd.ha  =hz;
                        pd.va  =atan2(h,hd);
                        pd.dist=hd/cos(pd.va);
                        pd.th  =0;

                        pd.dwFlags=CGMDF_HD|CGMDF_HA|CGMDF_VD|CGMDF_IH|CGMDF_TH;

                        m_measurements.Append(&pd);
                    }
                }
            }*/else
            if((t1.toString()=="X " && t2.toString()=="Y ") ||
               (t1.toString()=="Y " && t2.toString()=="X ") ||
               (t1.toString()=="N_" && t2.toString()=="E_") ||
               (t1.toString()=="E_" && t2.toString()=="N_")
              )
            { // Станция
                m_st=0;
                if(id.size())
                {
                    m_st=new CStation();
                    CHClass<CNode> node;
                    node=m_job->GiveNode(id);
                    m_st->m_node=node;
                    m_st.Lock();
                    m_job->m_stations.append(m_st);
                }
            }
        }
    }
    return 1;
}

int CDevImportTrimble::scan_line(QString &line)
{
    if(line.indexOf("For M5")!=-1 || line.indexOf("For_M5")!=-1)
        return scan_line_m5(line);
    return 1;
}


}
