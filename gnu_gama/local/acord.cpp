/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU Gama C++ library.

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

#include <gnu_gama/local/acord.h>
#include <gnu_gama/local/orientation.h>
#include <gnu_gama/local/median/g2d_coordinates.h>
#include <gnu_gama/local/acord/approx_heights.h>
#include <gnu_gama/local/acord/approx_vectors.h>
#include <gnu_gama/local/acord/reduce_observations.h>

#include <iomanip>
#include <cmath>

using namespace std;
using namespace GNU_gama::local;

Acord::Acord(PointData& b, ObservationData& m)
    : PD(b), OD(m), RO(b,m)
{
  missing_coordinates = false;
  observations = 0;
  given_xy = given_z = given_xyz = 0;
  computed_xy = computed_z = computed_xyz = 0;
  total_xy = total_z = total_xyz = 0;

  for (PointData::const_iterator i=PD.begin(); i!=PD.end(); ++i)
    {
      const PointID&    c = (*i).first;
      const LocalPoint& p = (*i).second;
      bool cp = p.test_xy();
      bool hp = p.test_z();

      if (cp && hp) given_xyz++, set_xyz.insert(c);
      else if (cp)  given_xy++,  set_xy .insert(c);
      else if (hp)  given_z++,   set_z  .insert(c);
    }

  for (ObservationData::const_iterator
         i=OD.begin(), e=OD.end(); i!=e; ++i, ++observations);


  if (GaMaConsistent(PD)) return;

  for (PointData::iterator ii=PD.begin(); ii!=PD.end(); ++ii)
    {
      LocalPoint& p = (*ii).second;

      if (p.test_xy()) p.set_xy(p.x(), -p.y());
    }

  for (ObservationData::ClusterList::iterator
         ci=OD.clusters.begin(), ei=OD.clusters.end(); ci!=ei; ++ci)
    {
      ObservationData::ClusterType *cluster = *ci;
      for (ObservationList::iterator
             m = cluster->observation_list.begin(),
             e = cluster->observation_list.end()  ; m!=e; ++m)
        {
          Observation *obs = *m;
          bool b = false;

          if      (dynamic_cast<Y*>    (obs))  b = true;
          else if (dynamic_cast<Ydiff*>(obs))  b = true;

          if (b)  obs->set_value( -obs->value() );
        }
    }
}

void Acord::execute()
{
  try
    {
      int all;

      // ReducedObservations RO(PD, OD);
      RO.execute();

      do {
        all = total_z + total_xy + total_xyz;

        missing_coordinates = false;
        computed_xy = computed_z = computed_xyz = 0;
        total_xy = total_z = total_xyz = 0;

        ApproximateHeights ah(PD, OD);
        ah.execute();

        ApproximateVectors av(PD, OD);
        av.execute();

        {
          // all transformed slope distances go to a single standpoint
          StandPoint* standpoint = new StandPoint(&OD);
          for (ObservationData::iterator t=OD.begin(), e=OD.end(); t!=e; ++t)
            {
              Observation* obs = *t;

              S_Distance* s = dynamic_cast<S_Distance*>(obs);
              if (s == 0) continue;

              StandPoint* c = dynamic_cast<StandPoint*>(s->ptr_cluster());
              if (c == 0) continue;   // this should newer happen

              PointID from = s->from();
              PointID to   = s->to();

              // look for a zenith angle corresponding to the given slope distance
              ObservationList::iterator i   = c->observation_list.begin();
              ObservationList::iterator end = c->observation_list.end();
              for ( ;i!=end; ++i)
                if (Z_Angle* z = dynamic_cast<Z_Angle*>(*i))
                  if (from == z->from() && to == z->to())
                    {
                      // ... and fake a horizontal distance
                      standpoint->observation_list.push_back(new Distance(from, to, s->value()*fabs(sin(z->value()))));
                      continue;
                    }

            }
          // bind observations to the cluster
          standpoint->update();
          // insert standpoint into `observation data'
          OD.clusters.push_back(standpoint);

          ApproximateCoordinates ps(PD, OD);
          ps.calculation();

          OD.clusters.pop_back();
          delete standpoint;
        }

	RO.execute();

        ObservationList local;
        for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
        {
          local.push_back(*i);
        }

        Orientation orp(PD, local);
        orp.add_all();

        for (PointData::const_iterator i=PD.begin(); i!=PD.end(); ++i)
          {
            const PointID&    c = (*i).first;
            const LocalPoint& p = (*i).second;
            bool cp = p.test_xy();
            bool hp = p.test_z();

            if (cp && hp) total_xyz++;
            else if (cp)  total_xy++;
            else if (hp)  total_z++;

            if (p.active_xy() && !cp) missing_coordinates = true;
            if (p.active_z()  && !hp) missing_coordinates = true;

            int t = 0;
            if      (set_xyz.find(c) != set_xyz.end()) t = 1;
            else if (set_xy .find(c) != set_xy .end()) t = 2;
            else if (set_z  .find(c) != set_z  .end()) t = 3;

            if (cp && hp)
              {
                switch (t)
                  {
                  case 0 : computed_xyz++; break;
                  case 1 :                 break;
                  case 2 : computed_z++;   break;
                  case 3 : computed_xy++;  break;
                  default:                 break;
                  }
              }
            else if (cp)
              {
                if (t!=2)  computed_xy++;
              }
            else if (hp)
              {
                if (t!=3)  computed_z++;
              }
          }

      } while ((all != (total_z + total_xy + total_xyz)) && missing_coordinates);
    }
  catch(...)
    {
      // we must handle the case when there are no observations here
      throw;
    }
}








