
// iopwidget.h

#include "iopwidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>



// IOPWidget::(constructor)

IOPWidget::IOPWidget(QWidget* parent) : 
  QWidget(parent),
  tool_thread(phasetool, tool_prm) {

  QVBoxLayout *vl = new QVBoxLayout(this);

  QWidget* w = new QWidget(this);
  vl->addWidget(w);

  QGridLayout* grid = new QGridLayout(w);
  grid->setSpacing(2);
  grid->setMargin(2);
  grid->setColStretch(3, 100);

    // attenuation
  QLabel* label = new QLabel("Attenuation (c): ", w);
  grid->addWidget(label, 0, 0);
  atten_uni_rb = new QRadioButton("Uniform", w);
  atten_button_grp.addButton(atten_uni_rb);
  grid->addWidget(atten_uni_rb, 0, 1);
  atten_uni_widget = new QWidget(w);
  QHBoxLayout* hl2 = new QHBoxLayout(atten_uni_widget);
  hl2->setContentsMargins(0,0,0,0);
  atten_uni_le = new QLineEdit(atten_uni_widget);
  atten_uni_le->setFixedWidth(80);
  hl2->addWidget(atten_uni_le);
  label = new QLabel("(m-1)", atten_uni_widget);
  hl2->addWidget(label);
  hl2->addStretch(100);
  grid->addWidget(atten_uni_widget, 0, 2, 1, 3);
  atten_file_rb = new QRadioButton("File", w);
  atten_button_grp.addButton(atten_file_rb);
  grid->addWidget(atten_file_rb, 1, 1);
  atten_sl = new SpecLoad("", w, grid, 1, 2, "Load spectral beam attenuation coefficients");

    // absorption
  label = new QLabel("Absorption (a): ", w);
  grid->addWidget(label, 2, 0);
  absorp_uni_rb = new QRadioButton("Uniform", w);
  absorp_button_grp.addButton(absorp_uni_rb);
  grid->addWidget(absorp_uni_rb, 2, 1);
  absorp_uni_widget = new QWidget(w);
  hl2 = new QHBoxLayout(absorp_uni_widget);
  hl2->setContentsMargins(0,0,0,0);
  absorp_uni_le = new QLineEdit( absorp_uni_widget);
  absorp_uni_le->setFixedWidth(80);
  hl2->addWidget(absorp_uni_le);
  label = new QLabel("(m-1)", absorp_uni_widget);
  hl2->addWidget(label);
  hl2->addStretch(100);
  grid->addWidget(absorp_uni_widget, 2, 2, 1, 3);
  absorp_file_rb = new QRadioButton("File", w); 
  absorp_button_grp.addButton(absorp_file_rb);
  grid->addWidget(absorp_file_rb, 3, 1);
  absorp_sl = new SpecLoad("", w, grid, 3, 2, "Load spectral absorption coefficients");

  label = new QLabel("Phase function: ", w);
  grid->addWidget(label, 4, 0);
  pf_isotropic_rb = new QRadioButton("Isotropic", w);
  pf_button_grp.addButton(pf_isotropic_rb);
  grid->addWidget(pf_isotropic_rb, 4, 1);
  pf_petzold_rb = new QRadioButton("Petzold", w);
  pf_button_grp.addButton(pf_petzold_rb);
  grid->addWidget(pf_petzold_rb, 5, 1);
  pf_ff_rb = new QRadioButton("Fournier-Forand", w);
  pf_button_grp.addButton(pf_ff_rb);
  grid->addWidget(pf_ff_rb, 6, 1);

  bb_frac_widget = new QWidget(w);
  hl2 = new QHBoxLayout(bb_frac_widget);
  hl2->setContentsMargins(0,0,0,0);
  bb_frac_le = new QLineEdit(bb_frac_widget);
  bb_frac_le->setFixedWidth(80);
  hl2->addWidget(bb_frac_le);
  label = new QLabel("(bb prop.)", bb_frac_widget);
  hl2->addWidget(label);
  hl2->addStretch(100);
  grid->addWidget(bb_frac_widget, 6, 2, 1, 3);

  pf_file_rb = new QRadioButton("Load from file:", w);
  pf_button_grp.addButton(pf_file_rb);
  grid->addWidget(pf_file_rb, 7, 1);
  phase_func_fl = new FileLoad("", w, grid, 7, 2, "Load phase function file", false);

  vl->addStretch(100);

  connect(&atten_button_grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotUpdateEnabled()));
  connect(&absorp_button_grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotUpdateEnabled()));
  connect(&pf_button_grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotUpdateEnabled()));

  connect(&tool_thread, SIGNAL(processingFinished(bool)), this, SLOT(slotProcessingFinished(bool)));
}



// IOPWidget::parserToGui()

void IOPWidget::parserToGui(const JString& prefix, const JParser& prm, prm_context pc) {

  JString s;

  if (prm.isDefined(prefix+"atten_fp")) {
    atten_file_rb->setChecked(true);
    s = prm.getStringOrEmpty(prefix+"atten_fp");
    atten_sl->setSourceFilepath(s);
    s = prm.getStringOrEmpty(prefix+"atten_band_spec_fp");
    atten_sl->setBandSpecFilepath(s);
  } else {
    atten_uni_rb->setChecked(true);
    atten_uni_le->setText(J2Q(prm.getStringOrEmpty(prefix+"atten_data")));
  }

  if (prm.isDefined(prefix+"absorp_fp")) {
    absorp_file_rb->setChecked(true);
    s = prm.getStringOrEmpty(prefix+"absorp_fp");
    absorp_sl->setSourceFilepath(s);
    s = prm.getStringOrEmpty(prefix+"absorp_band_spec_fp");
    absorp_sl->setBandSpecFilepath(s);
  } else {
    absorp_uni_rb->setChecked(true);
    absorp_uni_le->setText(J2Q(prm.getStringOrEmpty(prefix+"absorp_data")));
  }

  if (!prm.isDefined(prefix+"type")) {
    pf_petzold_rb->setChecked(true);
  } else {
      // set which model is being used
    if (prm.valueCheck(prefix+"type", "isotropic")) {
      pf_isotropic_rb->setChecked(true);
    } else if (prm.valueCheck(prefix+"type", "ff")) {
      pf_ff_rb->setChecked(true);
    } else if (prm.valueCheck(prefix+"type", "fileload")) {
      pf_file_rb->setChecked(true);
      s = prm.getStringOrEmpty(prefix+"file_load_fp");
      phase_func_fl->setFilepath(s);
    } else {
      pf_petzold_rb->setChecked(true);
    }
  }
 
    // set this anyway
  s = prm.getStringOrEmpty(prefix+"backscatter_proportion_list");
  bb_frac_le->setText(J2Q(s));
 
  slotUpdateEnabled();
}



// IOPWidget::guiToParser()

void IOPWidget::guiToParser(const JString& prefix, JParser& prm, prm_context pc) const {

  JString s;

  if (pc == SLABTOOL) {
    if (pf_file_rb->isChecked() && !(s = phase_func_fl->filepath()).isEmpty()) prm.setLabelAndValue(prefix+"pf_fp", s);
    else prm.setLabelAndValue(prefix+"pf_fp", pf_fp);
  }

  if (pc != PHASETOOL) {

    if (atten_file_rb->isChecked()) {
      if (!(s = atten_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"atten_fp", s);
      if (!(s = atten_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"atten_band_spec_fp", s);
    } else {
      prm.setLabelAndValue(prefix+"atten_data", Q2J(atten_uni_le->text()));
    }
    
    if (absorp_file_rb->isChecked()) {
      if (!(s = absorp_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"absorp_fp", s);
      if (!(s = absorp_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue(prefix+"absorp_band_spec_fp", s);
    } else {
      prm.setLabelAndValue(prefix+"absorp_data", Q2J(absorp_uni_le->text()));
    }

  }

  if (pc != SLABTOOL) {

    if (pf_isotropic_rb->isChecked()) {
      prm.setLabelAndValue(prefix+"type", "isotropic");
    } else if (pf_ff_rb->isChecked()) { 
      prm.setLabelAndValue(prefix+"type", "ff");
    } else if (pf_file_rb->isChecked()) {
      prm.setLabelAndValue(prefix+"type", "fileload");  
      if (!(s = phase_func_fl->filepath()).isEmpty()) prm.setLabelAndValue(prefix+"file_load_fp", s);
    } else prm.setLabelAndValue(prefix+"type", "petzold");
    
      // remember this anyway
    prm.setLabelAndValue(prefix+"backscatter_proportion_list", Q2J(bb_frac_le->text()));
  }
}



// IOPWidget::buildPhaseFuncThread()

bool IOPWidget::buildPhaseFuncThread(const DirecSpec& ds_spec, int sub_quad_count, const JString& output_dir, bool fp_option) {

      // the file to create
  pf_fp = output_dir+"/pf_rdd_"+ds_spec.code()+"_1";

  bool is_file_load(pf_file_rb->isChecked());

  if (fp_option && !is_file_load) {
    QString qs(QFileDialog::getSaveFileName(this, "Save Phase Function File", J2Q(pf_fp)));
    pf_fp = Q2J(qs);
  }

  if (pf_fp.isEmpty()) return false;
	
  tool_prm.clear();
  ds_spec.addToParser(tool_prm, "");
  guiToParser("", tool_prm, IOPWidget::PHASETOOL);
  tool_prm.setLabelAndValue("band_count", 1);
  if (sub_quad_count > 0) tool_prm.setLabelAndValue("sub_quad_count", sub_quad_count);
  if (!is_file_load) tool_prm.setLabelAndValue("phase_func_save_fp", pf_fp);
  tool_prm.setLabelAndValue("verbose", jlog::verboseLevel());

  tool_thread.start();

  return true;
}



// IOPWidget::slotProcessingFinished()

void IOPWidget::slotProcessingFinished(bool ret_ok) {

  if (jlog::vbLevel(5)) jlog::ls << "IOPWidget::processingFinished() ok: " << ret_ok << ".\n";

  signalProcessingFinished(ret_ok);
}



// IOPWidget::slotUpdateEnabled()

void IOPWidget::slotUpdateEnabled() {
  atten_uni_widget->setEnabled(atten_uni_rb->isChecked());
  atten_sl->setEnabled(atten_file_rb->isChecked());
  absorp_uni_widget->setEnabled(absorp_uni_rb->isChecked());
  absorp_sl->setEnabled(absorp_file_rb->isChecked());
  phase_func_fl->setEnabled(pf_file_rb->isChecked());
  bb_frac_widget->setEnabled(pf_ff_rb->isChecked());
}



// IOPWidget::rebuildSpecLoads()

void IOPWidget::rebuildSpecLoads() {
  atten_sl->rebuild();
  absorp_sl->rebuild();
}

