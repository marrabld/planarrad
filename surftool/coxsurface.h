
// coxsurface.h

#ifndef _JUDETOOLS_COXSURFACE_H
#define _JUDETOOLS_COXSURFACE_H

#include "S3/vec3d.h"
#include "base/jparser.h"
#include "facet.h"

using namespace jude::base;
using namespace jude::S3;



class CoxSurface {

  static const double au;
  static const double ac;

  int upwind_vertices;       // upwind number of points 
  int crosswind_vertices;    // crosswind number of points;
  double wind_speed;         // wind speed in ms-1

    // wind forward direction in degrees from x-axis clockwise looking down on surface
  double wind_direc_deg;    
    // wind forward direction in radians
  double wind_direc_rad;    

  Vec3D* vec;
  Facet* fct;
 
  double surface_size;
  double surface_radius;
  double target_radius;
 
  double scale;
  double delta;
  double epsilon;
  double sigma;

  double min_z;
  double max_z;

  int total_facet_count;

  /*
  int rays_per_quad;
  bool is_cube;
  int quads_num;
  int quads_vn;
  int quads_hn;
  JString theta_points_file;

  JString file_base;

  Vector3D sensor_x_ref;
  Vector3D sensor_y_ref;
  */

  bool withinSRad(int i); 

public:

  CoxSurface();
  ~CoxSurface();

  bool init(const JParser& prm);

  void buildSurface();
  void checkSlopes();

  double minZ() const { return min_z; }
  double maxZ() const { return max_z; }

  const Facet* findIsecFacet(const Vec3D& v0, const Vec3D& dv, const Facet* ignore_facet, double& dist) const;

};



#endif // _JUDETOOLS_COXSURFACE_H


