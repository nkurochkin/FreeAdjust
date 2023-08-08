#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <stdio.h>

namespace jobnet
{


int CJob::exportTXT(QString &fname)
{
    FILE *out=_wfopen((wchar_t *)fname.unicode(),L"wt");
    if(!out)
        return -1;
    { // Список узлов
        int i;
        for(i=0;i<m_nodes.size();i++)
        {
            CHClass<CNode> node=m_nodes[i];
            {
                double x,y;
                if(node->GetXY(x,y))
                {
                    node->xml_utf_id(out);
                    fprintf(out,"\t%.3f",x);
                    fprintf(out,"\t%.3f",y);
                    double z;
                    if(node->GetZ(z))
                    {
                        fprintf(out,"\t%.3f",z);
                    }
                }
            }
            fprintf(out,"\n");
        }
    }
    fclose(out);
    return 0;
}

int CJob::exportCSV(QString &fname)
{
    FILE *out=_wfopen((wchar_t *)fname.unicode(),L"wt");
    if(!out)
        return -1;
    { // Список узлов
        int i;
        for(i=0;i<m_nodes.size();i++)
        {
            CHClass<CNode> node=m_nodes[i];
            {
                double x,y;
                if(node->GetXY(x,y))
                {
                    node->xml_utf_id(out);
                    fprintf(out,";%.3f",x);
                    fprintf(out,";%.3f",y);
                    double z;
                    if(node->GetZ(z))
                    {
                        fprintf(out,";%.3f",z);
                    }
                }
            }
            fprintf(out,"\n");
        }
    }
    fclose(out);
    return 0;
}


class CVBLayer
{
public:
    std::string m_name;

    CVBLayer &operator=(CVBLayer &src)
    {
        m_name =src.m_name;
        return *this;
    }
    void Init()
    {
    }
    CVBLayer()
    {
    }
    CVBLayer(CVBLayer &src)
    {
        m_name=src.m_name;
    }
    CVBLayer(const char *name)
    {
        m_name=name;
    }
};

class C3DExtents
{
public:
    C3DPoint min,max;

    void Middle(C3DPoint &mid)
    {
        mid=(min+max)*0.5;
    }

    void ExpandXY(double x,double y)
    {
        if(x<min.x)
            min.x=x;
        if(x>max.x)
            max.x=x;
        if(y<min.y)
            min.y=y;
        if(y>max.y)
            max.y=y;
    }
    void ExpandZ(double z)
    {
        if(z<min.z)
            min.z=z;
        if(z>max.z)
            max.z=z;
    }

    C3DExtents()
    {
        min= DBL_MAX;
        max=-DBL_MIN;
    }

};

class CDXF_Export
{
public:
    FILE *m_out;
    union
    {
        unsigned m_uFlags;
        struct{
            unsigned m_fSaveGrid:1;
        };
    };
    double      m_scale;
    C3DExtents  m_ext;
    std::vector<CVBLayer> m_layer_list;

    void _exportDXF12Header()
    {
        fprintf(m_out,
            "  0\nSECTION\n"
            "  2\nHEADER\n"
            "  9\n$ACADVER\n"
            "  1\nAC1009\n"
            "  9\n$INSBASE\n"
            " 10\n0.0\n"
            " 20\n0.0\n"
            " 30\n0.0\n");
        fprintf(m_out,
            "  9\n$EXTMIN\n 10\n%.64g\n 20\n%.64g\n 30\n%.64g\n",
            m_ext.min.y,m_ext.min.x,m_ext.min.z);
        fprintf(m_out,
            "  9\n$EXTMAX\n 10\n%.64g\n 20\n%.64g\n 30\n%.64g\n",
            m_ext.max.y,m_ext.max.x,m_ext.max.z);
        C3DPoint m;
        m_ext.Middle(m);
        fprintf(m_out,
            "  0\nENDSEC\n"
            "  0\nSECTION\n"
            "  2\nTABLES\n"
            "  0\nTABLE\n"
            "  2\nVPORT\n"
            " 70\n     2\n"
            "  0\nVPORT\n"
            "  2\n*ACTIVE\n"
            " 70\n     0\n"
            " 10\n0.0\n"
            " 20\n0.0\n"
            " 11\n1.0\n"
            " 21\n1.0\n"
            " 12\n%.64g\n"
            " 22\n%.64g\n"
            " 13\n0.0\n"
            " 23\n0.0\n"
            " 14\n1.0\n"
            " 24\n1.0\n"
            " 15\n0.0\n"
            " 25\n0.0\n"
            " 16\n0.0\n"
            " 26\n0.0\n"
            " 36\n1.0\n"
            " 17\n0.0\n"
            " 27\n0.0\n"
            " 37\n0.0\n"
            " 40\n297.0\n"
            " 41\n1.92798353909465\n"
            " 42\n50.0\n"
            " 43\n0.0\n"
            " 44\n0.0\n"
            " 50\n0.0\n"
            " 51\n0.0\n"
            " 71\n     0\n"
            " 72\n   100\n"
            " 73\n     1\n"
            " 74\n     1\n"
            " 75\n     0\n"
            " 76\n     0\n"
            " 77\n     0\n"
            " 78\n     0\n"
            "  0\nENDTAB\n"
            "  0\nTABLE\n"
            "  2\nLTYPE\n"
            " 70\n     1\n"
            "  0\nLTYPE\n"
            "  2\nCONTINUOUS\n"
            " 70\n    64\n"
            "  3\nSolid line\n"
            " 72\n    65\n"
            " 73\n     0\n"
            " 40\n0.0\n"
            "0\nENDTAB\n",
                m.y,
                m.x
            );

        fprintf(m_out,
                "0\nTABLE\n"
                "2\nLAYER\n"
                );
        {
            unsigned i;
            for(i=0;i<m_layer_list.size();i++)
            {
                CVBLayer *layer=&m_layer_list[i];
                std::string name;
                unsigned color=i&0xFF;
                name=layer->m_name;
                if(!name.size())
                    name="NONAME";
                //ToAcadLayer(name);
                if(!color)
                    color=1;
                else
                if(color==0xFF)
                    color=0xFE;

                fprintf(m_out,
                    "  0\nLAYER\n"
                    "  2\n%s\n"
                    " 70\n    64\n"
                    " 62\n    %u\n"
                    "  6\nCONTINUOUS\n",
                    name.c_str(),color
                    //FindACIColor(gfl->m_gc->GetPenColor())
                );
                i++;
            }
            if(m_fSaveGrid)
            { // SCALEGRID
                fprintf(m_out,
                    "  0\nLAYER\n"
                    "  2\nSCALEGRID\n"
                    " 70\n    64\n"
                    " 62\n    90\n"
                    "  6\nCONTINUOUS\n"
                );
            }
        }
        fprintf(m_out,
            "0\nENDTAB\n");
        {
            fprintf(m_out,
                "0\nTABLE\n"
                "2\nSTYLE\n"
                " 70\n     3\n"
                "  0\nSTYLE\n"
                "  2\nSTANDARD\n"
                " 70\n     0\n"
                " 40\n0.0\n"
                " 41\n1.0\n"
                " 50\n0.0\n"
                " 71\n     0\n"
                " 42\n0.2\n"
                "  3\ntxt\n"
                "  4\n\n"
                );
            //unsigned i;
            //for(i=0;i<m_TextStyles.GetQ();i++)
            //{
            //    m_TextStyles[i]->ExportDXF12(out);
            //}
            fprintf(m_out,"  0\nENDTAB\n");
        }
        fprintf(m_out,"  0\nENDSEC\n");
        fprintf(m_out,
            "  0\nSECTION\n"
            "  2\nBLOCKS\n");

        fprintf(m_out,
            "  0\nBLOCK\n"
            "  67\n     1\n"
            "  8\n0\n"
            "  2\nNODEBLK\n"
            "  70\n    0\n"
            "  10\n0.0\n"
            "  20\n0.0\n"
            "  30\n0.0\n"
            "  3\nNODEBLK\n"
            "  1\n\n"

            "  0\nCIRCLE\n"
            "8\n%s\n"
            "62\n%u\n"
            "10\n%.64g\n"
            "20\n%.64g\n"
            "30\n%.64g\n"
            "39\n%.64g\n"
            "40\n%.64g\n"

            "  0\nENDBLK\n"
            "  67\n     1\n"
            "  8\n0\n",
                "0",
                0,
                0.,//mid.x,
                0., //mid.y,
                0., //mid.z,
                0., //m_penwidth*scale,
                0.0005);

        if (m_fSaveGrid)
        {
            double delta = 0.003*m_scale;
            fprintf(m_out,
                "  0\nBLOCK\n"
                "  67\n     1\n"
                "  8\n0\n"
                "  2\nGRIDBLK\n"
                "  70\n    0\n"
                "  10\n0.0\n"
                "  20\n0.0\n"
                "  30\n0.0\n"
                "  3\nGRIDBLK\n"
                "  1\n\n"

                "  0\nLINE\n"
                "  8\nSCALEGRID\n"
                "  10\n%.6f\n"
                "  20\n0.0\n"
                "  30\n0.0\n"
                "  11\n%.6f\n"
                "  21\n0.0\n"
                "  31\n0.0\n"

                "  0\nLINE\n"
                "  8\nSCALEGRID\n"
                "  10\n0.0\n"
                "  20\n%.6f\n"
                "  30\n0.0\n"
                "  11\n0.0\n"
                "  21\n%.6f\n"
                "  31\n0.0\n"

                "  0\nENDBLK\n"
                "  67\n     1\n"
                "  8\n0\n",
                    -delta,
                    delta,
                    -delta,
                    delta
                );
        }
        // Блоки сохранить сдесь
        // ....
        fprintf(m_out,
            "  0\nENDSEC\n"
            "  0\nSECTION\n"
            "  2\nENTITIES\n");
    }

    void _exportDXF12Footer()
    {
        if(m_fSaveGrid)
        {
            double step=m_scale*0.1;
            double vxl=ceil(m_ext.min.x/step)*step - step;
            const char *lname = "SCALEGRID";
            unsigned ix=0;
            while((vxl<=m_ext.max.x + step) && ix<100)
            {
                    double vyl=ceil(m_ext.min.y/step)*step - step;
                    unsigned iy=0;
                    while((vyl<=m_ext.max.y + step) && iy<100)
                    {
                         fprintf(m_out,
                                "0\nINSERT\n"
                                "2\nGRIDBLK\n"
                                "8\n%s\n"
                                "10\n%.6f\n"
                                "20\n%.6f\n",
                                lname,
                                vxl,
                                vyl
                             );
                        vyl+=step;
                        iy++;
                    }
                 vxl+=step;
                 ix++;
            }
        }
        fprintf(m_out,
            "  0\nENDSEC\n");
        fprintf(m_out,
            "  0\nEOF\n");
    }

    CDXF_Export(FILE *out)
    {
        m_out=out;
        m_uFlags=0;
        m_scale=500.;
    }
    ~CDXF_Export()
    {
        if(m_out)
            fclose(m_out);
    }
};



int CJob::exportDXF(QString &fname)
{
    FILE *out=_wfopen((wchar_t *)fname.unicode(),L"wt");
    if(!out)
        return -1;
    CDXF_Export dxf(out);
    dxf.m_layer_list.push_back(CVBLayer("NODES"));
    dxf.m_scale=500.;
    // Вычисление экстентов
    {
        int i;
        for(i=0;i<m_nodes.size();i++)
        {
            CHClass<CNode> node=m_nodes[i];
            {
                double x,y;
                if(node->GetXY(x,y))
                    dxf.m_ext.ExpandXY(x,y);
                if(node->GetZ(x))
                    dxf.m_ext.ExpandZ(x);
            }
        }
    }
    dxf._exportDXF12Header();
    { // Список узлов
        int i;
        for(i=0;i<m_nodes.size();i++)
        {
            CHClass<CNode> node=m_nodes[i];
            double x,y,z=0.;
            if(node->GetXY(x,y))
            {
                node->GetZ(z);
                fprintf(out,
                        "0\nINSERT\n"
                        "8\n%s\n"
                        "62\n0\n"
                        "2\n%s\n"
                        "10\n%.64g\n"
                        "20\n%.64g\n"
                        "30\n%.64g\n"
                        "41\n%f\n"
                        "42\n%f\n"
                        "50\n%f\n",
                         dxf.m_layer_list[0].m_name.c_str(),
                         "NODEBLK",
                         y,x,z,
                         dxf.m_scale,dxf.m_scale,0.);
                std::string name=node->m_id.toStdString();
                fprintf(out,
                        "0\nTEXT\n"
                        "8\n%s\n"
                        "62\n%u\n"
                        "10\n%.64g\n"
                        "20\n%.64g\n"
                        "30\n%.64g\n"
                        "40\n%.64g\n"
                        "1\n%s\n"
                        "7\n%s\n"
                        "50\n%.4f\n"
                        "51\n%.1f\n"
                        "72\n%u\n"
                        //"11\n%.64g\n"
                        //"21\n%.64g\n"
                        //"31\n%.64g\n"
                        "73\n%u\n",
                        dxf.m_layer_list[0].m_name.c_str(),
                        0, // pencolor,
                        y+0.001*dxf.m_scale,
                        x,
                        z,
                        0.0015*dxf.m_scale,
                        name.c_str(),
                        "STANDARD",//(LPCTSTR)m_TextStyle,
                        0.,//angle,
                        0.,//obliqueAngle,
                        0, //m_halign,
                        //y,
                        //x,
                        //0., //sv.z
                        0   // valign
                     );

            }
        }
    }
    dxf._exportDXF12Footer();
    return 0;
}


}

void MainWindow::exportTXT()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
         tr("Export plain text file"), "", tr("Text files (*.txt)"));
    if( !fileName.isNull() )
    {
        m_job.exportTXT(fileName);
    }
}

void MainWindow::exportCSV()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
         tr("Export CSV text file"), "", tr("Text files (*.csv)"));
    if( !fileName.isNull() )
    {
        m_job.exportCSV(fileName);
    }
}

void MainWindow::exportDXF()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
         tr("Export DXF file"), "", tr("Autocad files (*.dxf)"));
    if( !fileName.isNull() )
    {
        m_job.exportDXF(fileName);
    }
}

