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

bool get_temp_file_name(std::string &rv,char *file);

namespace jobnet
{

//#define TEMP_INP_FILE "d:\\gl_temp.xml"
//#define TEMP_OUT_TEXT "d:\\adj_res.txt"
//#define TEMP_OUT_XML  "d:\\adj_res.xml"

#define TEMP_INP_FILE "gl_temp.xml"
#define TEMP_OUT_FILE "adj_res.txt"

int  CJob::check_available_dimentions(int a_mode)
{
    int rv=0;
    unsigned i;
    for(i=0;i<m_nodes.size();i++)
    {
        CHClass<CNode> node=m_nodes[i];
        if(node->toAdjust(a_mode))
        {
            if((node->m_fX && node->m_fY) || node->m_fZ)
            {
                if(node->m_fX && node->m_fY)
                    rv|=1;
                if(node->m_fZ)
                    rv|=2;
            }
        }
    }
    return rv;
}

bool CJob::save_adj_XML(const char *fname)
{
    FILE *out;
    int a_mode=(int)m_adjust_sit^1;
    int dim=check_available_dimentions(a_mode);
    if(!dim)
        return false;

    out=fopen(fname,"wt");
    if(!out)
        return false;

    fprintf(out,"<?xml version=\"1.0\" ?>\n");
    fprintf(out,"<!DOCTYPE gama-local SYSTEM \"gama-local.dtd\">\n");
    fprintf(out,"<gama-local version=\"2.0\">\n");
    fprintf(out,"<network axes-xy=\"ne\">\n");
    fprintf(out,"<description>");
//        {
//            QByteArray utf;
//            utf=m_title.toUtf8();
//            if(utf.size())
//                fwrite(utf.data(),1,utf.size(),out);
//        }
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
    { // Список всех уравниваемых узлов
        unsigned i;
        for(i=0;i<m_nodes.size();i++)
        {
            CHClass<CNode> node=m_nodes[i];
            if(node->toAdjust(a_mode))
            {
                fprintf(out,"<point id=\"");
                node->xml_utf_id(out);
                fprintf(out,"\"");
                if((dim&1) && node->m_fX && node->m_fY)
                {
                    fprintf(out," x=\"%.3f\"",node->fx);
                    fprintf(out," y=\"%.3f\"",node->fy);
                }
                if((dim&2) && node->m_fZ)
                    fprintf(out," z=\"%.3f\"",node->fz);

                if(((dim&1) && node->m_fX && node->m_fY) || ((dim&2) && node->m_fZ))
                {
                    fprintf(out," fix=\"");
                    if((dim&1) && node->m_fX && node->m_fY)
                    {
                        fprintf(out,"xy");
                    }

                    if((dim&2) && node->m_fZ)
                    {
                        fprintf(out,"z");
                    }

                    fprintf(out,"\"");
                }

                if(((dim&1) && (node->m_adj_flags&0x3)) || ((dim&2) && (node->m_adj_flags&0x0C)))
                {
                    fprintf(out," adj=\"");
                    if(dim&1)
                    {
                        if((node->m_adj_flags&0x03)==0x01)
                            fprintf(out,"xy");
                        else
                        if((node->m_adj_flags&0x03)==0x02)
                            fprintf(out,"XY");
                    }
                    if(dim&2)
                    {
                        if((node->m_adj_flags&0xC)==0x04)
                            fprintf(out,"z");
                        else
                        if((node->m_adj_flags&0xC)==0x08)
                            fprintf(out,"Z");
                    }
                    fprintf(out,"\"");
                }
                fprintf(out,"/>\n");
            }
        }
    }
/*        { // Контрольные координаты
        fprintf(out,"<coordinates>\n");
        unsigned i;
        for(i=0;i<m_nodes.size();i++)
        {
            CHClass<CNode> node=m_nodes[i];
            if((node->m_fX && node->m_fY)|| node->m_fZ)
            {
                if(node->toAdjust(a_mode))
                {
                    fprintf(out,"<point id=\"");
                    node->xml_utf_id(out);
                    fprintf(out,"\"");
                    if(node->m_fX)
                        fprintf(out," x=\"%.3f\"",node->fx);
                    if(node->m_fY)
                        fprintf(out," y=\"%.3f\"",node->fy);
                    if(node->m_fZ)
                        fprintf(out," z=\"%.3f\"",node->fz);
                    fprintf(out,"/>\n");
                 }
            }
        }
        fprintf(out,"</coordinates>\n");
    }*/
    { // Список станций
        unsigned i;
        for(i=0;i<m_stations.size();i++)
        {
            CHClass<CStation> st=m_stations[i];
            if(st->m_fActive)
            {
                fprintf(out,"<obs from=\"");
                st->m_node->xml_utf_id(out);
                fprintf(out,"\">\n");
                unsigned j;
                for(j=0;j<st->m_observations.size();j++)
                {
                    if(st->m_observations[j]->m_fActive && st->m_observations[j]->m_node->toAdjust(a_mode))
                        st->m_observations[j]->xml_save(out);
                }
                fprintf(out,"</obs>\n");
            }
        }
    }

    fprintf(out,"</points-observations>\n");
    fprintf(out,"</network>\n");
    fprintf(out,"</gama-local>\n");

    fclose(out);
    return true;
}

int CJob::Adjust(QTextBrowser *tb)
{
    update_node_stats();
    adjust_prepare();
    std::string inp_file;
    std::string out_file;
    if(!get_temp_file_name(inp_file,TEMP_INP_FILE))
        return -1;
    if(!get_temp_file_name(out_file,TEMP_OUT_FILE))
        return -1;

    {
        QFile::remove(out_file.c_str());
        {
#ifdef EXTERNAL_GNU_GAMA
            QString  qcl_path;
            QProcess qp;
            QStringList arg;
            arg.append(TEMP_INP_FILE);
            arg.append("-algorithm");
            arg.append("svd");
            //arg.append("gso");
            //arg.append("cholesky");
            //arg.append("envelope");
            arg.append("-language");
            arg.append("ru");  // en
            arg.append("--encoding");
            arg.append("utf-8");
            arg.append("--angles");
            arg.append("400");
            arg.append("--text");
            arg.append(TEMP_OUT_TEXT);
            arg.append("--xml");
            arg.append(TEMP_OUT_XML);

            //qp.start("d:\\Development\\Qt\\gl-adjust.exe",arg);
            qp.start("D:\\Projects\\GAjust\\debug\\gl-adjust.exe",arg);
            qp.waitForFinished(-1);
#else
            //int ec=adjust_gamma_local(TEMP_INP_FILE,m_adjust_algo.c_str(),::GNU_gama::local::ru,"utf-8","400",-1,0,TEMP_OUT_TEXT,TEMP_OUT_XML,0,-1);
            int ec=adjust_gamma_local(inp_file.c_str(),m_adjust_algo.c_str(),::GNU_gama::local::ru,"utf-8","360",-1,0,out_file.c_str(),0,0,-1);
#endif
            if(tb)
            {
#ifdef EXTERNAL_GNU_GAMA
                QString text=qp.readAll();
                int ec=qp.exitCode();
                {
                    QString text1;
                    text1.sprintf("\ngl-adjust.exe еxit code %d\n",ec);
                    text+=text1;
                }
#else
                QString text;
#endif

                if(QFile::exists(out_file.c_str()))
                {
                    QFile qf(out_file.c_str());
                    if(qf.open(QIODevice::ReadOnly | QIODevice::Text))
                    {
                        QString text1;
                        QByteArray rv=qf.readAll();
                        text1=QString::fromUtf8(rv.data(),rv.size());
                        text+=text1;
                    }
                }else
                {
                    text.sprintf("gama_local код возврата %d\n",ec);
                }
                tb->setText(text);
                //QFile::remove(TEMP_IN_FILE);
            }
            if(ec==0)
                adjust_finish();
        }
    }
    return 0;
}

}

