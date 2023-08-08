#include "CJob.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QTextCodec>

#include <gnu_gama/local/language.h>
#include <gnu_gama/ellipsoids.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <stdio.h>

namespace jobnet
{

Class::Class()
{
    m_lockcounter=0;
}

Class::~Class(){}
int  Class::Lock()
{
    return m_lockcounter.ref();
}

int  Class::Unlock()
{
    int rv=m_lockcounter;
    if(m_lockcounter>=0)
        m_lockcounter.deref();
    if(m_lockcounter<0)
        delete this;
    return rv;
}


CJob::CJob()
{
    m_sigma_apr=10.f;   // value of a priori reference standard deviation—square root of reference variance (default value 10)
    m_conf_pr  =0.95f;  // confidence probability used in statistical tests (dafault value 0.95)
    m_tol_abs  =1000.f; // tolerance for identification of gross absolute terms in project equations (default value 1000 mm)
    m_sigma_act=aposteriori; // actual type of reference standard deviation use in statistical tests (aposteriori | apriori); default value is aposteriori
    m_ucc      =0;

    m_direction_stdev    =10; // defines the implicit value of observed direction (default value is not defined)
    m_angle_stdev        =10; // defines the implicit value of observed angle (default value is not defined)
    m_zenith_angle_stdev =10; // defines the implicit value of observed zenith angle (default value is not defined)
    m_distance_stdev     =5; // defines the implicit value of observed horizontal distance (default value is not defined)

    m_adjust_algo="svd";
    m_IS=0;
}

CJob::~CJob()
{
    if(m_IS)
        delete m_IS;
}

CNode *CJob::FindNode(QString &id)
{
    int i;
    for(i=0;i<m_nodes.size();i++)
    {
        if(m_nodes[i]->m_id==id)
            return m_nodes[i].Lock();
    }
    return 0;
}

CNode *CJob::GiveNode(QString &id)
{
    int i;
    for(i=0;i<m_nodes.size();i++)
    {
        if(m_nodes[i]->m_id==id)
            return m_nodes[i].Lock();
    }
    CHClass<CNode> rv=new CNode();
    rv->m_id=id;
    rv.Lock();
    m_nodes.append(rv);
    return rv.Lock();
}

CStation *CJob::FindStation(QString &id)
{
    int i;
    for(i=0;i<m_stations.size();i++)
    {
        CHClass<CStation> st=m_stations[i];
        if(st->m_node->m_id==id)
            return st.Lock();
    }
    return 0;
}

void   CJob::update_node_stats()
{
    int i;
    for(i=0;i<m_nodes.size();i++)
    {
        m_nodes[i]->reset_stat();
    }

    for(i=0;i<m_stations.size();i++)
    {
        CHClass<CStation> st=m_stations[i];
        int j;
        for(j=0;j<st->m_observations.size();j++)
        {
            st->m_observations[j]->update_o_flags();
        }
    }

    for(i=0;i<m_stations.size();i++)
    {
        CHClass<CStation> st=m_stations[i];
        if(st->m_fActive)
        {
            st->m_node->m_stations++;
            int j;
            for(j=0;j<st->m_observations.size();j++)
            {
                CHClass<CStationNode> stn=st->m_observations[j];
                if(stn->m_fActive)
                {
                    // Проверить есть ли активные измерения
                    int jj;
                    unsigned mq=0;
                    for(jj=0;jj<stn->m_list.size();jj++)
                    {
                        if(stn->m_list[jj]->m_fActive)
                            mq++;
                    }
                    if(mq)
                        st->m_observations[j]->m_node->m_observations++;
                }
            }
        }
    }
}

void CJob::adjust_prepare()
{
    int i;
    // Сброс флагов уравненных и расчитанных координат
    for(i=0;i<m_nodes.size();i++)
    {
        m_nodes[i]->m_uFlags&=0x7;
    }
    // Сброс корректированных значений измерений
    for(i=0;i<m_stations.size();i++)
    {
        m_stations[i]->reset_adjustment();
    }
}

void CJob::adjust_finish()
{ // Вычисление возможных координат после уравнивания
    int i;
    unsigned rcount;
    for(i=0;i<m_nodes.size();i++)
    {
        CHClass<CNode> np=m_nodes[i];
        if(np->m_fX)
        {
            if(!np->m_faX)
            {
                np->ax=np->fx;
                np->m_faX=1;
            }
        }
        if(np->m_fY)
        {
            if(!np->m_faY)
            {
                np->ay=np->fy;
                np->m_faY=1;
            }
        }
        if(np->m_fZ)
        {
            if(!np->m_faZ)
            {
                np->az=np->fz;
                np->m_faZ=1;
            }
        }
    }
    do{
        rcount=0; // счетчик изменений в течении иттерации
        for(i=0;i<m_stations.size();i++)
        {
            CHClass<CStation> st=m_stations[i];
            if(st->m_fActive)
            {
                if(st->m_node->m_faX && st->m_node->m_faY)
                { // Есть уравненные координаты станции
                    // Поиск точки ориентирования
                    int j;
                    CHClass<CStationNode> stn_bs;
                    double bs_ha;
                    for(j=0;j<st->m_observations.size() && !stn_bs.Exist();j++)
                    {
                        CHClass<CStationNode> stn=st->m_observations[j];
                        if(stn->m_fActive && stn->m_node->m_faX && stn->m_node->m_faY)
                        {
                            // Проверить есть ли направление
                            int jj;
                            for(jj=0;jj<stn->m_list.size();jj++)
                            {
                                if(stn->m_list[jj]->m_fActive && stn->m_list[jj]->m_type==ST_OBS_HA)
                                {
                                    stn_bs=stn;
                                    bs_ha=stn->m_list[jj]->m_val;
                                    break;
                                }
                            }
                        }
                    }
                    if(stn_bs.Exist())
                    { // Определяем координаты всех точек снятых с данной станции
                        double dx=stn_bs->m_node->ax-st->m_node->ax;
                        double dy=stn_bs->m_node->ay-st->m_node->ay;
                        double bsa=atan2(dy,dx);

                        for(j=0;j<st->m_observations.size();j++)
                        {
                            CHClass<CStationNode> stn=st->m_observations[j];
                            if(stn->m_fActive && (!stn->m_node->m_faX || !stn->m_node->m_faY))
                            {
                                // Найти HA и HD
                                double ha,hd,vd;
                                if((stn->get_ha_hd_vd(ha,hd,vd)&3)==3)
                                { // Возможно определение координат пикета
                                    ha-=bs_ha;
                                    double ta=bsa+ha;
                                    stn->m_node->ax=st->m_node->ax+cos(ta)*hd;
                                    stn->m_node->ay=st->m_node->ay+sin(ta)*hd;
                                    stn->m_node->m_faX=1;
                                    stn->m_node->m_faY=1;
                                }
                            }
                        }
                    }
                }
                if(st->m_node->m_faZ)
                {
                    int j;
                    for(j=0;j<st->m_observations.size();j++)
                    {
                        CHClass<CStationNode> stn=st->m_observations[j];
                        if(stn->m_fActive && !stn->m_node->m_faZ)
                        {
                            double ha,hd,vd;
                            if(stn->get_ha_hd_vd(ha,hd,vd)&4)
                            { // Возможно определение высоты пикета
                                stn->m_node->az=st->m_node->az+vd;
                                stn->m_node->m_faZ=1;
                            }
                        }
                    }
                }
            }
        }
    }while(rcount); // продолжить если иттерация чтото поменяла
}

void CJob::Clear()
{
    m_stations.clear();
    m_nodes.clear();
    m_curr_page=0;
    m_curr_view=0;
    m_views.Clear();
    m_title.clear();
    {
        CHClass<C2DPrintViewport> pw=new C2DPrintViewport(QString("Общий вид"),QPointF(0.,0.),QPointF(100.,100.));
        m_views.Append(pw);
        m_curr_view=pw;
    }
}

int CJob::save(QString &fname)
{
    FILE *out=_wfopen((wchar_t *)fname.unicode(),L"wt");
    if(!out)
        return -1;
    fprintf(out,"<?xml version=\"1.0\" ?>\n");
    fprintf(out,"<!DOCTYPE gama-local SYSTEM \"gama-local.dtd\">\n");
    fprintf(out,"<gama-local version=\"2.0\">\n");
    fprintf(out,"<network axes-xy=\"ne\">\n");
    fprintf(out,"<description>\n");
    {
        QByteArray str=m_title.toUtf8();
        fwrite(str.data(),1,str.size(),out);
    }
    fprintf(out,"</description>\n");
    fprintf(out,"<parameters\n");
    fprintf(out,"  sigma-apr=\"%.0f\"\n",m_sigma_apr);
    fprintf(out,"  conf-pr=\"%.2f\"\n",m_conf_pr);
    fprintf(out,"  tol-abs=\"%.0f\"\n",m_tol_abs);
    fprintf(out,"  sigma-act=\"%s\"\n",m_sigma_act?"apriori":"aposteriori");
    fprintf(out,"/>\n");

    fprintf(out,"<points-observations\n");
    if(m_distance_stdev!=0.)
        fprintf(out,"distance-stdev=\"%.1f\"\n",m_distance_stdev);
    if(m_direction_stdev!=0.)
        fprintf(out,"direction-stdev=\"%.1f\"\n",m_direction_stdev);
    if(m_angle_stdev!=0.)
        fprintf(out,"angle-stdev=\"%.1f\"\n",m_angle_stdev);
    if(m_zenith_angle_stdev!=0.)
        fprintf(out,"zenith-angle-stdev=\"%.1f\"\n",m_zenith_angle_stdev);
    fprintf(out,">\n");

    { // Список узлов
        int i;
        for(i=0;i<m_nodes.size();i++)
        {
            CHClass<CNode> node=m_nodes[i];
            {
                fprintf(out,"<point id=\"");
                node->xml_utf_id(out);
                fprintf(out,"\" comment=\"");
                node->xml_utf_comment(out);
                fprintf(out,"\"");
                if(node->m_fX)
                    fprintf(out," x=\"%.3f\"",node->fx);
                if(node->m_fY)
                    fprintf(out," y=\"%.3f\"",node->fy);                
                if(node->m_fZ)
                    fprintf(out," z=\"%.3f\"",node->fz);

                if((node->m_fX && node->m_fY) || node->m_fZ)
                {
                    fprintf(out," fix=\"");
                    if(node->m_fX && node->m_fY)
                        fprintf(out,"xy");

                    if(node->m_fZ)
                        fprintf(out,"z");

                    fprintf(out,"\"");
                }

                if(node->m_adj_flags)
                {
                    fprintf(out," adj=\"");
                    if((node->m_adj_flags&0x03)==1)
                        fprintf(out,"xy");
                    else
                    if((node->m_adj_flags&0x03)==2)
                        fprintf(out,"XY");

                    if((node->m_adj_flags&0x0C)==0x4)
                        fprintf(out,"z");
                    else
                    if((node->m_adj_flags&0x0C)==0x8)
                        fprintf(out,"Z");

                    fprintf(out,"\"");
                }
                fprintf(out,"/>\n");
            }
        }
    }
    { // Полный список станций
        int i;
        for(i=0;i<m_stations.size();i++)
        {
            CHClass<CStation> st=m_stations[i];
            if(st->m_fActive)
            {
                fprintf(out,"<obs from=\"");
                st->m_node->xml_utf_id(out);
                fprintf(out,"\" from_dh=\"%.3f\" active=\"%u\" >\n",st->m_height,st->m_fActive);
                int j;
                for(j=0;j<st->m_observations.size();j++)
                {
                    st->m_observations[j]->xml_save_all(out);
                }
                fprintf(out,"</obs>\n");
            }
        }
    }

    fprintf(out,"</points-observations>\n");
    fprintf(out,"</network>\n");
    fprintf(out,"</gama-local>\n");

    fclose(out);
    return 0;
}

bool CJob::getextents(double &xmin,double &ymin,double &xmax,double &ymax)
{
    bool rv=false;
    xmin=ymin= DBL_MAX;
    xmax=ymax=-DBL_MAX;
    int i;
    for(i=0;i<m_nodes.size();i++)
    {
        jobnet::CNode *np=m_nodes[i];
        double x,y;
        if(np->GetXY(x,y))
        {
            rv=true;
            if(xmax<x)
                xmax=x;
            if(ymax<y)
                ymax=y;

            if(xmin>x)
                xmin=x;
            if(ymin>y)
                ymin=y;
        }
    }
    return rv;
}


//=======================================
//  CNode
//
CNode::CNode()
{
    m_uFlags =0;
    m_uAFlags=0;
    m_adj_flags=0x5;

    fx=fy=fz=0.;
    cx=cy=cz=0.;
    ax=ay=az=0.;

    m_stations      =0;
    m_observations  =0;

}

void CNode::get_utf_id(std::string &id)
{
    QByteArray utf;
    utf=m_id.toUtf8();
    id.append(utf.data(),utf.size());
}

void CNode::get_utf_id(QByteArray &utf)
{
    utf=m_id.toUtf8();
}

void CNode::xml_utf_id(FILE *out)
{
    QByteArray utf;
    utf=m_id.toUtf8();
    fwrite(utf.data(),1,utf.size(),out);
}

void CNode::xml_utf_comment(FILE *out)
{
    QByteArray utf;
    utf=m_comment.toUtf8();
    fwrite(utf.data(),1,utf.size(),out);
}

void CNode::getFlags(QString &text)
{
    if(m_stations)
        text+="S";
    if(m_observations==1)
        text+="n";
    else
    if(m_observations>1)
        text+="N";
    if(m_fX && m_fY)
        text+="xy";
    if(m_fZ)
        text+="z";
}

CStationNode *CStation::GiveNode(QString &name,CJob &job)
{
    CHClass<CStationNode> obs;
    CHClass<CNode> node;

    if(name.size())
    { // Look for node
        int j;
        for(j=0;j<job.m_nodes.size();j++)
        {
            if(job.m_nodes[j]->m_id==name)
            {
                node=job.m_nodes[j];
                break;
            }
        }
        if(!node.Exist())
        { // add new node
            node=new CNode();
            node->m_id=name;
            node.Lock();
            job.m_nodes.append(node);
        }
    }else
    {
        node=new CNode();
    }
    // Поиск измерений на узел
    {
        int i;
        for(i=0;i<m_observations.size();i++)
        {
            if(m_observations[i]->m_node==node)
            {
                obs=m_observations[i];
                break;
            }
        }        
        if(!obs.Exist())
        { // Добавить измерения на узел
            obs=new CStationNode(this);
            obs->m_node=node;
            obs->Lock();
            m_observations.append(obs);
        }
    }
    return obs.Lock();
}

CStationNode *CStation::FindNode(QString &name)
{
    // Поиск измерений на узел
    int i;
    for(i=0;i<m_observations.size();i++)
    {
        CHClass<CStationNode> obs=m_observations[i];
        if(obs->m_node->m_id==name)
            return obs.Lock();
    }
    return 0;
}

void CStation::reset_adjustment()
{
    int i;
    for(i=0;i<m_observations.size();i++)
    {
        m_observations[i]->reset_adjustment();
    }
}

void CStation::previewCompute()
{
    int i;
    CHClass<CStationNode> obs_bs;
    double bs_ha;
    // Поиск BS
    for(i=0;i<m_observations.size();i++)
    {
        CHClass<CStationNode> obs=m_observations[i];
        double ha,hd,vd;
        unsigned flags=obs->get_ha_hd_vd(ha,hd,vd);
        if(flags&1) // есть HA
        {
            bs_ha=ha;
            obs_bs=obs;
            break;
        }
    }
    // расчет координат
    for(i=0;i<m_observations.size();i++)
    {
        CHClass<CStationNode> obs=m_observations[i];
        double ha,hd,vd;
        unsigned flags=obs->get_ha_hd_vd(ha,hd,vd);
        obs->m_dwPVFlags=0;
        if(flags&1) // есть HA
        {
            ha-=bs_ha;
            if(ha<M_PI*2)
                ha+=M_PI*2;
            if(ha<0)
                ha+=M_PI*2;

            obs->m_pvdir=ha;
            obs->m_fdir=1;
            if(flags&2)
            {
                obs->m_pv_pos.x=cos(ha)*hd;
                obs->m_pv_pos.y=sin(ha)*hd;
                obs->m_fXY=1;
            }
            if(flags&4)
            {
                obs->m_pv_pos.z=vd;
                obs->m_fZ=1;
            }
        }
    }

}

void CStationNode::update_o_flags()
{
    m_dwOFlags=0;
    int i;
    for(i=0;i<m_list.size();i++)
    {
        CStationObservation *obs=m_list[i].m_val;
        if(obs->m_fActive)
            m_dwOFlags|=1<<obs->m_type;
    }
    if(m_dwOFlags&(1<<ST_OBS_SD))
    {
        if(m_dwOFlags&((1<<ST_OBS_ZA)|(1<<ST_OBS_VA)))
            m_dwOFlags|=(1<<STF_OBS_HD)|(1<<STF_OBS_VD);
    }
    if(m_dwOFlags&(1<<ST_OBS_HD))
        m_dwOFlags|=(1<<STF_OBS_HD);
    if(m_dwOFlags&(1<<ST_OBS_VD))
        m_dwOFlags|=(1<<STF_OBS_VD);
}

void CStationNode::add_observation(unsigned type,double val,double stdev,double height)
{
    CHClass<CStationObservation> sto;
    if(type==ST_OBS_HA)
    { // поиск и корректировка обратных измерений
        if(val>M_PI)  val-=M_PI*2;
        if(val<-M_PI) val+=M_PI*2;
        unsigned i;
        for(i=0;i<m_list.size();i++)
        {
            if(m_list[i]->m_type==ST_OBS_HA)
            {
                double v=m_list[i]->m_val;
                if(val>M_PI)  val-=M_PI*2;
                if(val<-M_PI) val+=M_PI*2;
                if(fabs(val-v)>(M_PI*0.5)) // похоже на обратное измерение
                {
                    val+=M_PI;
                    if(val>M_PI)
                        val-=M_PI*2;
                    break;
                }
            }
        }
    }
    sto=new CStationObservation(type,val,stdev,height);
    sto.Lock();
    m_list.append(sto);
}


unsigned CStationNode::get_ha_hd_vd(double &pha,double &phd,double &pvd)
{
    // Найти HA и HD
    double ha,hd,va,vd,sd;
    double vah,sdh;
    unsigned haq=0,hdq=0,vaq=0,vdq=0,sdq=0;

    int i;
    for(i=0;i<m_list.size();i++)
    {
        if(m_list[i]->m_fActive)
        {
            CStationObservation *obs=m_list[i].m_val;
            double val=obs->m_val;
            switch(obs->m_type)
            {
            case ST_OBS_HA:
                if(val>M_PI)  val-=M_PI*2;
                if(val<-M_PI) val+=M_PI*2;

                if(!haq)
                    ha=val;
                else
                    ha+=val;
                haq++;
                break;
            case ST_OBS_SD:
                if(!sdq)
                {
                    sd =val;
                    sdh=obs->m_height;
                    sdq++;
                }else
                if(obs->m_height==sdh)
                {
                    sd+=val;
                    sdq++;
                }
                break;
            case ST_OBS_HD:
                if(!hdq)
                    hd=val;
                else
                    hd+=val;
                hdq++;
                break;
            case ST_OBS_ZA:
                val=M_PI_2-val;
            case ST_OBS_VA:
                if(val>M_PI)  val-=M_PI*2;
                if(val<-M_PI) val+=M_PI*2;

                if(!vaq)
                {
                    va=val;
                    vah=obs->m_height;
                    vaq++;
                }else
                if(obs->m_height==vah)
                {
                    va+=val;
                    vaq++;
                }
                break;
            case ST_OBS_VD:
                if(!vdq)
                    vd =val+m_st->m_height-obs->m_height;
                else
                    vd+=val+m_st->m_height-obs->m_height;
                vdq++;
                break;
            }
        }
    }
    // Проведено усреднение результатов
    if(vaq)
        va/=vaq;
    if(sdq)
        sd/=sdq;
    if(vaq && sdq && vah==sdh)
    {
        double tvd=sin(va)*sd+m_st->m_height-vah;
        double thd=cos(va)*sd;
        unsigned q=(vaq+sdq)/2;
        if(hdq)
            hd+=thd*q;
        else
            hd=thd*q;
        if(vdq)
            vd+=tvd*q;
        else
            vd=tvd*q;
        vdq+=q;
        hdq+=q;
    }

    unsigned rv=0;
    if(haq)
    {
        ha/=haq;
        pha=ha;
        rv|=1;
    }
    if(hdq)
    {
        hd/=hdq;
        phd=hd;
        rv|=2;
    }
    if(vdq)
    {
        vd/=vdq;
        pvd=vd;
        rv|=4;
    }
    return rv;
}

void CStationNode::set_corrected(ST_OBS_ENUM type,double cval,double dev,double devint)
{
    int i;
    for(i=0;i<m_list.size();i++)
    {
        CHClass<CStationObservation> sto=m_list[i];
        if(sto->m_type==type)
        {
            sto->m_cval=cval;
            sto->m_dev =dev;
            sto->m_devint=devint;
            sto->m_fcval=1;
        }
    }
}

void CStationNode::reset_adjustment()
{
    int i;
    for(i=0;i<m_list.size();i++)
    {
        m_list[i]->reset_adjustment();
    }
}

void CStationNode::update_adjustment(GNU_gama::local::LocalNetwork* IS)
{
    m_max_error=0.;
    int i;
    for(i=0;i<m_list.size();i++)
    {
        CStationObservation *sto=m_list[i];
        sto->update_adjustment(m_st,IS);
        if(sto->m_fcval)
        {
            if(m_max_error<sto->m_devint)
                m_max_error=sto->m_devint;
        }
    }
}


#define GA_ANGLE_UNITS 200.


void CStationNode::xml_save(FILE *out)
{
    double hd=0;
    int i;
    for(i=0;i<m_list.size();i++)
    {
        CHClass<CStationObservation> sto=m_list[i];
        if(sto->m_fActive)
        switch(sto->m_type)
        {
        case ST_OBS_HA:
            fprintf(out," <direction to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.10f\"",sto->m_val*(GA_ANGLE_UNITS/M_PI));
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," />\n");
            break;
        case ST_OBS_VA:
            fprintf(out," <z-angle to=\"");
            m_node->xml_utf_id(out);
            {
                double v=(M_PI_2-sto->m_val)*(GA_ANGLE_UNITS/M_PI);
                fprintf(out,"\" val=\"%.10f\"",v);
            }
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);            
            fprintf(out," from_dh=\"%.3f\"",m_st->m_height);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," />\n");
            break;
        case ST_OBS_ZA:
            fprintf(out," <z-angle to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.10f\"",sto->m_val*(GA_ANGLE_UNITS/M_PI));
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," from_dh=\"%.3f\"",m_st->m_height);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," />\n");
            break;
        case ST_OBS_SD:
            fprintf(out," <s-distance to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.4f\"",sto->m_val);
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," from_dh=\"%.3f\"",m_st->m_height);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," />\n");
            break;
        case ST_OBS_HD:
            fprintf(out," <distance to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.4f\"",sto->m_val);
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," from_dh=\"%.3f\"",m_st->m_height);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," />\n");
            hd=sto->m_val;
            break;
        case ST_OBS_VD:
            fprintf(out," <dh from=\"");
            m_st->m_node->xml_utf_id(out);
            fprintf(out,"\" to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.4f\"",sto->m_val+m_st->m_height-sto->m_height);
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            else
            if(hd>100.)
                fprintf(out," dist=\"%.1f\"",hd*0.001);
            else
                fprintf(out," dist=\"0.1\"");
            fprintf(out," />\n");
            break;
        }
    }
}

void CStationNode::xml_save_all(FILE *out)
{
    fprintf(out," <target name=\"");
    m_node->xml_utf_id(out);
    fprintf(out,"\" active=\"%u\" />\n",m_fActive);
    int i;
    for(i=0;i<m_list.size();i++)
    {
        CHClass<CStationObservation> sto=m_list[i];
        switch(sto->m_type)
        {
        case ST_OBS_HA:
            fprintf(out," <direction to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.10f\"",sto->m_val*(GA_ANGLE_UNITS/M_PI));
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," active=\"%u\" />\n",sto->m_fActive);
            break;
        case ST_OBS_VA:
            fprintf(out," <z-angle to=\"");
            m_node->xml_utf_id(out);
            {
                double v=(M_PI_2-sto->m_val)*(GA_ANGLE_UNITS/M_PI);
                fprintf(out,"\" val=\"%.10f\"",v);
            }
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," active=\"%u\" />\n",sto->m_fActive);
            break;
        case ST_OBS_ZA:
            fprintf(out," <z-angle to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.10f\"",sto->m_val*(GA_ANGLE_UNITS/M_PI));
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," active=\"%u\" />\n",sto->m_fActive);
            break;
        case ST_OBS_SD:
            fprintf(out," <s-distance to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.6f\"",sto->m_val);
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," active=\"%u\" />\n",sto->m_fActive);
            break;
        case ST_OBS_HD:
            fprintf(out," <distance to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.4f\"",sto->m_val);
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," />\n");
            break;
        case ST_OBS_VD:
            fprintf(out," <dh to=\"");
            m_node->xml_utf_id(out);
            fprintf(out,"\" val=\"%.4f\"",sto->m_val);
            if(sto->m_stdev!=0.)
                fprintf(out," stdev=\"%.1f\"",sto->m_stdev);
            fprintf(out," to_dh=\"%.3f\"",sto->m_height);
            fprintf(out," />\n");
            break;
        }
    }
}


}
