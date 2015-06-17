
#ifndef _IOPPROFILE_H_JDH
#define _IOPPROFILE_H_JDH

#include "parser.h"
#include "output.h"
#include "reflecimp.h"
#include "bmatrix.h"
#include "error.h"
#include "iopdata.h"



template <typename SD, typename HDS>
class IOPProfile : public IOPData<SD,HDS> {

    // depth points at which data is defined
  Vector phys_depth_array;

    // one vector for each band
    // elements of the vectors correspond to distance points
  Vector* opt_depth_array;
  Vector* atten_array;
  Vector* absorp_array;
  Vector* Bp_array;
  Vector* bw_div_bp_array;

  SD mean_atten;
  SD mean_absorp;
  SD mean_Bp;
  SD mean_bw_div_bp;

  bool force_mean;

    // phase function of pure water fraction expressed as an L->L function
  ReflecImp<SpectralData_Mono>* sea_water_phase_func;

    // the seawater phase function split into four upward/downward components
    // note Suu = Sdd and Sud = Sdu
  ReflecImp<SpectralData_Mono>* sw_Suu;
  ReflecImp<SpectralData_Mono>* sw_Sud;
  ReflecImp<SpectralData_Mono>* sw_Sdu;
  ReflecImp<SpectralData_Mono>* sw_Sdd;

  ReflecDFTSym<SpectralData_Mono, HDS> Bplus_dft;
  ReflecDFTSym<SpectralData_Mono, HDS> Bminus_dft;

    // table of Fornier-Forand phase functions
  int FF_num;
  double FF_Bp_start;
  double FF_Bp_step;
  ReflecDFTSym<SpectralData_Mono, HDS>** FF_Bplus_dft;
  ReflecDFTSym<SpectralData_Mono, HDS>** FF_Bminus_dft;

    // totals for whole profile
  double total_phys_depth;
  SD total_opt_depth;

  int curr_band;

  mat curr_tau;
  mat curr_rho;

  void loadVectorArray(Vector* array, SD& mean_sd, const JString& filepath);
  void calcOpticalThickness();
  void buildPhaseFunc(double Bp, double bw, double bp);

public:

  IOPProfile(const HDS& hds);
  ~IOPProfile();

  double physicalThickness() const { return total_phys_depth; }

    // todo
  double physicalDistanceToOptical(double d, int band) const {
    if (force_mean) return mean_atten[band] * d; 
    return mean_atten[band] * d;
  }

  void setPhysicalThickness(double t) { 
    //total_phys_depth = t;
    //calcOpticalThickness(); 
  }

  const SD& opticalThickness() const { return total_opt_depth; }

  int loadFromParameters(const Parser& prm);

  void init() { /* nothing to do */ }

  void setCurrentBand(int b);
  void setCurrentLMode(int lm);

  void setCurrOpticalPosition(double op);

  const mat& currTau() { return curr_tau; }
  const mat& currRho() { return curr_rho; }

};



// IOPProfile::(constructor)

template <typename SD, typename HDS>
IOPProfile<SD,HDS>::IOPProfile(const HDS& hds) : 
  IOPData<SD,HDS>(hds),
  sea_water_phase_func(0),
  sw_Suu(0),
  sw_Sud(0),
  sw_Sdu(0),
  sw_Sdd(0),
  Bplus_dft(hds),
  Bminus_dft(hds),
  FF_num(0),
  FF_Bplus_dft(0),
  FF_Bminus_dft(0) { 

  curr_tau.resize(hds.rowsDFT(), hds.rowsDFT());
  curr_rho.resize(hds.rowsDFT(), hds.rowsDFT()); 
}



// IOPProfile::(destructor)

template <typename SD, typename HDS>
IOPProfile<SD,HDS>::~IOPProfile() {
  delete sea_water_phase_func;
  delete sw_Suu;
  delete sw_Sud;
  delete sw_Sdu;
  delete sw_Sdd;
  for (int i=0; i<FF_num; i++) {
    delete FF_Bplus_dft[i];
    delete FF_Bminus_dft[i];
  }
  delete[] FF_Bplus_dft;
  delete[] FF_Bminus_dft;
}



// IOPProfile::calcOpticalThickness()

template <typename SD, typename HDS>
void IOPProfile<SD,HDS>::calcOpticalThickness() {

    // start optical depth at zero
  for (int b=0; b<SD::bands(); b++) {
    opt_depth_array[b].makeZeros(phys_depth_array.size());
    opt_depth_array[b][0] = 0;
  }

    // loop through depth points to calculate optical depth at each point
  for (int i=1; i<phys_depth_array.size(); i++) {
      // physical distance between these points
    double d = phys_depth_array[i] - phys_depth_array[i-1];
      // move optical depth on in each band
    for (int b=0; b<SD::bands(); b++) opt_depth_array[b][i] = opt_depth_array[b][i-1] + d * (atten_array[b][i-1] + atten_array[b][i]) / 2;
  }

    // set up total optical depth
  for (int b=0; b<SD::bands(); b++) total_opt_depth[b] = opt_depth_array[b][phys_depth_array.size()-1];
}



// IOPProfile::loadVectorArray() [private]

template <typename SD, typename HDS>
void IOPProfile<SD,HDS>::loadVectorArray(Vector* array, SD& mean_sd, const JString& filepath) {

  TextTable tab;
  tab.readFromFile(filepath);

    // check number of columns
  if (tab.columnCount() != SD::bands()) dataError("Wrong number of columns in '%s'",filepath.latin1());

    // check number of depth points
  if (tab.rowCount() != phys_depth_array.size()) dataError("Wrong number of rows in '%s'",filepath.latin1());

  for (int b=0; b<SD::bands(); b++) {

    array[b].makeZeros(tab.rowCount());
    mean_sd[b] = 0;

    for (int r=0; r<tab.rowCount(); r++) {

      array[b][r] = tab.getText(r,b).toDouble();

      if (r>0) {
	double d = phys_depth_array[r] - phys_depth_array[r-1];
	mean_sd[b] += d * ((array[b][r] + array[b][r-1]) / 2);
      }
    }
    mean_sd[b] /= total_phys_depth;
  }
}



// IOPProfile::loadFromParameters()

template <typename SD, typename HDS>
int IOPProfile<SD,HDS>::loadFromParameters(const Parser& prm) {

  const HDS& hds(IOPData<SD,HDS>::halfDirecStruct());

    // read in the physical distance points
  phys_depth_array.readFromTextFile(prm.getString("depth_list"));
  total_phys_depth = phys_depth_array[phys_depth_array.size()-1];

  force_mean = prm.getBool("force_mean");

    // these quantites have one vector per band
  opt_depth_array = new Vector[SD::bands()];
  atten_array = new Vector[SD::bands()];
  absorp_array = new Vector[SD::bands()];
  Bp_array = new Vector[SD::bands()];
  bw_div_bp_array = new Vector[SD::bands()];

    // read in attenuation
  loadVectorArray(atten_array, mean_atten, prm.getString("atten_list"));
    // read in absorption
  loadVectorArray(absorp_array, mean_absorp, prm.getString("absorp_list"));
    // read in particulate backscatter
  loadVectorArray(Bp_array, mean_Bp, prm.getString("Bp_list"));
    // read in proprtion of pure water scatter to particulate backscatter
  loadVectorArray(bw_div_bp_array, mean_bw_div_bp, prm.getString("bw_div_bp_list"));

  printf("mean_atten: ");
  mean_atten.display();
  printf("mean_absorp: ");
  mean_absorp.display();
  printf("mean_Bp: ");
  mean_Bp.display();
  printf("mean_bw_div_bp: ");
  mean_bw_div_bp.display();

  calcOpticalThickness();

    // pure water phase function
  FileDesc fd(prm.getString("sea_water_phase_func"), false);

  FileInfo fi(fd);

  if (!fi.magicNumOk()) 
    return dataError("Files %s is not an Aqrad File",fd.path().latin1());
  if (!fi.isReflecFile()) 
    return dataError("Files %s is not a Reflection File",fd.path().latin1());
  if (fi.bandCount() != 1)
    return dataError("Files %s has wrong number of bands (%d should be 1)",fd.path().latin1(),fi.bandCount());
    
  sea_water_phase_func = ReflecImp<SpectralData_Mono>::createFromFile(fd);

    // convert to L -> L function
  sea_water_phase_func->multiplyAllByInSA();
    // scatter functions are stored times 4pi
  sea_water_phase_func->divide(4*M_PI);

    // split into hemisphere functions
    // note due to symmetry Suu = Sdd and Sdu == Sud
  sea_water_phase_func->splitCreate(sw_Sdu, sw_Sdd, sw_Suu, sw_Sud);

  /*  
  printf("Sdd:\n");
  Sdd->display();
  printf("Suu:\n");
  Suu->display();
  printf("Sud:\n");
  Sud->display();
  printf("Sdu:\n");
  Sdu->display();
  */

  Output::displayDiffuseReflectance("sw_Sdd: ",*sw_Sdd);
  Output::displayDiffuseReflectance("sw_Suu: ",*sw_Suu);
  Output::displayDiffuseReflectance("sw_Sud: ",*sw_Sud);
  Output::displayDiffuseReflectance("sw_Sdu: ",*sw_Sdu);

  //Bplus.copy(*Suu);
  //Bminus.copy(*Sud);

  FF_num = prm.getInt("FF_num");
  FF_Bp_start = prm.getDouble("FF_Bp_start");
  FF_Bp_step = prm.getDouble("FF_Bp_step");

  FF_Bplus_dft = new ReflecDFTSym<SpectralData_Mono, HDS>*[FF_num];
  FF_Bminus_dft = new ReflecDFTSym<SpectralData_Mono, HDS>*[FF_num];

  JString filepath(prm.getString("FF_table_list"));

  TextTable tab;
  tab.readFromFile(filepath);

  double curr_Bp = FF_Bp_start;

  for (int i=0; i<tab.rowCount(); i++, curr_Bp+=FF_Bp_step) {

      // phase function file
    FileDesc fd(tab.getText(i,0), false);

    printf("Loading %s (Bp = %f)\n",fd.path().latin1(),curr_Bp);

    FileInfo fi(fd);

    if (!fi.magicNumOk()) 
      return dataError("Files %s is not an Aqrad File",fd.path().latin1());
    if (!fi.isReflecFile()) 
      return dataError("Files %s is not a Reflection File",fd.path().latin1());
    if (fi.bandCount() != 1)
      return dataError("Files %s has wrong number of bands (%d should be 1)",fd.path().latin1(),fi.bandCount());
    
    ReflecImp<SpectralData_Mono>* ri = ReflecImp<SpectralData_Mono>::createFromFile(fd);

      // convert to L -> L function
    ri->multiplyAllByInSA();
      // scatter functions are stored times 4pi
    ri->divide(4*M_PI);

    ReflecImp<SpectralData_Mono>* Suu;
    ReflecImp<SpectralData_Mono>* Sud;
    ReflecImp<SpectralData_Mono>* Sdu;
    ReflecImp<SpectralData_Mono>* Sdd;

      // split into hemisphere functions
      // note due to symmetry Suu = Sdd and Sdu == Sud
    sea_water_phase_func->splitCreate(Sdu, Sdd, Suu, Sud);

      // allocate dft structres
    FF_Bplus_dft[i] = new ReflecDFTSym<SpectralData_Mono, HDS>(hds);
    FF_Bminus_dft[i] = new ReflecDFTSym<SpectralData_Mono, HDS>(hds);

      // build dft representation
    FF_Bplus_dft[i]->copy(*Suu);
    FF_Bminus_dft[i]->copy(*Sud);

      // clean up temporaries
    delete ri;
    delete Suu;
    delete Sud;
    delete Sdu;
    delete Sdd;
  }

  return 0;
}



// IOPProfile::setCurrentBandAndLMode()
// this needs to setup tau and rho

template <typename SD, typename HDS>
void IOPProfile<SD,HDS>::setCurrentBand(int band) {

  curr_band = band;

    // if using profile mean then
  if (force_mean) {

      // relative contribution of particles and pure sea water to the phase function
    double bp = 1.0 / (1.0 + mean_bw_div_bp[band]);
    double bw = 1.0 - bp;

    bw=bw;

      // builds the phase function in Bplus_dft and Bminus_dft
    //buildPhaseFunc(mean_Bp[band], bw, bp);
 
    Bplus_dft.copy(*FF_Bplus_dft[0]);
    Bminus_dft.copy(*FF_Bminus_dft[0]);
 
      // divide by attenuation
    Bplus_dft.divide(mean_atten[band]);
    Bminus_dft.divide(mean_atten[band]);

    return;
  }
}



// IOPProfile::buildPhaseFunc()

template <typename SD, typename HDS>
void IOPProfile<SD,HDS>::buildPhaseFunc(double Bp, double bw, double bp) {

    // initialise pure water part of phase function
  Bplus_dft.copy(*sw_Suu);
  Bminus_dft.copy(*sw_Sud);
  Bplus_dft.multiply(bw);
  Bminus_dft.multiply(bw);

    // index into table for FF phase function
  int i1 = (int) floor((Bp - FF_Bp_start) / FF_Bp_step);

    // if before start of table use first entry
  if (i1<0) {
    Bplus_dft.addMultiple(*FF_Bplus_dft[0], bp);
    Bminus_dft.addMultiple(*FF_Bminus_dft[0], bp);
    return;
  }

    // if after end of table use last entry
  if (i1>=FF_num-1) {
    Bplus_dft.addMultiple(*FF_Bplus_dft[FF_num-1], bp);
    Bminus_dft.addMultiple(*FF_Bminus_dft[FF_num-1], bp);
    return;
  }

    // fraction of next phase function in table to use
  double frac2 = Bp - (i1 * FF_Bp_step);
    // fraction of this function in table to use
  double frac1 = 1 - frac2;

    // add on linear interpolation of forward part
  Bplus_dft.addMultiple(*FF_Bplus_dft[i1], frac1 * bp); 
  Bplus_dft.addMultiple(*FF_Bplus_dft[i1+1], frac2 * bp); 
 
    // add on linear interpolation of backward part
  Bminus_dft.addMultiple(*FF_Bminus_dft[i1], frac1 * bp); 
  Bminus_dft.addMultiple(*FF_Bminus_dft[i1+1], frac2 * bp); 
}



// IOPProfile::setCurrentLMode()

template <typename SD, typename HDS>
void IOPProfile<SD,HDS>::setCurrentLMode(int lm) {

    // if using mean profile values then can calculate tau and rho now
  if (force_mean) {
    calcTauAndRho(curr_tau, curr_rho, Bplus_dft, Bminus_dft, curr_band, lm);
    printf("BAND %d lm %d\n",curr_band,lm);
    std::cout << curr_tau << "\n";
    std::cout << curr_rho << "\n";
    return;
  }

}



// IOPProfile::setCurrOpticalPosition() 

template <typename SD, typename HDS>
void IOPProfile<SD,HDS>::setCurrOpticalPosition(double) { 

    // if using mean values then already set up
  if (force_mean) return;

}



#endif // _IOPPROFILE_H_JDH

