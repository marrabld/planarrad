
#include "spquadintegrator.h"
#include "spquad.h"

using namespace jude::ds;



/*! \brief Constructs a quad integrator for quad \a q with sub-quad solid angle \a sa. 

If \a sa is zero a sub-quad solid angle of 4PI / 10^6 is used, i.e. corresponding to subdividing the sphere into a million segments. 
*/

SpQuadIntegrator::SpQuadIntegrator(const SpQuad& q, double sa) :
  q(q) {

    // if sub quad solid angle not defined then default to 1/1E6 of sphere
  if (sa==0) sa = (4*M_PI) / 1E6;

    // set the integration step and initialise
  setSubSolidAngle(sa);
}



// SpQuadIntegrator::setSubSolidAngle()
// sets the integration steps (num_i and num_j) based on the approximate desired solid angle of sub quads

void SpQuadIntegrator::setSubSolidAngle(double sa) {
 
  num_ct = (int) sqrt(q.solidAngle() / sa);
  if (num_ct < 1) num_ct = 1; 
  num_phi = num_ct;

  ct_start = cos(q.thetaMax());
  ct_d = (cos(q.thetaMin()) - ct_start) / num_ct;

  phi_start = q.phiMin();
  phi_d = (q.phiMax() - phi_start) / num_phi;

    // solid angle is in fact the same for all subquads
  curr_sa = ct_d * phi_d; 
 
  reset();
}



// SpQuadIntegrator::reset()

void SpQuadIntegrator::reset() {

  curr_ct = 0;
  curr_phi = 0;
   
  setDirection();
}



// SpQuadIntegrator::next()

void SpQuadIntegrator::next() { 
  if ((++curr_ct) == num_ct) { 
    curr_ct = 0; 
    curr_phi++; 
  } 
  setDirection();
}



//  SpQuadIntegrator::setDirection() [private]

void SpQuadIntegrator::setDirection() {

    // cos theta is based on the mean cos theta for the sub quad
  curr_cos_theta = ct_start + (curr_ct + 0.5) * ct_d;

  double theta = acos(curr_cos_theta);
  double phi = phi_start + (curr_phi + 0.5) * phi_d; 

  curr_direc = Vec3D(theta, phi);
}

