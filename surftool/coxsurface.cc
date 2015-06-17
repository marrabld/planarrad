
// coxsurface.h

#include "coxsurface.h"
#include "math/mathutil.h"
#include "math/rand.h"
#include "base/jlog.h"

using namespace jude::math;



  // static constants from the wind-speed law
const double CoxSurface::au = 0.00316;
const double CoxSurface::ac = 0.00192;
 


// CoxSurface::(constructor)

CoxSurface::CoxSurface() : 
  vec(0), 
  fct(0) { }



// CoxSurface::(destructor)

CoxSurface::~CoxSurface() { 
  delete[] vec; 
  delete[] fct; 
}



bool CoxSurface::init(const JParser& prm) {

  wind_speed = prm.getDouble("wind_speed");
  wind_direc_deg = prm.isDefined("wind_direc") ? prm.getDouble("wind_direc") : 0;

  crosswind_vertices = prm.getInt("crosswind_vertices");

  upwind_vertices = prm.getInt("upwind_vertices");

    // the along-wind size of the grid to generate in units of whatever dimensions are relevant
  surface_size = prm.getDouble("surface_size");
    // to generate a circular surface set the radius in the same units as surface_size
  surface_radius = prm.getDouble("surface_radius");

    // if something was not defined a message will have been streamed to jlog::es
  if (prm.somethingWasNotDefined()) return false;

    // wind forward direction angle from x-axis (phi) in radians
  wind_direc_rad = degToRad(wind_direc_deg);

    // list of vectors and facets to store the positions
  vec = new Vec3D[crosswind_vertices * upwind_vertices];
  fct = new Facet[2 * (crosswind_vertices-1) * (upwind_vertices-1)];
 
    // wind direction is angle from N (clockwise looking down) looking at the 
    // direction wind is coming from
  // sensor_x_ref = Vec3D(cos(direc_rad), sin(direc_rad), 0);
  // sensor_y_ref = Vec3D(-sin(direc_rad), cos(direc_rad), 0);

  //sensor_x_ref.multiply(target_size/2);
  //sensor_y_ref.multiply(target_size/2);

  return true;
}




// CoxSurface::withinSRad() [private]

bool CoxSurface::withinSRad(int i) {
  return vec[i].lengthSquared() <= (surface_radius * surface_radius); 
}



// ResWaterSurface::generateFramework() [private]
// calculates the resolution and sd of hieghts and builds the spatial frame of the surface
// the framework is centred around x=0, y=0
// if up is odd and cp is even then (0,0) will lie in the centre of a facet

void CoxSurface::buildSurface() {
  
  scale = surface_size / (upwind_vertices-1);

    // the along-wind distance between points (relative to other dimensions)
  delta = 1;
    // the crosswind distance between points (relative to other dimensions)
  epsilon = sqrt((3*au)/(4*ac));
    // the standard deviation of the heights (relative to other dimensions)
  sigma = sqrt((au*wind_speed)/2);

  min_z = 0;
  max_z = 0;

    // y-axis is in the crosswind direction
  double y = - (crosswind_vertices - 1) * epsilon / 2;
  double sx = - (((upwind_vertices-1) * delta / 2) + (delta / 4));

    // current vertex index
  int v = 0;
    // current facet pointer
  Facet *cf = fct;

    // in crosswind direction vertices are spaced uniformly
  for (int i=0; i<crosswind_vertices; i++, y+=epsilon) {
  
      // x is in the upwind direction and start position depends on row
    double x = (i&1) ? sx+delta/2 : sx;

    for (int j=0; j<upwind_vertices; j++, x+=delta, v++) {

      double z = normal_dbn_m48(0, sigma) * scale;

        // x and y position
      vec[v].setX(x * scale);
      vec[v].setY(y * scale); 
      vec[v].setZ(z);

      if (z < min_z) min_z = z;
      if (z > max_z) max_z = z;

      if (i>0 && j>0) {
	if (i&1) {

	    // v0, v1, are along wind
	  if (withinSRad(v-1) || withinSRad(v) || withinSRad(v-upwind_vertices)) 
	    (cf++)->setTo(v-1, v, v-upwind_vertices);

	  if (withinSRad(v-upwind_vertices-1) || withinSRad(v-1) || withinSRad(v-upwind_vertices)) 
	    (cf++)->setTo(v-upwind_vertices, v-upwind_vertices-1, v-1);

	} else {

	  if (withinSRad(v-1) || withinSRad(v) || withinSRad(v-upwind_vertices-1)) 
	    (cf++)->setTo(v-1, v, v-upwind_vertices-1);

	  if (withinSRad(v-upwind_vertices-1) || withinSRad(v) || withinSRad(v-upwind_vertices)) 
	    (cf++)->setTo(v-upwind_vertices, v-upwind_vertices-1, v);
	}
      }
    }
  }

  total_facet_count = cf - fct;
  if (jlog::vbLevel(2)) jlog::ls << "Total facets: " << total_facet_count << "\n";

    // calculate facet normals etc.
  if (jlog::vbLevel(3)) jlog::ls << "Calculating facet geometry...\n";
  for (int i=0; i<total_facet_count; i++) fct[i].calcGeometry(vec);
}



// CoxSurface::findIsecFacet()

const Facet* CoxSurface::findIsecFacet(const Vec3D& v0, const Vec3D& dv, const Facet* ignore_facet, double& dist) const {

  const Facet* closest_facet = 0;
  double d;

  for (int i=0; i<total_facet_count; i++) {

    if (&fct[i] == ignore_facet) continue;

    if (fct[i].intersects(v0, dv, d)) {
      if (d > 0) if (!closest_facet || d < dist) { dist = d; closest_facet = &fct[i]; }
    }
  }

  return closest_facet;
}



// CoxSurface::checkSlopes() 

void CoxSurface::checkSlopes() {
  
  double mean_along_slope = 0;
  double mean_across_slope = 0;
  double mean_slope = 0;
  double across_sumsq = 0;
  double along_sumsq = 0;
  double total_sumsq = 0;
  double max_slope = 0;

  for (int i=0; i<total_facet_count; i++) {

    const Vec3D& v0 = vec[fct[i].vertexIndex0()];
    const Vec3D& v1 = vec[fct[i].vertexIndex1()];
    const Vec3D& v2 = vec[fct[i].vertexIndex2()];

    double along_slope = (v1.z() - v0.z()) / (v1.x() - v0.x());
    double across_slope = ((v1.z() + v0.z())/2 - v2.z()) / (v0.y() - v2.y());
    
      // calculate normal vector to facet
    Vec3D dv0(v1 - v0);
    Vec3D dv1(v2 - v0);
    Vec3D norm(Vec3D::crossProduct(dv0,dv1));
    norm.setLength(1);
      // get the slope from the normal z-component
    //double slope = (fabs(norm.z()) - 1 < 1E-10) ? 0 : tan(acos(norm.z()));
    double slope = tan(acos(norm.z()));
    total_sumsq += slope * slope;
    //jlog::ls << norm.z() << " " << slope << "\n";
   
    if (fabs(slope) > max_slope) max_slope = fabs(slope);

    //printf("%d %d %d along %f across %f %f %f\n",fct[i].v0,fct[i].v1,fct[i].v2,along_slope,across_slope,(v0.y() - v2.y())/scale,epsilon);
    //v0.display();
    //v1.display();
    //v2.display();

    mean_along_slope += along_slope;
    mean_across_slope += across_slope;
    mean_slope += fabs(slope);

    along_sumsq += along_slope * along_slope;
    across_sumsq += across_slope * across_slope;
    
  }

  mean_along_slope /= total_facet_count;
  mean_across_slope /= total_facet_count;
  mean_slope /= total_facet_count;

  double along_sd = sqrt(along_sumsq / total_facet_count - mean_along_slope * mean_along_slope);
  double across_sd = sqrt(across_sumsq / total_facet_count - mean_across_slope * mean_across_slope);

  jlog::ls << "Maximum z-value: " << max_z << "\n";
  jlog::ls << "Minimum z-value: " << min_z << "\n";

  jlog::ls.printf("mean_along_slope %f\n", mean_along_slope);
  jlog::ls.printf("along_sd %f (should be %f)\n", along_sd, sqrt(au*wind_speed));
  jlog::ls.printf("mean_across_slope %f\n", mean_across_slope);
  jlog::ls.printf("across_sd %f (should be %f)\n", across_sd, sqrt(ac*wind_speed));
  jlog::ls.printf("mss_along %f\n", along_sumsq / total_facet_count);
  jlog::ls.printf("mss_across %f\n", across_sumsq / total_facet_count);

  double mss_est_lo = (3 + 5.12 * wind_speed) / 1000 - 0.004;
  double mss_est_hi = mss_est_lo + 0.008;
  jlog::ls.printf("mss_total %f (should be %f - %f)\n", total_sumsq / total_facet_count, mss_est_lo, mss_est_hi);
  jlog::ls.printf("max_slope %f mean_slope %f\n",max_slope,mean_slope);
}

