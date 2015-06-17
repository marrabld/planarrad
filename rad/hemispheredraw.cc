
// hemispheredraw.cc

#include "rad/hemispheredraw.h"
#include "rad/bandspec.h"
#include "rad/radutils.h"
#include "base/jbuffer.h"
#include "base/jlist.h"
#include "base/jsortable.h"
//#include <math.h>

using namespace jude::rad;



// HemiSphereDraw::(constructor)

HemiSphereDraw::HemiSphereDraw() :
  bs(0),
  rdata(0),
  do_zplus(true),
  sens_k(0),
  max_sens_k(1),
  rgb_data(0) { }



// HemiSphereDraw::(destructor)

HemiSphereDraw::~HemiSphereDraw() {
  delete[] rgb_data;
}



// HemiSphereDraw::setSourceData()

void HemiSphereDraw::setSourceData(const RadDirecImp<SBandData>* rd) {
    // copy pointer to spec or null to disable the view
  rdata = rd;
    // clear and reallocate table of rgb values
  delete[] rgb_data;
  rgb_data = (rdata) ? new uns32[rdata->direcStruct().units()] : 0;
    // calculate initial rgb table data based on sens_k
  recalcRGBData();
}



// HemiSphereDraw::recalcRGBData()

void HemiSphereDraw::recalcRGBData() {

  if (!rdata || !bs) return;
  //if (!rdata || !bs || sens_k == 0) return;

  for (int i=0; i<rdata->direcStruct().units(); i++) {
    rgb_data[i] = RadUtils::radianceToRGB(rdata->atIndex(i), *bs, sens_k);
    //jlog::ls << rdata->atIndex(i) << "\n" <<  rgb_data[i] << "\n";
  }
}



// HemiSphereDraw::setSensK()

void HemiSphereDraw::setSensK2(double s) { 
  sens_k = s; 
  recalcRGBData();
}



// HemiSphereDraw::setSensK()

void HemiSphereDraw::setRelativeSensK(double s) { 
  sens_k = s * max_sens_k; 
  recalcRGBData();
}



// HemiSphereDraw::autoSetSensK()
// perhaps this function ought to be globally avalaible somehow so can use the same system everywhere
// need to auto set sensitivities

double HemiSphereDraw::autoSetSensK() {
 
    // if can't calculate for whatever reason then sens_k is zero which means everything is black
  sens_k = 0;

    // if no data or band specification then cannot calculate
  if (!rdata || !bs) return 0;

  const DirecStruct& ds(rdata->direcStruct());

    // a list for sorting the entries
  JList<JSortableDouble> lum_list;
  lum_list.setAutoDelete(true);

    // loop through all indices in the directional data
  for (int i=0; i<rdata->direcStruct().units(); i++) {

      // only do if this index is on the hemisphere we are doing
    if (ds.isZPlus(i) == do_zplus) {
        // spectral radiance data at this index
      const SBandData& sd(rdata->atIndex(i));
        // maximum lumens across the RGB
      double lumens = max(max(bs->redLumens(sd), bs->greenLumens(sd)), bs->blueLumens(sd));
        // add to list if not zero
      if (lumens>0) lum_list.append(new JSortableDouble(lumens));
    }
  }

    // we want to retain half
  int num = lum_list.count() / 2;
    // the lower 10 per cent plus one to ensure is at least one entry
  int lower_num = (lum_list.count() / 10) + 1;
    // if none then can't calculate sens_k
  if (num == 0) return 0;
    // an array for the middle section
  JSortableDouble* middle_half[num];
    // an array for the lower section
  JSortableDouble* lower_part[lower_num];
    // sort the list
  lum_list.sort();
    // pull out the middle section
  lum_list.buildArrayFromList((void**)middle_half, num/2, num);
     // pull out the bottom section
  lum_list.buildArrayFromList((void**)lower_part, 0, lower_num);

    // get the mean lumens of the middle half
  double mean_lumens = 0;
  for (int i=0; i<num; i++) mean_lumens += middle_half[i]->value();
  mean_lumens /= num;

    // the mean lumens of the lower half
  double mean_lower_lumens = 0;
  for (int i=0; i<lower_num; i++) mean_lower_lumens += lower_part[i]->value();
  mean_lower_lumens /= lower_num;

    // set sensitivity to give mean value in middle of rgb range
  sens_k = RadUtils::sensToGiveRGBFromRad(mean_lumens, 200);
     // log for debugging
  if (jlog::vbLevel(5)) jlog::ls << "mean lumens: " << mean_lumens << " sens_k: " << sens_k << "\n";

    // try to estimate the maximum sensitivity that is any use
  max_sens_k = RadUtils::sensToGiveRGBFromRad(mean_lower_lumens, 250);
     // log for debugging
  if (jlog::vbLevel(5)) jlog::ls << "mean lower lumens: " << mean_lower_lumens << " max_sens_k: " << max_sens_k << "\n";

  recalcRGBData();

  lum_list.clear();

  return sens_k;
}



// HemiSphereDraw::drawToBuffer()

void HemiSphereDraw::drawToBuffer(JBuffer& buff) {
 
  buff.fill(0xff808080);

  if (!rdata) return;
  //if (!rdata || sens_k==0) return;

    // width and height to draw in pixels
  int draw_w = buff.width();
  int draw_h = buff.height();
    // where to start drawing
  int draw_xs = 0;
  int draw_ys = 0;

    // make square
  if (draw_w > draw_h) {
    draw_xs += (draw_w - draw_h) / 2;
    draw_w = draw_h;
  } else if (draw_h > draw_w) {
    draw_ys += (draw_h - draw_w) / 2;
    draw_h = draw_w;
  }

    // these will now be the same
  double xfac = 2.0 / draw_w;
  double yfac = 2.0 / draw_h;

  SBandData sd;

  double xp = xfac/2 - 1;
  for (int x=0; x<draw_w; x++, xp+=xfac) {

    double yp = yfac/2 - 1;
    for (int y=0; y<draw_h; y++, yp+=yfac) {

      double ss = (xp*xp + yp*yp);
      if (ss < 1) {

	double zp = std::sqrt(1 - ss);

	  // flip yp for normal draw of z-plus hemishpere (I.e looking in z- direc)
	Vec3D v(xp,-yp,zp);
	  // if is not looking at z-plus hemisphere then flip by rotating about x
	if (!do_zplus) v = v.rotate180AboutX();

	buff.writePixel(x+draw_xs, y+draw_ys, rgb_data[rdata->direcStruct().direcToIndex(v)]);
      }
    }
  }
}

