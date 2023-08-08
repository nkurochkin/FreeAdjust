#include "CJob.h"
#include "cxmlparser.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace jobnet
{


class CGKF_target : public CXML_AttrParser
{
public:
    CJob *m_job;
    CHClass<CStation> m_st;
    QString m_id;
    int     m_active;

    void SetEntity(std::string &att,std::string &text)
    {
        if(att=="name")
        {
            m_id=QString::fromUtf8(text.c_str());
        }else
        if(att=="active")
        {
            bool ok=false;
            int v=0;
            ok=sscanf(text.c_str(),"%d",&v);
            if(ok)
                m_active=v;
        }
    }

    CGKF_target(CJob *job,CStation *st)
    {
        m_job=job;
        m_st =*st;
    }

    ~CGKF_target()
    {
        if(m_id.size())
        {
            CHClass<CStationNode> stn=m_st->GiveNode(m_id,*m_job);
            stn->m_fActive=(bool)m_active;
        }
    }
};


class CGKF_observ_val : public CXML_AttrParser
{
public:
    CJob *m_job;
    CHClass<CStation> m_st;
    QString m_id;
    double m_val;
    double m_th;
    double m_ih;
    double m_stdev;

    void SetEntity(std::string &att,std::string &text)
    {
        if(att=="to")
        {
            m_id=QString::fromUtf8(text.c_str());
        }else
        if(att=="val")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_val=val;
        }else
        if(att=="from_dh")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_ih=val;
        }else
        if(att=="to_dh")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_th=val;
        }
    }

    CGKF_observ_val(CJob *job,CStation *st)
    {
        m_job=job;
        m_st=*st;

        m_val  =0;
        m_th   =0;
        m_ih   =0;
        m_stdev=0;
    }
};

class CGKF_dir : public CGKF_observ_val
{
public:

    CGKF_dir(CJob *job,CStation *st):CGKF_observ_val(job,st){}
    ~CGKF_dir()
    {
        if(m_id.size())
        {
            CHClass<CStationNode> stn=m_st->GiveNode(m_id,*m_job);
            CHClass<CStationObservation> sto=new CStationObservation(ST_OBS_HA,m_val*(M_PI/200.),m_stdev,m_th);
            sto.Lock();
            stn->m_list.append(sto);
        }
    }
};

class CGKF_sdist : public CGKF_observ_val
{
public:

    CGKF_sdist(CJob *job,CStation *st):CGKF_observ_val(job,st){}
    ~CGKF_sdist()
    {
        if(m_id.size())
        {
            CHClass<CStationNode> stn=m_st->GiveNode(m_id,*m_job);
            CHClass<CStationObservation> sto=new CStationObservation(ST_OBS_SD,m_val,m_stdev,m_th);
            sto.Lock();
            stn->m_list.append(sto);
            if(m_ih!=0. && m_st->m_height==0.)
                m_st->m_height=m_ih;
        }
    }
};

class CGKF_dist : public CGKF_observ_val
{
public:

    CGKF_dist(CJob *job,CStation *st):CGKF_observ_val(job,st){}
    ~CGKF_dist()
    {
        if(m_id.size())
        {
            CHClass<CStationNode> stn=m_st->GiveNode(m_id,*m_job);
            CHClass<CStationObservation> sto=new CStationObservation(ST_OBS_HD,m_val,m_stdev,m_th);
            sto.Lock();
            stn->m_list.append(sto);
            if(m_ih!=0. && m_st->m_height==0.)
                m_st->m_height=m_ih;
        }
    }
};

class CGKF_dh : public CGKF_observ_val
{
public:

    CGKF_dh(CJob *job,CStation *st):CGKF_observ_val(job,st){}
    ~CGKF_dh()
    {
        if(m_id.size())
        {
            CHClass<CStationNode> stn=m_st->GiveNode(m_id,*m_job);
            CHClass<CStationObservation> sto=new CStationObservation(ST_OBS_VD,m_val,m_stdev,m_th);
            sto.Lock();
            stn->m_list.append(sto);
            if(m_ih!=0. && m_st->m_height==0.)
                m_st->m_height=m_ih;
        }
    }
};

class CGKF_zangle : public CGKF_observ_val
{
public:

    CGKF_zangle(CJob *job,CStation *st):CGKF_observ_val(job,st){}
    ~CGKF_zangle()
    {
        if(m_id.size())
        {
            CHClass<CStationNode> stn=m_st->GiveNode(m_id,*m_job);
            //CHClass<CStationObservation> sto=new CStationObservation(ST_OBS_VA,(100.-m_val)*(M_PI/200.),m_stdev,m_th);
            CHClass<CStationObservation> sto=new CStationObservation(ST_OBS_ZA,m_val*(M_PI/200.),m_stdev,m_th);
            sto.Lock();
            stn->m_list.append(sto);
        }
    }
};


class CGKF_obs : public CXML_AttrParser
{
public:
    CJob *m_job;
    CHClass<CStation> m_st;

    void SetEntity(std::string &att,std::string &text)
    {
        if(att=="from")
        {
            if(text.size())
            {
                CHClass<CNode> node;
                node=m_job->GiveNode(QString::fromUtf8(text.c_str()));
                m_st=new CStation();
                m_st->m_node=node;
                m_st.Lock();
                m_job->m_stations.append(m_st);
            }
        }else
        if(att=="from_dh")
        {
            if(m_st.Exist())
            {
                bool ok=false;
                double val=0.;
                ok=sscanf(text.c_str(),"%lf",&val);
                if(ok)
                    m_st->m_height=val;
            }
        }
    }
    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(m_st.isNull())
            return 0;
        if(name=="target")
        {
            return new CGKF_target(m_job,m_st);
        }else
        if(name=="direction")
        {
            return new CGKF_dir(m_job,m_st);
        }else
        if(name=="s-distance")
        {
            return new CGKF_sdist(m_job,m_st);
        }else
        if(name=="distance")
        {
            return new CGKF_dist(m_job,m_st);
        }else
        if(name=="dh")
        {
            return new CGKF_dh(m_job,m_st);
        }else
        if(name=="z-angle")
        {
            return new CGKF_zangle(m_job,m_st);
        }
        return 0;
    }

    CGKF_obs(CJob *job)
    {
        m_job=job;
    }
};


class CGKF_point : public CXML_AttrParser
{
public:
    CJob *m_job;
    CHClass<CNode> m_node;

    void SetEntity(std::string &att,std::string &text)
    {
        if(att=="id")
        {
            m_node->m_id=QString::fromUtf8(text.c_str());
        }else
        if(att=="comment")
        {
            m_node->m_comment=QString::fromUtf8(text.c_str());
        }else
        if(att=="x")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
            {
                m_node->fx=val;
                m_node->m_fX=1;
            }
        }else
        if(att=="y")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
            {
                m_node->fy=val;
                m_node->m_fY=1;
            }
        }else
        if(att=="z")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
            {
                m_node->fz=val;
                m_node->m_fZ=1;
            }
        }else
        if(att=="fix")
        {
            ;
        }else
        if(att=="adj")
        {
            if(strchr(text.c_str(),'y')!=0)
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x01;
            else
            if(strchr(text.c_str(),'Y')!=0)
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x02;

            if(strchr(text.c_str(),'x')!=0)
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x01;
            else
            if(strchr(text.c_str(),'X')!=0)
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x02;

            if(strchr(text.c_str(),'z')!=0)
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x0C)|0x04;
            else
            if(strchr(text.c_str(),'Z')!=0)
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x0C)|0x08;
        }
    }

    CGKF_point(CJob *job)
    {
        m_job=job;
        m_node=new CNode();
        m_node->m_adj_flags=0;
    }

    ~CGKF_point()
    {
        if(m_node->m_id.size())
        {
            m_node.Lock();
            m_job->m_nodes.append(m_node);
        }
    }
};


class CGKF_po : public CXML_AttrParser
{
public:
    CJob *m_job;

    void SetEntity(std::string &att,std::string &text)
    {
        if(att=="distance-stdev")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_job->m_distance_stdev=val;
        }else
        if(att=="direction-stdev")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_job->m_direction_stdev=val;
        }else
        if(att=="angle-stdev")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_job->m_angle_stdev=val;
        }else
        if(att=="zenith-angle-stdev")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_job->m_zenith_angle_stdev=val;
        }
    }

    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(name=="point")
        {
            return new CGKF_point(m_job);
        }else
        if(name=="obs")
        {
            return new CGKF_obs(m_job);
        }
        return 0;
    }
    CGKF_po(CJob *job)
    {
        m_job=job;
    }
};

class CGKF_par : public CXML_AttrParser
{
public:
    CJob *m_job;

    void SetEntity(std::string &att,std::string &text)
    {
        if(att=="sigma-apr")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_job->m_sigma_apr=val;
        }else
        if(att=="conf-pr")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_job->m_conf_pr=val;
        }else
        if(att=="tol-abs")
        {
            bool ok=false;
            double val=0.;
            ok=sscanf(text.c_str(),"%lf",&val);
            if(ok)
                m_job->m_tol_abs=val;
        }else
        if(att=="sigma-act")
        {
            if(text=="apriori")
                m_job->m_sigma_act=apriori;
            else
                m_job->m_sigma_act=aposteriori;
        }
    }

    CGKF_par(CJob *job)
    {
        m_job=job;
    }
};

class CGKF_description : public CXML_AttrParser
{
public:
    CJob *m_job;

    void SetEntity(std::string &att,std::string &text)
    {
        m_job->m_title=QString::fromUtf8(text.c_str());
    }

    CGKF_description(CJob *job)
    {
        m_job=job;
    }
};


class CGKF_network : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(name=="description")
        {
            return new CGKF_description(m_job);
        }else
        if(name=="parameters")
        {
            return new CGKF_par(m_job);
        }
        if(name=="points-observations")
        {
            return new CGKF_po(m_job);
        }
        return 0;
    }
    CGKF_network(CJob *job)
    {
        m_job=job;
    }
};

class CGKF_AP : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(name=="network")
        {
            return new CGKF_network(m_job);
        }
        return 0;
    }
    CGKF_AP(CJob *job)
    {
        m_job=job;
    }
};

class CGKF_AttrParser : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(name=="gama-local")
        {
            return new CGKF_AP(m_job);
        }
        return 0;
    }
    CGKF_AttrParser(CJob *job)
    {
        m_job=job;
    }
};

int CJob::load(QString &fname)
{
    FILE *in=_wfopen((wchar_t *)fname.unicode(),L"rt");
    if(in)
    {
        Clear();
        CXMLParser xml(in,new CGKF_AttrParser(this));
        xml.Parse();
        update_node_stats();
        //adjust_finish();
        return 0;
    }
    return -1;
}

}
