
// boundarywidget.h

#ifndef _PLANARRAD_BOUNDARYWIDGET_H
#define _PLANARRAD_BOUNDARYWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QStackedWidget>
#include "base/jstring.h"
#include "base/jparser.h"
#include "fe/specload.h"

using namespace jude::base;
using namespace jude::fe;



class BoundaryWidget : public QWidget {

  Q_OBJECT

  QButtonGroup button_grp;

  QRadioButton* lamb_uni_rb;
  QRadioButton* lamb_file_rb;
  QRadioButton* brdf_file_rb;

  QWidget* lamb_uni_widget;
  QLineEdit* lamb_uni_le;

  SpecLoad* subs_diffuse_r0_sl;
  SpecLoad* subs_brdf_r0_sl;

  JString tempDir() { return ""; }

public:

  typedef enum { SAVE, SLABTOOL } prm_context;

  BoundaryWidget(QWidget* parent);

  void parserToGui(const JString& prefix, const JParser& prm, prm_context pc);
  void guiToParser(const JString& prefix, JParser& prm, prm_context pc) const;

  void rebuildSpecLoads();

public slots:

  void slotUpdateEnabled();
};



#endif // _PLANARRAD_BOUNDARYWIDGET_H

