/*
    GNU Gama C++ library
    Copyright (C) 1999, 2010  Ales Cepek <cepek@fsv.cvut.cz>
                  2011  Vaclav Petras <wenzeslaus@gmail.com>

    This file is part of the GNU Gama C++ library

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/** \file test_linearization.h
 * \brief Function and visitor for linearization testing
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#ifndef GaMa_GaMaProg_Prehled_Test_Chyby_z_Linearizace_h_
#define GaMa_GaMaProg_Prehled_Test_Chyby_z_Linearizace_h_

#include <gnu_gama/gon2deg.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/pobs/bearing.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/utf8.h>
#include <gnu_gama/visitor.h>
#include <cmath>
#include <algorithm>


namespace GNU_gama { namespace local {


class TestLinearizationVisitor : public AllObservationsVisitor
{
private:
    GNU_gama::local::LocalNetwork* IS;
    const GNU_gama::local::Vec& v; ///< residuals
    const GNU_gama::local::Vec& x; ///< unknowns
    GNU_gama::Index i;
    Double pol;
    Double mer;
public:
    TestLinearizationVisitor(GNU_gama::local::LocalNetwork* localNetwork,
                             const GNU_gama::local::Vec& residuals,
                             const GNU_gama::local::Vec& unknowns)
        : IS(localNetwork), v(residuals), x(unknowns)

    {}

    /** \brief Sets index of observation which will be used in the next visit. */
    void setObservationIndex(GNU_gama::Index index) { i = index; }

    Double getPol() { return  pol; }
    Double getMer() { return  mer; }

    void visit(Distance* obs)
    {
        Double ds, dd;
        computeBearingAndDistance(obs, ds, dd);
        mer  = obs->value() + v(i)/1000;
        mer -= dd;
        mer *= 1000;
        pol  = mer;
    }

    void visit(Direction* obs)
    {
        Double ds, dd;
        computeBearingAndDistance(obs, ds, dd);
        Double orp = obs->orientation();
        mer = obs->value() + v(i)*CC2R + orp + x(obs->index_orientation())*CC2R;
        mer -= ds;
        while (mer >  M_PI) mer -= 2*M_PI;
        while (mer < -M_PI) mer += 2*M_PI;
        pol  = mer*dd*1000;
        mer *= R2CC;
    }

    void visit(Angle* obs)
    {
        Double ds, dd;
        computeBearingAndDistance(obs, ds, dd);

        Double sx, sy, cx, cy;
        computeFromTo(obs, sx, sy, cx, cy);

        const LocalPoint& cil2 = IS->PD[obs->fs() ];
        Double cy2 = cil2.y() + x(cil2.index_y())/1000;
        Double cx2 = cil2.x() + x(cil2.index_x())/1000;
        Double ds2, dd2;
        GNU_gama::local::bearing_distance(sy, sx, cy2, cx2, ds2, dd2);
        mer = obs->value() + v(i)*CC2R - ds2 + ds;
        while (mer >  M_PI) mer -= 2*M_PI;
        while (mer < -M_PI) mer += 2*M_PI;
        pol  = mer*max(dd,dd2)*1000;
        mer *= R2CC;
    }

    void visit(H_Diff*)     { mer = 0; pol = 0; }
    void visit(S_Distance*) { mer = 0; pol = 0; }
    void visit(Z_Angle*)    { mer = 0; pol = 0; }
    void visit(X*)          { mer = 0; pol = 0; }
    void visit(Y*)          { mer = 0; pol = 0; }
    void visit(Z*)          { mer = 0; pol = 0; }
    void visit(Xdiff*)      { mer = 0; pol = 0; }
    void visit(Ydiff*)      { mer = 0; pol = 0; }
    void visit(Zdiff*)      { mer = 0; pol = 0; }

private:
    void computeBearingAndDistance(const Observation* pm, Double& ds, Double& dd)
    {
        Double sx;
        Double sy;
        Double cx;
        Double cy;
        computeFromTo(pm, sx, sy, cx, cy);
        GNU_gama::local::bearing_distance(sy, sx, cy, cx, ds, dd);
    }

    void computeFromTo(const Observation* pm, Double& sx, Double& sy, Double& cx, Double& cy)
    {
        const LocalPoint& stan = IS->PD[pm->from()];
        const LocalPoint& cil  = IS->PD[pm->to() ];
        sy = stan.y();
        sx = stan.x();
        if (stan.free_xy())
          {
            sy += x(stan.index_y())/1000;
            sx += x(stan.index_x())/1000;
          }
        cy = cil .y();
        cx = cil .x();
        if (cil.free_xy())
          {
            cy += x(cil .index_y())/1000;
            cx += x(cil .index_x())/1000;
          }
    }

};

/** \brief Writes observation name and gets values of observation value.
 *
 * \todo Enhance functions to do more.
 */
template <typename OutStream>
class TestLinearizationWriteVisitor : public AllObservationsVisitor
{
private:
    OutStream& out;
    GNU_gama::local::LocalNetwork* IS;
    Double dms;
    Double mer;

    static const int distPrecision = 5;
    static const int angularPrecision = 6;

public:
    TestLinearizationWriteVisitor(OutStream& outStream,
                                  GNU_gama::local::LocalNetwork* localNetwork)
        : out(outStream), IS(localNetwork)
    {}

    ///* \brief Sets index of observation which will be used in the next visit. */
    //void setObservationIndex(GNU_gama::Index index) { i = index; }

    Double getDms() { return  dms; }
    Double getMer() { return  mer; }

    void visit(Distance* obs)
      {
        dms = false;
        out << T_GaMa_distance;
        mer = obs->value();
        out.precision(distPrecision);
      }
    void visit(Direction* obs)
      {
        dms = IS->degrees();
        out << T_GaMa_direction;
        mer = (obs->value())*R2G;
        out.precision(angularPrecision);
      }
    void visit(Angle* obs)
      {
        dms = IS->degrees();
        out << '\n';
        const int w = IS->maxw_obs() + 2 + 2*(IS->maxw_id());
        out << Utf8::leftPad((obs->fs()).str(), w);
        out << T_GaMa_angle;
        mer = (obs->value())*R2G;
        out.precision(angularPrecision);
      }

    void visit(H_Diff*)     { mer = 0; dms = false; }
    void visit(S_Distance*) { mer = 0; dms = false; }
    void visit(Z_Angle*)    { mer = 0; dms = false; }
    void visit(X*)          { mer = 0; dms = false; }
    void visit(Y*)          { mer = 0; dms = false; }
    void visit(Z*)          { mer = 0; dms = false; }
    void visit(Xdiff*)      { mer = 0; dms = false; }
    void visit(Ydiff*)      { mer = 0; dms = false; }
    void visit(Zdiff*)      { mer = 0; dms = false; }

};

/** \brief
 *
 * \todo Reorganize code by moving some observation dependent code to TestLinearizationWriteVisitor.
 */
template <typename OutStream>
bool
TestLinearization(GNU_gama::local::LocalNetwork* IS, OutStream& out,
                  double max_pyx = 0.1500, // suspicious coorection in meters
                  double max_dif = 0.0005  // max. positional difference in mm
                  )
{

  using namespace std;
  using namespace GNU_gama::local;
  using GNU_gama::local::Double;

  bool test  = false;     // result of bad linearization test

  // const char hlavicka[] = "Bad linearization test\n"
  //                        "***********************\n";

  /****************************  test removed  **************************
  * // repeat adjustment if suspiciously large corrections of coordinates
  * // ==================================================================
  * {
  *   const int N = IS->sum_unknowns();
  *
  *   const Vec& x = IS->solve();
  *   Double pyx = 0;
  *   for (int i=1; i<=N; i++)
  *     if (IS->typ_neznama(i) == 'X')
  *       {
  *         const PointID cb = IS->bod_neznama(i);
  *         const LocalPoint&  b = IS->PD[cb];
  *         if (!b.constrained_xy())
  *           {
  *             Double dx = x(i++)/1000;
  *             Double dy = x(i  )/1000;
  *             pyx = max(pyx, dy*dy + dx*dx);
  *           }
  *       }
  *   pyx = sqrt(pyx);
  *   if (pyx >= max_pyx)
  *     {
  *       if (!test) out << hlavicka;
  *       test  = true;
  *       out.setf(ios_base::fixed, ios_base::floatfield);
  *       out.precision(3);
  *       out << "\nMaximal correction of point " << pyx
  *           << " is greater then " << max_pyx << " [m]\n";
  *     }
  * }
  ******************************************************************/

  // difference in adjusted observations computed from residuals and
  // from adjusted coordinates
  // ===============================================================
  {
    const int M = IS->sum_observations();

    Vec dif_m(M);   // difference in computation of adjusted observation
    Vec dif_p(M);   //               corresponds to positional shift

    const Vec& v = IS->residuals();
    const Vec& x = IS->solve();

    TestLinearizationVisitor testVisitor(IS, v, x);

    for (int i=1; i<=M; i++)
      {
        dif_m(i) = 0;
        dif_p(i) = 0;
        // if (IS->obs_control(i) < 0.1) continue;  // uncontrolled observation
        // if (IS->obs_control(i) < 5.0) continue;  // weakly controlled obs.

        Double  mer = 0, pol = 0;

        Observation* pm = IS->ptr_obs(i);

        // special case for coordinates
        if (dynamic_cast<const Coordinates*>(pm->ptr_cluster()))
          {
            dif_m(i) = dif_p(i) = 0;
            continue;
          }

        // other observations by visitor or by default values
        testVisitor.setObservationIndex(i);
        pm->accept(&testVisitor);

        mer = testVisitor.getMer();
        pol = testVisitor.getPol();

        dif_m(i) = mer;
        dif_p(i) = pol;
      }


    Double max_pol = 0;
    {
      for (Vec::iterator i=dif_p.begin(); i != dif_p.end(); ++i)
        if (fabs(*i) > max_pol)
          max_pol = fabs(*i);
    }
    if (max_pol >= max_dif)
      {
        // print header
        // ------------
        {
          // if (!test) out << hlavicka;
          if (!test)
            out << T_GaMa_tstlin_Test_of_linearization_error << "\n"
                << underline(T_GaMa_tstlin_Test_of_linearization_error, '*')
                << "\n";
          test = true;

          out << "\n"
               << T_GaMa_tstlin_Differences
              << "\n"
              << underline(T_GaMa_tstlin_Differences, '*')
              << "\n\n";

          out.width(IS->maxw_obs());
          out << "i" << " ";
          out.width(IS->maxw_id());
          out << T_GaMa_standpoint << " ";
          out.width(IS->maxw_id());
          out << T_GaMa_target << "      ";
          out << T_GaMa_tstlin_obs_r_diff << "\n";

          for (int i=0; i<(IS->maxw_obs()+2*IS->maxw_id()+8); i++) out << '=';
          out << T_GaMa_tstlin_header_value;
          if (IS->gons())
            out << "= [mm|cc] == [cc] == [mm] =\n\n";
          else
            out << "= [mm|ss] == [ss] == [mm] =\n\n";
          out.flush();
        }

        // print table
        // -----------

        TestLinearizationWriteVisitor<OutStream> writeVisitor(out, IS);

        PointID predcs;   // previous standpoint ID

        for (int i=1; i<=M; i++)
          {
            if (fabs(dif_p(i)) < max_dif) continue;

            Observation* pm = IS->ptr_obs(i);
            out.width(IS->maxw_obs());
            out << i << " ";
            PointID cs = pm->from();
            out.width(IS->maxw_id());
            if (cs != predcs)
              out << Utf8::leftPad(cs.str(), IS->maxw_id());
            else
              out << " ";
            out << " ";
            PointID cc = pm->to();
            out << Utf8::leftPad(cc.str(), IS->maxw_id());
            out.setf(ios_base::fixed, ios_base::floatfield);

            bool dms = false;
            Double mer = 0;

            pm->accept(&writeVisitor);

            mer = writeVisitor.getMer();
            dms = writeVisitor.getDms();

            if (!dms)
              {
                out.width(12);
                out << mer << ' ';
              }
            else
              {
                out << GNU_gama::gon2deg(mer, 1, 1) << ' ';
              }

            out.precision(3);
            out.width(9);
            if (!dms)
              out << v(i) << ' ';
            else
              out << v(i)*0.324 << ' ';

            if (dynamic_cast<Distance*>(pm))
              {
                out << "         ";
              }
            else
              {
                out.precision(3);
                out.width(8);
                if (!dms)
                  out << dif_m(i) << ' ';
                else
                  out << dif_m(i)*0.324 << ' ';
              }

            out.precision(3);
            out.width(7);
            out << dif_p(i);

            out << '\n';
            out.flush();

            predcs = cs;  // previous standpoint ID
          }
      }
  }

  if (test && !(IS->update_constrained_coordinates()))
    {
      out << "\n\n";
      out.flush();


      // if all adjusted points are constrained, adjustment is never
      // repeated (unless explicitly asked for)
      // ------------------------------------------------------

      test = false;
      for (PointData::const_iterator i=IS->PD.begin(); i!=IS->PD.end(); ++i)
        {
          const LocalPoint& b = (*i).second;
          if (b.free_xy() && !b.constrained_xy())
            {
              test = true;
              break;
            }
        }
    }

  return test;
}

}}

#endif




