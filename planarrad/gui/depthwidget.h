
// depthwidget.h

#ifndef _PLANARRAD_DEPTHWIDGET_H
#define _PLANARRAD_DEPTHWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QStackedWidget>
#include "base/jparser.h"

using namespace jude::base;


class DepthWidget : public QWidget {

  QLineEdit* depth_le;
  QLineEdit* sample_interval_le;
  QLineEdit* delta_eval_interval_le;

public:

  typedef enum { SAVE, SLABTOOL } prm_context;

  DepthWidget(QWidget* parent);

  void parserToGui(const JString& prefix, const JParser& prm, prm_context pc);
  void guiToParser(const JString& prefix, JParser& prm, prm_context pc) const;
};



#endif // _PLANARRAD_DEPTHWIDGET_H

