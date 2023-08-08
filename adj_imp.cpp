#include "CJob.h"
#include "cxmlparser.h"

namespace jobnet
{

class CGLA_ngp : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        return 0;
    }
    CGLA_ngp(CJob *job)
    {
        m_job=job;
    }
};

class CGLA_nps : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        return 0;
    }
    CGLA_nps(CJob *job)
    {
        m_job=job;
    }
};

//-------------------------------
//       coordinates
        class CGLA_coords_point;
        class CGLA_coords_point_id : public CXML_AttrParser
        {
        public:
            CGLA_coords_point *m_host;

            void SetEntity(std::string &att,std::string &text);
            CGLA_coords_point_id(CGLA_coords_point *p)
            {
                m_host=p;
            }
        };

        class CGLA_coords_point_x : public CXML_AttrParser
        {
        public:
            CGLA_coords_point *m_host;

            void SetEntity(std::string &att,std::string &text);
            CGLA_coords_point_x(CGLA_coords_point *p)
            {
                m_host=p;
            }
        };

        class CGLA_coords_point_y : public CXML_AttrParser
        {
        public:
            CGLA_coords_point *m_host;

            void SetEntity(std::string &att,std::string &text);
            CGLA_coords_point_y(CGLA_coords_point *p)
            {
                m_host=p;
            }
        };

        class CGLA_coords_point_z : public CXML_AttrParser
        {
        public:
            CGLA_coords_point *m_host;

            void SetEntity(std::string &att,std::string &text);
            CGLA_coords_point_z(CGLA_coords_point *p)
            {
                m_host=p;
            }
        };


        class CGLA_coords_point : public CXML_AttrParser
        {
        public:
            CJob *m_job;
            int   m_mode;
            QString m_id;
            double  m_x,m_y,m_z;
            union{
                unsigned m_uFlags;
                struct{
                    unsigned m_fx:1;
                    unsigned m_fy:1;
                    unsigned m_fz:1;
                };
            };

            CXML_AttrParser *AddAttr(std::string &name)
            {
                if(name=="id")
                    return new CGLA_coords_point_id(this);
                else
                if(name=="x")
                    return new CGLA_coords_point_x(this);
                else
                if(name=="y")
                    return new CGLA_coords_point_y(this);
                else
                if(name=="z")
                    return new CGLA_coords_point_z(this);
                return 0;
            }
            CGLA_coords_point(CJob *job,int mode)
            {
                m_job =job;
                m_mode=mode;
                m_uFlags=0;
                m_x=m_y=m_z=0.;
            }
            ~CGLA_coords_point()
            {
                if(m_id.size())
                {
                    CNode *pn=m_job->FindNode(m_id);
                    if(pn)
                    {
                        if(m_mode)
                        {
                            if(m_fx)
                            {
                                pn->ax=m_x;
                                pn->m_faX=1;
                            }
                            if(m_fy)
                            {
                                pn->ay=m_y;
                                pn->m_faY=1;
                            }
                            if(m_fz)
                            {
                                pn->az=m_z;
                                pn->m_faZ=1;
                            }
                        }else
                        {
                            if(m_fx)
                            {
                                pn->cx=m_x;
                                pn->m_fcX=1;
                            }
                            if(m_fy)
                            {
                                pn->cy=m_y;
                                pn->m_fcY=1;
                            }
                            if(m_fz)
                            {
                                pn->cz=m_z;
                                pn->m_fcZ=1;
                            }
                        }
                    }
                }
            }
        };

        void CGLA_coords_point_id::SetEntity(std::string &att,std::string &text)
        {
            m_host->m_id=QString::fromUtf8(text.c_str());
        }

        void CGLA_coords_point_x::SetEntity(std::string &att,std::string &text)
        {
            bool ok=false;
            double v=0.;
            ok=sscanf(text.c_str(),"%lf",&v);
            if(ok)
            {
                m_host->m_x=v;
                m_host->m_fx=1;
            }
        }

        void CGLA_coords_point_y::SetEntity(std::string &att,std::string &text)
        {
            bool ok=false;
            double v=0.;
            ok=sscanf(text.c_str(),"%lf",&v);
            if(ok)
            {
                m_host->m_y=v;
                m_host->m_fy=1;
            }
        }

        void CGLA_coords_point_z::SetEntity(std::string &att,std::string &text)
        {
            bool ok=false;
            double v=0.;
            ok=sscanf(text.c_str(),"%lf",&v);
            if(ok)
            {
                m_host->m_z=v;
                m_host->m_fz=1;
            }
        }

    class CGLA_coords_approx : public CXML_AttrParser
    {
    public:
        CJob *m_job;

        CXML_AttrParser *AddAttr(std::string &name)
        {
            if(name=="point")
            {
                return new CGLA_coords_point(m_job,0);
            }
            return 0;
        }
        CGLA_coords_approx(CJob *job)
        {
            m_job=job;
        }
    };

    class CGLA_coords_adjusted : public CXML_AttrParser
    {
    public:
        CJob *m_job;

        CXML_AttrParser *AddAttr(std::string &name)
        {
            if(name=="point")
            {
                return new CGLA_coords_point(m_job,1);
            }
            return 0;
        }
        CGLA_coords_adjusted(CJob *job)
        {
            m_job=job;
        }
    };


class CGLA_coords : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(name=="approximate")
            return new CGLA_coords_approx(m_job);
        else
        if(name=="adjusted")
            return new CGLA_coords_adjusted(m_job);
        else
        return 0;
    }
    CGLA_coords(CJob *job)
    {
        m_job=job;
    }
};

class CGLA_observ : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        return 0;
    }
    CGLA_observ(CJob *job)
    {
        m_job=job;
    }
};



class CGLA_AP : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(name=="network-general-parameters")
        {
            return new CGLA_ngp(m_job);
        }else
        if(name=="network-processing-summary")
        {
            return new CGLA_nps(m_job);
        }else
        if(name=="coordinates")
        {
            return new CGLA_coords(m_job);
        }else
        if(name=="observations")
        {
            return new CGLA_observ(m_job);
        }
        return 0;
    }
    CGLA_AP(CJob *job)
    {
        m_job=job;
    }
};

class CGLA_XML_AttrParser : public CXML_AttrParser
{
public:
    CJob *m_job;

    CXML_AttrParser *AddAttr(std::string &name)
    {
        if(name=="gama-local-adjustment")
        {
            return new CGLA_AP(m_job);
        }
        return 0;
    }
    CGLA_XML_AttrParser(CJob *job)
    {
        m_job=job;
    }
};


bool CJob::importGADJ(char *name)
{
    FILE *in=fopen(name,"rt");
    if(in)
    {
        //QSharedDataPointer<CXML_AttrParser> pp=(CXML_AttrParser *)new CGLA_XML_AttrParser(this);
        CXMLParser xml(in,new CGLA_XML_AttrParser(this));
        xml.Parse();
        adjust_finish();
    }
    return false;
}

}
