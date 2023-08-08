/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_localnetwork_adjustment_results__gnugamalocalnetworkadjres_h
#define GNU_gama_localnetwork_adjustment_results__gnugamalocalnetworkadjres_h

#include <utility>
#include <stack>
#include <string>
#include <vector>
#include <gnu_gama/exception.h>
#include <gnu_gama/xml/baseparser.h>
#include <matvec/covmat.h>


namespace GNU_gama
{
  class LocalNetworkAdjustmentResults
  {
  public:

    LocalNetworkAdjustmentResults() { init(); }

    void read_xml(std::istream&) throw(Exception::parser);

    bool gons;
    std::string description;

    struct
    {
      std::string gama_local_version;
      std::string gama_local_algorithm;
      std::string gama_local_compiler;
      std::string epoch;
      std::string axes_xy;
      std::string angles;

    } network_general_parameters;

    struct count
    {
      int xyz, xy, z;
    };

    struct
    {
      count adjusted;
      count constrained;
      count fixed;

    } coordinates_summary;

    struct
    {
      int distances;
      int directions;
      int angles;
      int xyz_coords;
      int h_diffs;
      int z_angles;
      int s_dists;
      int vectors;

    } observations_summary;

    struct
    {
      int    equations;
      int    unknowns;
      int    degrees_of_freedom;
      int    defect;
      double sum_of_squares;
      bool   connected_network;

    } project_equations;

    struct
    {
      double apriori;
      double aposteriori;
      bool   using_aposteriori;
      double probability;
      double ratio;
      double lower;
      double upper;
      bool   passed;
      double confidence_scale;

    } standard_deviation;

    struct Point
    {
      std::string id;
      double x, y, z;

      bool   hxy, hz;             // point has   x, y, z
      bool   cxy, cz;             // constrained x, y, z
      int    indx, indy, indz;    // adjustment indexes

      void clear()
      {
        x=y=z=0;
        hxy=hz=cxy=cz=false;
        indx=indy=indz=0;
      }
    };

    typedef std::vector<Point> PointList;
    PointList  fixed_points, approximate_points, adjusted_points;

    struct Orientation
    {
      std::string id;
      double      approx;
      double      adj;
      int         index;          // adjustment index
    };

    typedef std::vector<Orientation> OrientationList;
    OrientationList orientations;

    CovMat<> cov;

    std::vector<int> original_index; // original indexes from the adjustment

    struct Observation
    {
      std::string xml_tag;
      std::string from;
      std::string to;
      std::string left;         // used in angle observation
      std::string right;        //  ....   angle   ....

      double obs;               // observed value
      double adj;               // adjusted
      double stdev;             // standard deviation of adj. value
      double qrr;               // weight coefficient of the residual
      double f;

      double      std_residual; // standardized residual
      std::string err_obs;      // estimate of observed value error
      std::string err_adj;      //  ....       adjusted  ....

      void clear()
      {
        obs = adj = stdev = qrr = f = std_residual = 0;
        xml_tag     .clear();
        from        .clear();
        to          .clear();
        left        .clear();
        right       .clear();
        err_obs     .clear();
        err_adj     .clear();
      }

      double residual() const throw();
    };

    typedef std::vector<Observation> ObservationList;
    ObservationList  obslist;

  private:

    void init()
    {
      gons = true;

      network_general_parameters.gama_local_version.  clear();
      network_general_parameters.gama_local_algorithm.clear();
      network_general_parameters.gama_local_compiler. clear();
      network_general_parameters.epoch.  clear();
      network_general_parameters.axes_xy.clear();
      network_general_parameters.angles. clear();

      coordinates_summary.adjusted.xyz = 0;
      coordinates_summary.adjusted.xy  = 0;
      coordinates_summary.adjusted.z   = 0;
      coordinates_summary.constrained.xyz = 0;
      coordinates_summary.constrained.xy  = 0;
      coordinates_summary.constrained.z   = 0;
      coordinates_summary.fixed.xyz = 0;
      coordinates_summary.fixed.xy  = 0;
      coordinates_summary.fixed.z   = 0;

      observations_summary.distances = 0;
      observations_summary.directions = 0;
      observations_summary.angles = 0;
      observations_summary.xyz_coords = 0;
      observations_summary.h_diffs = 0;
      observations_summary.z_angles = 0;
      observations_summary.s_dists = 0;
      observations_summary.vectors = 0;

      project_equations.equations = 0;
      project_equations.unknowns = 0;
      project_equations.degrees_of_freedom = 0;
      project_equations.defect = 0;
      project_equations.sum_of_squares = 0;
      project_equations.connected_network = true;

      standard_deviation.apriori = 0;
      standard_deviation.aposteriori = 0;
      standard_deviation.using_aposteriori = true;
      standard_deviation.probability = 0;
      standard_deviation.ratio = 0;
      standard_deviation.lower = 0;
      standard_deviation.upper = 0;
      standard_deviation.passed = false;
      standard_deviation.confidence_scale = 0;

      fixed_points      .clear();
      approximate_points.clear();
      adjusted_points   .clear();
    }


    // ----------------------------------------------------------------------

    friend class Parser;

    class Parser : public BaseParser<Exception::parser>
    {
    public:

      LocalNetworkAdjustmentResults *adj;

      Parser(LocalNetworkAdjustmentResults *a) : adj(a) { init(); }

      void init();

      int startElement(const char *name, const char **atts)
      {
        check_and_clear_data();
        attributes = atts;
        tmp_tag = name;
        int t = tag(name);
        TagFun f = tagfun[state][t];
        (this->*f)(true);

        return 0;
      }

      int characterDataHandler(const char *s, int len)
      {
        data += std::string(s, len);

        return 0;
      }

      int endElement(const char *name)
      {
        if (stack.empty()) stack.push(&Parser::unknown);

        TagFun f = stack.top();
        stack.pop();
        (this->*f)(false);
        data.clear();

        return 0;
      }

      void check_and_clear_data();

      std::string name;
      std::string data;

      enum parser_state
        {
          s_error,   /*** error state must be 0 ***/
          s_start,

          s_coordinates_summary,
          s_coordinates_summary_end,
          s_coordinates_summary_adjusted,
          s_coordinates_summary_adjusted_end,
          s_coordinates_summary_constrained,
          s_coordinates_summary_constrained_end,
          s_coordinates_summary_fixed,
          s_coordinates_summary_fixed_end,
          s_count_xyz,
          s_count_xyz_end,
          s_count_xy,
          s_count_xy_end,
          s_count_z,
          s_count_z_end,
          s_description,
          s_description_end,
          s_network_general_parameters,
          s_network_general_parameters_end,
          s_gama_local_adjustment,
          s_network_processing_summary,
          s_network_processing_summary_end,
          s_observations_summary,
          s_observations_summary_end,
          s_distances,
          s_distances_end,
          s_directions,
          s_directions_end,
          s_angles,
          s_angles_end,
          s_xyz_coords,
          s_xyz_coords_end,
          s_h_diffs,
          s_h_diffs_end,
          s_z_angles,
          s_z_angles_end,
          s_s_dists,
          s_s_dists_end,
          s_vectors,
          s_vectors_end,
          s_project_equations,
          s_project_equations_end,
          s_equations,
          s_equations_end,
          s_unknowns,
          s_unknowns_end,
          s_degrees_of_freedom,
          s_degrees_of_freedom_end,
          s_defect,
          s_defect_end,
          s_sum_of_squares,
          s_sum_of_squares_end,
          s_connected_network,
          s_connected_network_end,
          s_disconnected_network,
          s_disconnected_network_end,
          s_standard_deviation,
          s_standard_deviation_end,
          s_apriori,
          s_apriori_end,
          s_aposteriori,
          s_aposteriori_end,
          s_used,
          s_used_end,
          s_probability,
          s_probability_end,
          s_ratio,
          s_ratio_end,
          s_lower,
          s_lower_end,
          s_upper,
          s_upper_end,
          s_passed,
          s_passed_end,
          s_failed,
          s_failed_end,
          s_confidence_scale,
          s_confidence_scale_end,
          s_coordinates,
          s_coordinates_end,
          s_fixed,
          s_fixed_end,
          s_approximate,
          s_approximate_end,
          s_adjusted,
          s_adjusted_end,
          s_point,
          s_point_end,
          s_id,
          s_id_end,
          s_obs_id,
          s_obs_id_end,
          s_x,
          s_x_end,
          s_y,
          s_y_end,
          s_z,
          s_z_end,
          s_orientation_shifts,
          s_orientation_shifts_end,
          s_orientation,
          s_orientation_end,
          s_original_index,
          s_original_index_end,
          s_ors_approx,
          s_ors_approx_end,
          s_ors_adj,
          s_ors_adj_end,
          s_cov_mat,
          s_cov_mat_end,
          s_dim,
          s_dim_end,
          s_band,
          s_band_end,
          s_flt,
          s_flt_end,
          s_observations,
          s_observations_end,
          s_observation,
          s_observation_end,
          s_from,
          s_from_end,
          s_to,
          s_to_end,
          s_ind,
          s_ind_end,
          s_left,
          s_left_end,
          s_right,
          s_right_end,
          s_obs,
          s_obs_end,
          s_obs_adj,
          s_obs_adj_end,
          s_stdev,
          s_stdev_end,
          s_obs_qrr,
          s_obs_qrr_end,
          s_obs_f,
          s_obs_f_end,
          s_std_residual,
          s_std_residual_end,
          s_err_obs,
          s_err_obs_end,
          s_err_adj,
          s_err_adj_end,

          s_stop
        };

      enum xml_tag
        {
          t_adj,
          t_adjusted,
          t_angle,
          t_angles,
          t_approx,
          t_apriori,
          t_aposteriori,
          t_approximate,
          t_band,
          t_confidence_scale,
          t_connected_network,
          t_coordinate_x,
          t_coordinate_y,
          t_coordinate_z,
          t_coordinates,
          t_coordinates_summary,
          t_coordinates_summary_adjusted,
          t_coordinates_summary_constrained,
          t_coordinates_summary_fixed,
          t_count_xyz,
          t_count_xy,
          t_count_z,
          t_cov_mat,
          t_defect,
          t_degrees_of_freedom,
          t_description,
          t_dim,
          t_distances,
          t_distance,
          t_direction,
          t_directions,
          t_disconnected_network,
          t_dx,
          t_dy,
          t_dz,
          t_equations,
          t_err_adj,
          t_err_obs,
          t_f,
          t_failed,
          t_fixed,
          t_flt,
          t_from,
          t_gama_local_adjustment,
          t_h_diffs,
          t_height_diff,
          t_id,
          t_ind,
          t_left,
          t_lower,
          t_network_general_parameters,
          t_network_processing_summary,
          t_obs,
          t_observation,
          t_observations,
          t_observations_summary,
          t_orientation_shifts,
          t_orientation,
          t_original_index,
          t_passed,
          t_point,
          t_probability,
          t_project_equations,
          t_qrr,
          t_ratio,
          t_right,
          t_s_dists,
          t_slope_distance,
          t_standard_deviation,
          t_stdev,
          t_std_residual,
          t_sum_of_squares,
          t_to,
          t_upper,
          t_used,
          t_vectors,
          t_x,
          t_xyz_coords,
          t_y,
          t_z,
          t_z_angles,
          t_zenith_angle,
          t_unknowns,

          t_unknown
        };

      typedef void (Parser::*TagFun)(bool);  // true: start / false: end

      TagFun tagfun[s_stop+1][t_unknown+1];
      std::stack<TagFun>  stack;
      const char ** attributes;
      int   coordinates_summary_stage;

      int                tmp_adj_index;
      std::string        tmp_id;
      Point              tmp_point;
      PointList         *pointlist;
      bool               point_has_x, point_has_y, point_has_z;
      bool               point_con_x, point_con_y, point_con_z;
      bool               tmp_point_adjusted;
      Orientation        tmp_orientation;
      int                tmp_dim;
      int                tmp_band;
      CovMat<>::iterator tmp_i;
      CovMat<>::iterator tmp_e;
      std::string        tmp_tag;
      Observation        tmp_obs;

      int    tag(const char*);
      int    get_int();
      double get_float();
      std::string get_string();

      void unknown(bool);
      void set_state(parser_state new_state) { if (state) state = new_state; }
      void gama_local_adjustment(bool);
      void description(bool);
      void network_general_parameters(bool);
      void network_processing_summary(bool);
      void coordinates_summary(bool);
      void coordinates_summary_adjusted(bool);
      void coordinates_summary_constrained(bool);
      void coordinates_summary_fixed(bool);
      void count_xyz(bool);
      void count_xy(bool);
      void count_z(bool);
      void observations_summary(bool);
      void distances(bool);
      void directions(bool);
      void angles(bool);
      void xyz_coords(bool);
      void id(bool);
      void obs_id(bool);
      void h_diffs(bool);
      void z_angles(bool);
      void s_dists(bool);
      void vectors(bool);
      void project_equations(bool);
      void equations(bool);
      void unknowns(bool);
      void degrees_of_freedom(bool);
      void defect(bool);
      void sum_of_squares(bool);
      void connected_network(bool);
      void disconnected_network(bool);
      void standard_deviation(bool);
      void apriori(bool);
      void aposteriori(bool);
      void used(bool);
      void probability(bool);
      void ratio(bool);
      void lower(bool);
      void upper(bool);
      void passed(bool);
      void failed(bool);
      void confidence_scale(bool);
      void coordinates(bool);
      void fixed(bool);
      void approximate(bool);
      void adjusted(bool);
      void point(bool);
      void x(bool);
      void y(bool);
      void z(bool);
      void orientation_shifts(bool);
      void orientation(bool);
      void ors_approx(bool);
      void ors_adj(bool);
      void cov_mat(bool);
      void original_index(bool);
      void dim(bool);
      void band(bool);
      void flt(bool);
      void ind(bool);
      void observations(bool);
      void observation(bool);
      void from(bool);
      void to(bool);
      void left(bool);
      void right(bool);
      void obs(bool);
      void obs_adj(bool);
      void stdev(bool);
      void obs_qrr(bool);
      void obs_f(bool);
      void std_residual(bool);
      void err_obs(bool);
      void err_adj(bool);
    };



  // --------------------------------------------------------------------------

  };
}

#endif
