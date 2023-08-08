#ifndef _CJOB_H
#define _CJOB_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QList>
#include <stdio.h>
#include <QtGui/QTextBrowser>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include <gnu_gama/local/newnetwork.h>
#include <gnu_gama/local/pointid.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/cluster.h>
#include <gnu_gama/local/float.h>
#include <gnu_gama/outstream.h>
#include "c3dpoint.h"

#include "chcarray.h"

namespace jobnet
{


enum trsd_enum{
    aposteriori=0,
    apriori
};

#define NFL_X     0x0001
#define NFL_Y     0x0002
#define NFL_Z     0x0004

#define NFL_CX    0x0008
#define NFL_CY    0x0010
#define NFL_CZ    0x0020

#define NFL_AX    0x0040
#define NFL_AY    0x0080
#define NFL_AZ    0x0100

class CJob;
class CNode : public Class
{
public:
    QString m_id;
    QString m_comment;
    double fx,fy,fz;
    double cx,cy,cz;
    double ax,ay,az;
    double ee_a,ee_b;
    double ee_ak,ee_bk;
    double ee_alfa;

    unsigned m_adj_flags;

    unsigned m_stations;     // количество станций на данной точке
    unsigned m_observations; // количество наблюдений на данной точке

    union{
        unsigned m_uFlags;
        struct{
            unsigned m_fX:1;
            unsigned m_fY:1;
            unsigned m_fZ:1;
            unsigned m_fcX:1;
            unsigned m_fcY:1;
            unsigned m_fcZ:1;
            unsigned m_faX:1;
            unsigned m_faY:1;
            unsigned m_faZ:1;
            unsigned m_fee:1;
            unsigned m_feek:1;
        };
    };

    union{
        unsigned m_uAFlags;
        struct{
            unsigned m_fAdjustDisable:1;
            unsigned m_fAdjustInclude:1;
        };
    };

    // last adjust point id
    GNU_gama::local::PointID m_pp_id;

//-----------------------------------
    void get_utf_id(std::string &);
    void get_utf_id(QByteArray &);
    void xml_utf_id(FILE *);
    void xml_utf_comment(FILE *);
    void reset_stat()
    {
        m_stations    =0;
        m_observations=0;
    }
    bool toAdjust(int mode)
    {
        switch(mode)
        {
        case 0: // Все
            if(m_stations || m_observations)
                return true;
            break;
        case 1: // Только станции, пункты или пересечения
            if(m_stations || m_observations>1 || (((m_fX && m_fY) || m_fZ) && (m_stations || m_observations)))
                return true;
        }
        return false;
    }
    void fixXY(double x,double y)
    {
        fx=x;
        fy=y;
        m_fX=m_fY=1;
    }
    void fixXY(QPointF xy)
    {
        fx=xy.x();
        fy=xy.y();
        m_fX=m_fY=1;
    }
    void unfixXY()
    {
        m_fX=m_fY=0;
    }

    void fixZ(double z)
    {
        fz=z;
        m_fZ=1;
    }
    void unfixZ()
    {
        m_fZ=0;
    }

    bool GetXY(double &x,double &y)
    {
        if(m_faX && m_faY)
        {
            x=ax;
            y=ay;
            return true;
        }
        if(m_fcX && m_fcY)
        {
            x=cx;
            y=cy;
            return true;
        }
        if(m_fX && m_fY)
        {
            x=fx;
            y=fy;
            return true;
        }
        return false;
    }
    bool GetXY(QPointF &val)
    {
        if(m_faX && m_faY)
        {
            val.rx()=ax;
            val.ry()=ay;
            return true;
        }
        if(m_fcX && m_fcY)
        {
            val.rx()=cx;
            val.ry()=cy;
            return true;
        }
        if(m_fX && m_fY)
        {
            val.rx()=fx;
            val.ry()=fy;
            return true;
        }
        return false;
    }
    bool GetZ(double &z)
    {
        if(m_faZ)
        {
            z=az;
            return true;
        }
        if(m_fcZ)
        {
            z=cz;
            return true;
        }
        if(m_fZ)
        {
            z=fz;
            return true;
        }
        return false;
    }
    void getFlags(QString &text);

    CNode();
};

enum ST_OBS_ENUM
{
    ST_OBS_HA=0,
    ST_OBS_SD,
    ST_OBS_HD,
    ST_OBS_VA,
    ST_OBS_ZA,
    ST_OBS_VD,

    STF_OBS_HD,
    STF_OBS_VD
};

class CStation;
class CStationObservation : public Class
{
public:
    unsigned m_type;
    double   m_val;
    double   m_stdev;
    double   m_height;
    union{
        unsigned m_dwFlags;
        struct{
            unsigned m_fActive:1;
            unsigned m_fcval:1;    // Наличие корректированного значения
        };
    };
    double   m_cval;
    double   m_dev,m_devint;

    GNU_gama::local::Observation *m_pobs;

    void reset_adjustment()
    {
        m_fcval=0;
        m_pobs =0;
    }
    void update_adjustment(CStation *st,GNU_gama::local::LocalNetwork* IS);

    CStationObservation(unsigned type,double val,double stdev,double height)
    {
        m_type   = type;
        m_val    = val;
        m_stdev  = stdev;
        m_height = height;
        m_dwFlags= 0;
        m_fActive= 1;

        m_cval  =0.;
        m_dev   =0.;
        m_devint=0.;

        m_pobs  =0;
    }
};

class CStationNode : public Class
{
public:
    CStation      *m_st;
    CHClass<CNode> m_node;
    QList<CHClass<CStationObservation> > m_list;
    double m_max_error;

    union{
        unsigned m_dwFlags;
        struct{
            unsigned m_fActive:1;
        };
    };
    union{
        unsigned m_dwOFlags;
        struct{
            unsigned m_foHA:1;
            unsigned m_foSD:1;
            unsigned m_foHD:1;
            unsigned m_foVA:1;
            unsigned m_foZA:1;
            unsigned m_foVD:1;

            unsigned m_fmHD:1;
            unsigned m_fmVD:1;
        };
    };

    C3DPoint m_pv_pos;
    double   m_pvdir;
    union{
        unsigned m_dwPVFlags;
        struct{
            unsigned m_fXY:1;
            unsigned m_fZ:1;
            unsigned m_fdir:1;
        };
    };

    void add_observation(unsigned type,double val,double stdev,double height);
    void xml_save(FILE *);
    void to_network(CJob &job,GNU_gama::local::StandPoint *standpoint,std::vector<std::pair<GNU_gama::local::Double, bool> > &sigma);
    void xml_save_all(FILE *);
    void reset_adjustment();
    void update_adjustment(GNU_gama::local::LocalNetwork* IS);
    void set_corrected(ST_OBS_ENUM type,double cval,double dev,double devint);
    void update_o_flags();

    unsigned get_ha_hd_vd(double &ha,double &hd,double &vd);

    CStationNode(CStation *st)
    {
        m_st     =st;
        m_dwFlags=0;
        m_fActive=1;
        m_dwOFlags =0;
        m_dwPVFlags=0;
        m_max_error=0.;
    }
   ~CStationNode(){}
};

class CStation : public Class
{
public:
    double  m_height;
    CHClass<CNode> m_node;
    QList<CHClass<CStationNode> > m_observations;
    double m_max_error;

    CStationNode *FindNode(QString &name);
    CStationNode *GiveNode(QString &name,CJob &job);
    union{
        unsigned m_dwFlags;
        struct{
            unsigned m_fActive:1;
        };
    };
    void reset_adjustment();
    void previewCompute();

    CStation(){ m_height=0.; m_dwFlags=0.; m_fActive=1; m_max_error=0.; }
    ~CStation(){}
};

enum ImpDevEnum{
    ImpDev_Nikon=0,
    ImpDev_Sokkia,
    ImpDev_Trimble
};

class C2DPrintViewport;
class C2DPrintPage : public Class
{
public:
    C2DPrintViewport *m_host;

    unsigned    m_index;
    QPointF     m_pos;   // Левый нижний угол
    QPointF     m_size;  // Размер

    union{
        unsigned m_uiFlags;
        struct{
            unsigned m_fActive:1;
        };
    };

    C2DPrintPage(C2DPrintViewport *host,QPointF &pos,QPointF &size,unsigned index)
    {
        m_host   =host;
        m_pos    =pos;
        m_size   =size;
        m_index  =index;
        m_uiFlags=0x0001;
    }
};

typedef union{
    unsigned m_dwFlags;
    struct{
        unsigned m_fShowNames:1;
        unsigned m_fShowHeights:1;
        unsigned m_fShowSitPoints:1;
        unsigned m_fShowEllipses:1;
    };
}print_flags;

class C2DPrintViewport : public Class
{
public:
    unsigned      m_mode;
    unsigned      m_pages_dx;
    unsigned      m_pages_dy;
    QPointF       m_pos;
    QPointF       m_size;
    QPointF       m_page_size;
    QPointF       m_page_align_grid;
    double        m_mgn_top;
    double        m_mgn_bottom;
    double        m_mgn_left;
    double        m_mgn_right;
    double        m_scale;
    double        m_ellipse_scale;
    QString       m_name;

    print_flags   m_flags;

    CHCArray<C2DPrintPage> m_pages;
    QPrinter      m_printer;

    void setExtents(double xmin,double ymin,double xmax,double ymax);
    bool pageSetupDlg(QWidget *parent);
    bool pagePrint(CJob &,unsigned);
    void update();

    C2DPrintViewport(QString &name,QPointF &pos,QPointF &size);
};

class C2DViewportTranslator;

class CJob
{
public:
    QString     m_title;
    float       m_sigma_apr; // value of a priori reference standard deviation—square root of reference variance (default value 10)
    float       m_conf_pr;   // confidence probability used in statistical tests (dafault value 0.95)
    float       m_tol_abs;   // tolerance for identification of gross absolute terms in project equations (default value 1000 mm)
    trsd_enum   m_sigma_act; // actual type of reference standard deviation use in statistical tests (aposteriori | apriori); default value is aposteriori
    int         m_ucc;       // enables user to control if coordinates of constrained points are updated in iterative adjustment. (default value no)

    std::string m_adjust_algo;
    bool        m_adjust_sit;

    // Net
    float m_direction_stdev;    // defines the implicit value of observed direction (default value is not defined)
    float m_angle_stdev;        // defines the implicit value of observed angle (default value is not defined)
    float m_zenith_angle_stdev; // defines the implicit value of observed zenith angle (default value is not defined)
    float m_distance_stdev;     // defines the implicit value of observed horizontal distance (default value is not defined)

    QList<CHClass<CNode> > m_nodes;
    QList<CHClass<CStation> > m_stations;

    GNU_gama::local::LocalNetwork* m_IS; // last adjusted network
    QByteArray m_adj_res;

    // Подсистема печати
    CHCArray<C2DPrintViewport> m_views;
    CHClass<C2DPrintViewport>  m_curr_view;
    CHClass<C2DPrintPage>  m_curr_page;

    //-------------------------------------

    void process_line_nikon_sdr(QString &line);

    int ImportDeviceFile(QString &fname,int mode);
    void Clear();
    int  Adjust(QTextBrowser *);
    bool importGADJ(char *name);

    CNode *FindNode(QString &id);
    CNode *GiveNode(QString &id);

    CStation *FindStation(QString &id);

    void update_node_stats();
    void adjust_prepare();
    void adjust_finish();
    int  adjust_gamma_local(
                    const char *argv_1,
                    const char *argv_algo,
                    int   a_lang,
                    const char *argv_enc,
                    const char *argv_angles,
                    int   a_ellipsoid,
                    const char *a_latitude,
                    const char *argv_txt_out,
                    const char *argv_xmlout,
                    const char *argv_obsout,
                    int   a_covband);


    int save(QString &fname);
    int load(QString &fname);
    bool getextents(double &xmin,double &ymin,double &xmax,double &ymax);
    void cov_matrix(GNU_gama::local::LocalNetwork* netinfo,GNU_gama::OutStream &out);

    int exportTXT(QString &fname);
    int exportCSV(QString &fname);
    int exportDXF(QString &fname);

    void DrawNet(QPainter &dc,C2DViewportTranslator &vt,C2DPrintViewport *vp,C2DPrintPage *pp);

    CJob();
   ~CJob();
private:
    void read_node_data(GNU_gama::local::LocalNetwork* IS);
    void read_observ_data(GNU_gama::local::LocalNetwork* IS);
    bool setup_adjust_network(GNU_gama::local::LocalNetwork* IS);
    int  check_available_dimentions(int a_mode);
    bool save_adj_XML(const char *);
};

}

class CBasePoint : public jobnet::Class
{
public:
    QString m_name;
    QPointF m_xy;
    double  m_z;
    union{
        unsigned m_dwFlags;
        struct
        {
            unsigned m_fXY:1;
            unsigned m_fZ:1;
        };

    };

    CBasePoint()
    {
        m_xy.setX(0);
        m_xy.setY(0);
        m_z=0;
        m_dwFlags=0;
    }
    CBasePoint(CBasePoint &bp)
    {
        m_name   =bp.m_name;
        m_xy     =bp.m_xy;
        m_z      =bp.m_z;
        m_dwFlags=bp.m_dwFlags;
    }
    CBasePoint(CBasePoint *bp)
    {
        m_name   =bp->m_name;
        m_xy     =bp->m_xy;
        m_z      =bp->m_z;
        m_dwFlags=bp->m_dwFlags;
    }
    CBasePoint(QString &name,double x,double y,double z,unsigned flags)
    {
        m_name=name;
        m_xy.setX(x);
        m_xy.setY(y);
        m_z =z;
        m_dwFlags=flags;
    }
    CBasePoint(QString &name,QPointF xy,double z,unsigned flags)
    {
        m_name=name;
        m_xy=xy;
        m_z =z;
        m_dwFlags=flags;
    }

};


#endif
