/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000, 2002  Ales Cepek <cepek@fsv.cvut.cz>

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

#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <utility>

#include <gnu_gama/xml/gkfparser.h>
#include <gnu_gama/xml/encoding.h>
#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/cluster.h>
#include <gnu_gama/local/language.h>
#include <gnu_gama/intfloat.h>
#include <gnu_gama/gon2deg.h>


namespace {
  typedef std::pair<double, bool> DB_pair;
}

using namespace std;
namespace GNU_gama { namespace local {


  int GKFparser::characterDataHandler(const char* s, int len)
  {
    if (state == state_description)
      {
        description += string(s, len);
      }
    else if (state == state_obs_cov     ||
             state == state_coords_cov  ||
             state == state_vectors_cov ||
             state == state_hdiffs_cov )
      {
        cov_mat_data += string (s, len);
      }
    else
      {
        if (len == 0) return 0;
        int b = 0;
        while (b < len && isspace(s[b]))
          b++;
        if (b == len) return 0;

        error(T_GKF_illegal_text);
      }

    return 0;
  }



  int GKFparser::startElement(const char *cname, const char **atts)
  {
    const gkf_tag ntag  = tag(cname);

    switch (state)
      {

      case state_start:
        switch (ntag) {
        case tag_gama_xml: return process_gama_xml(atts);
        default:
          return error(T_GKF_must_start_with_gama_xml);
        }

      case state_gama_xml:
        switch (ntag) {
        case tag_network: return process_network(atts);
        default:
          return error(T_GKF_missing_tag_network);
        }

      case state_network:
        switch (ntag) {
        case tag_description        : return (state = state_description);
        case tag_parameters         : return process_parameters(atts);
        case tag_points_observations: return process_point_obs(atts);
        default:
          return error(T_GKF_e01a_illegal_tag + string(cname) +
                       T_GKF_e01b_after_tag + " <network>");
        }

      case state_point_obs:
        switch(ntag) {
        case tag_point             : return process_point(atts);
        case tag_obs               : return process_obs(atts);
        case tag_coordinates       : return process_coords(atts);
        case tag_height_differences: return process_hdiffs(atts);
        case tag_vectors           : return process_vectors(atts);
        default:
          return error(T_GKF_e01a_illegal_tag + string(cname) +
                       T_GKF_e01b_after_tag + " <points-observations>");
        }

      case state_obs:
          switch(ntag) {
          case tag_direction : return process_direction(atts);
          case tag_distance  : return process_distance(atts);
          case tag_angle     : return process_angle(atts);
          case tag_s_distance: return process_sdistance(atts);
          case tag_z_angle   : return process_zangle(atts);
          case tag_dh        : return process_obs_dh(atts);
          case tag_cov_mat   : return process_obs_cov(atts);
          default:
            return error(T_GKF_e01a_illegal_tag + string(cname) +
                         T_GKF_e01b_after_tag + " <obs>");
        }

      case state_coords:
        switch (ntag) {
        case tag_point  : return process_coords_point(atts);
        case tag_cov_mat: return process_coords_cov(atts);
        default:
          return error(T_GKF_e01a_illegal_tag + string(cname) +
                       T_GKF_e01b_after_tag + " <coordinates>");
        }

      case state_hdiffs:
        switch (ntag) {
        case tag_dh     : return process_dh(atts);
        case tag_cov_mat: return process_hdiffs_cov(atts);
        default:
          return error(T_GKF_e01a_illegal_tag + string(cname)  +
                       T_GKF_e01b_after_tag + " <height-differences>");
        }

      case state_vectors:
        switch(ntag) {
        case tag_vec    : return process_vec(atts);
        case tag_cov_mat: return process_vectors_cov(atts);
        default:
          return error(T_GKF_e01a_illegal_tag + string(cname) + ">");
        }

      case state_obs_after_cov:
      case state_coords_after_cov:
      case state_hdiffs_after_cov:
      case state_vectors_after_cov:
        {
          return error(T_GKF_no_observations_after_cov_mat);
        }

      default:
        return error(T_GKF_e01a_illegal_tag + string(cname) + ">");

      }

    return 0;
  }



  int GKFparser::endElement(const char * /*name*/)
  {
    switch (state)
      {
      case state_gama_xml:
        state = state_stop;
        break;
      case state_network:
        state = state_gama_xml;
        break;
      case state_description:
        state = state_network;
        break;
      case state_parameters:
        state = state_network;
        break;
      case state_point_obs:
        state = state_network;
        break;
      case state_point:
        state = state_point_obs;
        break;


      case state_obs_direction:
        state = state_obs;
        break;
      case state_obs_distance:
        state = state_obs;
        break;
      case state_obs_angle:
        state = state_obs;
        break;
      case state_obs_sdistance:
        state = state_obs;
        break;
      case state_obs_zangle:
        state = state_obs;
        break;
      case state_obs_dh:
        state = state_obs;
        break;
      case state_obs_cov:
        state = state_obs_after_cov;
        break;
      case state_obs_after_cov:
        state = state_point_obs;
        finish_obs();
        break;
      case state_obs:
        state = state_point_obs;
        finish_obs();
        break;


      case state_hdiffs_dh:
        state = state_hdiffs;
        break;
      case state_hdiffs_cov:
        state = state_hdiffs_after_cov;
        break;
      case state_hdiffs_after_cov:
        state = state_point_obs;
        finish_hdiffs();
        break;
      case state_hdiffs:
        state = state_point_obs;
        finish_hdiffs();
        break;


      case state_coords_point:
        state = state_coords;
        break;
      case state_coords_cov:
        state = state_coords_after_cov;
        break;
      case state_coords_after_cov:
        state = state_point_obs;
        finish_coords();
        break;


      case state_vectors_vec:
        state = state_vectors;
        break;
      case state_vectors_cov:
        state = state_vectors_after_cov;
        break;
      case state_vectors_after_cov:
        state = state_point_obs;
        finish_vectors();
        break;


      case state_stop:
        state = state_error;
        break;
      default:
        state = state_error;
        break;
      }

    return 0;   // return value is never used in GKFparser
  }



  /* grep ELEMENT gamaxml/gama-local.dtd | awk '//{print $2}' | sort
   * -------------------------------------------------------------
   * angle
   * coordinates cov-mat
   * description dh direction distance
   * gama-local
   * height-differences
   * network
   * obs
   * parameters point points-observations
   * s-distance
   * vec vectors
   * z-angle
   */

  GKFparser::gkf_tag GKFparser::tag(const char* c)
  {
    switch (*c)
      {
      case 'a':
        if (!strcmp(c, "angle"              )) return tag_angle;
        break;
      case 'c':
        if (!strcmp(c, "coordinates"        )) return tag_coordinates;
        if (!strcmp(c, "cov-mat"            )) return tag_cov_mat;
        break;
      case 'd':
        if (!strcmp(c, "description"        )) return tag_description;
        if (!strcmp(c, "dh"                 )) return tag_dh;
        if (!strcmp(c, "direction"          )) return tag_direction;
        if (!strcmp(c, "distance"           )) return tag_distance;
        break;
      case 'g':
        if (!strcmp(c, "gama-local"           )) return tag_gama_xml;
        break;
      case 'h':
        if (!strcmp(c, "height-differences" )) return tag_height_differences;
        break;
      case 'n':
        if (!strcmp(c, "network"            )) return tag_network;
        break;
      case 'o':
        if (!strcmp(c, "obs"                )) return tag_obs;
        break;
      case 'p':
        if (!strcmp(c, "parameters"         )) return tag_parameters;
        if (!strcmp(c, "point"              )) return tag_point;
        if (!strcmp(c, "points-observations")) return tag_points_observations;
        break;
      case 's':
        if (!strcmp(c, "s-distance"         )) return tag_s_distance;
        break;
      case 'v':
        if (!strcmp(c, "vec"                )) return tag_vec;
        if (!strcmp(c, "vectors"            )) return tag_vectors;
        break;
      case 'z':
        if (!strcmp(c, "z-angle"            )) return tag_z_angle;
        break;
      }

    return tag_unknown;
  }



  GKFparser::GKFparser(PointData& sb, ObservationData& od)
    : SB(sb), OD(od)
  {
    // implicit parameters' values

    m0_apr          = 10;
    konf_pr         = 0.95;
    tol_abs         = 1000;
    typ_m0_apriorni = false;
    update_constr   = false;

    state       = state_start;
    standpoint  = 0;
    idim        = 0;
    coordinates = 0;
    vectors     = 0;

    obsolete_attribute = true;

    // throw exception if a covariance matrix is not positive-definite
    check_cov_mat = true;

  }



  GKFparser::~GKFparser()
  {
  }



  int GKFparser::process_gama_xml(const char** atts)
  {
    string  nam, val;
    state = state_gama_xml;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if (nam == "version")
          {
            if (val != "2.0")
              return error(T_GKF_illegal_value_of_gama_xml_version + val);

            gama_xml_version = val;
          }
        else
          {
            return error(T_GKF_undefined_attribute_of_gama_xml
                         + nam + " = " + val);
          }
      }

    return 0;
  }



  int GKFparser::process_network(const char** atts)
  {
    string  nam, val;
    state = state_network;

    epoch = 0.0;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if (nam == "axes-xy")
          {
            LocalCoordinateSystem::CS& lcs = SB.local_coordinate_system;

            if      (val == "ne") lcs = LocalCoordinateSystem::NE;
            else if (val == "sw") lcs = LocalCoordinateSystem::SW;
            else if (val == "es") lcs = LocalCoordinateSystem::ES;
            else if (val == "wn") lcs = LocalCoordinateSystem::WN;
            else if (val == "en") lcs = LocalCoordinateSystem::EN;
            else if (val == "nw") lcs = LocalCoordinateSystem::NW;
            else if (val == "se") lcs = LocalCoordinateSystem::SE;
            else if (val == "ws") lcs = LocalCoordinateSystem::WS;
            else
              return error(T_GKF_undefined_value_of_attribute
                           + nam + " = " + val);
          }
        else if (nam == "angles")
          {
            if      (val == "right-handed") SB.setAngularObservations_Righthanded();
            else if (val == "left-handed" ) SB.setAngularObservations_Lefthanded();
            else
              return error(T_GKF_undefined_value_of_attribute
                           + nam + " = " + val);
          }
        else if (nam == "epoch")
          {
            if (!toDouble(val, epoch))
              return error(T_GKF_error_on_reading_of_epoch
                           + nam + " = " + val);
          }
        else
          {
            return error(T_GKF_undefined_attribute_of_gama_xml
                         + nam + " = " + val);
          }
      }

    return 0;
  }



  int GKFparser::process_parameters(const char** atts)
  {
    string jmeno, hodnota;
    double dhod;
    state = state_parameters;

    while (*atts)
      {
        jmeno   = string(*atts++);
        hodnota = string(*atts++);

        if      (jmeno == "txt") TXT = hodnota;
        else if (jmeno == "stx") STX = hodnota;
        else if (jmeno == "opr") OPR = hodnota;
        else if (jmeno == "sigma-apr")
          {
            if (!toDouble(hodnota, dhod))
              return error(T_GKF_error_on_reading_of_standard_deviation);
            if (dhod <= 0)
              return error(T_GKF_error_on_reading_of_standard_deviation);
            m0_apr = dhod;
          }
        else if (jmeno == "conf-pr")
          {
            if (!toDouble(hodnota, dhod))
              return error(T_GKF_error_on_reading_of_confidence_probability);
            if (dhod <= 0)
              return error(T_GKF_error_on_reading_of_confidence_probability);
            konf_pr = dhod;
          }
        else if (jmeno == "tol-abs")
          {
            if (!toDouble(hodnota, dhod))
              return error(T_GKF_error_on_reading_of_absolute_terms_tolerance);
            if (dhod <= 0)
              return error(T_GKF_error_on_reading_of_absolute_terms_tolerance);
            tol_abs = dhod;
          }
        else if (jmeno == "sigma-act")
          {
            if      (hodnota == "aposteriori") typ_m0_apriorni = false;
            else if (hodnota == "apriori"    ) typ_m0_apriorni = true;
            else
              return error(T_GKF_wrong_type_of_standard_deviation);
          }
        else if (jmeno == "update-constrained-coordinates")
          {
            if      (hodnota == "yes") update_constr = true;
            else if (hodnota == "no" ) update_constr = false;
            else
            return error(T_GKF_bad_network_configuration_unknown_parameter
                         + jmeno + " = " + hodnota);
          }
        else
          {
            return error(T_GKF_bad_network_configuration_unknown_parameter
                         + jmeno + " = " + hodnota);
          }
      }

    return 0;
  }



  int GKFparser::process_point_obs(const char** atts)
  {
    // setting values of implicit standard deviations to "undefined";
    // on reaching end tag </points-observations> this state is
    // ignored

    string nam, val, sds_abc, sds, sdk, sde, ss="0", su="0", sz="0";
    state = state_point_obs;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if (nam == "distance-stdev")
          {
            sds_abc = val;
            string::const_iterator i=val.begin();
            while (i != val.end() &&  isspace(*i)) ++i;
            while (i != val.end() && !isspace(*i)) { sds += *i; ++i; }
            while (i != val.end() &&  isspace(*i)) ++i;
            while (i != val.end() && !isspace(*i)) { sdk += *i; ++i; }
            while (i != val.end() &&  isspace(*i)) ++i;
            while (i != val.end() && !isspace(*i)) { sde += *i; ++i; }
            while (i != val.end() &&  isspace(*i)) ++i;

            if (i != val.end())
              return error(T_GKF_bad_attribute_distance_dev + sds_abc);
          }
        else if (nam == "direction-stdev"   ) ss = val;
        else if (nam == "angle-stdev"       ) su = val;
        else if (nam == "zenith-angle-stdev") sz = val;
        else
          return error(T_GKF_undefined_attribute_of_points_observations
                       + nam + " = " + val);
      }

    if (sds == "") sds = "0";
    if (sdk == "") sdk = "0";
    if (sde == "") sde = "1";
    if (!toDouble(sds, delka_str)    ||
        !toDouble(sdk, delka_str_km) ||
        !toDouble(sde, delka_str_exp))
      return error(T_GKF_bad_attribute_distance_dev  + sds_abc);
    if (!toDouble(ss, smer_str))
      return error(T_GKF_bad_attribute_direction_dev + ss );
    if (!toDouble(su, uhel_str))
      return error(T_GKF_bad_attribute_angle_dev     + su );
    if (!toDouble(sz, z_uhel_str))
      return error(T_GKF_bad_attribute_angle_dev     + sz );

    return 0;
  }



  int GKFparser::process_point(const char** atts)
  {
    string  nam, val, sy, sx, sv, sf, sa, st, sh;
    pp_xydef = pp_zdef = false;
    state = state_point;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "id" ) pp_id = val;
        else if (nam == "y"  ) sy = val;
        else if (nam == "x"  ) sx = val;
        else if (nam == "z"  ) sv = val;
        else if (nam == "fix") sf = val;
        else if (nam == "adj") sa = val;
        else if (nam == "xy" ) st = val;         // ###### obsoleted
        else if (nam == "height") sh = val;      // ###### obsoleted
        else
          return
            error(T_GKF_undefined_attribute_of_points + nam + " = " + val);

        if ((nam == "xy" || nam == "height") && obsolete_attribute)
          {
            description += "\n**** Warning: ";
            description += "obsolete XML attribute(s)";
            description += "   <point ...";
            description += " " + nam + "=\"" + val + "\" />\n";

            ostringstream ostr;
            ostr << "****          see line number "
                 << XML_GetCurrentLineNumber(parser) << "\n";;
            description += ostr.str();

            obsolete_attribute = false;
          }
      }

    if (pp_id == "") return error(T_GKF_missing_point_ID);

    if (sy != "" && sx == "") return error(T_GKF_coordinate_x_is_not_defined);
    if (sx != "" && sy == "") return error(T_GKF_coordinate_y_is_not_defined);

    if (sx != "") {
      double dy, dx;
      if (!toDouble(sx, dx)) return error(T_GKF_bad_coordinate_x + sx);
      if (!toDouble(sy, dy)) return error(T_GKF_bad_coordinate_y + sy);
      SB[pp_id].set_xy(dx, dy);

      if (pp_xydef) return error(T_GKF_multiple_definition_of_xy_in_tag_point);
      pp_x     = dx;
      pp_y     = dy;
      pp_xydef = true;
    }

    if (sv != "") {
      double dz;
      if (!toDouble(sv, dz)) return error(T_GKF_bad_height + sv);
      SB[pp_id].set_z(dz);

      if (pp_zdef) return error(T_GKF_multiple_definition_of_z_in_tag_point);
      pp_z    = dz;
      pp_zdef = true;
    }

    if (sa != "") {
      if      (sa == "xy" ) SB[pp_id].set_free_xy();
      else if (sa == "xyz") SB[pp_id].set_free_xy(),
                            SB[pp_id].set_free_z();
      else if (sa == "z"  ) SB[pp_id].set_free_z();
      else if (sa == "XY" ) SB[pp_id].set_constrained_xy();
      else if (sa == "XYZ") SB[pp_id].set_constrained_xy(),
                            SB[pp_id].set_constrained_z();
      else if (sa == "XYz") SB[pp_id].set_constrained_xy(),
                            SB[pp_id].set_free_z();
      else if (sa == "xyZ") SB[pp_id].set_free_xy(),
                            SB[pp_id].set_constrained_z();
      else if (sa == "Z"  ) SB[pp_id].set_constrained_z();
      else
        return error(T_GKF_undefined_point_type + sa);
    }

    if (sf != "") {
      if      (sf == "xy" ) SB[pp_id].set_fixed_xy();
      else if (sf == "xyz") SB[pp_id].set_fixed_xy(),
                            SB[pp_id].set_fixed_z();
      else if (sf == "z"  ) SB[pp_id].set_fixed_z();
      else if (sf == "XY" ) SB[pp_id].set_fixed_xy();
      else if (sf == "XYZ") SB[pp_id].set_fixed_xy(),
                            SB[pp_id].set_fixed_z();
      else if (sf == "XYz") SB[pp_id].set_fixed_xy(),
                            SB[pp_id].set_fixed_z();
      else if (sf == "xyZ") SB[pp_id].set_fixed_xy(),
                            SB[pp_id].set_fixed_z();
      else if (sf == "Z"  ) SB[pp_id].set_fixed_z();
      else
        return error(T_GKF_undefined_point_type + sf);
    }

    // ###### obsoleted

    if      (st == "fixed"     ) SB[pp_id].set_fixed_xy();
    else if (st == "free"      ) SB[pp_id].set_free_xy();
    else if (st =="constrained") SB[pp_id].set_constrained_xy();
    else if (st == "unused"    ) SB[pp_id].unused_xy();
    else if (st != "")
      return error(T_GKF_undefined_point_type + st);

    if      (sh == "fixed" ) SB[pp_id].set_fixed_z();
    else if (sh == "free"  ) SB[pp_id].set_free_z();
    else if (sh == "unused") SB[pp_id].unused_z();
    else if (sh != "")
      return error(T_GKF_undefined_height_type + sh);

    return 0;
  }



  int GKFparser::process_distance(const char** atts)
  {
    string nam, val, ss=standpoint_id, sc, sm, sv, hf, ht;
    state = state_obs_distance;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);
        if      (nam == "from"   ) ss = val;
        else if (nam == "to"     ) sc = val;
        else if (nam == "val"    ) sm = val;
        else if (nam == "stdev"  ) sv = val;
        else if (nam == "from_dh") hf = val;
        else if (nam == "to_dh"  ) ht = val;
        else
          return error(T_GKF_undefined_attribute_of_distance +nam+" = "+val);
      }

    if (ss == "") return error(T_GKF_missing_standpoint_id);
    if (sc == "") return error(T_GKF_missing_forepoint_id);
    if (sm == "") return error(T_GKF_missing_observed_value);

    double dm;
    if (!toDouble(sm, dm)) return error(T_GKF_bad_distance + sm);
    double dv = implicit_stdev_distance(dm);
    if (sv != "")
      if (!toDouble(sv, dv)) return error(T_GKF_illegal_standard_deviation);
    double df = obs_from_dh;
    if (hf != "")
      if (!toDouble(hf, df))
        return error(T_GKF_bad_instrument_reflector_height + hf);
    double dt = 0;
    if (ht != "")
      if (!toDouble(ht, dt))
        return error(T_GKF_bad_instrument_reflector_height + ht);

    try
      {
        if (standpoint == 0)
          {
            standpoint = new StandPoint(&OD);
            OD.clusters.push_back( standpoint );
          }
        Distance* d = new Distance(ss, sc, dm);
        d->set_from_dh(df);
        d->set_to_dh(dt);
        standpoint->observation_list.push_back( d );
        sigma.push_back(DB_pair(dv, false));
      }
    catch (const /*GNU_gama::local::*/Exception &e)
      {
        return error(e.what());
      }

    return 0;
  }



  int GKFparser::process_angle(const char** atts)
  {
    bool degrees = false;
    string nam, val, ss=standpoint_id, sl, sp, sm, sv, hf, ht, h2;
    state = state_obs_angle;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "from"   ) ss = val;
        else if (nam == "bs"     ) sl = val;  // backsight station
        else if (nam == "fs"     ) sp = val;  // foresight station
        else if (nam == "to"     ) sl = val;  // <== undocumented feature for
        else if (nam == "rs"     ) sp = val;  // <== backward compatibility
        else if (nam == "val"    ) sm = val;
        else if (nam == "stdev"  ) sv = val;
        else if (nam == "from_dh") hf = val;
        else if (nam == "bs_dh"  ) ht = val;
        else if (nam == "fs_dh"  ) h2 = val;
        else
          return error(T_GKF_undefined_attribute_of_angle + nam + " = " + val);
      }

    if (ss == "") return error(T_GKF_missing_standpoint_id);
    if (sl == "") return error(T_GKF_missing_left_forepoint_id);
    if (sp == "") return error(T_GKF_missing_right_forepoint_id);
    if (sm == "") return error(T_GKF_missing_observed_value);

    double dm;
    if (GNU_gama::deg2gon(sm, dm))
      degrees = true;
    else
      if (!toDouble(sm, dm)) return error(T_GKF_bad_angle + sm);

    double dv = implicit_stdev_angle();
    if (sv != "")
      if (!toDouble(sv, dv)) return error(T_GKF_illegal_standard_deviation);
    double df = obs_from_dh;
    if (hf != "")
      if (!toDouble(hf, df))
        return error(T_GKF_bad_instrument_reflector_height + hf);
    double dt = 0;
    if (ht != "")
      if (!toDouble(ht, dt))
        return error(T_GKF_bad_instrument_reflector_height + ht);
    double d2 = 0;
    if (h2 != "")
      if (!toDouble(h2, d2))
        return error(T_GKF_bad_instrument_reflector_height + h2);

    try
      {
        if (standpoint == 0)
          {
            standpoint = new StandPoint(&OD);
            OD.clusters.push_back( standpoint );
          }
        Angle* d = new Angle(ss, sl, sp, dm*G2R);
        d->set_from_dh(df);
        d->set_to_dh(dt);
        standpoint->observation_list.push_back( d );
        sigma.push_back(DB_pair(dv, degrees));
      }
    catch (const /*GNU_gama::local::*/Exception &e)
      {
        error(e.what());
      }

    return 0;
  }



  int GKFparser::process_sdistance(const char** atts)
  {
    string nam, val, ss=standpoint_id, sc, sm, sv, hf, ht;
    state = state_obs_sdistance;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);
        if      (nam == "from"   ) ss = val;
        else if (nam == "to"     ) sc = val;
        else if (nam == "val"    ) sm = val;
        else if (nam == "stdev"  ) sv = val;
        else if (nam == "from_dh") hf = val;
        else if (nam == "to_dh"  ) ht = val;
        else
          return error(T_GKF_undefined_attribute_of_slopedist +nam+" = "+val);
      }

    if (ss == "") return error(T_GKF_missing_standpoint_id);
    if (sc == "") return error(T_GKF_missing_forepoint_id);
    if (sm == "") return error(T_GKF_missing_observed_value);

    double dm;
    if (!toDouble(sm, dm)) return error(T_GKF_bad_distance + sm);
    double dv = implicit_stdev_distance(dm);
    if (sv != "")
      if (!toDouble(sv, dv)) return error(T_GKF_illegal_standard_deviation);
    double df = obs_from_dh;
    if (hf != "")
      if (!toDouble(hf, df))
        return error(T_GKF_bad_instrument_reflector_height + hf);
    double dt = 0;
    if (ht != "")
      if (!toDouble(ht, dt))
        return error(T_GKF_bad_instrument_reflector_height + ht);

    try
      {
        if (standpoint == 0)
          {
            standpoint = new StandPoint(&OD);
            OD.clusters.push_back( standpoint );
          }
        S_Distance* d = new S_Distance(ss, sc, dm);
        d->set_from_dh(df);
        d->set_to_dh(dt);
        standpoint->observation_list.push_back( d );
        sigma.push_back(DB_pair(dv, false));
      }
    catch (const /*GNU_gama::local::*/Exception &e)
      {
        return error(e.what());
      }

    return 0;
  }



  int GKFparser::process_zangle(const char** atts)
  {
    bool degrees = false;
    string nam, val, ss=standpoint_id, sc, sm, sv, hf, ht;
    state = state_obs_zangle;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);
        if      (nam == "from"   ) ss = val;
        else if (nam == "to"     ) sc = val;
        else if (nam == "val"    ) sm = val;
        else if (nam == "stdev"  ) sv = val;
        else if (nam == "from_dh") hf = val;
        else if (nam == "to_dh"  ) ht = val;
        else return error(T_GKF_undefined_attribute_of_zangle +nam+" = "+val);
      }

    if (ss == "") return error(T_GKF_missing_standpoint_id);
    if (sc == "") return error(T_GKF_missing_forepoint_id);
    if (sm == "") return error(T_GKF_missing_observed_value);

    double dm;
    if (GNU_gama::deg2gon(sm, dm))
      degrees = true;
    else
      if (!toDouble(sm, dm)) return error(T_GKF_bad_zangle + sm);

    double dv = implicit_stdev_zangle();
    if (sv != "")
      if (!toDouble(sv, dv)) return error(T_GKF_illegal_standard_deviation);
    double df = obs_from_dh;
    if (hf != "")
      if (!toDouble(hf, df))
        return error(T_GKF_bad_instrument_reflector_height + hf);
    double dt = 0;
    if (ht != "")
      if (!toDouble(ht, dt))
        return error(T_GKF_bad_instrument_reflector_height + ht);

    try
      {
        if (standpoint == 0)
          {
            standpoint = new StandPoint(&OD);
            OD.clusters.push_back( standpoint );
          }
        Z_Angle* d = new Z_Angle(ss, sc, dm*G2R);
        d->set_from_dh(df);
        d->set_to_dh(dt);
        standpoint->observation_list.push_back( d );
        sigma.push_back(DB_pair(dv, degrees));
      }
    catch (const /*GNU_gama::local::*/Exception &e)
      {
        return error(e.what());
      }

    return 0;
  }



  int GKFparser::process_obs(const char** atts)
  {
    string nam, val, ss, sz, sh;
    obs_from_dh = 0;           // implicit instrument height for <obs />
    state = state_obs;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "from"       ) ss = val;
        else if (nam == "orientation") sz = val;
        else if (nam == "from_dh"    ) sh = val;
        else return error(T_GKF_undefined_attribute_of_obs
                          + nam + " = " + val);
      }

    idim = 0;
    standpoint_id = ss;
    standpoint = new StandPoint(&OD);
    standpoint->station = standpoint_id;
    if (sz != "") {
      double dz;
      if (!toDouble(sz, dz)) return error(T_GKF_bad_orientation_angle + sz);
      standpoint->set_orientation(dz);
    }
    if (sh != "") {
      if (!toDouble(sh, obs_from_dh))
        return error(T_GKF_bad_instrument_reflector_height + sh);
    }
    OD.clusters.push_back(standpoint);

    return 0;
  }



  int GKFparser::finish_obs()
  {
    standpoint->update();             // bind observations to the cluster
    if (idim)
      {
        finish_cov(standpoint->covariance_matrix);
      }
    else
      {
        const Index N = sigma.size();
        standpoint->covariance_matrix.reset(N, 0);
        CovMat::iterator c=standpoint->covariance_matrix.begin();
        std::vector<DB_pair>::iterator s = sigma.begin();

        for (Index i=1; i<=N; ++i, ++c, ++s) *c = (*s).first * (*s).first;
      }

    if (check_cov_mat)
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
            return error(T_GKF_covariance_matrix_is_not_positive_definite);
          }
      }

    standpoint = 0;
    standpoint_id = "";
    sigma.erase(sigma.begin(), sigma.end());
    return 0;
  }



  int GKFparser::process_direction(const char** atts)
  {
    bool degrees = false;
    string nam, val, sc, sm, ss, hf, ht;
    state = state_obs_direction;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "to"     ) sc = val;
        else if (nam == "val"    ) sm = val;
        else if (nam == "stdev"  ) ss = val;
        else if (nam == "from_dh") hf = val;
        else if (nam == "to_dh"  ) ht = val;
        else return error(T_GKF_undefined_attribute_of_direction
                          + nam + " = "+val);
      }

    if (standpoint_id == "") return error(T_GKF_missing_standpoint_id);
    if (sc   == "") return error(T_GKF_missing_forepoint_id);
    if (sm   == "") return error(T_GKF_missing_observed_value);

    double dm;
    if (GNU_gama::deg2gon(sm, dm))
      degrees = true;
    else
      if (!toDouble(sm, dm)) return error(T_GKF_bad_direction + sm);

    double ds = implicit_stdev_direction();
    if (ss != "")
      if (!toDouble(ss, ds)) return error(T_GKF_illegal_standard_deviation);
    double df = obs_from_dh;
    if (hf != "")
      if (!toDouble(hf, df))
        return error(T_GKF_bad_instrument_reflector_height + hf);
    double dt = 0;
    if (ht != "")
      if (!toDouble(ht, dt))
        return error(T_GKF_bad_instrument_reflector_height + ht);

    try
      {
        Direction* d = new Direction(standpoint_id, sc, dm*G2R);
        d->set_from_dh(df);
        d->set_to_dh(dt);
        standpoint->observation_list.push_back( d );
        sigma.push_back(DB_pair(ds, degrees));
      }
    catch (const /*GNU_gama::local::*/Exception &e)
      {
        error(e.what());
      }

    return 0;
  }



  int GKFparser::process_obs_dh(const char** atts)
  {
    /*  the function body was copied from process_dh(const char**)  */
    /*  ##########################################################  */

    string  nam, val, sfrom, sto,  sval, sstdev, sdist;
    // ###### state = state_hdiff_dh;
    state = state_obs_dh;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "from" ) sfrom  = val;
        else if (nam == "to"   ) sto    = val;
        else if (nam == "val"  ) sval   = val;
        else if (nam == "stdev") sstdev = val;
        else if (nam == "dist" ) sdist  = val;
        else
          return error(T_GKF_undefined_attribute_of_height_differences
                       + nam + " = " + val);
      }

    if (sfrom == "") return error(T_GKF_missing_from_ID);
    if (sto   == "") return error(T_GKF_missing_to_ID);
    if (sval  == "") return error(T_GKF_missing_observed_value);

    double dm;
    if (!toDouble(sval, dm)) return error(T_GKF_bad_height_diff + sval);
    double dd = 0;
    if (sdist != "")
      if (!toDouble(sdist, dd) || dd < 0)
        return error(T_GKF_bad_distance + sdist);
    double ds = m0_apr * sqrt(dd);
    if (sstdev != "")
      if (!toDouble(sstdev, ds)) return error(T_GKF_illegal_standard_deviation);

    try
      {
        H_Diff* hd = new H_Diff(sfrom, sto, dm, dd);
        // ###### heightdifferences->observation_list.push_back( hd );
        standpoint->observation_list.push_back( hd );
        sigma.push_back(DB_pair(ds, false));
      }
    catch  (const /*GNU_gama::local::*/Exception &e)
      {
        error(e.what());
      }

    return 0;
  }


  int GKFparser::process_cov(const char** atts)
  {
    string nam, val, sdim, sband;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "dim" ) sdim  = val;
        else if (nam == "band") sband = val;
        else return error(T_GKF_undefined_attribute_of_cov_mat
                          + nam + " = "+val);
      }

    if (sdim  == "") return error(T_GKF_cov_mat_missing_dim);
    if (sband == "") return error(T_GKF_cov_mat_missing_band_width);

    if (!toIndex(sdim,  idim ))
      return error(T_GKF_cov_mat_bad_dim + nam + " = " + val);
    if (!toIndex(sband, iband))
      return error(T_GKF_cov_mat_bad_band_width + nam + " = " + val);

    if (idim  < 1) return error(T_GKF_cov_mat_bad_dim + nam + " = " + val);
    if (iband < 0 || iband >= idim)
      return error(T_GKF_cov_mat_bad_band_width + nam + " = " + val);

    return 0;
  }


  int GKFparser::finish_cov(CovMat& cov_mat)
  {
    cov_mat.reset(idim, iband);
    int elements =  idim*(iband+1) - iband*(iband+1)/2;
    string::const_iterator i=cov_mat_data.begin();
    Index row = 1;
    Index col = row;

    while (i!=cov_mat_data.end())
      {
        while (i!=cov_mat_data.end() &&  isspace(*i)) ++i;
        string w;
        while (i!=cov_mat_data.end() && !isspace(*i))
          {
            w += *i; ++i;
          }
        if (w.size())
          {
            if (elements == 0)
              return error(T_GKF_cov_mat_bad_dim_too_many_elements);
            double d;
            if (!toDouble(w, d))
              return error(T_GKF_cov_mat_bad_element);
            cov_mat(row,col) = d;
            elements--;
            col++;
            if (col > row+iband || col > idim) col = ++row;
          }
      }

    if (elements)
      return error(T_GKF_cov_mat_bad_dim_not_enough_elements);

    idim = 0;
    cov_mat_data = "";
    return 0;
  }


  int GKFparser::process_coords(const char** atts)
  {
    state = state_coords;

    if (*atts)
      {
        string nam, val;
        nam = string(*atts++);
        val = string(*atts++);
        return error(T_GKF_undefined_attribute_of_coordinates
                     + nam + " = " + val);
      }

    coordinates = new Coordinates(&OD);
    OD.clusters.push_back(coordinates);

    return 0;
  }


  int GKFparser::finish_coords()
  {
    if (!idim) return error(T_GKF_coordinates_without_covariance_matrix);
    if (idim != coordinates->observation_list.size())
      return error("T_GKF_cov_dim_differs_from_number_of_coordinates");

    coordinates->update();
    finish_cov(coordinates->covariance_matrix);

    if (check_cov_mat)
      {
        try
          {
            CovMat tmp = coordinates->covariance_matrix;
            tmp.cholDec();
          }
        catch(...)
          {
            return error(T_GKF_covariance_matrix_is_not_positive_definite);
          }
      }

    coordinates = 0;
    return 0;
  }


  int GKFparser::process_coords_point(const char** atts)
  {
    process_point(atts);
    state = state_coords_point;     // we must reset state here !!!

    if (!pp_xydef && !pp_zdef)
      return error(T_GKF_point_must_define_xy_andor_z_inside_tag_coordinates);

    if (pp_xydef)
      {
        coordinates->observation_list.push_back( new X(pp_id, pp_x) );
        coordinates->observation_list.push_back( new Y(pp_id, pp_y) );
      }
    if (pp_zdef)
      {
        coordinates->observation_list.push_back( new Z(pp_id, pp_z) );
      }

    return 0;
  }


  int GKFparser::process_hdiffs(const char** atts)
  {
    state = state_hdiffs;

    if (*atts)
      {
        string nam, val;
        nam = string(*atts++);
        val = string(*atts++);
        return error(T_GKF_undefined_attribute_of_height_differences
                     + nam + " = " + val);
      }

    heightdifferences = new HeightDifferences(&OD);
    OD.clusters.push_back(heightdifferences);

    return 0;
  }


  int GKFparser::finish_hdiffs()
  {
    heightdifferences->update();         // bind observations to the cluster
    if (idim)
      {
        finish_cov(heightdifferences->covariance_matrix);
      }
    else
      {
        const Index N = sigma.size();
        heightdifferences->covariance_matrix.reset(N, 0);
        CovMat::iterator c=heightdifferences->covariance_matrix.begin();
        std::vector<DB_pair>::iterator s = sigma.begin();

        for (Index i=1; i<=N; ++i, ++c, ++s) *c = (*s).first * (*s).first;
      }

    if (check_cov_mat)
      {
        try
          {
            CovMat tmp = heightdifferences->covariance_matrix;
            tmp.cholDec();
          }
        catch(...)
          {
            return error(T_GKF_covariance_matrix_is_not_positive_definite);
          }
      }

    heightdifferences = 0;
    sigma.erase(sigma.begin(), sigma.end());

    return 0;
  }

  int GKFparser::process_dh(const char** atts)
  {
    string  nam, val, sfrom, sto,  sval, sstdev, sdist;
    state = state_hdiffs_dh;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "from" ) sfrom  = val;
        else if (nam == "to"   ) sto    = val;
        else if (nam == "val"  ) sval   = val;
        else if (nam == "stdev") sstdev = val;
        else if (nam == "dist" ) sdist  = val;
        else
          return error(T_GKF_undefined_attribute_of_height_differences
                       + nam + " = " + val);
      }

    if (sfrom == "") return error(T_GKF_missing_from_ID);
    if (sto   == "") return error(T_GKF_missing_to_ID);
    if (sval  == "") return error(T_GKF_missing_observed_value);

    double dm;
    if (!toDouble(sval, dm)) return error(T_GKF_bad_height_diff + sval);
    double dd = 0;
    if (sdist != "")
      if (!toDouble(sdist, dd) || dd < 0)
        return error(T_GKF_bad_distance + sdist);
    double ds = m0_apr * sqrt(dd);
    if (sstdev != "")
      if (!toDouble(sstdev, ds)) return error(T_GKF_illegal_standard_deviation);

    try
      {
        H_Diff* hd = new H_Diff(sfrom, sto, dm, dd);
        heightdifferences->observation_list.push_back( hd );
        sigma.push_back(DB_pair(ds, false));
      }
    catch  (const /*GNU_gama::local::*/Exception &e)
      {
        error(e.what());
      }

    return 0;
  }


  int GKFparser::process_vectors(const char** atts)
  {
    state = state_vectors;

    if (*atts)
      {
        string nam, val;
        nam = string(*atts++);
        val = string(*atts++);
        return error(T_GKF_undefined_attribute_of_vectors + nam + " = " + val);
      }

    vectors = new Vectors(&OD);
    OD.clusters.push_back(vectors);

    return 0;
  }


  int GKFparser::finish_vectors()
  {
    if (!idim) return error(T_GKF_vectors_without_covariance_matrix);
    if (idim != vectors->observation_list.size())
      return error("T_GKF_cov_dim_differs_from_number_of_vectors");

    vectors->update();         // bind observations to the cluster
    finish_cov(vectors->covariance_matrix);

    if (check_cov_mat)
      {
        try
          {
            CovMat tmp = vectors->covariance_matrix;
            tmp.cholDec();
          }
        catch(...)
          {
            return error(T_GKF_covariance_matrix_is_not_positive_definite);
          }
      }

    vectors = 0;
    return 0;
  }


  int GKFparser::process_vec(const char** atts)
  {
    string  nam, val, sfrom, sto,  sdx, sdy, sdz, hf, ht;
    state = state_vectors_vec;

    while (*atts)
      {
        nam = string(*atts++);
        val = string(*atts++);

        if      (nam == "from"   ) sfrom = val;
        else if (nam == "to"     ) sto   = val;
        else if (nam == "dx"     ) sdx   = val;
        else if (nam == "dy"     ) sdy   = val;
        else if (nam == "dz"     ) sdz   = val;
        else if (nam == "from_dh") hf    = val;
        else if (nam == "to_dh"  ) ht    = val;
        else
          return error(T_GKF_undefined_attribute_of_height_differences
                       + nam + " = " + val);
      }

    if (sfrom == "") return error(T_GKF_missing_from_ID);
    if (sto   == "") return error(T_GKF_missing_to_ID);
    if (sdx   == "" ||
        sdy   == "" ||
        sdz   == "") return error(T_GKF_bad_vector_data);

    double dx, dy, dz;
    if (!toDouble(sdx, dx) ||
        !toDouble(sdy, dy) ||
        !toDouble(sdz, dz)  ) return error(T_GKF_bad_vector_data);
    double df = 0;
    if (hf != "")
      if (!toDouble(hf, df))
        return error(T_GKF_bad_instrument_reflector_height + hf);
    double dt = 0;
    if (ht != "")
      if (!toDouble(ht, dt))
        return error(T_GKF_bad_instrument_reflector_height + ht);

    try
      {
        Xdiff* xdiff = new Xdiff(sfrom, sto, dx);
        Ydiff* ydiff = new Ydiff(sfrom, sto, dy);
        Zdiff* zdiff = new Zdiff(sfrom, sto, dz);

        xdiff->set_from_dh(df);      xdiff->set_to_dh(dt);
        ydiff->set_from_dh(df);      ydiff->set_to_dh(dt);
        zdiff->set_from_dh(df);      zdiff->set_to_dh(dt);

        vectors->observation_list.push_back( xdiff );
        vectors->observation_list.push_back( ydiff );
        vectors->observation_list.push_back( zdiff );
      }
    catch  (const /*GNU_gama::local::*/Exception &e)
      {
        error(e.what());
      }

    return 0;
  }

}}   // namespace GNU_gama::local


// #########################################################################

#ifdef gama_local_GKFparser_demo

#include <fstream>
#include <gnu_gama/version.h>
#include <gnu_gama/local/pobs/format.h>

using namespace GNU_gama::local;

int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      cout << "\nusage: demo xml-file.gkf\n\n";
      return 1;
    }

  PointData        PData;
  ObservationData  OData;
  GKFparser gp(PData, OData);

  try {
    ifstream inp(argv[1]);
    string   rad;

    while (getline(inp, rad))
      {
        rad += "\n";      // for expat to count lines properly
        gp.xml_parse(rad.c_str(), rad.length(), 0);
      }
    gp.xml_parse(0, 0, 1);


    Format::coord_p(5);   // output precision in fixed format
    Format::gon_p(5);
    Format::stdev_p (3);

    cout <<
      "<?xml version=\"1.0\" ?>\n"
      "<!DOCTYPE gama-local SYSTEM \"http://gama.fsv.cvut.cz/gama-local.dtd\">\n\n"

      "<gama-local version=\"2.0\">\n"
      "<network>\n\n";

    if (gp.description != "")
      cout << "<description>" << gp.description << "</description>\n\n";

    cout << "<parameters\n"
         << "      sigma-apr = \"" << gp.m0_apr  << "\"\n"
         << "      conf-pr   = \"" << gp.konf_pr << "\"\n"
         << "      tol-abs   = \"" << gp.tol_abs << "\"\n"
         << "      sigma-act = \""
         <<  (gp.typ_m0_apriorni ? "apriori" : "aposteriori") << "\"\n"
         << "/>\n\n";

    cout << "<points-observations>\n\n"
         << PData << "\n" << OData
         << "\n</points-observations>\n"
         << "</network>\n"
         << "</gama-local>\n";
  }
  catch (GNU_gama::local::Exception e) {
    cout << "\nException : " << e.what() << "\n\n";
    // cout << "line = " << gp.errLineNumber
    //      << " expat error code = " << gp.errCode << "\n\n";
    return 2;
  }

  return 0;
}


#endif


