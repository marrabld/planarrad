
#include "skywidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QToolBox>
#include <QFileDialog>




SkyWidget::SkyWidget(bool enable_file, QWidget* parent) :
  QWidget(parent),
  tool_thread(skytool, tool_prm) {

  QVBoxLayout *vl = new QVBoxLayout(this);
  vl->setSpacing(4);

  QHBoxLayout* hbox = new QHBoxLayout(vl);
  hbox->setSpacing(4);

  QLabel* label = new QLabel("Type:");
  hbox->addWidget(label);

  sky_type_cm = new QComboBox;
  sky_type_cm->insertItem("Diffuse Radiance (Ed = 1.0)", DIFFUSE_CODE);
  sky_type_cm->insertItem("Downward Radiance (Ed = 1.0)", THETAZERO_CODE);
  sky_type_cm->insertItem("Total and Diffuse", SPECULAR_CODE);
  sky_type_cm->insertItem("HL Idealized", HLIDEAL_CODE);
  sky_type_cm->insertItem("Grant Model", GRANT_CODE);
  if (enable_file) sky_type_cm->insertItem("Load From File", FILE_CODE);
  //sky_type_cm->insertItem("libRadtran", LIBRADTRAN_CODE);
  connect(sky_type_cm, SIGNAL(activated(int)), this, SLOT(typeActivated(int)));
  hbox->addWidget(sky_type_cm);
  hbox->addStretch(100);

    // stack of widgets for input parameterers for each type
  input_stack = new QStackedWidget(this);
  vl->addWidget(input_stack);

  QWidget* w;

    // diffuse has no input options
  w = new QWidget(input_stack);
  input_stack->insertWidget(DIFFUSE_CODE, w);

    // down has no input options
  w = new QWidget(input_stack);
  input_stack->insertWidget(THETAZERO_CODE, w);

    // specular
  w = new QWidget(input_stack);
  QVBoxLayout* vbox2 = new QVBoxLayout(w);
  vbox2->addWidget(buildAzimuthAndZenith(w, spec_azimuth_le, spec_zenith_le));
  vbox2->addWidget(buildTotalAndDiffuse(w, spec_total_sl, spec_diffuse_sl));
  vbox2->addStretch(100);
  input_stack->insertWidget(SPECULAR_CODE, w);

  w = new QWidget(input_stack);
  vbox2 = new QVBoxLayout(w);
  vbox2->addWidget(buildAzimuthAndZenith(w, hlideal_azimuth_le, hlideal_zenith_le));
  vbox2->addWidget(buildHLIdealParams(w));
  vbox2->addStretch(100);
  input_stack->insertWidget(HLIDEAL_CODE, w);

  w = new QWidget(input_stack);
  vbox2 = new QVBoxLayout(w);
  vbox2->addWidget(buildAzimuthAndZenith(w, grant_azimuth_le, grant_zenith_le));
  vbox2->addWidget(buildTotalAndDiffuse(w, grant_total_sl, grant_diffuse_sl));
  vbox2->addSpacing(4);
  vbox2->addWidget(buildGrantCloudState(w));
  vbox2->addStretch(100);
  input_stack->insertWidget(GRANT_CODE, w);

  //w = new QWidget(input_stack);
  //vbox2 = new QVBoxLayout(w);
  //vbox2->addStretch(100);
  //input_stack->insertWidget(LIBRADTRAN_CODE, w);

  if (enable_file) {
    w = new QWidget(input_stack);
    vbox2 = new QVBoxLayout(w);
    file_load_sl = new SpecLoad("", w, vbox2, false, "Load input radiance distribution");
    vbox2->addStretch(100);
    input_stack->insertWidget(FILE_CODE, w);
  }

  vl->addStretch(100);

  connect(&tool_thread, SIGNAL(processingFinished(bool)), this, SLOT(slotProcessingFinished(bool)));
}



// SkyWidget::buildAzimuthAndZenith() [private]

QWidget* SkyWidget::buildAzimuthAndZenith(QWidget* parent, QLineEdit*& azimuth_le, QLineEdit*& zenith_le) {

  QWidget* w = new QWidget(parent);

  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(2);
  grid->setMargin(0);

  grid->setColStretch(3,100);
  QLabel* label = new QLabel("Zenith: ",w);
  grid->addWidget(label, 0, 0);
  label = new QLabel("Azimuth: ",w);
  grid->addWidget(label, 1, 0);

  zenith_le = new QLineEdit(w);
  zenith_le->setMaxLength(20);
  zenith_le->setFixedWidth(60);
  grid->addWidget(zenith_le, 0, 1);

  azimuth_le = new QLineEdit(w);
  azimuth_le->setMaxLength(20);
  azimuth_le->setFixedWidth(60);
  grid->addWidget(azimuth_le, 1, 1);

  label = new QLabel(" (deg)",w);
  grid->addWidget(label, 0, 2);
  label = new QLabel(" (deg)",w);
  grid->addWidget(label, 1, 2);

  return w;
}



// SkyWidget::buildTotalAndDiffuse() [private]

QWidget* SkyWidget::buildTotalAndDiffuse(QWidget* parent, SpecLoad*& total_sl, SpecLoad*& diffuse_sl) {

  QWidget* w = new QWidget(parent);
 
  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(2);
  grid->setMargin(0);
  grid->setColStretch(4,100);
 
  total_sl = new SpecLoad("Total Irradiance (Wm-2nm-1): ", w, grid, 0, 0, "Load total downwelling spectral irradiance");
  diffuse_sl = new SpecLoad("Diffuse Irradiance (Wm-2nm-1): ", w, grid, 1, 0, "Load diffuse component downwelling spectral irradiance");

  return w;
}



// SkyWidget::buildHLIdealParams() [private]

QWidget* SkyWidget::buildHLIdealParams(QWidget* parent) {

  QWidget* w = new QWidget(parent);

  QGridLayout *grid = new QGridLayout(w);
  grid->setSpacing(2);
  grid->setMargin(0);
  grid->setColStretch(3,100);

  QLabel* label = new QLabel("Cosine parameter (C): ",w);
  grid->addWidget(label, 0, 0);
  label = new QLabel("Diffuse Irradiance Ratio (r): ",w);
  grid->addWidget(label, 1, 0);

  hlideal_C_le = new QLineEdit(w);
  hlideal_C_le->setMaxLength(20);
  hlideal_C_le->setFixedWidth(60);
  grid->addWidget(hlideal_C_le, 0, 1);

  hlideal_r_le = new QLineEdit(w);
  hlideal_r_le->setMaxLength(20);
  hlideal_r_le->setFixedWidth(60);
  grid->addWidget(hlideal_r_le, 1, 1);

  return w;
}



// SkyWidget::buildGrantCloudState() [private]

QWidget* SkyWidget::buildGrantCloudState(QWidget* parent) {

  QWidget* w = new QWidget(parent);

  QHBoxLayout *hbox = new QHBoxLayout(w);
  hbox->setContentsMargins(0,0,0,0);
  hbox->setSpacing(20);

  QLabel* label = new QLabel("Cloud State:",w);
  hbox->addWidget(label);

  grant_clear_rb = new QRadioButton("Clear Sky",w);
  hbox->addWidget(grant_clear_rb);
  grant_high_cloud_rb = new QRadioButton("High Cloud",w);
  hbox->addWidget(grant_high_cloud_rb);
  grant_low_cloud_rb = new QRadioButton("Low Cloud",w);
  hbox->addWidget(grant_low_cloud_rb);
  grant_overcast_rb = new QRadioButton("Overcast",w);
  hbox->addWidget(grant_overcast_rb);
 
  hbox->addStretch(100);
 
  grant_clear_rb->setChecked(true);

  return w;
}



void SkyWidget::parserToGui(const JString& prefix, const JParser& prm, prm_context pc) {

  JString s;

    // set azimuth and zenith in all models
  s = prm.getStringOrEmpty(prefix+"azimuth");
  hlideal_azimuth_le->setText(J2Q(s));
  spec_azimuth_le->setText(J2Q(s));
  grant_azimuth_le->setText(J2Q(s));
  s = prm.getStringOrEmpty(prefix+"zenith");
  hlideal_zenith_le->setText(J2Q(s));
  spec_zenith_le->setText(J2Q(s));
  grant_zenith_le->setText(J2Q(s));
 
    // hlideal parameters
  hlideal_r_le->setText(prm.isDefined(prefix+"rdif") ? J2Q(prm.getString(prefix+"rdif")) : "0.3");
  hlideal_C_le->setText(prm.isDefined(prefix+"C") ? J2Q(prm.getString(prefix+"C")) : "1");

    // total and diffuse irradiance files 
    // files are the only way these can currently be specified in the gui tool
  s = prm.getStringOrEmpty(prefix+"total_irrad_fp");
  spec_total_sl->setSourceFilepath(s);
  grant_total_sl->setSourceFilepath(s);
  s = prm.getStringOrEmpty(prefix+"diffuse_irrad_fp");
  spec_diffuse_sl->setSourceFilepath(s);
  grant_diffuse_sl->setSourceFilepath(s);		  

    // total and diffuse irradiance band specs
  s = prm.getStringOrEmpty(prefix+"total_irrad_band_spec_fp");
  spec_total_sl->setBandSpecFilepath(s);
  grant_total_sl->setBandSpecFilepath(s);
  s = prm.getStringOrEmpty(prefix+"diffuse_irrad_band_spec_fp");
  spec_diffuse_sl->setBandSpecFilepath(s);
  grant_diffuse_sl->setBandSpecFilepath(s);
 
    // file load file path and band spec
  s = prm.getStringOrEmpty(prefix+"file_load_fp");
  file_load_sl->setSourceFilepath(s);
  s = prm.getStringOrEmpty(prefix+"file_load_band_spec_fp");
  file_load_sl->setBandSpecFilepath(s);

    // default grant model if none defined
  if (!prm.isDefined(prefix+"model_eqn")) grant_clear_rb->setChecked(true);
  else {
    grant_clear_rb->setChecked(prm.valueCheck(prefix+"model_eqn", "clear"));
    grant_high_cloud_rb->setChecked(prm.valueCheck(prefix+"model_eqn", "high_cloud"));
    grant_low_cloud_rb->setChecked(prm.valueCheck(prefix+"model_eqn", "low_cloud"));
    grant_overcast_rb->setChecked(prm.valueCheck(prefix+"model_eqn", "overcast"));
  }

  if (!prm.isDefined(prefix+"type")) {
    sky_type_cm->setCurrentItem(GRANT_CODE);
    input_stack->setCurrentIndex(GRANT_CODE);

  } else {
      // set which model is being used
    if (prm.valueCheck(prefix+"type","diffuse")) {
      sky_type_cm->setCurrentItem(DIFFUSE_CODE);
      input_stack->setCurrentIndex(DIFFUSE_CODE);
    } else if (prm.valueCheck(prefix+"type","thetazero")) {
      sky_type_cm->setCurrentItem(THETAZERO_CODE);
      input_stack->setCurrentIndex(THETAZERO_CODE);
    } else if (prm.valueCheck(prefix+"type","specular")) {
      sky_type_cm->setCurrentItem(SPECULAR_CODE);
      input_stack->setCurrentIndex(SPECULAR_CODE);
    } else if (prm.valueCheck(prefix+"type","hlideal")) {
      sky_type_cm->setCurrentItem(HLIDEAL_CODE);
      input_stack->setCurrentIndex(HLIDEAL_CODE);
    } else if (prm.valueCheck(prefix+"type","grant")) {
      sky_type_cm->setCurrentItem(GRANT_CODE);
      input_stack->setCurrentIndex(GRANT_CODE);
    } else if (prm.valueCheck(prefix+"type","fileload")) {
      sky_type_cm->setCurrentItem(FILE_CODE);
      input_stack->setCurrentIndex(FILE_CODE);
      //  } else if (prm.valueCheck("type","libradtran")) {
      // sky_type_cm->setCurrentItem(LIBRADTRAN_CODE);
      //input_stack->setCurrentIndex(LIBRADTRAN_CODE);
    }
  }

    // file to save
  //save_fl->setFilepath(outer_prm.getStringOrEmpty("sky_save_fp"));
}



// SkyWidget::guiToParser()

void SkyWidget::guiToParser(const JString& prefix, JParser& prm, prm_context pc) const {

  JString s;

  int type = sky_type_cm->currentItem();

  if (pc == SLABTOOL) {
    if (type != FILE_CODE) prm.setLabelAndValue(prefix+"sky_fp", sky_fp);
    else {
      if (!(s = file_load_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"sky_fp", s);
      if (!(s = file_load_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"sky_band_spec_fp", s);
    } 
    return;
  }
  
  if (type == DIFFUSE_CODE) {
    prm.setLabelAndValue(prefix+"type", "diffuse");

  } else if (type == THETAZERO_CODE) {
    prm.setLabelAndValue(prefix+"type", "thetazero");

  } else if (type == SPECULAR_CODE) {
    prm.setLabelAndValue(prefix+"type", "specular");
    if (!(s = spec_azimuth_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"azimuth", s);
    if (!(s = spec_zenith_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"zenith", s);
    if (!(s = spec_total_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"total_irrad_fp", s);
    if (!(s = spec_diffuse_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"diffuse_irrad_fp", s);
    if (!(s = spec_total_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"total_irrad_band_spec_fp", s);
    if (!(s = spec_diffuse_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"diffuse_irrad_band_spec_fp", s);

  } else if (type == HLIDEAL_CODE) {
    prm.setLabelAndValue(prefix+"type", "hlideal");
    if (!(s = hlideal_azimuth_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"azimuth", s);
    else prm.setLabelAndValue(prefix+"azimuth", "0");
    if (!(s = hlideal_zenith_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"zenith", s);
    else prm.setLabelAndValue(prefix+"zenith", "0");
    if (!(s = hlideal_C_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"C", s);
    else prm.setLabelAndValue(prefix+"C", "0");
    if (!(s = hlideal_r_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"rdif", s);
    else prm.setLabelAndValue(prefix+"rdif", "0");

  } else if (type == GRANT_CODE) {
    prm.setLabelAndValue(prefix+"type", "grant");
    if (!(s = grant_azimuth_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"azimuth", s);
    if (!(s = grant_zenith_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"zenith", s);

    if (!(s = grant_total_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"total_irrad_fp", s);
    if (!(s = grant_diffuse_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"diffuse_irrad_fp", s);
    if (!(s = grant_total_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"total_irrad_band_spec_fp", s);
    if (!(s = grant_diffuse_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"diffuse_irrad_band_spec_fp", s);
  
    if (grant_clear_rb->isChecked()) prm.setLabelAndValue(prefix+"model_eqn", "clear");
    else if (grant_high_cloud_rb->isChecked()) prm.setLabelAndValue(prefix+"model_eqn", "high_cloud");
    else if (grant_low_cloud_rb->isChecked()) prm.setLabelAndValue(prefix+"model_eqn", "low_cloud");
    else prm.setLabelAndValue(prefix+"model_eqn", "overcast");

  } else if (type == FILE_CODE) {
    prm.setLabelAndValue(prefix+"type", "fileload");
    if (!(s = file_load_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"file_load_fp", s);
    if (!(s = file_load_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"file_load_band_spec_fp", s);
  
    //} else if (type == LIBRADTRAN_CODE) {
    // prm.setLabelAndValue("type", "libradtran");
  }
}



void SkyWidget::rebuildSpecLoads() {
  spec_total_sl->rebuild();
  grant_total_sl->rebuild();
  spec_diffuse_sl->rebuild();
  grant_diffuse_sl->rebuild();	
  file_load_sl->rebuild();	  
}



bool SkyWidget::buildSkyThread(const BandSpec& bs_spec, const DirecSpec& ds_spec, int sub_quad_count, const JString& output_dir, bool fp_option) {

    // the file path where to save the built sky radiance
  sky_fp = output_dir+"/input_rd_"+ds_spec.code()+"_"+JString::number(bs_spec.bandCount());

  bool is_file_load(sky_type_cm->currentItem() == FILE_CODE);

  if (fp_option && !is_file_load) {
    QString qs(QFileDialog::getSaveFileName(this, "Save Input Radiance File", J2Q(sky_fp)));
    sky_fp = Q2J(qs);
  }

  if (sky_fp.isEmpty()) return false;
			 
  tool_prm.clear();
  bs_spec.addToParser(tool_prm, "");
  ds_spec.addToParser(tool_prm, "");
  guiToParser("", tool_prm, SKYTOOL);
  if (sub_quad_count > 0) tool_prm.setLabelAndValue("sub_quad_count", sub_quad_count);
    // only save file if not loading
  if (!is_file_load) tool_prm.setLabelAndValue("sky_save_fp", sky_fp);
  else sky_fp = tool_prm.getStringOrEmpty("file_load_fp");
  tool_prm.setLabelAndValue("verbose", jlog::verboseLevel());

  tool_thread.start();

  return true;
}



// SkyWidget::slotProcessingFinished()

void SkyWidget::slotProcessingFinished(bool ret_ok) {

  if (jlog::vbLevel(5)) jlog::ls << "SkyWidget::processingFinished() ok: " << ret_ok << "\n";

  if (!ret_ok) sky_fp = "";

  signalProcessingFinished(ret_ok);
}



// SkyWidget::typeActivated() [slot]

void SkyWidget::typeActivated(int code) {
  input_stack->setCurrentIndex(code);
}




