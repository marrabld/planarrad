
// boundarywidget.h

#include "boundarywidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>



// BoundaryWidget::(constructor)

BoundaryWidget::BoundaryWidget(QWidget* parent) : 
  QWidget(parent) {

  QVBoxLayout *vl = new QVBoxLayout(this);

  QWidget* w = new QWidget(this);
  vl->addWidget(w);

  QGridLayout* grid = new QGridLayout(w);
  grid->setSpacing(2);
  grid->setMargin(2);
  grid->setColStretch(2,100);

  lamb_uni_rb = new QRadioButton("Lambertian uniform", w);
  button_grp.addButton(lamb_uni_rb);

  grid->addWidget(lamb_uni_rb, 0, 0);
  lamb_uni_widget = new QWidget(w);
  QHBoxLayout* hl2 = new QHBoxLayout(lamb_uni_widget);
  hl2->setContentsMargins(0,0,0,0);
  lamb_uni_le = new QLineEdit(lamb_uni_widget);
  lamb_uni_le->setFixedWidth(100);
  hl2->addWidget(lamb_uni_le);
  QLabel* label = new QLabel("(prop. 0-1)", lamb_uni_widget);
  hl2->addWidget(label);
  hl2->addStretch(100);
  grid->addWidget(lamb_uni_widget, 0, 1, 1, 3);

  lamb_file_rb = new QRadioButton("Lambertian file", w);
  button_grp.addButton(lamb_file_rb);

  grid->addWidget(lamb_file_rb, 1, 0);
  subs_diffuse_r0_sl = new SpecLoad("", w, grid, 1, 1, "Load substrate diffuse spectral reflectance");

  brdf_file_rb = new QRadioButton("BRDF file", w);
  button_grp.addButton(brdf_file_rb);
  grid->addWidget(brdf_file_rb, 2, 0);
  subs_brdf_r0_sl = new SpecLoad("", w, grid, 2, 1, "Load substrate BRDF function file");

  vl->addStretch(100);

  connect(&button_grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotUpdateEnabled()));
}



// BoundaryWidget::parserToGui()

void BoundaryWidget::parserToGui(const JString& prefix, const JParser& prm, prm_context pc) {

  JString s;

  if (prm.isDefined(prefix+"bottom_reflec_diffuse_fp")) {
    lamb_file_rb->setChecked(true);
    s = prm.getStringOrEmpty(prefix+"bottom_reflec_diffuse_fp");
    subs_diffuse_r0_sl->setSourceFilepath(s);
    s = prm.getStringOrEmpty(prefix+"bottom_reflec_diffuse_band_spec_fp");
    subs_diffuse_r0_sl->setBandSpecFilepath(s);

  } else if (prm.isDefined(prefix+"bottom_reflec_fp")) {
    brdf_file_rb->setChecked(true);
    s = prm.getStringOrEmpty(prefix+"bottom_reflec_fp");
    subs_brdf_r0_sl->setSourceFilepath(s);
    s = prm.getStringOrEmpty(prefix+"bottom_reflec_band_spec_fp");
    subs_brdf_r0_sl->setBandSpecFilepath(s);

  } else {
    lamb_uni_rb->setChecked(true);
    lamb_uni_le->setText(J2Q(prm.getStringOrEmpty(prefix+"bottom_reflec_diffuse_data")));
  }

  slotUpdateEnabled();
}



// BoundaryWidget::guiToParser()

void BoundaryWidget::guiToParser(const JString& prefix, JParser& prm,  prm_context pc) const {
 
  JString s;

  if (lamb_file_rb->isChecked()) {
    if (!(s = subs_diffuse_r0_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"bottom_reflec_diffuse_fp", s);
    if (!(s = subs_diffuse_r0_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"bottom_reflec_diffuse_band_spec_fp", s);

  } else if (brdf_file_rb->isChecked()) {
    if (!(s = subs_brdf_r0_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"bottom_reflec_fp", s);
    if (!(s = subs_brdf_r0_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"bottom_reflec_band_spec_fp", s);

    // lamb_uni_rb->isChecked()) 
  } else {
    prm.setLabelAndValue(prefix+"bottom_reflec_diffuse_data", Q2J(lamb_uni_le->text()));
  }
}



void BoundaryWidget::rebuildSpecLoads() {
  subs_diffuse_r0_sl->rebuild();
  subs_brdf_r0_sl->rebuild();
}



// BoundaryWidget::slotUpdateEnabled()

void BoundaryWidget::slotUpdateEnabled() {
  lamb_uni_widget->setEnabled(lamb_uni_rb->isChecked());
  subs_diffuse_r0_sl->setEnabled(lamb_file_rb->isChecked());
  subs_brdf_r0_sl->setEnabled(brdf_file_rb->isChecked());
}

