
// additoonaloptionsdialog.h

#ifndef _PLANARRAD_ADDITIONALOPTIONSDIALOG_H
#define _PLANARRAD_ADDITIONALOPTIONSDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QButtonGroup>
#include <QSettings>
#include "base/jparser.h"

using namespace jude::base;




class AdditionalOptionsDialog : public QDialog {

  Q_OBJECT

  QLineEdit* sky_subquad_le;
  QLineEdit* iface_subquad_le;
  QLineEdit* pf_subquad_le;
 
  QButtonGroup button_grp;

  QRadioButton* euler_rb;
  QRadioButton* midpoint_rb;
  QRadioButton* runga4_rb;
  QRadioButton* runga4adap_rb;

  QLineEdit* euler_steps_le;
  QLineEdit* midpoint_steps_le;
  QLineEdit* runga4_steps_le;

  QLabel* runga4adap_min_err_label;
  QLabel* runga4adap_max_err_label;
  QLineEdit* runga4adap_min_err_le;
  QLineEdit* runga4adap_max_err_le;

  QLabel* runga4adap_min_steps_label;
  QLabel* runga4adap_max_steps_label;
  QLineEdit* runga4adap_min_steps_le;
  QLineEdit* runga4adap_max_steps_le;

  QCheckBox* save_images_cb;
  QRadioButton* auto_sens_rb;
  QRadioButton* spec_sens_rb;
  QLineEdit* spec_sens_le;
 
public:

  typedef enum { SAVE, SLABTOOL } prm_context;

  AdditionalOptionsDialog(QWidget* p=0);

  void saveSettings(const JString& prefix, QSettings& settings) const;
  void restoreSettings(const JString& prefix, const QSettings& settings);

  void parserToGui(const JString& prefix, const JParser& prm, prm_context pc);
  void guiToParser(const JString& prefix, JParser& prm, prm_context pc) const;

  int skySubQuadCount() const { return Q2J(sky_subquad_le->text()).toInt(); }
  int ifaceSubQuadCount() const { return Q2J(iface_subquad_le->text()).toInt(); }
  int pfSubQuadCount() const { return Q2J(pf_subquad_le->text()).toInt(); }

  bool saveImages() const { return save_images_cb->isChecked(); }
  bool saveImagesAutoSensK() const { return auto_sens_rb->isChecked(); }
  double saveImagesSensK() const { return Q2J(spec_sens_le->text()).toDouble(); }

public slots:

  void slotUpdateEnabled(); 
};



#endif // _PLANARRAD_ADDITIONALOPTIONSDIALOG_H

