
// additionaloptionsdialog.cc

#include "additionaloptionsdialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QFileDialog>
#include "base/jlog.h"
#include "fe/tool_gui.h"

using namespace jude::fe;



AdditionalOptionsDialog::AdditionalOptionsDialog(QWidget* p) : 
  QDialog(p) {

  setCaption("Additional Options");

  QLabel* label;
  QGridLayout* grid;
  QGroupBox *gb;

  QVBoxLayout* vl_top = new QVBoxLayout(this);

  gb = new QGroupBox(tr("Spherical Integration Sub-Quad Counts"));
  vl_top->addWidget(gb);
  grid = new QGridLayout(gb);
  grid->setSpacing(4);
  grid->setMargin(8);
  grid->setColStretch(3, 100);

  label = new QLabel("Input Radiance");
  grid->addWidget(label, 0, 0);
  sky_subquad_le = new QLineEdit(this);
  sky_subquad_le->setMaxLength(40);
  sky_subquad_le->setFixedWidth(100);
  grid->addWidget(sky_subquad_le, 0, 1);
  label = new QLabel("(default 1E6)");
  grid->addWidget(label, 0, 2);

  label = new QLabel("Interface");
  grid->addWidget(label, 1, 0);
  iface_subquad_le = new QLineEdit(this);
  iface_subquad_le->setMaxLength(40);
  iface_subquad_le->setFixedWidth(100);
  grid->addWidget(iface_subquad_le, 1, 1);
  label = new QLabel("(default 1E6)");
  grid->addWidget(label, 1, 2);
 
  label = new QLabel("Phase function");
  grid->addWidget(label, 2, 0);
  pf_subquad_le = new QLineEdit(this);
  pf_subquad_le->setMaxLength(40);
  pf_subquad_le->setFixedWidth(100);
  grid->addWidget(pf_subquad_le, 2, 1);
  label = new QLabel("(default 1E4)");
  grid->addWidget(label, 2, 2);

  gb = new QGroupBox(tr("Vertical Integration Method"));
  vl_top->addWidget(gb);
  grid = new QGridLayout(gb);
  grid->setSpacing(4);
  grid->setMargin(8);
  grid->setColStretch(3, 100);

  label = new QLabel("Steps per optical depth");
  grid->addWidget(label, 0, 2);
 
  euler_rb = new QRadioButton(tr("Euler"));
  button_grp.addButton(euler_rb);
  grid->addWidget(euler_rb, 1, 0);
  euler_steps_le = new QLineEdit();
  grid->addWidget(euler_steps_le, 1, 2);

  midpoint_rb = new QRadioButton(tr("Mid-point"));
  button_grp.addButton(midpoint_rb);
  grid->addWidget(midpoint_rb, 2, 0);
  midpoint_steps_le = new QLineEdit();
  grid->addWidget(midpoint_steps_le, 2, 2);

  runga4_rb = new QRadioButton(tr("Runga4"));
  button_grp.addButton(runga4_rb);
  grid->addWidget(runga4_rb, 3, 0);
  runga4_steps_le = new QLineEdit();
  grid->addWidget(runga4_steps_le, 3, 2);

  runga4adap_rb = new QRadioButton(tr("Runga4 Adaptive Step Size"));
  button_grp.addButton(runga4adap_rb);
  grid->addWidget(runga4adap_rb, 4, 0, 1, 3);

  runga4adap_min_steps_label = new QLabel("min. steps");
  grid->addWidget(runga4adap_min_steps_label, 5, 0, 1, 2, Qt::AlignRight);
  runga4adap_min_steps_le = new QLineEdit();
  grid->addWidget(runga4adap_min_steps_le, 5, 2);

  runga4adap_max_steps_label = new QLabel("max. steps");
  grid->addWidget(runga4adap_max_steps_label, 6, 0, 1, 2, Qt::AlignRight);
  runga4adap_max_steps_le = new QLineEdit();
  grid->addWidget(runga4adap_max_steps_le, 6, 2);

  runga4adap_min_err_label = new QLabel("min. error");
  grid->addWidget(runga4adap_min_err_label, 7, 0, 1, 2, Qt::AlignRight);
  runga4adap_min_err_le = new QLineEdit();
  grid->addWidget(runga4adap_min_err_le, 7, 2);

  runga4adap_max_err_label = new QLabel("max. error");
  grid->addWidget(runga4adap_max_err_label, 8, 0, 1, 2, Qt::AlignRight);
  runga4adap_max_err_le = new QLineEdit();
  grid->addWidget(runga4adap_max_err_le, 8, 2);

  gb = new QGroupBox(tr("Output Options"));
  vl_top->addWidget(gb);
  grid = new QGridLayout(gb);
  grid->setSpacing(4);
  grid->setMargin(8);
  grid->setColStretch(3, 100);

  save_images_cb = new QCheckBox("Save Images");
  grid->addWidget(save_images_cb, 0, 0); 

  auto_sens_rb = new QRadioButton("Automatic Sensitivity");
  grid->addWidget(auto_sens_rb, 1, 0); 
  spec_sens_rb = new QRadioButton("Specifiy Sensitivity:");
  grid->addWidget(spec_sens_rb, 2, 0); 
  spec_sens_le = new QLineEdit;
  grid->addWidget(spec_sens_le, 2, 1); 

  QPushButton* cancel_pb = new QPushButton(tr("Cancel"));
  QPushButton* ok_pb = new QPushButton(tr("OK"));

  QHBoxLayout* hl = new QHBoxLayout;
  vl_top->insertLayout(-1,hl);
  hl->insertStretch(100);
  hl->addWidget(cancel_pb);
  hl->addWidget(ok_pb);

  connect(cancel_pb, SIGNAL(pressed()), this, SLOT(reject()));
  connect(ok_pb, SIGNAL(pressed()), this, SLOT(accept()));

  connect(save_images_cb, SIGNAL(clicked()), this, SLOT(slotUpdateEnabled()));
  connect(&button_grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotUpdateEnabled()));
}



// AdditionalOptionsDialog::saveSettings()

void AdditionalOptionsDialog::saveSettings(const JString& prefix, QSettings& settings) const {

    // will collect parameters for this widget
  JParser prm;
    // add sky widget parameters
  guiToParser(prefix, prm, SAVE);
    // save to settings - uses the prefix here as well so the parser doesn't read unnecessary parameters
  Tool_Gui::parserAllToSettings(prm, prefix+"params", settings);
    // log for debugging
  if (jlog::vb(5)) prm.displayList(jlog::ls);
}



// AdditionalOptionsDialog::restoreSettings()

void AdditionalOptionsDialog::restoreSettings(const JString& prefix, const QSettings& settings) {

    // parameters for this widget - uses the prefix here as well so the parser doesn't read unnecessary parameters
  JParser prm;
  Tool_Gui::settingsToParserAll(prm, prefix+"params", settings);
    // set parameters from the parser
  parserToGui(prefix, prm, SAVE);
     // log for debugging
  if (jlog::vb(5)) prm.displayList(jlog::ls);
}



// AdditionalOptionsDialog::parserToGui()

void AdditionalOptionsDialog::parserToGui(const JString& prefix, const JParser& prm, prm_context pc) {

  JString s = prm.getStringOrEmpty(prefix+"integrator");

  if (s == "euler") {
    euler_rb->setChecked(true);
  } else if (s == "midpoint") {
    midpoint_rb->setChecked(true);
  } else if (s == "runga4_adap") {
    runga4adap_rb->setChecked(true);
  } else {
    runga4_rb->setChecked(true);
  }

  if (pc == SAVE) {
    s = prm.getStringOrEmpty(prefix+"sky_sub_quad_count");
    sky_subquad_le->setText(s.isEmpty() ? "1E6" : J2Q(s));
    s = prm.getStringOrEmpty(prefix+"iface_sub_quad_count");
    iface_subquad_le->setText(s.isEmpty() ? "1E6" : J2Q(s));
    s = prm.getStringOrEmpty(prefix+"pf_sub_quad_count");
    pf_subquad_le->setText(s.isEmpty() ? "1E4" : J2Q(s));
  }

  s = prm.getStringOrEmpty(prefix+"euler_steps_per_optical_depth");
  euler_steps_le->setText(s.isEmpty() ? "100" : J2Q(s));

  s = prm.getStringOrEmpty(prefix+"midpoint_steps_per_optical_depth");
  midpoint_steps_le->setText(s.isEmpty() ? "50" : J2Q(s));

  s = prm.getStringOrEmpty(prefix+"runga4_steps_per_optical_depth");
  runga4_steps_le->setText(s.isEmpty() ? "20" : J2Q(s));

  s = prm.getStringOrEmpty(prefix+"runga4adap_min_steps_per_optical_depth");
  runga4adap_min_steps_le->setText(s.isEmpty() ? "5" : J2Q(s));
  s = prm.getStringOrEmpty(prefix+"runga4adap_max_steps_per_optical_depth");
  runga4adap_max_steps_le->setText(s.isEmpty() ? "40" : J2Q(s));

  s = prm.getStringOrEmpty(prefix+"runga4adap_min_error");
  runga4adap_min_err_le->setText(s.isEmpty() ? "0.01" : J2Q(s));
  s = prm.getStringOrEmpty(prefix+"runga4adap_max_error");
  runga4adap_max_err_le->setText(s.isEmpty() ? "0.1" : J2Q(s));
  
  if (pc == SAVE) {
    save_images_cb->setChecked(prm.isDefinedAndTrueBool("save_images"));
    JString s = prm.getStringOrEmpty(prefix+"image_sens_k");
    if (!s.isEmpty()) {
      spec_sens_rb->setChecked(true);
      spec_sens_le->setText(J2Q(s));
    } else {
      auto_sens_rb->setChecked(true);
    }
  }

  slotUpdateEnabled();
}



// AdditionalOptionsDialog::guiToParser()

void AdditionalOptionsDialog::guiToParser(const JString& prefix, JParser& prm, prm_context pc) const {

  JString s;

  if (pc == SAVE) {
    s = sky_subquad_le->text();
    prm.setLabelAndValue(prefix+"sky_sub_quad_count", Q2J(s));
    s = iface_subquad_le->text();
    prm.setLabelAndValue(prefix+"iface_sub_quad_count", Q2J(s));
    s = pf_subquad_le->text();
    prm.setLabelAndValue(prefix+"pf_sub_quad_count", Q2J(s));
  }

  if (euler_rb->isChecked()) {
    prm.setLabelAndValue(prefix+"integrator", "euler");
  } else if (midpoint_rb->isChecked()) {
    prm.setLabelAndValue(prefix+"integrator", "midpoint");
  } else if (runga4_rb->isChecked()) {
    prm.setLabelAndValue(prefix+"integrator", "runga4");
  } else if (runga4adap_rb->isChecked()) {
    prm.setLabelAndValue("integrator", "runga4_adap");
  }

  s = euler_steps_le->text();
  prm.setLabelAndValue(prefix+"euler_steps_per_optical_depth", Q2J(s));

  s = midpoint_steps_le->text();
  prm.setLabelAndValue(prefix+"midpoint_steps_per_optical_depth", Q2J(s));

  s = runga4_steps_le->text();
  prm.setLabelAndValue(prefix+"runga4_steps_per_optical_depth", Q2J(s));

  s = runga4adap_min_steps_le->text();
  prm.setLabelAndValue(prefix+"runga4adap_min_steps_per_optical_depth", Q2J(s));
  s = runga4adap_max_steps_le->text();
  prm.setLabelAndValue(prefix+"runga4adap_max_steps_per_optical_depth", Q2J(s));
  s = runga4adap_min_err_le->text();
  prm.setLabelAndValue(prefix+"runga4adap_min_error", Q2J(s));
  s = runga4adap_max_err_le->text();
  prm.setLabelAndValue(prefix+"runga4adap_max_error", Q2J(s));

  if (pc == SAVE) {
    if (save_images_cb->isChecked()) prm.setLabelAndValue(prefix+"save_images", "yes");
    if (spec_sens_rb->isChecked()) {
      s = spec_sens_le->text();
      prm.setLabelAndValue(prefix+"image_sens_k", Q2J(s));
    } 
  }
}



// AdditionalOptionsDialog::slotUpdateEnabled()

void AdditionalOptionsDialog::slotUpdateEnabled() {

  euler_steps_le->setEnabled(euler_rb->isChecked());
  midpoint_steps_le->setEnabled(midpoint_rb->isChecked());
  runga4_steps_le->setEnabled(runga4_rb->isChecked());
 
  bool f = runga4adap_rb->isChecked();

  runga4adap_min_err_label->setEnabled(f);
  runga4adap_min_err_le->setEnabled(f);
  runga4adap_max_err_label->setEnabled(f);
  runga4adap_max_err_le->setEnabled(f);
  runga4adap_min_steps_label->setEnabled(f);
  runga4adap_min_steps_le->setEnabled(f);
  runga4adap_max_steps_label->setEnabled(f);
  runga4adap_max_steps_le->setEnabled(f);

  f = save_images_cb->isChecked();
  spec_sens_le->setEnabled(f);
  spec_sens_rb->setEnabled(f);
  auto_sens_rb->setEnabled(f);
}


