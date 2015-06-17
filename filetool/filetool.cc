
// filetool.cc

#include "filetool.h"
#include "rad/raddirecimp.h"
#include "base/jparser.h"
#include "base/jgenericfactory.h"
#include "file/filedesc.h"
#include "rad/sbanddata.h"
#include "rad/radunits.h"
#include "rad/radutils.h"
#include "rad/rgbimage.h"
#include "base/jimagesave.h"
#include "math/valuerange.h"
#include "config.h"



using namespace jude::base;
using namespace jude::rad;
using namespace jude::tool;
using namespace jude::file;
//using namespace jude::math;




// FileTool::(constructor)

FileTool::FileTool() : 
  Tool() { init(); }



// FileTool::(destructor)

FileTool::~FileTool() { }



// FileTool::init()

void FileTool::init() {

  static bool done_init = false;
  if (done_init) return;

    // somewhere in all programs there must occur a list of all the static polymorphic types that can occur
  JFactory< RadDirecImp< SBandData >, RadDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecImp< SBandData >, RadDirecTable< SBandData, SpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData >, RadDirecDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  JFactory< RadDirecDirecImp< SBandData >, RadDirecDirecTable< SBandData, SpherePartVxH > >::instance().reg();
  JFactory< SpectralImage< SBandData >, SpectralImage< SBandData > >::instance().reg();


  done_init = true;
}



template <typename T>
void FileTool::displayInfo(const T& obj, const FileInfo& fi, const JParser& prm) {

  uns32 v = fi.savedVersion();
  jlog::ls << "Saved by Jude version: " << MAJOR_VERSION(v) << "." << MINOR_VERSION(v) << "." << SUBMINOR_VERSION(v) << "\n";

  jlog::ls << "Notes\n-----\n";
  jlog::ls << fi.notesText();
  jlog::ls << "-----\n";
 
    // display tabulated values 
  RadUtils::displayInfo(obj, ds_table_code, jlog::ls);
 
    // display contents
  if (prm.isDefinedAndTrueBool("contents_display")) obj.display(jlog::ls);
}



// FileTool::run()

bool FileTool::run(const JParser& prm) {

    // basic initialisation common to all tools
  if (Tool::init(prm)) return true;
 
    // create a file
  if (prm.isDefined("rd_save_fp")) return createRadDirec(prm);

  if (prm.filePathCount() != 1) { jlog::es << "File must be specified.\n"; return false; }

  ds_table_code = 0;
  if (prm.isDefinedAndTrueBool("ds_sa_table_display")) ds_table_code |= DsQuad::SA;
  if (prm.isDefinedAndTrueBool("ds_sact_table_display")) ds_table_code |= DsQuad::SACT;
  if (prm.isDefinedAndTrueBool("ds_ct_table_display")) ds_table_code |= DsQuad::CT;
  if (prm.isDefinedAndTrueBool("ds_index_table_display")) ds_table_code |= DsQuad::INDEX;
  if (prm.isDefinedAndTrueBool("ds_mean_phi_table_display")) ds_table_code |= DsQuad::MEANPHI;
  if (prm.isDefinedAndTrueBool("ds_mean_theta_table_display")) ds_table_code |= DsQuad::MEANTHETA;
  if (prm.isDefinedAndTrueBool("ds_point_phi_table_display")) ds_table_code |= DsQuad::POINTPHI;
  if (prm.isDefinedAndTrueBool("ds_point_theta_table_display")) ds_table_code |= DsQuad::POINTTHETA;
  if (prm.isDefinedAndTrueBool("ds_point_direc_table_display")) ds_table_code |= DsQuad::POINTDIREC;
  if (prm.isDefinedAndTrueBool("ds_mean_direc_table_display")) ds_table_code |= DsQuad::MEANDIREC;

    // get file description and info
  const FileDesc fd(prm.getFilePath(0));

  FileInfo fi;

  if (jlog::vbLevel(2)) jlog::ls << "Loading file '" << fd.path() << "'.\n";

  if (jlog::vbLevel(5)) jlog::ls << "Trying RadDirecImp<SBandData> factory\n";
  RadDirecImp<SBandData>* rd = FileUtils::createFactoryBuiltFromFile< RadDirecImp<SBandData> >(fd, &fi);
  if (rd) { doRadDirec(rd, fi, prm); delete rd; return true; }

  if (jlog::vbLevel(5)) jlog::ls << "Trying RadDirecDirecImp<SBandData> factory\n";
  RadDirecDirecImp<SBandData>* rdd = FileUtils::createFactoryBuiltFromFile< RadDirecDirecImp<SBandData> >(fd, &fi);
  if (rdd) { doRadDirecDirec(rdd, fi, prm); delete rdd; return true; }

  if (jlog::vbLevel(5)) jlog::ls << "Trying SpectralImage<SBandData> factory\n";
  SpectralImage<SBandData>* sif = FileUtils::createFactoryBuiltFromFile< SpectralImage<SBandData> >(fd, &fi);
  if (sif) { doSpectralImage(sif, fi, prm); delete sif; return true; }

  jlog::ls << "File '" << fd.path() << "' is non existent or not a supported file type.\n";

  return true;
}



// FileTool::doRadDirec()

void FileTool::doRadDirec(RadDirecImp<SBandData>* rd, const FileInfo& fi, const JParser& prm) {

  if (jlog::ls.vbLevel(2)) displayInfo(*rd, fi, prm);

  /*
  uns32 v = fi.savedVersion();
  if (jlog::ls.vbLevel(2)) jlog::ls << "Saved by Jude version: " << MAJOR_VERSION(v) << "." << MINOR_VERSION(v) << "." << SUBMINOR_VERSION(v) << "\n";

  if (jlog::ls.vbLevel(2)) {
    jlog::ls << "Notes\n-----\n";
    jlog::ls << fi.notesText();
    jlog::ls << "-----\n";
    RadUtils::displayInfo(*rd, ds_table_code, jlog::ls);
  }

    // display contents
  if (jlog::ls.vbLevel(2) && prm.isDefinedAndTrueBool("show_contents")) rd->display(jlog::ls);
  */



  if (jlog::ls.vbLevel(2)) {

    if (prm.isDefined("show_band_hl_style")) {
      int band = prm.getInt("show_band_hl_style") - 1;
      if (band<0 || band>=rd->bandCount()) {
	jlog::es << "Band specified in show_band_hl_style is out of range 1 - " << rd->bandCount() << "\n";
      } else {
	RadUtils::displayHLBlockStyle(*rd, band, jlog::ls);
      }
    }
  }


    // if doing angular data output
  if (prm.isDefined("ang_data_save_fp")) writeAngularDataFile(prm, *rd);

  if (prm.isDefined("fisheye_image_save_fp")) {

      // try to make sure data is mean radiance
    if (rd->dataTypeIsPlanarIrrad()) rd->divideAllBySACT();
      //if (rd->dataTypeIsScalarIrrad()) rd->divideAllBySA();
    if (!rd->dataTypeIsMeanRad()) {
      jlog::es << "Fisheye image save requested but data cannot be converted to radiance.\n";
      return;
    } 
      // get band specification or quit with error
    BandSpec* bs = BandSpec::create(prm, "");
    if (!bs) {
      jlog::es << "Image save requested but no band specification given or it is incomplete.\n";
      return;
    }
      // save image
    if (!RadUtils::saveHemisphereImage("fisheye", "", *rd, *bs, prm)) jlog::es << "Error saving image.\n";
  }

  if (prm.isDefined("scale")) {
    double scale = prm.getDouble("scale");
    RadUtils::scale(*rd, scale);
    //RadUtils::makeSymmetricallyAveragedAroundPhi(*rdd, sym_phi_rad);
    //if (jlog::vbLevel(2)) jlog::ls << "Converted to symetrically averaged function around phi = " << sym_phi_rad << " (rad).\n";
  }

    // if full file to save to is specified 
  if (prm.isDefined("save_fp")) {
      // get file path
    JString out_filepath = prm.getString("save_fp");
      // build notes using function in Tool
    JTextDevice nd;
    buildNotes(nd, prm);
       // generic writer that includes information for reconstruction of dynamic types on reload
    FileUtils::writeFactoryBuiltToFile(*rd, FileDesc(out_filepath, false)); 
      // info
    if (jlog::vbLevel(1)) {
      jlog::ls << "Written file '" << out_filepath << "' as ";
      jlog::ls << "tabulated directional function of " << rd->dataString() << ".\n";
    }
  }


  /*

  if (prm.isDefined("add_fp")) {
      // get file description and info
    const FileDesc fd(prm.getString("add_fp"));

    FileInfo fi;

    if (jlog::vbLevel(2)) jlog::ls << "Adding file '" << fd.path() << "'.\n";

    RadDirecImp<SBandData>* rd_add = FileUtils::createFactoryBuiltFromFile< RadDirecImp<SBandData> >(fd, &fi);
    if (!rd_add) { jlog::es << "File '" << fd.path() << "' could not be loaded or is wrong type.\n"; return; }

    rd->add(*rd_add);
  }

    // if full file to save to is specified 
  if (prm.isDefined("save_fp")) {
      // get file path
    JString out_filepath = prm.getString("save_fp");
      // build notes using function in Tool
    JTextDevice nd;
    buildNotes(nd, prm);
       // generic writer that includes information for reconstruction of dynamic types on reload
    FileUtils::writeFactoryBuiltToFile(*rd, FileDesc(out_filepath, false)); 
      // info
    if (jlog::vbLevel(1)) {
      jlog::ls << "Written file '" << out_filepath << "' as ";
      jlog::ls << "tabulated direction-to-direction function of " << rd->inDataString() << ".\n";
    }
  }
  */
}



// FileTool::doRadDirecDirec()

void FileTool::doRadDirecDirec(RadDirecDirecImp<SBandData>* rdd, const FileInfo& fi, const JParser& prm) {

  if (jlog::ls.vbLevel(2)) displayInfo(*rdd, fi, prm);

  if (prm.isDefined("contents_display_in_index")) {
    rdd->displayInIndex(prm.getInt("contents_display_in_index"), jlog::ls);
  }


  /*
  uns32 v = fi.savedVersion();
  jlog::ls << "Saved by Jude version: " << MAJOR_VERSION(v) << "." << MINOR_VERSION(v) << "." << SUBMINOR_VERSION(v) << "\n";

  if (jlog::ls.vbLevel(2)) {

    jlog::ls << "Notes\n-----\n";
    jlog::ls << fi.notesText();
    jlog::ls << "-----\n";
  
    RadUtils::displayInfo(*rdd, ds_table_code, jlog::ls);
  }

    // display contents
  if (jlog::ls.vbLevel(2) && prm.isDefinedAndTrueBool("show_contents")) rdd->display(jlog::ls);
  */

  if (prm.isDefined("azimuthally_average")) {
    RadUtils::makeAzimuthallyAveraged(*rdd);
    if (jlog::vbLevel(2)) jlog::ls << "Converted to azimuthally averaged function.\n";
  }

  if (prm.isDefined("symmetrically_average_around_phi")) {
    double sym_phi_rad = prm.getDouble("symmetrically_average_around_phi");
    RadUtils::makeSymmetricallyAveragedAroundPhi(*rdd, sym_phi_rad);
    if (jlog::vbLevel(2)) jlog::ls << "Converted to symetrically averaged function around phi = " << sym_phi_rad << " (rad).\n";
  }

  if (prm.isDefined("reciprocal_invert")) {
    if (!rdd->inDataTypeIsPlanarIrrad() || !rdd->exDataTypeIsMeanRad()) {
      jlog::es << "Function 'reciprocal_invert' is only supported on directional functions of incoming planar irradiance to outgoing quad-averaged radiance.\n";
      return; 
    }  
    RadUtils::reciprocalInverse(*rdd);
    if (jlog::vbLevel(2)) jlog::ls << "WARNING: reciprocal inverse transform is probably never needed (?)\n";
    if (jlog::vbLevel(2)) jlog::ls << "Converted to reciprocal inverse.\n";
  }

  if (prm.isDefined("in_index") && prm.isDefined("ex_index")) {
    int in_index = prm.getInt("in_index");
    int ex_index = prm.getInt("ex_index");
    jlog::ls << "v: " << rdd->atIndices(in_index, ex_index) << " sact_in: " << rdd->direcStruct().solidAngleAbsCosTheta(in_index) << " sact_ex: " << rdd->direcStruct().solidAngleAbsCosTheta(ex_index) << "\n";
  }

    // if doing angular data output
  if (prm.isDefined("ang_data_save_fp")) writeAngularDataFile(prm, *rdd);

    // if full file to save to is specified 
  if (prm.isDefined("save_fp")) {
      // get file path
    JString out_filepath = prm.getString("save_fp");
      // build notes using function in Tool
    JTextDevice nd;
    buildNotes(nd, prm);
       // generic writer that includes information for reconstruction of dynamic types on reload
    FileUtils::writeFactoryBuiltToFile(*rdd, FileDesc(out_filepath, false)); 
      // info
    if (jlog::vbLevel(1)) {
      jlog::ls << "Written file '" << out_filepath << "' as ";
      jlog::ls << "tabulated direction-to-direction function of " << rdd->inDataString() << " to " << rdd->exDataString() << ".\n";
    }
  }

  /*
  if (!prm.isDefined("show_data_in_bands")) return;

  std::vector<int> display_bands = prm.getVectorOfInts("show_data_in_bands");

  for (int i=0; i<display_bands.size(); i++) {

    for (int in_index=0; in_index<base.totalUnits(); in_index++) {

      jlog::ls << "Incident index: " << in_index << "\n";

      RadDirecDirecQDS qds(rdd, in_index, display_bands(i));
      rdd->direcStruct().displayTableValues(str, qds);
    }
  }
  */
}



// FileTool::doSpectralImage()

void FileTool::doSpectralImage(SpectralImage<SBandData>* sif, const FileInfo& fi, const JParser& prm) {

  jlog::ls << "Spectral image file " << sif->width() << " x " << sif->height() << " pixels, " << sif->bandCount() << " bands.\n";

  if (prm.isDefined("rgb_save_fp")) convertSifToRGB(sif, fi, prm);

  if (prm.isDefined("scale_save_fp")) convertSifToScale(sif, fi, prm);
}



// FileTool::convertSifToRGB()

void FileTool::convertSifToRGB(SpectralImage<SBandData>* sif, const FileInfo& fi, const JParser& prm) {

  const BandSpec* bs = sif->bandSpec();

    // image size
  int xs = sif->width();
  int ys = sif->height();

  RgbImage rgb_buff(xs, ys);
  
    // sensitivity in each RGB band
  double red_sens_k = prm.isDefined("rgb_red_sens_k") ? prm.getDouble("rgb_red_sens_k") : 0;
  double green_sens_k = prm.isDefined("rgb_green_sens_k") ? prm.getDouble("rgb_green_sens_k") : 0;
  double blue_sens_k = prm.isDefined("rgb_blue_sens_k") ? prm.getDouble("rgb_blue_sens_k") : 0;
    // or overall
  if (prm.isDefined("rgb_sens_k")) {
    double s = prm.getDouble("rgb_sens_k");
    red_sens_k = s;
    green_sens_k = s;
    blue_sens_k = s;
  }

    // set the sens_k
  if (red_sens_k!=0 || green_sens_k!=0 || blue_sens_k!=0) rgb_buff.setSensK(red_sens_k, green_sens_k, blue_sens_k);
    // otherwise try and get a multiplier
  else {

      // multiplier for  each RGB band
    double red_mult = prm.isDefined("rgb_red_mult") ? prm.getDouble("rgb_red_mult") : 0;
    double green_mult = prm.isDefined("rgb_green_mult") ? prm.getDouble("rgb_green_mult") : 0;
    double blue_mult = prm.isDefined("rgb_blue_mult") ? prm.getDouble("rgb_blue_mult") : 0;
      // or overall
    if (prm.isDefined("rgb_mult")) {
      double s = prm.getDouble("rgb_mult");
      red_mult = s;
      green_mult = s;
      blue_mult = s;
    }

    rgb_buff.setMultiplier(red_mult, green_mult, blue_mult);
  }

    // if bands are specified
  bool got_rb = prm.isDefined("rgb_red_band");
  bool got_gb = prm.isDefined("rgb_green_band");
  bool got_bb = prm.isDefined("rgb_blue_band");
    // individual band specifications
  int rb = (got_rb) ? prm.getInt("rgb_red_band") : -1;
  int gb = (got_gb) ? prm.getInt("rgb_green_band") : -1;
  int bb = (got_bb) ? prm.getInt("rgb_blue_band") : -1;
    // any individual specification overrides automatic
  if (got_rb || got_gb || got_bb) rgb_buff.setRGBBands(rb, gb, bb);
    // otherwise if got band spec then set automatically
  else if (prm.isDefined("rgb_mono_band")) {
    rb = prm.getInt("rgb_mono_band");
    gb = rb;
    bb = rb;
    rgb_buff.setRGBBands(rb, gb, bb);
  } else if (bs) {
    rgb_buff.setClosestRGB(*bs);
  } else {
    rb = 0;
    gb = (1 % sif->bandCount());
    bb = (2 % sif->bandCount());
    rgb_buff.setRGBBands(rb, gb, bb);
  }

  jlog::ls << "Red band: " << rgb_buff.redBand() << "\n";;
  jlog::ls << "Green band: " << rgb_buff.greenBand() << "\n";;
  jlog::ls << "Blue band: " << rgb_buff.blueBand() << "\n";;

    // clear the buffer
  JColour back_col(0,0,0);
  if (prm.isDefined("back_col")) back_col = prm.getColour("back_col");
  rgb_buff.clear(back_col);

  vector<float> v;
  bool is_masked;

    // render image
  for (int y=0; y<ys; y++) {
    for (int x=0; x<xs; x++) {
      sif->getPixel(x, y, v, is_masked);
      if (!is_masked) {
	rgb_buff.renderPixel(x,y,v);
	//rgb_buff.renderPixel(x,y,JColour(128,128,128));
      }
    }
  }

    // save image
  JString fp(prm.getString("rgb_save_fp"));
  if (jlog::vbLevel(2)) jlog::ls << "Saving: '" << fp << "'\n";
  JImageSave::savePPM(rgb_buff.buffer(), fp);
}



// FileTool::convertSifToScale()

void FileTool::convertSifToScale(SpectralImage<SBandData>* sif, const FileInfo& fi, const JParser& prm) {
  
  JString ttab_fp = prm.getStringOrEmpty("scale_defn_fp");
  if (ttab_fp.isEmpty()) {
    jlog::es << "No scale definition file given with 'scale_defn_fp'\n";
    return;
  }

  JTextTable ttab;
  if (!ttab.readFromFile(ttab_fp)) return;

  if (ttab.columnCount() != 4) {
    jlog::es << "Scale definition file does not have four columns.\n";
    return;
  }

  vector<double> scale_v;
  vector<double> red_v;
  vector<double> green_v;
  vector<double> blue_v;

  std::vector<double> sv;

  ttab.vectorOfDoublesFromColumn(sv, 0);
  copy(scale_v, sv);
  ttab.vectorOfDoublesFromColumn(sv, 1);
  copy(red_v, sv);
  ttab.vectorOfDoublesFromColumn(sv, 2);
  copy(green_v, sv);
  ttab.vectorOfDoublesFromColumn(sv, 3);
  copy(blue_v, sv);

    // image size
  int xs = sif->width();
  int ys = sif->height();
  
  RgbImage rgb_buff(xs, ys);

    // clear the buffer
  JColour back_col(0,0,0);
  if (prm.isDefined("back_col")) back_col = prm.getColour("back_col");
  rgb_buff.clear(back_col);
  
  int b = prm.getInt("scale_band");

  vector<float> v;
  bool is_masked;
  
  for (int y=0; y<ys; y++) {
    for (int x=0; x<xs; x++) {
      sif->getPixel(x, y, v, is_masked);
      if (!is_masked) {
	if (v(3) == 0) {      
	  double val = v(b);
	  int r = (int) (255 * Ipol::linearInterpolate(val, red_v, scale_v)); 
	  int g = (int) (255 * Ipol::linearInterpolate(val, green_v, scale_v)); 
	  int b = (int) (255 * Ipol::linearInterpolate(val, blue_v, scale_v)); 
	  rgb_buff.renderPixel(x, y, JColour(r,g,b));
	} else {
	  rgb_buff.renderPixel(x, y, JColour(128,128,128));
	}
      }
    }
  }
  
    // save image
  JString fp(prm.getString("scale_save_fp"));
  if (jlog::vbLevel(2)) jlog::ls << "Saving: '" << fp << "'\n";
  JImageSave::savePPM(rgb_buff.buffer(), fp);
}



bool FileTool::createRadDirec(const JParser& prm) {

    // get output directional specification or quit with error
  DirecSpec* master_ds_spec = DirecSpec::create(prm, "");
  if (!master_ds_spec) { jlog::es << "No output directional specification given or it is incomplete.\n"; return false; }

    // try to build a hemi directional spectral data table
  bool do_full = false;
  int band_count = 1;
  RadDirecImp<SBandData>* emis = buildRadDirec<SBandData>(*master_ds_spec, band_count, do_full);
    // if failed an error will have been streamed to jlog::es
  if (!emis) return false;
    // quit if not spherical partition
  const DirecStruct& ds = emis->direcStruct();
  const SpherePartition* sds = dynamic_cast<const SpherePartition*>(&ds);
  if (!ds.isSameType(HemiSpherePartVxH()) || !sds) { jlog::es << "Unsupported directional structure."; return false; } 

    // set the data type
  emis->setDataTypeMeanRad();
    // any quads that don't have values written to them will be zero (esp. upward looking quads)
  emis->makeAll(SBandData(1,0));

    // get source data file path
  JString filepath = prm.getStringOrEmpty("hl_table_data_fp");
  if (filepath.isEmpty()) {
    jlog::es << "A source data file must be specified with hl_table_data_fp=<filepath>\n";
    return false;
  }

  int row_count;
  int max_cols;
  bool is_consistent;

  JTextTable::getSize(row_count, max_cols, is_consistent, filepath);

  if (!is_consistent) {
    jlog::es << "Source table data '" << filepath << "' does not have a consistent number of entries on each line.\n";
    return false;
  }

  if (jlog::vbLevel(2)) jlog::ls << "Table size: " << row_count << " rows by " << max_cols << " columns.\n";

  if (row_count != sds->horizSize() || max_cols != sds->vertSize()+1) {
    jlog::es << "Source table data '" << filepath << "' is " << row_count << " rows by " << max_cols << " columns ";
    jlog::es << "but the specified\ndirectional structure implies the size should be " << sds->horizSize() << " rows by " << (sds->vertSize()+1) << " columns.\n"; 
    return false;
  }

  JTextTable ttab;
  ttab.readFromFile(filepath);

  RadUtils::fillFromHLTable(*emis, 0, ttab);

    // get file path
  JString out_filepath = prm.getString("rd_save_fp");
    // generic writer that includes information for reconstruction of dynamic types on reload
  FileUtils::writeFactoryBuiltToFile(*emis, FileDesc(out_filepath, false)); 
    // for info
  if (jlog::vbLevel(1)) {
    jlog::ls << "Written file '" << out_filepath << "' as ";
    jlog::ls << (emis->dataTypeIsMeanRad() ? "quad-averaged radiance" : "quad-averaged planar irradiance") << ".\n";
  }
  
  return true;
}



// FileTool::writeAngularDataFile()

bool FileTool::writeAngularDataFile(const JParser& prm, const RadDirecImp<SBandData>& rd) {

  ValueRange ang_theta_deg;
  ValueRange ang_phi_deg;
    
  if (prm.isDefined("ang_theta")) {
    ang_theta_deg = ValueRange(prm.getDouble("ang_theta"));
  } else if (prm.isDefined("ang_theta_data")) {
    ang_theta_deg = ValueRange(prm.getVectorOfDoubles("ang_theta_data"));
  } else {
    ang_theta_deg = ValueRange(prm.getDouble("ang_theta_min"), prm.getDouble("ang_theta_max"), prm.getInt("ang_theta_steps")); 
    if (prm.somethingWasNotDefined()) {
      jlog::es << "Either 'ang_theta' must be defined to give a single theta angle,";
      jlog::es << "or 'ang_theta_min', 'ang_theta_max' and 'ang_theta_steps' must all be defined";
      jlog::es << "to give a range of theta angles.";
      return false;
    }
  }

  if (prm.isDefined("ang_phi")) {
    ang_phi_deg = ValueRange(prm.getDouble("ang_phi"));
  } else if (prm.isDefined("ang_phi_data")) {
    ang_phi_deg = ValueRange(prm.getVectorOfDoubles("ang_phi_data"));
  } else {
    ang_phi_deg = ValueRange(prm.getDouble("ang_phi_min"), prm.getDouble("ang_phi_max"), prm.getInt("ang_phi_steps")); 
    if (prm.somethingWasNotDefined()) {
      jlog::es << "Either 'ang_phi' must be defined to give a single phi incident angle,";
      jlog::es << "or 'ang_phi_min', 'ang_phi_max' and 'ang_phi_steps' must all be defined";
      jlog::es << "to give a range of phi incident angles.";
      return false;
    }
  }
 
    // open the file for write
  JFile f(prm.getString("ang_data_save_fp"));
  if (!f.open(JIO_WriteOnly)) {
    jlog::es << "Failed to open file '" << f.name() << "' for write\n";
    return false;
  }

  JTextStream str(&f);
  str.setScientific(6);

  str << "#phi\ttheta";
  for (int i=0; i<rd.bandCount(); i++) str << "\tband" << (i+1);
  str << "\n";
	 
  for (int pi=0; pi<=ang_phi_deg.stepCount(); pi++) {
    double phi = ang_phi_deg.stepValue(pi);
    
    for (int ti=0; ti<=ang_theta_deg.stepCount(); ti++) {
      double theta = ang_theta_deg.stepValue(ti);
          
        // convert degrees to radians
      double theta_rad = degToRad(theta);
      double phi_rad = degToRad(phi);

      const SBandData& sd = rd.at(Vec3D(theta_rad, phi_rad));
	  
      str << phi << "\t" << theta;
      for (int i=0; i<sd.bandCount(); i++) str << "\t" << sd.bandValue(i);
      str << "\n";
    }
  }

  f.close();

  return true;
}



//  FileTool::writeAngularDataFile()

bool FileTool::writeAngularDataFile(const JParser& prm, const RadDirecDirecImp<SBandData>& rdd) {

  ValueRange ang_theta_deg_in;
  ValueRange ang_phi_deg_in;
  ValueRange ang_theta_deg_out;
  ValueRange ang_phi_deg_out;
    
  if (prm.isDefined("ang_theta_in")) {
    ang_theta_deg_in = ValueRange(prm.getDouble("ang_theta_in"));
  } else if (prm.isDefined("ang_theta_in_data")) {
    ang_theta_deg_in = ValueRange(prm.getVectorOfDoubles("ang_theta_in_data"));
  } else {
    ang_theta_deg_in = ValueRange(prm.getDouble("ang_theta_in_min"), prm.getDouble("ang_theta_in_max"), prm.getInt("ang_theta_in_steps")); 
    if (prm.somethingWasNotDefined()) {
      jlog::es << "Either 'ang_theta_in' must be defined to give a single theta incident angle,";
      jlog::es << "or 'ang_theta_in_min', 'ang_theta_in_max' and 'ang_theta_in_steps' must all be defined";
      jlog::es << "to give a range of theta incident angles.";
      return false;
    }
  }
  
  if (prm.isDefined("ang_phi_in")) {
    ang_phi_deg_in = ValueRange(prm.getDouble("ang_phi_in"));
  } else if (prm.isDefined("ang_phi_in_data")) {
    ang_phi_deg_in = ValueRange(prm.getVectorOfDoubles("ang_phi_in_data"));
  } else {
    ang_phi_deg_in = ValueRange(prm.getDouble("ang_phi_in_min"), prm.getDouble("ang_phi_in_max"), prm.getInt("ang_phi_in_steps")); 
    if (prm.somethingWasNotDefined()) {
      jlog::es << "Either 'ang_phi_in' must be defined to give a single phi incident angle,";
      jlog::es << "or 'ang_phi_in_min', 'ang_phi_in_max' and 'ang_phi_in_steps' must all be defined";
      jlog::es << "to give a range of phi incident angles.";
      return false;
    }
  }
  
  if (prm.isDefined("ang_theta_out")) {
    ang_theta_deg_out = ValueRange(prm.getDouble("ang_theta_out"));
  } else if (prm.isDefined("ang_theta_out_data")) {
    ang_theta_deg_out = ValueRange(prm.getVectorOfDoubles("ang_theta_out_data"));
  } else {
    ang_theta_deg_out = ValueRange(prm.getDouble("ang_theta_out_min"), prm.getDouble("ang_theta_out_max"), prm.getInt("ang_theta_out_steps")); 
    if (prm.somethingWasNotDefined()) {
      jlog::es << "Either 'ang_theta_out' must be defined to give a single theta exitant angle,";
      jlog::es << "or 'ang_theta_out_min', 'ang_theta_out_max' and 'ang_theta_out_steps' must all be defined";
      jlog::es << "to give a range of theta exitant angles.";
      return false;
    }
  }
  
  if (prm.isDefined("ang_phi_out")) {
    ang_phi_deg_out = ValueRange(prm.getDouble("ang_phi_out"));
  } else if (prm.isDefined("ang_phi_out_data")) {
    ang_phi_deg_out = ValueRange(prm.getVectorOfDoubles("ang_phi_out_data"));
  } else {
    ang_phi_deg_out = ValueRange(prm.getDouble("ang_phi_out_min"), prm.getDouble("ang_phi_out_max"), prm.getInt("ang_phi_out_steps")); 
    if (prm.somethingWasNotDefined()) {
      jlog::es << "Either 'ang_phi_out' must be defined to give a single theta exitant angle,";
      jlog::es << "or 'ang_phi_out_min', 'ang_phi_out_max' and 'ang_phi_out_steps' must all be defined";
      jlog::es << "to give a range of phi exitant angles.";
      return false;
    }
  }

    // open the file for write
  JFile f(prm.getString("ang_data_save_fp"));
  if (!f.open(JIO_WriteOnly)) {
    jlog::es << "Failed to open file '" << f.name() << "' for write\n";
    return false;
  }

  JTextStream str(&f);
  str.setScientific(6);

  str << "#phi_in\ttheta_in\tphi_out\ttheta_out";
  for (int i=0; i<rdd.bandCount(); i++) str << "\tband" << (i+1);
  str << "\n";
	 
  for (int pi=0; pi<=ang_phi_deg_in.stepCount(); pi++) {
    double phi_in = ang_phi_deg_in.stepValue(pi);
    
    for (int ti=0; ti<=ang_theta_deg_in.stepCount(); ti++) {
      double theta_in = ang_theta_deg_in.stepValue(ti);
      
      for (int po=0; po<=ang_phi_deg_out.stepCount(); po++) {
	double phi_out = ang_phi_deg_out.stepValue(po);
	
	for (int to=0; to<=ang_theta_deg_out.stepCount(); to++) {
	  double theta_out = ang_theta_deg_out.stepValue(to);

	  //jlog::ls << phi_in << " " << theta_in << " " << phi_out << " " << theta_out << "\n";
	  
	    // convert degrees to radians
	  double theta_in_rad = degToRad(theta_in);
	  double theta_out_rad = degToRad(theta_out);
	  double phi_in_rad = degToRad(phi_in);
	  double phi_out_rad = degToRad(phi_out);

	  const SBandData& sd = rdd.at(Vec3D(theta_in_rad, phi_in_rad), Vec3D(theta_out_rad, phi_out_rad));
	  
	  str << phi_in << "\t" << theta_in << "\t" << phi_out << "\t" << theta_out;
	  for (int i=0; i<sd.bandCount(); i++) str << "\t" << sd.bandValue(i);
	  str << "\n";
	}
      }
    }
  }
  f.close();

  return true;
}



void FileTool::helpText(JTextStream& str) const {

  str << "\n";

  str << "Basic usage:\n\n";
  str << "filetool param1=<xxx> param2=<xxx> <filepath>\n\n";
  str << "Reads a file of directionally tabulated data or a direction-to-direction function and displays information about it on the console or to text files.\n";

  str << "Tabulated Data To Display\n";
  str << "-------------------------\n";
  str << "ds_sa_table_display           = yes | no  (display quad solid angles, optional)\n";
  str << "ds_sact_table_display         = yes | no  (display quad integral of abs(cos(theta)) over solid angle, optional)\n";
  str << "ds_ct_table_display           = yes | no  (display quad averaged abs(cos(theta)) values, optional)\n";
  str << "ds_index_table_display        = yes | no  (display quad indices, optional)\n";
  str << "ds_mean_direc_table_display   = yes | no  (display outward direction vector through the quad solid angle mean point, optional)\n";
  str << "ds_mean_phi_table_display     = yes | no  (display quad solid angular mean phi angles, optional)\n";
  str << "ds_mean_theta_table_display   = yes | no  (display quad solid angular mean theta angles, optional)\n";
  str << "ds_point_direc_table_display  = yes | no  (display outward direction vector through the quad centre point, optional)\n";
  str << "ds_point_phi_table_display    = yes | no  (display quad centre point phi angles, optional)\n";
  str << "ds_point_theta_table_display  = yes | no  (display quad centre point theta angles, optional)\n\n";

  str << "contents_display              = yes | no  (display the data in the file)\n\n";

  str << "Tabulated Data To Save As Text Files\n";
  str << "------------------------------------\n";
  str << "ds_sa_table_save_fp           = <filepath>  (filepath to save quad solid angles, optional)\n";
  str << "ds_sact_table_save_fp         = <filepath>  (filepath to save quad integral of abs(cos(theta)) over solid angle, optional)\n";
  str << "ds_ct_table_save_fp           = <filepath>  (filepath to save quad averaged abs(cos(theta)) values, optional)\n";
  str << "ds_index_table_save_fp        = <filepath>  (filepath to save quad indices, optional)\n";
  str << "ds_mean_phi_table_save_fp     = <filepath>  (filepath to save quad centre phi angles, optional)\n";
  str << "ds_mean_theta_table_save_fp   = <filepath>  (filepath to save quad centre theta angles, optional)\n";
  str << "ds_point_direc_table_save_fp  = <filepath>  (filepath to save outward direction vector through the quad centre point, optional)\n";
  str << "ds_mean_direc_table_save_fp   = <filepath>  (filepath to save outward direction vector through the quad solid angle mean point, optional)\n\n";

  str << "contents_save_fp              = <filepath>  (filepath to save the data in the file)\n\n";

  str << "Modifying Files\n";
  str << "----------------\n";
  str << "contents_load_fp                  = <filepath>  (filepath to a text file for loading data in the file)\n";
  str << "azimuthally_average               = yes | no    (set to 'yes' to convert file to azimuthally averaged, optional)\n";
  str << "symmetrically_average_around_phi  = <real>      (supply phi in radians to convert file to symmetrically averaged, optional)\n\n";

  str << "Output Options For Tabulated Directional Data\n";
  str << "---------------------------------------------\n";
  str << "ang_data_save_fp   = <filepath>  (if defined means save a text file of angular data)\n\n";
  str << "The angles at which to extract data may be specified as single angles:\n\n";
  str << "ang_theta          = <real>      (the zenith angle of the outward direction)\n";
  str << "ang_phi            = <real>      (the azimuth angle of the outward direction)\n\n";
  str << "Or for either angle a range may be specified like this, for theta:\n\n";
  str << "ang_theta_min      = <real>      (the minimum value, inclusive)\n";
  str << "ang_theta_max      = <real>      (the maximum value, inclusive)\n";
  str << "ang_theta_steps    = <real>      (the number of steps to get from the minimum to the maximum)\n\n";
  str << "Or a series of angle points may be give, like this:\n\n";
  str << "ang_theta_data     = <r1,r2,r3...rn>      (comma seperated angle points in degrees)\n\n";
  str << "Hence a tabulated file over one or more angles can be extracted.\n\n";

  str << "Output Options For Tabulated Direction-To-Direction Functions\n";
  str << "-------------------------------------------------------------\n";
  str << "ang_data_save_fp   = <filepath>  (if defined means save a text file of angular data)\n\n";
  str << "The angles at which to extract data may be specified as single angles:\n\n";
  str << "ang_theta_in       = <real>      (the zenith angle of the incident direction)\n";
  str << "ang_phi_in         = <real>      (the azimuth angle of the incident direction)\n";
  str << "ang_theta_out      = <real>      (the zenith angle of the exitant direction)\n";
  str << "ang_phi_out        = <real>      (the azimuth angle of the exitant direction)\n\n";
  str << "Or for any of the four angles a range may be specified like this, for incident theta:\n\n";
  str << "ang_theta_in_min   = <real>      (the minimum value, inclusive)\n";
  str << "ang_theta_in_max   = <real>      (the maximum value, inclusive)\n";
  str << "ang_theta_in_steps = <real>      (the number of steps to get from the minimum to the maximum)\n\n";
  str << "Or a series of angle points may be give, like this:\n\n";
  str << "ang_theta_in_data  = <r1,r2,r3...rn>      (comma seperated angle points in degrees)\n\n";
  str << "Hence a tabulated file over one or more angles can be extracted.\n\n";
 
  str << "\n";
}

