
#include "rad/radutils.h"
#include "rad/hemispheredraw.h"
#include "base/jbuffer.h"
#include "base/jimagesave.h"
#include "ds/dsquad.h"

using namespace jude::rad;



uns32 RadUtils::radianceToRGB(const SBandData& sd, const BandSpec& bs, double sens_k) {

    // apply exposure function	
  int r = radianceToRGB(bs.redLumens(sd), sens_k);
  int g = radianceToRGB(bs.greenLumens(sd), sens_k);
  int b = radianceToRGB(bs.blueLumens(sd), sens_k);

  if (r < 0) r = 0;
  if (b < 0) b = 0;
  if (g < 0) g = 0;

  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;

  uns32 c = 0xff000000 | (r<<16) | (g<<8) | b;

  return c;
}



uns32 RadUtils::radianceToRGB(const SBandData& sd, const SBandData& mean, 
			      const SBandData& rf, const SBandData& gf, const SBandData& bf, double sens_k) {

  SBandData temp(sd);
  //temp.divide(temp.sum());
  temp.subtract(mean);
  //temp.subtract(SpectralData(sqrt(1.0 / SpectralData::bands())));

    // apply exposure function	
  int r = radianceToRGB(temp.project(rf), sens_k) + 128;
  int g = radianceToRGB(temp.project(gf), sens_k) + 128;
  int b = radianceToRGB(temp.project(bf), sens_k) + 128;

  if (r<0) r = 0;
  if (g<0) g = 0;
  if (b<0) b = 0;

  if (r>255) r = 255;
  if (g>255) g = 255;
  if (b>255) b = 255;

  uns32 c = 0xff000000 | (r<<16) | (g<<8) | b;

  return c;
}


    
  /*

    // check we got some data
  if (sd.bandCount() == 0) { jlog::es << msg << " is zero length.\n"; return false; }

    // see if there is a band specification associated with this spectral data
  BandSpec* src_bs = BandSpec::create(prm, name+"_");
  if (src_bs) {

      // can't resample if no wavelength data
    if (!src_bs->hasWavelengthData()) {
      jlog::es << msg << " band specification has no wavelength data. Either provide wavelengths or remove it.\n"; 
      return false;
    }
      // can't resample if no target wavelength data
    if (!master_bs.hasWavelengthData()) {
      jlog::es << "No output band wavelengths are given so " << msg << " cannot be resampled. ";
      jlog::es << "Remove its band specification or provide output wavelengths.\n"; 
      return false;
    }
      // info
    if (jlog::vbLevel(3)) jlog::ls << "Resampling " << msg << " from " << src_bs->bandCount() << " bands.\n";
      // check that really is the number of bands
    if (!src_bs->checkBandCount(msg, sd)) return false;
      // resample
    sd = master_bs.resampleToThis(sd, *src_bs);
    delete src_bs;

    // if the loaded data is in one band only then can just repeat up to required bands if not requesting interpolation
  } else if (sd.bandCount()==1 && master_bs.bandCount()!=1) {
      // info
    if (jlog::vbLevel(3)) jlog::ls << "Expanding " << msg << " from one band to " << src_bs->bandCount() << " bands by repeating the data.\n";
      // repeat ethe data to the right number of bands
    sd = SBandData(master_bs.bandCount(), sd[0]);

  } else {

      // final check we now have total irradiance in the number of output bands
    if (sd.bandCount() != master_bs.bandCount()) {
      jlog::es << "Number of bands of " << msg << " (" << sd.bandCount() << ") does not ";
      jlog::es << "match band specification (" << master_bs.bandCount() << ") and no source band specification was supplied for resampling.\n";
      return false;
    }
 }

    // final check we now have total irradiance in the number of output bands
  if (!master_bs.checkBandCount(msg, sd)) return false;
  */



template <typename OBJ>
bool RadUtils::handleResample(OBJ& obj, const JParser& prm, const JString& param_code, const JString& desc, const BandSpec& targ_bs) {

    // check we got some data
  if (obj.bandCount() == 0) { jlog::es << desc << " spectral data is zero length.\n"; return false; }

    // see if there is a band specification associated with loaded entity
  BandSpec* src_bs = BandSpec::create(prm, param_code+"_");
  if (src_bs) {

      // can't resample if no wavelength data
    if (!src_bs->hasWavelengthData()) {
      jlog::es << "Band specification for " << desc << " (prefix '" << param_code << "') has no wavelength data. ";
      jlog::es << "It cannot be used for resampling so the need for a band specification is ambiguous. Remove it or provide wavelengths.\n"; 
      return false;
    }
      // can't resample if no target wavelength data
    if (!targ_bs.hasWavelengthData()) {
      jlog::es << "No output band wavelengths are given so " << desc << " cannot be resampled. ";
      jlog::es << "Remove its band specification or provide output wavelengths.\n"; 
      return false;
    }
      // check that really is the number of bands
    if (!src_bs->bandCount() == obj.bandCount()) {
      jlog::es << "Number of bands of loaded " << desc << " file (" << obj.bandCount() << ") does not ";
      jlog::es << "match " << desc << " band specification (" << src_bs->bandCount() << ").\n";
      return false;
    }
      // info
    if (jlog::vbLevel(3)) jlog::ls << "Resampling " << desc << " from " << src_bs->bandCount() << " bands to " << targ_bs.bandCount() << " bands.\n";
      // resample
    obj.resample(targ_bs, *src_bs);
    delete src_bs;

    // if the loaded data is in one band only then can just repeat up to required bands if not requesting interpolation
  } else if (obj.bandCount()==1 && targ_bs.bandCount()!=1) {
      // info
    if (jlog::vbLevel(3)) jlog::ls << "Expanding " << desc << " from one band to " << targ_bs.bandCount() << " bands by repeating the data.\n";
      // repeat the data to the right number of bands
    obj.expandBandCount(targ_bs.bandCount());

    // otherwise couldn't resample so better be in right number of bands
  } else {

      // final check we now have total irradiance in the number of output bands
    if (obj.bandCount() != targ_bs.bandCount()) {
      jlog::es << "Number of bands of " << desc << " (" << obj.bandCount() << ") does not ";
      jlog::es << "match output band specification (" << targ_bs.bandCount() << ") and no source band specification was supplied for resampling.\n";
      return false;
    }
  }

  return true;
}



// RadUtils::loadSBandData()

bool RadUtils::loadSBandData(SBandData& sd, const JParser& prm, const JString& name, const JString& msg, const BandSpec& master_bs) {

    // the parameter name for specifying directly
  JString direct_name(name+"_data"); 
    // the parameter name for specifying from a text file
  JString filepath_name(name+"_fp");
  
    // if is directly specified then read it as a comma seperated list of values
  if (prm.isDefined(direct_name)) {

    if (jlog::ls.vbLevel(3)) jlog::ls << "Loading directly specified spectral data '" << direct_name << "'.\n";
  
      // read the values
    std::vector<double> v_std = prm.getVectorOfDoubles(direct_name);
      // copy into the spectral data
    vector<double> v;
    copy(v, v_std);
    sd = SBandData(v);

    // if a filepath is specified then load it
  } else if (prm.isDefined(filepath_name)) {
    
      // file descriptor of a text file
    FileDesc fd(prm.getString(filepath_name), true);
      // read the file
    if (!FileUtils::readFactoryBuiltFromFile(sd, fd)) {
      jlog::es << "Unable to read file for " << msg << " '" << fd.path() << "'\n";
      return false;
    }

    if (jlog::ls.vbLevel(3)) jlog::ls << "Read spectral data file '" << fd.path() << "'.\n";

    // otherwise no specification attempted so return zero length spectral data as caller may set a default value
  } else {

    sd = SBandData(0,0);
    return true;
  }

    // resample if required - returns false if couldn't get into the right number of bands
  if (!handleResample(sd, prm, name, msg, master_bs)) return false;

  return true;
}



// RadUtils::loadSpectralDataFile()

bool RadUtils::loadRadDirec(RadDirecImp<SBandData>*& rd, const JParser& prm, const JString& param_code, const JString& desc, const DirecStruct& targ_ds, const BandSpec& targ_bs) {

    // the parameter name for specifying from a file
  JString filepath_name(param_code+"_fp");
    // if no filepath specified return zero pointer as caller may set a default value
  if (!prm.isDefined(filepath_name)) { rd = 0; return true; }
    // file descriptor of a data file
  FileDesc fd(prm.getString(filepath_name), false); 
    // try to load it
  rd = FileUtils::createFactoryBuiltFromFile< RadDirecImp<SBandData> >(fd);
    // any errors
  if (!rd) {
    jlog::es << "Error trying to read file for " << desc << " '" << fd.path() << "'\n";
    return false;
  }
    // went ok
  if (jlog::ls.vbLevel(3)) jlog::ls << "Read spectral directional data file '" << fd.path() << "'.\n";

    // check directional structure 
  bool ret_val = checkDirecStruct(desc, rd->direcStruct(), targ_ds);
    // resample if required - returns false if couldn't get into the right number of bands
  if (ret_val) ret_val = handleResample(*rd, prm, param_code, desc, targ_bs);

    // clean up if couldn't use the file
  if (!ret_val) {
    delete rd;
    rd = 0;
  }

  return ret_val;
}



bool RadUtils::savePlanarIrradiance(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const JParser& prm) {

    // get filepath
  JString fp = prm.getStringOrEmpty(prefix+"_save_fp");
    // quit if don't want to save
  if (fp.isEmpty()) return true;
    // add on the postfix if any
  fp.append(postfix);
    // save as text file
  if (!FileUtils::writeFactoryBuiltToFile(Ldata.planarIrradiance(), FileDesc(fp,true))) return false;
    // info
  if (jlog::ls.vbLevel(1)) jlog::ls << "Saved planar irradiance text file '" << fp << "'\n";

  return true;
}



bool RadUtils::saveScalarIrradiance(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const JParser& prm) {

    // get filepath
  JString fp = prm.getStringOrEmpty(prefix+"_save_fp");
    // quit if don't want to save
  if (fp.isEmpty()) return true;
    // add on the postfix if any
  fp.append(postfix);
    // save as text file
  if (!FileUtils::writeFactoryBuiltToFile(Ldata.scalarIrradiance(), FileDesc(fp,true))) return false;
    // info
  if (jlog::ls.vbLevel(1)) jlog::ls << "Saved scalar irradiance text file '" << fp << "'\n";

  return true;
}



bool RadUtils::saveRadiance(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const JParser& prm, const JString& notes_text) {

    // get filepath
  JString fp = prm.getStringOrEmpty(prefix+"_save_fp");
    // quit if don't want to save
  if (fp.isEmpty()) return true;
    // add on the postfix if any
  fp.append(postfix);
    // save as text file
  if (!FileUtils::writeFactoryBuiltToFile(Ldata, FileDesc(fp, false), notes_text)) return false;
    // info
  if (jlog::ls.vbLevel(1)) jlog::ls << "Saved radiance data file '" << fp << "'\n";

  return true;
}



// RadUtils::saveHemisphereImage() [static]

bool RadUtils::saveHemisphereImage(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const BandSpec& bs, const JParser& prm) {
  
    // check data type
  if (!Ldata.dataTypeIsMeanRad()) { jlog::es << __FUNCTION__ << " Ldata is not mean radiance - should check before calling.\n"; return false; } 

    // get filepath of the image to save
  JString fp(prm.getStringOrEmpty(prefix+"_image_save_fp"));
    // quit if don't want to save
  if (fp.isEmpty()) return true;
    // add on the postfix if any
  if (fp.right(4).lower() == ".ppm") fp = fp.left(fp.length()-4) + postfix + ".ppm";
  else fp.append(postfix+".ppm");

    // get the size of the image to draw or default to 100 x 100 pixels
  int sz = 128;
  if (prm.isDefined(prefix+"_image_size")) sz = prm.getInt(prefix+"_image_size");

    // inititialise a biffer to draw to
  JBuffer buff(sz, sz);
    // construct a hemisphere image drawer
  HemiSphereDraw hsd;
  hsd.setBandSpec(&bs);
  hsd.setSourceData(&Ldata);

    // set the sensitivity either from a parameter
  double sens_k;
  if (prm.isDefined(prefix+"_image_sens_k")) { 
    sens_k = prm.getDouble(prefix+"_image_sens_k"); 
    hsd.setSensK2(sens_k);
    // or automatically from the data
  } else sens_k = hsd.autoSetSensK();

  //if (jlog::vbLevel(3)) jlog::ls << "Image sens_k: " << sens_k << "\n";

    // draw the hemisphere image to the buffer
  hsd.drawToBuffer(buff);
    // save the hemisphere image as a ppm file
  if (!JImageSave::savePPM(buff, fp)) return false;
    // info
  if (jlog::vbLevel(1)) jlog::ls << "Written hemisphere projection image file '" << fp << "'\n";

  return true;
}



// RadUtils::checkDirecStruct()

bool RadUtils::checkDirecStruct(const JString& fp, const DirecStruct& fp_ds, const DirecStruct& ref_ds) {

  if (fp_ds.isEqual(ref_ds)) return true;
 
  if (jlog::ls.vbLevel(6)) {
    jlog::ls << "Reference DS ----------\n"; 
    ref_ds.display(jlog::ls, 0, ", ");
    jlog::ls << "File DS ---------------\n"; 
    fp_ds.display(jlog::ls, 0, ", ");
  }

  jlog::es << "File '" << fp << "' has incompatible directional structure.\n";

  return false; 
}



// RadUtils::fillFromHLTable()

void RadUtils::fillFromHLTable(RadDirecImp<SBandData>& rd, int band, const JTextTable& ttab) {

    // directional structure of object
  const DirecStruct& ds(rd.direcStruct());
    // quit if not spherical partition
  const SpherePartition* sds = dynamic_cast<const SpherePartition*>(&ds);
  if (!sds) return;

  if (sds->isFull()) return;

  if (sds->horizSize() != ttab.rowCount() || sds->vertSize()+1 != ttab.columnCount()) {
    jlog::es << __FUNCTION__ << " table and directional structure are not compatible sizes.\n";
    return;
  }

  for (int h=0; h<sds->horizSize(); h++) {
    for (int v=0; v<sds->vertSize(); v++) {

        // there is soething odd regarding the phi locations in the tabulated values in the HydroLight D files
        // its not clear how I and J relate to the locations
        // in Pjol00.txt it is stated that the solar quad is in (I,J) (7,1) line 170
        // but in Djol00.txt in the direct downward radiances in air the solar quad appears to be at (7,13) line 1247
        // I think this is because phi flips by 180 degrees when looking up which is implied by the manual fig.1 but not by the text
        // the backscatter peak appears in the same row in line 139
      //int table_row = h + (ttab.rowCount() / 2);
      //if (table_row >= ttab.rowCount()) table_row -= ttab.rowCount();

      int i = sds->positionToIndex(v+1, h); 	
      //jlog::ls << "v " << v << " h " << h << " i " << i << "\n"; 

      int table_row = h + (sds->horizSize() / 2);
      if (table_row >= sds->horizSize()) table_row -= sds->horizSize();

      rd.atIndex(i)[band] = ttab.getText(table_row, sds->vertSize()-v-1).toDouble();
    }
      // top end cap is always from the top left entry
    rd.atIndex(0)[band] = ttab.getText(0, sds->vertSize()).toDouble();
  }
}



void RadUtils::displayHLBlockStyle(const RadDirecImp<SBandData>& rd, int band, JTextStream& str) {

  const DirecStruct& ds(rd.direcStruct());

  if (!ds.isSpherePartition()) return;

  const SpherePartition* sds = dynamic_cast<const SpherePartition*>(&ds);
  if (!sds) return;

  for (int h=0; h<sds->horizSize(); h++) {
    for (int v=sds->vertSize(); v>=0; v--) {

      if (v==0 && h>0) {
	str << "  " << 0.0;
      } else {
	int i = sds->positionToIndex(v, h); 
	str << "  " << rd.atIndex(i)[band];
      }
    }

    str << "\n";
  }
}



// RadUtils::meanCosine()

const SBandData RadUtils::meanCosine(const RadDirecImp<SBandData>& rd) {

  SBandData zp;
  SBandData zm;
  
  rd.sumBySolidAngleAbsCosTheta(zp, zm);

  if (zp.bandCount()==0) return SBandData(rd.bandCount(), 0);

  if (zm.bandCount() == rd.bandCount()) zp.subtract(zm);

  zp.divide(rd.sumBySolidAngle());
  
  return zp;
}



// RadUtils:scale()

void RadUtils::scale(RadDirecImp<SBandData>& rd, double scale) {

  const DirecStruct& ds(rd.direcStruct());

    // for each quad
  for (int index=0; index<ds.units(); index++) {
    rd.setData(index, rd.atIndex(index) * scale);
  }
}



// RadUtils::makeAzimultallyAveraged()

void RadUtils::makeAzimuthallyAveraged(RadDirecImp<SBandData>& rd) {

  const DirecStruct& ds(rd.direcStruct());

  const RadDirecImp<SBandData>* rd_temp = rd.duplicate();

  rd.makeZero();

    // for each azimuthally equivalent position
  for (int rotated_pos=0; rotated_pos<ds.azimuthallyRotatedPositionCount(); rotated_pos++) {
      // accumulate over rotational positions
    rd.addToData(rotated_pos, *rd_temp);
  }

  rd.divide(ds.azimuthallyRotatedPositionCount());

  delete rd_temp;
}



// RadUtils::makeAzimultallyAveraged()

void RadUtils::makeAzimuthallyAveraged(RadDirecDirecImp<SBandData>& rdd) {

  const DirecStruct& ds(rdd.direcStruct());

  const RadDirecDirecImp<SBandData>* rdd_temp = rdd.duplicate();

  RadDirecImp<SBandData>* dt = rdd.createDirec();

  rdd.makeZero();

    // for each incoming quad
  for (int in_index=0; in_index<ds.units(); in_index++) {
      // for each azimuthally equivalent outgoing direction
    for (int rotated_pos=0; rotated_pos<ds.azimuthallyRotatedPositionCount(); rotated_pos++) {
        // collect the out data from this rotational position
      rdd_temp->extractOutData(in_index, rotated_pos, *dt);
        // accumulate it in the zero rotation position (i.e. standard position).
      rdd.addToOutData(in_index, 0, *dt);
    }
  }

  rdd.divide(ds.azimuthallyRotatedPositionCount());

  delete rdd_temp;
}



// RadUtils::makeSymmetricallyAveragedAroundPhi()
// makes the direction to direction function left-right symmetrical about a given phi
// value (actually the quad corresponding to it)
// phi_sym = phi about which to make symmetrical
// phi_in = incident phi angle (vector is in the direction of photon travel - inward)
// phi_out = exitant phi angle (vector is in the direction of photon travel - outward)
// theta_in, theta_out = similarly theta directions
// so for each table entry [phi_in, theta_in, phi_out, theta_out] can be identfied another entry
// [phi_in', theta_in', phi_out', theta_out'] which should have the same value - make bothe the average of the two
// the relationships are:
// theta_in' = theta_in
// theta_out' = theta_out
// phi_in' = 2 * phi_sym - phi_in
// phi_out' = 2 * phi_sym - phi_in


void RadUtils::makeSymmetricallyAveragedAroundPhi(RadDirecDirecImp<SBandData>& rdd, double phi_sym) {

  const DirecStruct& ds(rdd.direcStruct());

  const RadDirecDirecImp<SBandData>* rdd_temp = rdd.duplicate();

  rdd.makeZero();

  double phi_sym_x2 = 2 * phi_sym;

    // for each incoming quad
  for (int in_index=0; in_index<ds.units(); in_index++) {

      // these are phi and theta for an outward directed vector
    Vec3D v(ds.quadAtIndex(in_index)->pointDirec());
    double phi_in = v.phiFromUnit();
    double theta_in = v.thetaFromUnit();
      // the associated phi of the symmetrical pair
    double phi_in2 = phi_sym_x2 - phi_in;
      // the pair index but this is for incident direction so negate vector 
    int in_index2 = ds.direcToIndex(Vec3D(theta_in, phi_in2));

      // for each outgoing quad
    for (int out_index=0; out_index<ds.units(); out_index++) {

        // these are phi and theta for an outward directed vector
      Vec3D v(ds.quadAtIndex(out_index)->pointDirec());
      double phi_out = v.phiFromUnit();
      double theta_out = v.thetaFromUnit();
        // the associated phi of the symmetrical pair
      double phi_out2 = phi_sym_x2 - phi_out;
        // the pair index but this is already outgoing 
      int out_index2 = ds.direcToIndex(Vec3D(theta_out, phi_out2));

        // add the two
      rdd.addToData(in_index, out_index, rdd_temp->atIndices(in_index, out_index));
      rdd.addToData(in_index, out_index, rdd_temp->atIndices(in_index2, out_index2));
    }
  }

  rdd.divide(2);

  delete rdd_temp;
}



// RadUtils:reciprocalInverse()
// this function doesn't really do anything and is probably completely redundant

void RadUtils::reciprocalInverse(RadDirecDirecImp<SBandData>& rdd) {

  const DirecStruct& ds(rdd.direcStruct());

  RadDirecDirecImp<SBandData>* rdd_temp = rdd.duplicate();

    // changes the input type from planar irrad to radiance
  rdd_temp->multiplyAllByInSACT();

    // clear the values and we will be working with radiance to radiance
  rdd.makeZero();
  rdd.setInDataTypeMeanRad();
  rdd.setExDataTypeMeanRad();

    // for each incoming quad
  for (int in_index=0; in_index<ds.units(); in_index++) {

      // the in-index of the reversed direction
    int in_index2 = ds.direcToIndex(-ds.quadAtIndex(in_index)->pointDirec());

      // for each outgoing quad
    for (int ex_index=0; ex_index<ds.units(); ex_index++) {

        // the ex-index of the reversed direction
      int ex_index2 = ds.direcToIndex(-ds.quadAtIndex(ex_index)->pointDirec());

      //jlog::ls << in_index << " " << ex_index << " " << in_index2 << " " << ex_index2 << "\n";

      rdd.setData(in_index, ex_index, rdd_temp->atIndices(in_index2, ex_index2));
    }
  }

    // changes the input type from radiance to planar irrad
  rdd.divideAllByInSACT();

  delete rdd_temp;
}



void RadUtils::displayInfo(const RadDirecImp<SBandData>& rd, int ds_table_code, JTextStream& str) {

  const DirecStruct& ds(rd.direcStruct());

  jlog::ls << "File is tabulated directional " << rd.dataString() << "\n";
  jlog::ls << "Defined over the " << (ds.isFull() ? "full sphere.\n" : "hemi-sphere.\n");
  jlog::ls << "Bands: " << rd.bandCount() << "\n";
  jlog::ls << "Directional structure:\n";
  ds.display(str, ds_table_code, ", ");

  if (rd.dataTypeIsUnspecified()) return;

  rd.lazyInvalidate();

  SBandData sd(rd.planarIrradiance());
  //str << "Planar irradiance (" << spectralIrradianceUnits() << ") " << sd << "\n";
  str << "Planar irradiance\n" << sd << "\n";
}



void RadUtils::displayInfo(const RadDirecDirecImp<SBandData>& rdd, int ds_table_code, JTextStream& str) {

  const DirecStruct& ds(rdd.direcStruct());

  str << "File is tabulated direction-to-direction function of " << rdd.inDataString() << " to " << rdd.exDataString() << ".\n";
  str << "Defined over the " << (ds.isFull() ? "full sphere.\n" : "hemi-sphere.\n");
  str << "Bands: " << rdd.bandCount() << "\n";
  str << "Directional structure:\n";
  ds.display(str, ds_table_code, ", ");

  if (rdd.inDataTypeIsUnspecified() || rdd.exDataTypeIsUnspecified()) return;

  rdd.lazyInvalidate();

    // if incident data type is scalar irradiance then treat as a scattering function
  if (rdd.inDataTypeIsScalarIrrad()) {
    str << "Total diffuse scatter: " << rdd.diffuseScatter() << "\n";
    if (ds.isFull()) RadUtils::displayHalfFunctionsAsScatter(rdd, str);
  }
    // if incident data type is planar irradiance then treat as a reflectance function
  if (rdd.inDataTypeIsPlanarIrrad()) {
    if (ds.isFull()) RadUtils::displayHalfFunctionsAsReflectance(rdd, str);
    else str << "Diffuse reflectance: " << rdd.diffuseReflectance() << "\n";
  }
}



// RadUtils::displayHalfFunctionsAsReflectance()

void RadUtils::displayHalfFunctionsAsReflectance(const RadDirecDirecImp<SBandData>& rdd, JTextStream& str) {

    // create four tables with half-directional structure one for each hemisphere-hemisphere function
  RadDirecDirecImp<SBandData>* trans_zp = rdd.duplicateHalfStructure();
  RadDirecDirecImp<SBandData>* trans_zm = rdd.duplicateHalfStructure();
  RadDirecDirecImp<SBandData>* reflec_zp_zm = rdd.duplicateHalfStructure();
  RadDirecDirecImp<SBandData>* reflec_zm_zp = rdd.duplicateHalfStructure();

  trans_zp->copyDataTypes(rdd);
  trans_zm->copyDataTypes(rdd);
  reflec_zp_zm->copyDataTypes(rdd);
  reflec_zm_zp->copyDataTypes(rdd);

    // split the full sphere function into four
    // if we only report diffuse relflectances doesnt matter which solit function we use
  rdd.splitRotateAboutX(reflec_zm_zp, trans_zm, trans_zp, reflec_zp_zm);
 
  SBandData t_zp(trans_zp->diffuseReflectance());
  SBandData t_zm(trans_zm->diffuseReflectance());
  SBandData r_zp_zm(reflec_zp_zm->diffuseReflectance());
  SBandData r_zm_zp(reflec_zm_zp->diffuseReflectance());

  str << "Side 0 (incident vector in z- direction):\n";
  str << "  Reflectance (z- to z+): " << r_zm_zp << "\n";
  str << "  Transmission (z-): " << t_zm << "\n";
  str << "  Sum: " << (r_zm_zp + t_zm) << "\n";

  str << "Side 1 (incident vector in z+ direction):\n";
  str << "  Reflectance (z+ to z-): " << r_zp_zm << "\n";
  str << "  Transmission (z+): " << t_zp << "\n";
  str << "  Sum: " << (r_zp_zm + t_zp) << "\n";
 
  delete trans_zp;
  delete trans_zm;
  delete reflec_zm_zp;
  delete reflec_zp_zm;
}



// RadUtils::displayHalfFunctionsAsScatter()

void RadUtils::displayHalfFunctionsAsScatter(const RadDirecDirecImp<SBandData>& rdd, JTextStream& str) {

   // create four tables with half-directional structure one for each hemisphere-hemisphere function
  RadDirecDirecImp<SBandData>* trans_zp = rdd.duplicateHalfStructure();
  RadDirecDirecImp<SBandData>* trans_zm = rdd.duplicateHalfStructure();
  RadDirecDirecImp<SBandData>* reflec_zp_zm = rdd.duplicateHalfStructure();
  RadDirecDirecImp<SBandData>* reflec_zm_zp = rdd.duplicateHalfStructure();

  trans_zp->copyDataTypes(rdd);
  trans_zm->copyDataTypes(rdd);
  reflec_zp_zm->copyDataTypes(rdd);
  reflec_zm_zp->copyDataTypes(rdd);

    // split the full sphere function into four
    // if we only report diffuse relflectances doesnt matter which solit function we use
  rdd.splitRotateAboutX(reflec_zm_zp, trans_zm, trans_zp, reflec_zp_zm);
 
  SBandData fwd_zp(trans_zp->diffuseScatter());
  SBandData fwd_zm(trans_zm->diffuseScatter());
  SBandData back_zp_zm(reflec_zp_zm->diffuseScatter());
  SBandData back_zm_zp(reflec_zm_zp->diffuseScatter());

  str << "Forward diffuse scatter (incident vector z+):\n" << fwd_zp << "\n";
  str << "Forward diffuse scatter (incident vector z-):\n" << fwd_zm << "\n";
  str << "Backward diffuse scatter (from z+ to z-):\n" << back_zp_zm << "\n";
  str << "Backward diffuse scatter (from z- to z+):\n" << back_zm_zp << "\n";
  
    // the total scattering for the two incident hemispheres
  SBandData total_zp(fwd_zp + back_zp_zm);
  SBandData total_zm(fwd_zm + back_zm_zp);

  str << "Total diffuse scatter (incident vector z+):\n" << total_zp << "\n";
  str << "Total diffuse scatter (incident vector z-):\n" << total_zm << "\n";

    // the backscatter fraction for the two incident hemispheres
  SBandData backscatter_frac_zp(back_zp_zm / total_zp);
  SBandData backscatter_frac_zm(back_zm_zp / total_zm);

  str << "Diffuse backscatter fraction (not bb/B! but is 0.5 for isotropic) (incident vector z+): " << backscatter_frac_zp << "\n";
  str << "Diffuse backscatter fraction (not bb/B! but is 0.5 for isotropic) (incident vector z-): " << backscatter_frac_zm << "\n";
 
  delete trans_zp;
  delete trans_zm;
  delete reflec_zm_zp;
  delete reflec_zp_zm;

  //RadDirecImp<SBandData>* zp_rd =   rdd.inZplus() rdd.createRadDirecTable();

}

