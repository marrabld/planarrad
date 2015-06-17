
// mergemenu.h

#ifndef _WLTOOL_GUI_MERGEMENU_H
#define _WLTOOL_GUI_MERGEMENU_H

#include <QMenu>
#include "base/jparser.h"
#include "base/jlist.h"
#include "wltool.h"

using namespace jude::base;



class MergeMenu : public QMenu {

  Q_OBJECT

  const WLTool& wltool;

  JList<QAction> act_list;

public:

  MergeMenu(const WLTool& wltool);

  void addChannelInclude(int channel_index, bool is_ed, bool is_lu, bool is_es);

  void rebuild();

  void parserToGui(const JParser& prm);
  void guiToParser(JParser& prm) const;
 
public slots:
  void slotDoMergeTriggered();

  //void radioButtonClicked(QAbstractButton* b);
  //void acsDeviceLocatePressed();

signals:
  void signalDoMerge(int channel_index);
};



#endif // _WLTOOL_GUI_MERGEMENU_H

