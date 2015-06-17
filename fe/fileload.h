
// fileload.h

#ifndef _JUDE_FE_FILELOAD_H
#define _JUDE_FE_FILELOAD_H

#include <qwidget.h>
#include <qlabel.h>
#include "base/jstring.h"

class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QGridLayout;
class QHBoxLayout;

using namespace jude::base;

namespace jude {
namespace fe {



class FileLoad : public QWidget {

  Q_OBJECT

  bool is_save_file;

  QLabel* label;
  QLineEdit* filepath_le;
  QPushButton* filepath_pb;

  JString caption_text;

protected:

  QWidget* w; 
  QHBoxLayout *hbox;

public:

  FileLoad(const JString& text, QWidget* parent, QLayout* layout, bool stretch_hbox, const JString& ct, bool is_save);
  FileLoad(const JString& text, QWidget* parent, QGridLayout* grid, int row, int start_col, const JString& ct, bool is_save);
 
  int labelWidth() const { return label->width(); }
  void setLabelFixedWidth(int w) { label->setFixedWidth(w); }

  const JString filepath(); 
  void setFilepath(const JString& fp);

  void setEnabled(bool f);

public slots:

  void fileButtonClicked();
};



} } // namespace jude::fe

#endif // _JUDE_FE_FILELOAD_H

