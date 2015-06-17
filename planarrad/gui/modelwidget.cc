
#include "modelwidget.h"
#include "base/jdir.h"
#include "base/jlog.h"
#include "fe/tool_gui.h"
#include "rad/bandspec.h"
#include "ds/direcspec.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>



ModelWidget::ModelWidget(const AdditionalOptionsDialog& aod, QWidget* parent) : 
  aod(aod), 
  tool_thread(slabtool, tool_prm),
  processing_chain(DONE) {

  QVBoxLayout *vl = new QVBoxLayout(this);
  vl->addSpacing(10);

    // run name
  QHBoxLayout *hl = new QHBoxLayout(vl);
  QLabel* label = new QLabel("Name: ", this);
  hl->addWidget(label);
  name_le = new QLineEdit(this);
  hl->addWidget(name_le);
  vl->addSpacing(4);

  hl = new QHBoxLayout(vl);

    // band selector
  label = new QLabel("Bands: ", this);
  hl->addWidget(label);
  target_bandspec_cm = new BandSpecComboBox(false, this);
  hl->addWidget(target_bandspec_cm);
  hl->addSpacing(20);

    // directional structure selector
  label = new QLabel("Directional Structure: ", this);
  hl->addWidget(label);
  target_direcspec_cm = new DirecSpecComboBox(false, this);
  hl->addWidget(target_direcspec_cm);

    // white space to the right
  hl->addStretch(100);

  vl->addSpacing(10);
 
  tb = new QToolBox(this);

  sky_widget = new SkyWidget(true, this);
  tb->addItem(sky_widget, "Input Radiance Distribution");

  interface_widget = new InterfaceWidget(this);
  tb->addItem(interface_widget, "Interface Properties");

  iop_widget = new IOPWidget(this);
  tb->addItem(iop_widget, "Medium Properties");

  depth_widget = new DepthWidget(this);
  tb->addItem(depth_widget, "Depth Options");

  boundary_widget = new BoundaryWidget(this);
  tb->addItem(boundary_widget, "Lower Boundary Properties");

  tb->setMinimumHeight(360);

  vl->addWidget(tb);

  vl->addStretch(100);

  hl = new QHBoxLayout(vl);
  process_pb = new QPushButton(" Process ", this);
  connect(process_pb, SIGNAL(clicked()), this, SLOT(slotStartProcessing()));
  hl->addWidget(process_pb);

  activity_label = new QLabel("Idle", this);
  activity_label->setFrameStyle(QFrame::Panel);
  activity_label->setFrameShadow(QFrame::Sunken);
  activity_label->setAlignment(Qt::AlignCenter);
  activity_label->setFixedWidth(100);
  hl->addWidget(activity_label);

  progress_bar = new QProgressBar(this);
  progress_bar->setMinimum(0);
  progress_bar->setMaximum(100);
  hl->addWidget(progress_bar);

  cancel_pb = new QPushButton(" Cancel ", this);
  connect(cancel_pb, SIGNAL(clicked()), this, SLOT(slotCancelProcessing()));
  hl->addWidget(cancel_pb);

  setIsProcessing(false);

  connect(&progress_timer, SIGNAL(timeout()), this, SLOT(slotUpdateProgress()));

  connect(sky_widget, SIGNAL(signalProcessingFinished(bool)), this, SLOT(slotProcessingFinished(bool)));
  connect(interface_widget, SIGNAL(signalProcessingFinished(bool)), this, SLOT(slotProcessingFinished(bool)));
  connect(iop_widget, SIGNAL(signalProcessingFinished(bool)), this, SLOT(slotProcessingFinished(bool)));
  connect(&tool_thread, SIGNAL(processingFinished(bool)), this, SLOT(slotProcessingFinished(bool)));
}



// ModelWidget::saveSettings()

void ModelWidget::saveSettings(const JString& prefix, QSettings& settings) const {

    // current tab
  settings.setValue("main_window/toolbox", tb->currentIndex());

    // will collect parameters for this widget
  JParser prm;
    // add sky widget parameters
  guiToParser(prefix, prm);
    // save to settings - uses the prefix here as well so the parser doesn't read unnecessary parameters
  Tool_Gui::parserAllToSettings(prm, prefix+"params", settings);
    // log for debugging
  if (jlog::vb(5)) prm.displayList(jlog::ls);
}



// ModelWidget::restoreSettings()

void ModelWidget::restoreSettings(const JString& prefix, const QSettings& settings) {

    // selected tab
  tb->setCurrentIndex(settings.value("main_window/toolbox").toInt());
  
    // parameters for this widget
    // uses the prefix here as well so the parser doesn't read unnecessary parameters
  JParser prm;
  Tool_Gui::settingsToParserAll(prm, prefix+"params", settings);
    // set parameters from the parser
  parserToGui(prefix, prm);
    // log for debugging
  if (jlog::vb(5)) prm.displayList(jlog::ls);
}



// ModelWidget::parserToGui()

void ModelWidget::parserToGui(const JString& prefix, const JParser& prm) {

    // the name of the run that is set up
  name_le->setText(J2Q(prm.getStringOrEmpty(prefix+"name")));

  target_bandspec_cm->setCurrentBandSpecFromFilepath(prm.getStringOrEmpty("band_spec_fp"));
  target_direcspec_cm->setCurrentDirecSpecFromFilepath(prm.getStringOrEmpty("direc_spec_fp"));
 
    // parameters of the sub-widgets
  sky_widget->parserToGui(prefix+"sky_", prm, SkyWidget::SAVE);
  interface_widget->parserToGui(prefix+"iface_", prm, InterfaceWidget::SAVE);
  iop_widget->parserToGui(prefix+"iop_", prm, IOPWidget::SAVE);
  depth_widget->parserToGui(prefix, prm, DepthWidget::SAVE);
  boundary_widget->parserToGui(prefix+"bound_", prm, BoundaryWidget::SAVE);
}



// ModelWidget::guiToParser()

void ModelWidget::guiToParser(const JString& prefix, JParser& prm) const {

    // the name of the run that is set up
  prm.setLabelAndValue(prefix+"name", Q2J(name_le->text()));

  BandSpec& bs_spec = *target_bandspec_cm->currentBandSpec();
  DirecSpec& ds_spec = *target_direcspec_cm->currentDirecSpec();

    // add the band spec and direc spec to the parser
  bs_spec.addToParser(prm, prefix);
  ds_spec.addToParser(prm, prefix);
  
    // parameters of the sub-widgets
  sky_widget->guiToParser(prefix+"sky_", prm, SkyWidget::SAVE);
  interface_widget->guiToParser(prefix+"iface_", prm, InterfaceWidget::SAVE);
  iop_widget->guiToParser(prefix+"iop_", prm, IOPWidget::SAVE);
  depth_widget->guiToParser(prefix, prm, DepthWidget::SAVE);
  boundary_widget->guiToParser(prefix+"bound_", prm, BoundaryWidget::SAVE);
}



// ModelWidget::bBuildInputRadianceFile()

void ModelWidget::slotBuildInputRadianceFileThread() {

  slotProcessingActivity("Input Rad.");

  bool one_op(processing_chain==DONE);

    // disable stuff here if just this call
  if (one_op) setIsProcessing(true);

  BandSpec* bs_spec = target_bandspec_cm->currentBandSpec();
  if (!bs_spec) return;

  DirecSpec* ds_spec = target_direcspec_cm->currentDirecSpec();
  if (!ds_spec) return;

    // make out directory if required, output_dir will be empty if failed
  const JString output_dir(one_op ? outputDirExistsOrEmpty() : outputDirMake());

    // get the sub quad count from the additional options
  int sub_quad_count = aod.skySubQuadCount();

    // make sure pct complete is zero before starting the update timer
  sky_widget->clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);
    // the iop_widget will start a processing thread
    // after its finished ModelWidget::slotProcessingFinished() will be called
  if (!sky_widget->buildSkyThread(*bs_spec, *ds_spec, sub_quad_count, output_dir, one_op)) setIsProcessing(false);
}



// ModelWidget::slotBuildInterfaceFile()

void ModelWidget::slotBuildInterfaceFileThread() {
 
  slotProcessingActivity("Interface Func.");

  bool one_op(processing_chain==DONE);

    // disable stuff here if just this call
  if (one_op) setIsProcessing(true);
 
  DirecSpec* ds_spec = target_direcspec_cm->currentDirecSpec();
  if (!ds_spec) return;

    // make out directory if required, output_dir will be empty if failed
  const JString output_dir(one_op ? outputDirExistsOrEmpty() : outputDirMake());

    // get the sub quad count from the additional options
  int sub_quad_count = aod.ifaceSubQuadCount();

    // make sure pct complete is zero before starting the update timer
  interface_widget->clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);
    // the iop_widget will start a processing thread
    // after its finished ModelWidget::slotProcessingFinished() will be called
  if (!interface_widget->buildInterfaceThread(*ds_spec, sub_quad_count, output_dir, one_op)) setIsProcessing(false);
}



// ModelWidget::slotBuildPhaseFunctionFile()

void ModelWidget::slotBuildPhaseFunctionFileThread() {

  slotProcessingActivity("Phase Func.");
 
  bool one_op(processing_chain==DONE);

    // disable stuff here if just this call
  if (one_op) setIsProcessing(true);
 
  DirecSpec* ds_spec = target_direcspec_cm->currentDirecSpec();
  if (!ds_spec) return;

    // make out directory if required, output_dir will be empty if failed
  const JString output_dir(one_op ? outputDirExistsOrEmpty() : outputDirMake());

    // get the sub quad count from the additional options
  int sub_quad_count = aod.pfSubQuadCount();

    // make sure pct complete is zero before starting the update timer
  iop_widget->clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);
    // the iop_widget will start a processing thread
    // after its finished ModelWidget::slotProcessingFinished() will be called
  if (!iop_widget->buildPhaseFuncThread(*ds_spec, sub_quad_count, output_dir, one_op)) setIsProcessing(false);
}



// ModelWidget::slotRunSlabToolThread()

void ModelWidget::slotRunSlabToolThread() {

  slotProcessingActivity("Integration");
  slow_bit_message = false; 

  BandSpec* bs_spec = target_bandspec_cm->currentBandSpec();
  if (!bs_spec) return;

  DirecSpec* ds_spec = target_direcspec_cm->currentDirecSpec();
  if (!ds_spec) return;

    // make out directory if required, output_dir will be empty if failed
  const JString output_dir(outputDirMake());

    // make sure pct complete is zero before starting the update timer
  slabtool.clearPctComplete();
    // a progress update every 100 ms
  processing_canceled = false;
  progress_timer.start(100);

  tool_prm.clear();
  tool_prm.setLabelAndValue("name", Q2J(name_le->text()));
  bs_spec->addToParser(tool_prm, "");
  ds_spec->addToParser(tool_prm, "");
  depth_widget->guiToParser("", tool_prm, DepthWidget::SLABTOOL);
  boundary_widget->guiToParser("", tool_prm, BoundaryWidget::SLABTOOL);
  aod.guiToParser("", tool_prm, AdditionalOptionsDialog::SLABTOOL);
  sky_widget->guiToParser("", tool_prm, SkyWidget::SLABTOOL);
  interface_widget->guiToParser("", tool_prm, InterfaceWidget::SLABTOOL);
  iop_widget->guiToParser("", tool_prm, IOPWidget::SLABTOOL);

  if (aod.saveImages()) {

    int im_size = 128;
    double sens_k = aod.saveImagesAutoSensK() ? 0 : aod.saveImagesSensK();

    addImageSave(tool_prm, output_dir, "Ld_a", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Lu_a", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Ld_w", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Lu_w", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Ld_b", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Lu_b", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Ld_it", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Lu_it", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Ld_ir", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Lu_ir", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Ld_sample", im_size, sens_k);
    addImageSave(tool_prm, output_dir, "Lu_sample", im_size, sens_k);
  }

  //tool_prm.setLabelAndValue("Ld_sample_save_fp", output_dir+"/Ld_sample_data");
  //tool_prm.setLabelAndValue("Lu_sample_save_fp", output_dir+"/Lu_sample_data");

  tool_prm.setLabelAndValue("report_save_fp", output_dir+"/report.txt");
  tool_prm.setLabelAndValue("verbose", jlog::verboseLevel());

  tool_thread.start();
}



void ModelWidget::addImageSave(JParser& prm, const JString& outdir, const JString& prefix, int im_size, double sens_k) {
  prm.setLabelAndValue(prefix+"_image_save_fp", outdir+"/"+prefix+"_image.ppm");
  prm.setLabelAndValue(prefix+"_image_size", im_size);
  if (sens_k > 0) prm.setLabelAndValue(prefix+"_image_sens_k", sens_k);
}



const JString ModelWidget::lastOutputFile() const {
  return outputDir()+"/report.txt";
}




// ModelWidget::slotStartProcessing()

void ModelWidget::slotStartProcessing() {

  if (jlog::vb(5)) jlog::ls << "ModelWidget::slotStartProcessing()\n";

  JString fp(outputDirMake());

  if (!fp.isEmpty()) {
    fp += "/params.txt";
    JParser save_prm;
    guiToParser("", save_prm);
    aod.guiToParser("", save_prm, AdditionalOptionsDialog::SAVE);
    save_prm.writeFile(fp);
  }

  setIsProcessing(true);
  processing_chain = IFACE;
  slotBuildInputRadianceFileThread();
}



// ModelWidget::outputDir()

const JString ModelWidget::outputDir() const {
  JString output_dir = "default_output";
  if (!name_le->text().isEmpty()) output_dir = Q2J(name_le->text());
  return output_dir;
}



// ModelWidget::outputDirOrEmpty()

const JString ModelWidget::outputDirExistsOrEmpty() const {
  JString output_dir(outputDir());
  JDir dir(output_dir);
  if (!dir.exists()) return "./";
  return output_dir;
}



// ModelWidget::outputDirMake()

const JString ModelWidget::outputDirMake() const {
  JString output_dir(outputDir());
  JDir dir(output_dir);
  dir.mkdir();
  if (!dir.exists()) return "";
  return output_dir;
}



// ModelWidget::slotCancelProcessing()

void ModelWidget::slotCancelProcessing() {

  if (jlog::vb(5)) jlog::ls << "ModelWidget::slotCancelProcessing()\n";

    // this flag means we know that the processing finished as a result of a cancel
  processing_canceled = true;
    // set the cancel flag in any objects in other threads that might be processing
    // it is their responsibility to stop and when there thread quits slotProcessingFinished() will be called
  sky_widget->cancelProcessing();
  interface_widget->cancelProcessing();
  iop_widget->cancelProcessing();
  slabtool.cancelProcessing();
}



// ModelWidget::slotProcessingFinished()

void ModelWidget::slotProcessingFinished(bool ret_ok) {

  progress_timer.stop();
 
  if (processing_canceled) {

    slotProcessingActivity("Canceled");
    setIsProcessing(false);

  } else if (!ret_ok) {

    slotProcessingActivity("Error");
    setIsProcessing(false);
    emit signalReportError();

  } else {

    progress_bar->setValue(100);

    switch (processing_chain) {
    case IFACE :
      processing_chain = PHASEFUNC;
      slotBuildInterfaceFileThread();
      break;
    case PHASEFUNC :
      processing_chain = SLAB;
      slotBuildPhaseFunctionFileThread();
      break;
    case SLAB :
      processing_chain = DONE;
      slotRunSlabToolThread();
      break;
    default:
      processing_chain = DONE;
      setIsProcessing(false);
      emit signalProcessingFinishedOk();
    }
  }
}



// ModelWidget::slotUpdateProgress()

void ModelWidget::slotUpdateProgress() {
  double pct = 0;
  if (sky_widget->isProcessing()) pct = sky_widget->pctComplete();
  else if (interface_widget->isProcessing()) pct = interface_widget->pctComplete();
  else if (iop_widget->isProcessing()) pct = iop_widget->pctComplete();
  else if (tool_thread.isRunning()) {
    pct = slabtool.pctComplete();
    if (pct >= 40 && !slow_bit_message) { slow_bit_message=true; slotProcessingActivity("Slow Bit"); }
  }
  progress_bar->setValue((int)pct);
}



void ModelWidget::slotProcessingActivity(const JString& s) {
  activity_label->setText(J2Q(s));
}




void ModelWidget::slotCurrentDirChanged() {
  target_bandspec_cm->rebuild();
  target_direcspec_cm->rebuild();

  sky_widget->rebuildSpecLoads();
  iop_widget->rebuildSpecLoads();
  boundary_widget->rebuildSpecLoads();
}



// ModelWidget::setIsProcessing()

void ModelWidget::setIsProcessing(bool f) {

  name_le->setEnabled(!f);
  target_bandspec_cm->setEnabled(!f);
  target_direcspec_cm->setEnabled(!f);

  sky_widget->setEnabled(!f);
  interface_widget->setEnabled(!f);
  iop_widget->setEnabled(!f);
  depth_widget->setEnabled(!f);
  boundary_widget->setEnabled(!f);

  process_pb->setEnabled(!f);
  cancel_pb->setEnabled(f);

  if (!f) slotProcessingActivity("Idle");

  emit signalIsProcessing(f);
}

