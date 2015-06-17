
// basepanel.h

#ifndef _JUDE_FE_BASEPANEL_H
#define _JUDE_FE_BASEPANEL_H

#include <QWidget>

namespace jude {
namespace fe {



class BasePanel : public QWidget {

  double aspect_ratio;
  int fixed_additional_height;
 
public:

  BasePanel(QWidget* p=0) :
    QWidget(p),
    aspect_ratio(1),
    fixed_additional_height(0) { }

  double aspectRatio() { return aspect_ratio; }
  void setAspectRatio(double d) { aspect_ratio = d; }

  int fixedAdditionalHeight() { return fixed_additional_height; }
  void setFixedAdditionalHeight(int h) { fixed_additional_height = h; }

    //public slots:

  //void buttonsChanged();
  //void sensitivityChanged(int v);
};



} } // namespace jude::fe

#endif // _JUDE_FE_BASEPANEL_H

