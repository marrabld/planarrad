
// interfacewidget.h

#include "interfacewidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>



// InterfaceWidget::(constructor)

InterfaceWidget::InterfaceWidget(QWidget* parent) : 
  QWidget(parent), 
  tool_thread(surftool, tool_prm) {
 
  QVBoxLayout *vl = new QVBoxLayout(this);

  iface_none_rb = new QRadioButton("None", this);
  button_grp.addButton(iface_none_rb);
  vl->addWidget(iface_none_rb);
  vl->addSpacing(2);

  QFrame* bar = new QFrame(this);
  bar->setFrameStyle(QFrame::HLine | QFrame::Plain); 
  vl->addWidget(bar);

  refrac_widget = new QWidget(this);
  vl->addWidget(refrac_widget);

  QHBoxLayout* hl = new QHBoxLayout(refrac_widget);
  hl->setSpacing(4);
  hl->setContentsMargins(0,0,0,0);

  QLabel* label = new QLabel("Refractive index inside:", refrac_widget);
  hl->addWidget(label);
  iface_flat_refrac0_le = new QLineEdit(refrac_widget);
  hl->addWidget(iface_flat_refrac0_le);
  iface_flat_refrac0_le->setFixedWidth(70);
  label = new QLabel("outside:", refrac_widget);
  hl->addWidget(label);
  iface_flat_refrac1_le = new QLineEdit(refrac_widget);
  iface_flat_refrac1_le->setFixedWidth(70);
  hl->addWidget(iface_flat_refrac1_le);
  hl->addStretch(100);

  hl = new QHBoxLayout(vl);
  hl->setSpacing(4);
  hl->setContentsMargins(0,0,0,0);

  iface_flat_rb = new QRadioButton("Flat", this);
  button_grp.addButton(iface_flat_rb);
  hl->addWidget(iface_flat_rb);

  hl = new QHBoxLayout(vl);
  hl->setSpacing(4);
  hl->setContentsMargins(0,0,0,0);

  QGridLayout* grid = new QGridLayout(vl);
  grid->setSpacing(8);
  grid->setMargin(0);
  grid->setColStretch(3, 100);

  iface_wb_rb = new QRadioButton("Wind speed:", this);
  button_grp.addButton(iface_wb_rb);
  grid->addWidget(iface_wb_rb, 0, 0);
  wind_speed_le = new QLineEdit(this);
  grid->addWidget(wind_speed_le, 0, 1);
  wind_speed_le->setFixedWidth(70);
  wb_label3 = new QLabel("(ms-1)", this);
  grid->addWidget(wb_label3, 0, 2);
 
  wb_label1 = new QLabel("Direction:", this);
  grid->addWidget(wb_label1, 1, 0, 1, 1, Qt::AlignRight);
  wind_direc_le = new QLineEdit(this);
  grid->addWidget(wind_direc_le, 1, 1);
  wind_direc_le->setFixedWidth(70);
  wb_label2 = new QLabel("(degrees),   or:", this);
  grid->addWidget(wb_label2, 1, 2);
  azavg_cb = new QCheckBox("Azimuthally average");
  grid->addWidget(azavg_cb, 1, 3);

  bar = new QFrame(this);
  bar->setFrameStyle(QFrame::HLine | QFrame::Plain); 
  vl->addWidget(bar);
 
  hl = new QHBoxLayout(vl);
  hl->setSpacing(0);
  hl->setContentsMargins(0,0,0,0);
  iface_file_rb = new QRadioButton("Load from file:", this);
  button_grp.addButton(iface_file_rb);
  hl->addWidget(iface_file_rb);
  file_load_fl = new FileLoad("", this, hl, true, "Load interface reflection and transmission function file", false);
  //hl->addStretch(100);

  vl->addStretch(100);

  connect(&button_grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotUpdateEnabled()));
  connect(azavg_cb, SIGNAL(stateChanged(int)), this, SLOT(slotUpdateEnabled()));
  connect(&tool_thread, SIGNAL(processingFinished(bool)), this, SLOT(slotProcessingFinished(bool)));
}



// InterfaceWidget::parserToGui()

void InterfaceWidget::parserToGui(const JString& prefix, const JParser& prm, prm_context pc) {

  JString s;

  if (prm.isDefined(prefix+"type")) {

    if (prm.valueCheck(prefix+"type", "absent")) iface_none_rb->setChecked(true);
    else if (prm.valueCheck(prefix+"type", "fileload")) iface_file_rb->setChecked(true);
    else if (prm.valueCheck(prefix+"type", "coxmunk")) iface_wb_rb->setChecked(true);
    else iface_flat_rb->setChecked(true);

  } else iface_flat_rb->setChecked(true);

    // refrac_index_0 is the water side
  s = prm.getStringOrEmpty(prefix+"refrac_index_0");
  if (s.isEmpty()) s = "1.34";
  iface_flat_refrac0_le->setText(J2Q(s));
    // refrac_index_1 is the water side
  s = prm.getStringOrEmpty(prefix+"refrac_index_1");
  if (s.isEmpty()) s = "1.00";
  iface_flat_refrac1_le->setText(J2Q(s));

    // wind speed and direction
  s = prm.getStringOrEmpty(prefix+"wind_speed");
  wind_speed_le->setText(J2Q(s));
  s = prm.getStringOrEmpty(prefix+"wind_direc");
  wind_direc_le->setText(J2Q(s));
    // azimuthal averaging
  azavg_cb->setChecked(prm.isDefinedAndTrueBool(prefix+"azimuthally_average"));

    // file
  s = prm.getStringOrEmpty(prefix+"file_load_fp");
  file_load_fl->setFilepath(s);

  slotUpdateEnabled();
}



// InterfaceWidget::guiToParser()

void InterfaceWidget::guiToParser(const JString& prefix, JParser& prm, prm_context pc) const {

  JString s;

  if (pc == SLABTOOL) {
    if (iface_file_rb->isChecked()) prm.setLabelAndValue(prefix+"water_surface_fp", file_load_fl->filepath());
    else prm.setLabelAndValue(prefix+"water_surface_fp", iface_fp);
    return;
  }

  if (iface_none_rb->isChecked()) prm.setLabelAndValue(prefix+"type", "absent");
  else if (iface_file_rb->isChecked()) prm.setLabelAndValue(prefix+"type", "fileload");
  else if (iface_wb_rb->isChecked()) prm.setLabelAndValue(prefix+"type", "coxmunk");
  else prm.setLabelAndValue(prefix+"type", "flat");

    // refrac_index_0 is the water side
  if (!(s = iface_flat_refrac0_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"refrac_index_0", s);
    // refrac_index_1 is the water side
  if (!(s = iface_flat_refrac1_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"refrac_index_1", s);

    // wind speed and direction
  if (!(s = wind_speed_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"wind_speed", s);
  if (!(s = wind_direc_le->text()).isEmpty()) prm.setLabelAndValue(prefix+"wind_direc", s);
    // azimuthal averaging
  if (azavg_cb->isChecked()) prm.setLabelAndValue(prefix+"azimuthally_average", "yes");

    // file
  if (!(s = file_load_fl->filepath()).isEmpty()) prm.setLabelAndValue(prefix+"file_load_fp", s);
}



// InterfaceWidget::buildInterfaceThread()

bool InterfaceWidget::buildInterfaceThread(const DirecSpec& ds_spec, int sub_quad_count, const JString& output_dir, bool fp_option) {

      // the file path where the built sky radiance can be found
  iface_fp = output_dir+"/iface_rdd_"+ds_spec.code()+"_1";

  if (fp_option && !iface_file_rb->isChecked()) {
    QString qs(QFileDialog::getSaveFileName(this, "Save Interface Function File", J2Q(iface_fp)));
    iface_fp = Q2J(qs);
  }

  if (iface_fp.isEmpty()) return false;

  tool_prm.clear();
  ds_spec.addToParser(tool_prm, "");
  guiToParser("", tool_prm, InterfaceWidget::SURFTOOL);
  tool_prm.setLabelAndValue("band_count", "1");
  tool_prm.setLabelAndValue("surface_size", "1");
  tool_prm.setLabelAndValue("surface_radius", "0.5");
  tool_prm.setLabelAndValue("target_size", "0.2");
  tool_prm.setLabelAndValue("rays_per_quad", "10");
  tool_prm.setLabelAndValue("surface_count", "200");
  tool_prm.setLabelAndValue("min_pwr_frac", "1E-6");
  tool_prm.setLabelAndValue("crosswind_vertices", "100");
  tool_prm.setLabelAndValue("upwind_vertices", "100");
  if (sub_quad_count > 0) tool_prm.setLabelAndValue("sub_quad_count", sub_quad_count);
    // don't save file if loading a file
  if (!iface_file_rb->isChecked()) tool_prm.setLabelAndValue("surface_save_fp", iface_fp);
  tool_prm.setLabelAndValue("verbose", jlog::verboseLevel());

  tool_thread.start();

  return true;
}



// InterfaceWidget::slotProcessingFinished()

void InterfaceWidget::slotProcessingFinished(bool ret_ok) {

  if (jlog::vbLevel(5)) jlog::ls << "InterfaceWidget::processingFinished() ok: " << ret_ok << "\n";

  signalProcessingFinished(ret_ok);
}



// InterfaceWidget::slotUpdateEnabled()

void InterfaceWidget::slotUpdateEnabled() {

  file_load_fl->setEnabled(iface_file_rb->isChecked());
  refrac_widget->setEnabled(iface_flat_rb->isChecked() || iface_wb_rb->isChecked());

  bool f = iface_wb_rb->isChecked();
  wb_label1->setEnabled(f);
  wb_label2->setEnabled(f);
  wb_label3->setEnabled(f);
  wind_direc_le->setEnabled(f && !azavg_cb->isChecked());
  wind_speed_le->setEnabled(f);
  azavg_cb->setEnabled(f);
}

