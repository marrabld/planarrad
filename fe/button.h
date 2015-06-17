
// button.h

#ifndef _JUDE_FE_BUTTON_H_JDH
#define _JUDE_FE_BUTTON_H_JDH

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QIcon>


namespace jude {
namespace fe {

class ButtonGroup;



class Button : public QWidget {

  Q_OBJECT

    //QPixmap pix;
  QIcon icon;

  int border;

  ButtonGroup* grp;

  bool is_togglable;

  bool down;

public:

  //Button(const QImage& im, QWidget* p=0);

  Button(const QImage& im, QWidget* p=0);
  Button(const QImage& im, ButtonGroup* g, QWidget* p);
 
  Button(const QPixmap& im, QWidget* p=0);
  Button(const QPixmap& im, ButtonGroup* g, QWidget* p=0);

  Button(const QPixmap& im, int border, QWidget* p=0);
  Button(const QPixmap& im, int border, ButtonGroup* g, QWidget* p=0);

  void setIsTogglable(bool f) { is_togglable = f; }
  bool isTogglable() const { return is_togglable; }

  void setDown(bool f);
  bool isDown() const { return down; }

  void paintEvent(QPaintEvent*); 
  void mousePressEvent(QMouseEvent*);
 
signals:

  void clicked(Button*);
};



} } // namespace jude::fe

#endif // _BUTTON_H_JDH



