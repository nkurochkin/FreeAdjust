#include "CJob.h"

#ifdef   GNU_GAMA_LOCAL_SQLITE_READER
#include <gnu_gama/local/sqlitereader.h>
#endif

#include <gnu_gama/outstream.h>

#include <cstring>
#include <gnu_gama/version.h>
#include <gnu_gama/intfloat.h>
#include <gnu_gama/xml/localnetwork.h>
#include <gnu_gama/xml/gkfparser.h>

//#include <gnu_gama/local/observation.h>
//#include <gnu_gama/local/cluster.h>
//#include <gnu_gama/local/language.h>
//#include <gnu_gama/intfloat.h>
//#include <gnu_gama/gon2deg.h>


#include <gnu_gama/local/language.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/newnetwork.h>
#include <gnu_gama/local/acord.h>

#include <gnu_gama/local/results/text/approximate_coordinates.h>
#include <gnu_gama/local/results/text/network_description.h>
#include <gnu_gama/local/results/text/general_parameters.h>
#include <gnu_gama/local/results/text/fixed_points.h>
#include <gnu_gama/local/results/text/adjusted_observations.h>
#include <gnu_gama/local/results/text/adjusted_unknowns.h>
#include <gnu_gama/local/results/text/outlying_abs_terms.h>
#include <gnu_gama/local/results/text/reduced_observations.h>
#include <gnu_gama/local/results/text/reduced_observations_to_ellipsoid.h>
#include <gnu_gama/local/results/text/residuals_observations.h>
#include <gnu_gama/local/results/text/error_ellipses.h>
#include <gnu_gama/local/results/text/test_linearization.h>

#include <gnu_gama/local/exception.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace {
  typedef std::pair<double, bool> DB_pair;
}

using namespace std;
using namespace GNU_gama::local;

namespace jobnet
{

void CJob::read_node_data(GNU_gama::local::LocalNetwork* IS)
{
  const int y_sign = GaMaConsistent(IS->PD) ? +1 : -1;

   const Vec& x = IS->solve();
   Double elp_k = 0;
   {
     Double alfa = (1 - IS->conf_pr());
     if (IS->m_0_apriori())
       {
         elp_k = sqrt(GNU_gama::Chi_square(float(alfa), 2));
       }
     else
       {
         int n = IS->degrees_of_freedom();
         if (n > 0)
           elp_k = sqrt( n*(pow(alfa, -2.0/n) - 1));
         else
           elp_k = 0;
       }
   }
   const int pocnez = IS->sum_unknowns();

   bool sour = false;
   {   // for ...
   for (int i=1; i<=pocnez; i++)
       if (IS->unknown_type(i) == 'X')
       {
         sour = true;
         break;
       }
   }   // for ...

   Double mp_max = -1, mp_prum = 0;
   PointID mp_max_cb;
   int pocbod = 0;

   if (sour)
   {
     {   // for ...
       // 1.3.13 for (int i=1; i<=pocnez; i++)
       // 1.3.13  if (IS->unknown_type(i) == 'X')
       for (PointData::const_iterator
              point=IS->PD.begin(); point!=IS->PD.end(); ++point)
         if ((*point).second.free_xy())
           if ((*point).second.index_x())
             {
               const PointID point_id  = (*point).first;
               QString uid=point_id.str().c_str();
               CHClass<CNode> node=FindNode(uid);
               if(node.Exist())
               {
                   //out << Utf8::leftPad(point_id.str(), IS->maxw_id()) << ' ';
                   const LocalPoint& p = (*point).second;
                   Double my = IS->unknown_stdev(p.index_y());
                   Double mx = IS->unknown_stdev(p.index_x());

                   Double mp = sqrt(my*my+mx*mx);
                   //out << mp << ' ';

                   mp_prum += mp;
                   if (mp > mp_max) {
                     mp_max = mp;
                     mp_max_cb = point_id;
                   }
                   pocbod++;

                   Double myx = mp/sqrt(2.0);
                   //out << myx << ' ' ;

                   Double a, b, alfa;
                   IS->std_error_ellipse(point_id, a, b, alfa);
                   node->ee_a=a;
                   node->ee_b=b;
                   node->ee_alfa=alfa;
                   node->m_fee  =1;

                   //out << a << ' ';
                   //out << b << ' ';
                   //double ea = alfa*R2G;
                   //if (IS->degrees()) ea *= 360.0/400;
                   //out << ea << ' ';

                   if (mp < 1000 && mp > 1e-3)
                   {           // ********* testing noise (coordinates are OK)
                       Double ak = a*elp_k;
                       Double bk = b*elp_k;
                       //out << ak << ' ';
                       //out << bk << ' ';
                       node->ee_ak=ak;
                       node->ee_bk=bk;
                       node->m_feek=1;

                       Double g  = 0;
                       Double dx = x( p.index_x() );
                       Double dy = y_sign*x( p.index_y() );
                       Double p1 = (dx*cos(alfa) + dy*sin(alfa));
                       Double p2 = (dy*cos(alfa) - dx*sin(alfa));
                       if (ak > 0 && bk > 0 && bk > ak*1e-4)
                         {           // ***** testing noise (bk is practically 0)
                           p1 /= ak;
                           p2 /= bk;
                           g = sqrt(p1*p1 + p2*p2);
                         }
                       //out << g;
                   }
                }
            }
     }   // for ...
   }

   {
     const int y_sign = GaMaConsistent(IS->PD) ? +1 : -1;

     const GNU_gama::local::Vec& X = IS->solve();
     std::vector<Index> ind(IS->sum_unknowns() + 1);
     Index dim = 0;


     // out << "\n<fixed>\n";
     //for (PointData::const_iterator
     //       i=netinfo->PD.begin(); i!=netinfo->PD.end(); ++i)
     //  {
     //    const LocalPoint& p = (*i).second;
     //    if (!p.active_xy() && !p.active_z()) continue;
     //    bool bxy = p.active_xy() && p.index_x() == 0;
     //    bool bz  = p.active_z () && p.index_z() == 0;
     //    if (!bxy && !bz) continue;

     //    tagsp(out, "id", (*i).first);
     //    if (bxy)
     //      {
     //        const double x = (p.x()+X(p.index_x())/1000);
     //        const double y = (p.y()+X(p.index_y())/1000)*y_sign;
     //        tagsp(out, "x", x);
     //        tagsp(out, "y", y);
     //      }
     //    if (bz)
     //      {
     //        const double z = (p.z()+X(p.index_z())/1000);
     //        tagsp(out, "z", z);
     //      }
     //  }


     // out << "\n<approximate>\n";

     for (PointData::const_iterator i=IS->PD.begin(); i!=IS->PD.end(); ++i)
     {
         CHClass<CNode> node=FindNode(QString((*i).first.str().c_str()));
         if(node.Exist())
         {
             const LocalPoint& p = (*i).second;
             if (!p.active_xy() && !p.active_z()) continue;
             bool bxy = p.active_xy() && p.index_x() != 0;
             bool bz  = p.active_z () && p.index_z() != 0;
             if (!bxy && !bz) continue;
             //tagsp(out, "id", (*i).first);
             if (bxy)
             {
                 // if (p.constrained_xy())
                 node->cx = p.x();
                 node->cy = p.y()*y_sign;
                 node->m_fcX=1;
                 node->m_fcY=1;

                 node->ax = (p.x()+X(p.index_x())/1000);
                 node->ay = (p.y()+X(p.index_y())/1000)*y_sign;
                 node->m_faX=1;
                 node->m_faY=1;

             }
             if (bz)
             {
                 // if (p.constrained_z())
                 node->cz=p.z();
                 node->m_fcZ=1;

                 node->az=(p.z()+X(p.index_z())/1000);
                 node->m_faZ=1;
             }
          }
        }
    }
}

class AdjustedObservationsJobVisitor : public AllObservationsVisitor
{
private:
    CJob *m_job;
    GNU_gama::local::LocalNetwork* IS;
    const GNU_gama::local::Vec& v; ///< residuals
    const int y_sign;
    GNU_gama::Index i;
    CHClass<CStationNode> m_stn;
    double m_ml,m_mlk;

public:
    /**
     * \param localNetwork pointer to local network
     * \param residuals vector of residuals
     * \param ySign sing of y coordinates
     */
    AdjustedObservationsJobVisitor(CJob *job,GNU_gama::local::LocalNetwork* localNetwork,
                                    const GNU_gama::local::Vec& residuals,
                                    int ySign
                                    )
        : IS(localNetwork), v(residuals), y_sign(ySign)
    {
        m_job=job;
    }

    /** \brief Sets index of observation which will be used in the next visit. */
    void setObservationNode(CStationNode &stn,GNU_gama::Index idx,double ml,double mlk)
    {
        m_stn=stn;
        i    =idx;
        m_ml =ml;
        m_mlk=mlk;
    }

    void visit(Distance* obs)
    {
        Double vs  = obs->value();
        Double vc  = vs  + v(i)/1000;
        m_stn->set_corrected(ST_OBS_HD,vc,m_ml,m_mlk);
    }

    void visit(Direction* obs)
    {
        Double vs = obs->value();
        Double m = R2G*vs;
        m += v(i)/10000;
        if (m < 0) m += 400;
        if (m >= 400) m -= 400;
        Double vc = m*G2R;
        m_stn->set_corrected(ST_OBS_HA,vc,m_ml,m_mlk);
    }

    void visit(Angle* obs)
    {
/*
        out << '\n';
        const int w = IS->maxw_obs() + 2 + 2*(IS->maxw_id());
        out << Utf8::leftPad(obs->fs().str(), w);
        out << T_GaMa_angle;
        out.precision(angularPrecision);
        out.width(maxval);
        Double m = R2G*(obs->value());
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
        out.width(maxval);
        m += v(i)/10000;
        if (m < 0) m += 400;
        if (m >= 400) m -= 400;
        if (IS->gons())
            out << m << " ";
        else
            out << GNU_gama::gon2deg(m, 0, 2) << " ";
*/
    }

    void visit(H_Diff* obs)
    {
/*
        out << T_GaMa_levell;
        out.precision(distPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " ";
*/
    }

    void visit(S_Distance* obs)
    {
        Double vs  = obs->value();
        Double vc  = vs + v(i)/1000;
        m_stn->set_corrected(ST_OBS_SD,vc,m_ml,m_mlk);
    }

    void visit(Z_Angle* obs)
    {
        Double vs = obs->value();
        Double m = R2G*vs;
        m += v(i)/10000;
        Double vc = m*G2R;
        m_stn->set_corrected(ST_OBS_ZA,vc,m_ml,m_mlk);
        //-----------------------------------------------
        vs=M_PI*0.5-vs;
        vc=M_PI*0.5-vc;
        if(vs>M_PI)
            vs-=M_PI*2.;
        if(vc>M_PI)
            vc-=M_PI*2.;
        m_stn->set_corrected(ST_OBS_VA,vc,m_ml,m_mlk);
    }

    void visit(X* obs)
    { /*
        out << T_GaMa_x;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " "; */
    }
    void visit(Y* obs)
    { /*
        out << T_GaMa_y;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << y_sign*m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << y_sign*m << " "; */
    }

    void visit(Z* obs)
    { /*
        out << T_GaMa_z;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " "; */
    }
    void visit(Xdiff* obs)
    { /*
        out << T_GaMa_xdiff;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " "; */
    }

    void visit(Ydiff* obs)
    { /*
        out << T_GaMa_ydiff;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << y_sign*m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << y_sign*m << " "; */
    }

    void visit(Zdiff* obs)
    {  /*
        out << T_GaMa_zdiff;
        out.precision(coordPrecision);
        out.width(maxval);
        Double m = obs->value();
        out << m << " ";
        out.width(maxval);
        m += v(i)/1000;
        out << m << " "; */
    }

};


void CJob::read_observ_data(GNU_gama::local::LocalNetwork* IS)
{
    { // Список станций
        unsigned i;
        for(i=0;i<m_stations.size();i++)
        {
            CHClass<CStation> st=m_stations[i];
            st->m_max_error=0.;
            if(st->m_fActive)
            {
                int j;
                for(j=0;j<st->m_observations.size();j++)
                {
                    CStationNode *stn=st->m_observations[j];
                    stn->update_adjustment(IS);
                    if(st->m_max_error<stn->m_max_error)
                        st->m_max_error=stn->m_max_error;
                }
            }
        }
    }
    /*
    using namespace std;
    using namespace GNU_gama::local;
    // using GNU_gama::local::Double;

    const int    y_sign = GaMaConsistent(IS->PD) ? +1 : -1;
    const Vec&   v      = IS->residuals();
    const int    pocmer = IS->sum_observations();
    const double scale  = IS->gons() ? 1.0 : 0.324;

    Double kki = IS->conf_int_coef();
    AdjustedObservationsJobVisitor jobVisitor(this,IS, v, y_sign);

    for (int i=1; i<=pocmer; i++)
    {
       Observation* pm = IS->ptr_obs(i);
       PointID cs = pm->from();
       CHClass<CStation> st=FindStation(QString(cs.str().c_str()));
       if(st.Exist())
       {
           PointID cc = pm->to();
           CHClass<CStationNode> stn=st->FindNode(QString(cc.str().c_str()));
           if(stn.Exist())
           {
               Double ml = IS->stdev_obs(i);
               if (dynamic_cast<Direction*>(pm))
                 ml *= scale;
               else if (dynamic_cast<Angle*>(pm))
                 ml *= scale;
               else if (dynamic_cast<Z_Angle*>(pm))
                 ml *= scale;

               jobVisitor.setObservationNode(*stn.m_val,i,ml,ml*kki);
               pm->accept(&jobVisitor);
            }
        }
    }
*/

}

void CStationObservation::update_adjustment(CStation *st,GNU_gama::local::LocalNetwork* IS)
{
    if(!m_pobs)
        return;
    const Vec& v=IS->residuals();
    int i;
    const int    pocmer = IS->sum_observations();
    for(i=1;i<=pocmer;i++)
    {
        if(IS->ptr_obs(i)==m_pobs)
            break;
    }
    if(i>pocmer)
        return;

    Double f  = IS->obs_control(i);
    if(f>0.1)
    {
        Double kki = fabs(IS->conf_int_coef());
        Double no = fabs(IS->studentized_residual(i));

        m_dev=no;
        if(kki>0.)
            m_devint=no/kki;
        else
            m_devint=0.;
    }else
    {
        m_dev=0.;
        m_devint=0.;
    }

    switch(m_type)
    {
    case ST_OBS_HA:
        {
            Double vs = m_pobs->value();
            Double m = R2G*vs;
            m += v(i)/10000;
            if (m < 0) m += 400;
            if (m >= 400) m -= 400;
            Double vc = m*G2R;
            m_cval=vc;
            m_fcval=1;
        }
        break;
    case ST_OBS_VA:
        {
            Double vs = m_pobs->value();
            Double m = R2G*vs;
            m += v(i)/10000;
            Double vc = m*G2R;
            vs=M_PI*0.5-vs;
            vc=M_PI*0.5-vc;
            if(vs>M_PI)
                vs-=M_PI*2.;
            if(vc>M_PI)
                vc-=M_PI*2.;
            m_cval=vc;
            m_fcval=1;
        }
        break;
    case ST_OBS_ZA:
        {
            Double vs = m_pobs->value();
            Double m = R2G*vs;
            m += v(i)/10000;
            Double vc = m*G2R;
            m_cval=vc;
            m_fcval=1;
        }
        break;
    case ST_OBS_SD:
        {
            Double vs  = m_pobs->value();
            Double vc  = vs + v(i)/1000;
            m_cval=vc;
            m_fcval=1;
        }
        break;
    case ST_OBS_HD:
        {
            Double vs  = m_pobs->value();
            Double vc  = vs + v(i)/1000;
            m_cval=vc;
            m_fcval=1;
        }
        break;
    case ST_OBS_VD:
        {
            Double vs  = m_pobs->value();
            Double vc  = vs + v(i)/1000;
            m_cval=vc-st->m_height+m_height;
            m_fcval=1;
        }
        break;
    }
}


void CStationNode::to_network(CJob &job,GNU_gama::local::StandPoint *standpoint,std::vector<std::pair<GNU_gama::local::Double, bool> > &sigma)
{
    double hd=0.;
    bool   degrees=true;

    int i;
    for(i=0;i<m_list.size();i++)
    {
        CHClass<CStationObservation> sto=m_list[i];
        if(sto->m_fActive)
        switch(sto->m_type)
        {
        case ST_OBS_HA:
            try
            {
                double val=sto->m_val;
                if(val<0)
                    val+=M_PI*2.;
                GNU_gama::local::Direction* d = new GNU_gama::local::Direction(standpoint->station, m_node->m_pp_id, val);
                d->set_from_dh(m_st->m_height);
                d->set_to_dh(sto->m_height);
                standpoint->observation_list.push_back( d );
                double dv=sto->m_stdev;
                if(dv==0.)
                    dv=job.m_direction_stdev;
                sigma.push_back(DB_pair(dv, degrees));
                sto->m_pobs=d;
            }
            catch (...) //const GNU_gama::local::Exception &e)
            {
                return;
            }
            break;
        case ST_OBS_VA:
            try
            {
                GNU_gama::local::Z_Angle* d = new GNU_gama::local::Z_Angle(standpoint->station, m_node->m_pp_id, M_PI_2-sto->m_val);
                d->set_from_dh(m_st->m_height);
                d->set_to_dh(sto->m_height);
                standpoint->observation_list.push_back( d );
                double dv=sto->m_stdev;
                if(dv==0.)
                    dv=job.m_zenith_angle_stdev;
                sigma.push_back(DB_pair(dv, degrees));
                sto->m_pobs=d;
            }
            catch (...) //const GNU_gama::local::Exception &e)
            {
                return;
            }
            break;
        case ST_OBS_ZA:
            try
            {
                GNU_gama::local::Z_Angle* d = new GNU_gama::local::Z_Angle(standpoint->station, m_node->m_pp_id, sto->m_val);
                d->set_from_dh(m_st->m_height);
                d->set_to_dh(sto->m_height);
                standpoint->observation_list.push_back( d );
                double dv=sto->m_stdev;
                if(dv==0.)
                    dv=job.m_zenith_angle_stdev;
                sigma.push_back(DB_pair(dv, degrees));
                sto->m_pobs=d;
            }
            catch (...) //const GNU_gama::local::Exception &e)
            {
                return;
            }
            break;
        case ST_OBS_SD:
            try
            {
                GNU_gama::local::S_Distance* d = new GNU_gama::local::S_Distance(standpoint->station, m_node->m_pp_id, sto->m_val);
                d->set_from_dh(m_st->m_height);
                d->set_to_dh(sto->m_height);
                standpoint->observation_list.push_back( d );
                double dv=sto->m_stdev;
                if(dv==0.)
                    dv=job.m_distance_stdev;
                sigma.push_back(DB_pair(dv, false));
                sto->m_pobs=d;
            }
            catch (...) //const GNU_gama::local::Exception &e)
            {
                return;
            }
            break;
        case ST_OBS_HD:
            try
            {
                GNU_gama::local::Distance* d = new GNU_gama::local::Distance(standpoint->station, m_node->m_pp_id, sto->m_val);
                d->set_from_dh(m_st->m_height);
                d->set_to_dh(sto->m_height);
                standpoint->observation_list.push_back( d );
                double dv=sto->m_stdev;
                if(dv==0.)
                    dv=job.m_distance_stdev;
                sigma.push_back(DB_pair(dv, false));
                hd=sto->m_val;
                sto->m_pobs=d;
            }
            catch (...) //const GNU_gama::local::Exception &e)
            {
                return;
            }
            break;
        case ST_OBS_VD:
            try
            {
                double dd=hd*0.001;
                GNU_gama::local::H_Diff* d = new GNU_gama::local::H_Diff(standpoint->station, m_node->m_pp_id, sto->m_val,dd);
                standpoint->observation_list.push_back( d );
                double ds=sto->m_stdev;
                if(ds==0.)
                    ds=job.m_sigma_apr*sqrt(dd);
                sigma.push_back(DB_pair(ds, false));
                sto->m_pobs=d;
            }
            catch (...) //const GNU_gama::local::Exception &e)
            {
                return;
            }
            break;
        }
    }
}

bool CJob::setup_adjust_network(GNU_gama::local::LocalNetwork* IS)
{
    int a_mode=(int)m_adjust_sit^1;
    int dim=check_available_dimentions(a_mode);
    if(!dim)
        return false;    
    IS->PD.local_coordinate_system=LocalCoordinateSystem::NE;
    //IS->PD.setAngularObservations_Righthanded();
    //IS->PD.setAngularObservations_Lefthanded();
    int i;
    // Список всех уравниваемых узлов
    for(i=0;i<m_nodes.size();i++)
    {
        CHClass<CNode> node=m_nodes[i];
        {
            std::string utf_sname;
            node->get_utf_id(utf_sname);
            node->m_pp_id=utf_sname.c_str();
        }
        if(node->toAdjust(a_mode))
        {

            if(((dim&1) && (node->m_adj_flags&0x3)) || ((dim&2) && (node->m_adj_flags&0xC)))
            {
                if(dim&1)
                {
                    if((node->m_adj_flags&0x03)==0x01)
                        IS->PD[node->m_pp_id].set_free_xy();
                    else
                    if((node->m_adj_flags&0x03)==0x02)
                        IS->PD[node->m_pp_id].set_constrained_xy();
                }

                if(dim&2)
                {
                    if((node->m_adj_flags&0xC)==0x4)
                        IS->PD[node->m_pp_id].set_free_z();
                    else
                    if((node->m_adj_flags&0xC)==0x8)
                        IS->PD[node->m_pp_id].set_constrained_z();
                }
            }

            if((dim&1) && node->m_fX && node->m_fY)
            {
                IS->PD[node->m_pp_id].set_xy(node->fx,node->fy);
                IS->PD[node->m_pp_id].set_fixed_xy();
            }
            if((dim&2) && node->m_fZ)
            {
                IS->PD[node->m_pp_id].set_z(node->fz);
                IS->PD[node->m_pp_id].set_fixed_z();
            }
        }
    }
    { // Список станций
        unsigned i;
        for(i=0;i<m_stations.size();i++)
        {
            CHClass<CStation> st=m_stations[i];
            if(st->m_fActive)
            {
                StandPoint * standpoint=new StandPoint(&IS->OD);
                //std::string standpoint_id;
                //st->m_node->get_utf_id(standpoint_id);
                //standpoint->station = standpoint_id;
                standpoint->station = st->m_node->m_pp_id;
                IS->OD.clusters.push_back( standpoint );

                std::vector<std::pair<GNU_gama::local::Double, bool> > sigma;
                int j;
                for(j=0;j<st->m_observations.size();j++)
                {
                    if(st->m_observations[j]->m_fActive && st->m_observations[j]->m_node->toAdjust(a_mode))
                        st->m_observations[j]->to_network(*this,standpoint,sigma);
                }
                standpoint->update();             // bind observations to the cluster
                {
                  const Index N = sigma.size();
                  standpoint->covariance_matrix.reset(N, 0);
                  CovMat::iterator c=standpoint->covariance_matrix.begin();
                  std::vector<DB_pair>::iterator s = sigma.begin();

                  for (Index i=1; i<=N; ++i, ++c, ++s) *c = (*s).first * (*s).first;
                }
                //if (check_cov_mat)
                  {
                    try
                      {
                        // scaling of rows/columns corresponding to covariances
                        // given in sexagesimal seconds
                        std::vector<DB_pair>::iterator s = sigma.begin();
                        for (Index i=1; i<=sigma.size(); ++i, ++s)
                          {
                            if ((*s).second) standpoint->scaleCov(i, 1.0/0.324);
                          }

                        CovMat tmp = standpoint->covariance_matrix;
                        tmp.cholDec();
                      }
                    catch(...)
                      {
                        return false; //error(T_GKF_covariance_matrix_is_not_positive_definite);
                      }
                  }

            }
        }
    }

// Установка параметров
    IS->apriori_m_0(m_sigma_apr);
    IS->conf_pr    (m_conf_pr);
    IS->tol_abs    (m_tol_abs);

    IS->update_constrained_coordinates(m_ucc);

    if (m_sigma_act==apriori)
      IS->set_m_0_apriori();
    else
      IS->set_m_0_aposteriori();

    //IS->description = gkf.description;
    //IS->epoch = gkf.epoch;
    IS->epoch = 0.0;
    return true;
}


//#define _GL_USE_XML_OUTPUT

int CJob::adjust_gamma_local(
        const char *argv_1,
        const char *argv_algo,
        int   a_lang,
        const char *argv_enc,
        const char *argv_angles,
        int   a_ellipsoid,
        const char *argv_latitude,
        const char *argv_txtout,
        const char *argv_xmlout,
        const char *argv_obsout,
        int   a_covband)
  try {

    using namespace std;
    using namespace GNU_gama::local;

//    const char* c;
//    const char* argv_1 = 0;           // xml input or sqlite db name
//    const char* argv_algo = 0;
//    const char* argv_lang = 0;
//    const char* argv_enc  = 0;
//    const char* argv_angles = 0;
//    const char* argv_ellipsoid = 0;
//    const char* argv_latitude = 0;
//    const char* argv_txtout = 0;
//    const char* argv_xmlout = 0;
//    const char* argv_obsout = 0;
//    const char* argv_covband = 0;

#ifdef GNU_GAMA_LOCAL_SQLITE_READER
    const char* argv_confname = 0;
    const char* argv_readonly = 0;
    const char* argv_sqlitedb = 0;
#endif

    bool correction_to_ellipsoid = false;
    GNU_gama::Ellipsoid el;
    double latitude = M_PI/4.0;

#ifdef GNU_GAMA_LOCAL_SQLITE_READER
    if (argv_confname && argv_readonly) return help();
    if (!argv_1 && !argv_sqlitedb) return help();
#endif

#ifdef _GL_USE_XML_OUTPUT
    if(!save_adj_XML(argv_1))
        return 8;
#endif

    set_gama_language((gama_language)a_lang);
    ostream* output = 0;

    ofstream fcout;
    if (argv_txtout)
      {
        if (!strcmp(argv_txtout, "-"))
          {
            output = &std::cout;
          }
        else
          {
            fcout.open(argv_txtout);
            output = &fcout;
          }
      }

    GNU_gama::OutStream cout(output);

    if (argv_enc)
      {
        using namespace GNU_gama;

        if (!strcmp("utf-8", argv_enc))
          cout.set_encoding(OutStream::utf_8);
        else if (!strcmp("iso-8859-2", argv_enc))
          cout.set_encoding(OutStream::iso_8859_2);
        else if (!strcmp("iso-8859-2-flat", argv_enc))
          cout.set_encoding(OutStream::iso_8859_2_flat);
        else if (!strcmp("cp-1250", argv_enc))
          cout.set_encoding(OutStream::cp_1250);
        else if (!strcmp("cp-1251", argv_enc))
          cout.set_encoding(OutStream::cp_1251);
        else
          return -1;
      }

    if(m_IS)
    {
        delete m_IS;
        m_IS=0;
    }
    LocalNetwork* IS = 0;

    if (argv_algo)
      {
        const std::string algorithm = argv_algo;
        if (algorithm != "svd"      &&
            algorithm != "gso"      &&
            algorithm != "cholesky" &&
            algorithm != "envelope" ) return -1;

        IS = newLocalNetwork(algorithm);
      }

#ifdef GNU_GAMA_LOCAL_SQLITE_READER
    if (argv_sqlitedb)
      {
        GNU_gama::local::sqlite_db::SqliteReader reader(argv_sqlitedb);

        const char* conf = argv_confname;
        if (argv_readonly) conf = argv_readonly;
        reader.retrieve(IS, conf);
      }
    else
#endif

#ifdef _GL_USE_XML_OUTPUT
    {
        if (IS == 0)
            IS = newLocalNetwork();        // implicit algorithm

        ifstream soubor(argv_1);
        GKFparser gkf(IS->PD, IS->OD);
        try
          {
            char c;
            int  n, konec = 0;
            string radek;
            do
              {
                radek = "";
                n     = 0;
                while (soubor.get(c))
                  {
                    radek += c;
                    n++;
                    if (c == '\n') break;
                  }
                if (!soubor) konec = 1;

                gkf.xml_parse(radek.c_str(), n, konec);
              }
            while (!konec);

            IS->apriori_m_0(gkf.m0_apr );
            IS->conf_pr    (gkf.konf_pr);
            IS->tol_abs    (gkf.tol_abs);

            IS->update_constrained_coordinates(gkf.update_constr);

            if (gkf.typ_m0_apriorni)
              IS->set_m_0_apriori();
            else
              IS->set_m_0_aposteriori();

            IS->description = gkf.description;
            IS->epoch = gkf.epoch;
          }
        catch (const GNU_gama::local::ParserException& v) {
          cerr << "\n" << T_GaMa_exception_2a << "\n\n"
               << T_GaMa_exception_2b << v.line << " : " << v.what() << endl;
          return 3;
        }
        catch (const GNU_gama::local::Exception& v) {
          cerr << "\n" <<T_GaMa_exception_2a << "\n"
               << "\n***** " << v.what() << "\n\n";
          return 2;
        }
        catch (...)
          {
            cerr << "\n" << T_GaMa_exception_2a << "\n\n";
            throw;
          }
      }
#else
    if (IS == 0)
        IS = newLocalNetwork();        // implicit algorithm
    if(!setup_adjust_network(IS))
        return 1+10;
#endif

    if (argv_angles)
      {
        if (!strcmp("400", argv_angles))
          IS->set_gons();
        else if (!strcmp("360", argv_angles))
          IS->set_degrees();
        else
          return -1;
      }

//    if (argv_covband)
//      {
//        std::istringstream istr(argv_covband);
//        int band = -1;
//        if (!(istr >> band) || band < -1) return help();
//        char c;
//        if (istr >> c) return -1;
//
//        IS->set_xml_covband(band);
//      }
    if(a_covband>-1)
    {
        IS->set_xml_covband(a_covband);
    }

    if (argv_latitude)
      {
        if ( !GNU_gama::IsFloat(string(argv_latitude)) )
          return -1;

        latitude = atof(argv_latitude) * M_PI / (IS->gons() ? 200 : 180);

        correction_to_ellipsoid = true;
      }

    GNU_gama::set(&el, GNU_gama::ellipsoid_wgs84);

    {
        using namespace GNU_gama;
        gama_ellipsoid gama_el=(gama_ellipsoid)a_ellipsoid;
        if  ( gama_el != ellipsoid_unknown)
            GNU_gama::set(&el,  gama_el);
    }
//    if (argv_ellipsoid)
//      {
//        using namespace GNU_gama;
//
//        gama_ellipsoid gama_el = ellipsoid(argv_ellipsoid);
//        if  ( (gama_el == ellipsoid_unknown) || GNU_gama::set(&el,  gama_el) )
//          return help();
//
//        correction_to_ellipsoid = true;
//      }


    {
      cout << T_GaMa_Adjustment_of_geodetic_network << "        "
           << T_GaMa_version << GNU_gama::GNU_gama_version
           << "-" << IS->algorithm()
           << " / " << GNU_gama::GNU_gama_compiler << "\n"
           << underline(T_GaMa_Adjustment_of_geodetic_network, '*') << "\n"
           << "http://www.gnu.org/software/gama/\n\n\n";
    }

    if (IS->PD.empty())
      throw GNU_gama::local::Exception(T_GaMa_No_points_available);

    if (IS->OD.clusters.empty())
      throw GNU_gama::local::Exception(T_GaMa_No_observations_available);

    try
      {
        if (!GaMaConsistent(IS->PD))
          {
            cout << T_GaMa_inconsistent_coordinates_and_angles << "\n\n\n";
          }
        Acord acord(IS->PD, IS->OD);
        acord.execute();
        ReducedObservationsText(IS,&(acord.RO), cout);

        if (correction_to_ellipsoid)
          {
            ReduceToEllipsoid reduce_to_el(IS->PD, IS->OD, el, latitude);
            reduce_to_el.execute();
            ReducedObservationsToEllipsoidText(IS, reduce_to_el.getMap(), cout);
          }

        ApproximateCoordinates(&acord, cout);

      }
    catch(GNU_gama::local::Exception e)
      {
        cerr << e.what() << endl;
        return 1;
      }
    catch(...)
      {
        cerr << "Gama / Acord: approximate coordinates failed\n\n";
        return 1;
      }


    if (IS->sum_points() == 0 || IS->sum_unknowns() == 0)
      {
        throw GNU_gama::local::Exception(T_GaMa_No_network_points_defined);
      }
    else
      {
        if (IS->huge_abs_terms())
          {
            OutlyingAbsoluteTerms(IS, cout);
            IS->remove_huge_abs_terms();
            cout << T_GaMa_Observatios_with_outlying_absolute_terms_removed
                 << "\n\n\n";
          }

        if (!IS->connected_network())
          cout  << T_GaMa_network_not_connected << "\n\n\n";

        NetworkDescription(IS->description, cout);
        if (GeneralParameters(IS, cout))
          {
            int iterace = 0;
            do
              {
                if(++iterace > 1)
                  {
                    cout << "\n         ******  "
                         << iterace << T_GaMa_adjustment_iteration
                         << "  ******\n\n"
                         << T_GaMa_Approximate_coordinates_replaced << "\n"
                         << underline(T_GaMa_Approximate_coordinates_replaced,
                                      '*') << "\n\n\n";

                    IS->refine_approx();
                    GeneralParameters(IS, cout);
                  }
                FixedPoints     (IS, cout);
                AdjustedUnknowns(IS, cout);
              }
            while (TestLinearization(IS, cout) && iterace < 3);

            ErrorEllipses        (IS, cout);
            AdjustedObservations (IS, cout);
            ResidualsObservations(IS, cout);
            // чтение данных
            read_node_data(IS);
            read_observ_data(IS);
          }

        if (argv_obsout)
          {
            ofstream opr(argv_obsout);
            IS->project_equations(opr);
          }

        // implicit output
        if (!argv_txtout && !argv_xmlout) argv_xmlout = "-";

        if (argv_xmlout)
          {
            IS->set_gons();

            GNU_gama::LocalNetworkXML xml(IS);

            if (!strcmp(argv_xmlout, "-"))
              {
                xml.write(std::cout);
              }
            else
              {
                ofstream file(argv_xmlout);
                xml.write(file);
              }
          }
      }

    m_IS=IS;
    //delete IS;
    return 0;

  }
#ifdef GNU_GAMA_LOCAL_SQLITE_READER
  catch(const GNU_gama::Exception::sqlitexc& gamalite)
    {
      std::cout << "\n" << "****** " << gamalite.what() << "\n\n";
      return 1;
    }
#endif
  catch(const GNU_gama::Exception::adjustment& choldec)
    {
      using namespace GNU_gama::local;
      std::cout << "\n" << T_GaMa_solution_ended_with_error << "\n\n"
                << "****** " << choldec.str << "\n\n";
      return 1+30;
    }
  catch (const GNU_gama::local::Exception& V)
    {
      using namespace GNU_gama::local;
      std::cout << "\n" << T_GaMa_solution_ended_with_error << "\n\n"
                << "****** " << V.what() << "\n\n";
      return 1+31;
    }
  catch (std::exception& stde) {
    std::cout << "\n" << stde.what() << "\n\n";
  }
  catch(...) {
    using namespace GNU_gama::local;
    std::cout << "\n" << T_GaMa_internal_program_error << "\n\n";
    return 1+32;
  }

}


