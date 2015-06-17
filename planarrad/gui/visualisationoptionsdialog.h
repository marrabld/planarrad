
// visualisationoptionsdialog.h

#ifndef _PLANARRAD_VISUALISATIONOPTIONSDIALOG_H
#define _PLANARRAD_VISUALISATIONOPTIONSDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QButtonGroup>
#include <QSettings>
#include "base/jlog.h"
#include "base/jparser.h"

using namespace jude::base;




class VisualisationOptionsDialog : public QDialog {

  Q_OBJECT
 
  QButtonGroup button_grp;

  QRadioButton* auto_depth_range_rb;
  QRadioButton* fixed_depth_range_rb;

  QLineEdit* fixed_max_depth_le;
  QLabel* fixed_max_depth_label;

public:

  typedef enum { SAVE, SLABTOOL } prm_context;

  VisualisationOptionsDialog(QWidget* p=0);

  void saveSettings(const JString& prefix, QSettings& settings) const;
  void restoreSettings(const JString& prefix, const QSettings& settings);

  void parserToGui(const JString& prefix, const JParser& prm, prm_context pc);
  void guiToParser(const JString& prefix, JParser& prm, prm_context pc) const;

  bool isFixedDepthRange() const { return fixed_depth_range_rb->isChecked(); }
  double fixedMaxDepth() const { return Q2J(fixed_max_depth_le->text()).toDouble(); }

public slots:

  void slotUpdateEnabled(); 
};



#endif // _PLANARRAD_VISUALISATIONOPTIONSDIALOG_H

