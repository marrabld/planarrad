
// fileload.cc

#include "fe/fileload.h"
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>
//#include "base/jlog.h"

using namespace jude::fe;



// FileLoad::(constructor)

FileLoad::FileLoad(const JString& text, QWidget* parent, QLayout* layout, bool stretch_hbox, const JString& ct, bool is_save) :
  is_save_file(is_save),
  label(0),
  caption_text(ct) {

  w = new QWidget(parent); 
  layout->addWidget(w);

  hbox = new QHBoxLayout(w);
  hbox->setContentsMargins(0,0,0,0);
  hbox->setSpacing(2);

  if (!text.isEmpty()) {
    label = new QLabel(J2Q(text), w);
    hbox->addWidget(label);
  }

  filepath_le = new QLineEdit(w);
  filepath_le->setMaxLength(1024);
  //filepath_le->setMinimumWidth(100);
  hbox->addWidget(filepath_le);
  // hbox->addSpacing(4); 

  filepath_pb = new QPushButton("File", w);
  filepath_pb->setFixedWidth(40);
  connect(filepath_pb, SIGNAL(clicked()), this, SLOT(fileButtonClicked()));
  hbox->addWidget(filepath_pb);

  if (stretch_hbox) hbox->addStretch(100);
}
 


// FileLoad::(constructor)

FileLoad::FileLoad(const JString& text, QWidget* parent, QGridLayout* grid, int row, int start_col, const JString& ct, bool is_save) :
  is_save_file(is_save),
  label(0),
  caption_text(ct) {
 
  if (!text.isEmpty()) {
    label = new QLabel(J2Q(text), parent);
    grid->addWidget(label, row, start_col);
    start_col++;  
  }

  filepath_le = new QLineEdit(parent);
  filepath_le->setMaxLength(1024);
  //filepath_le->setMinimumWidth(100);
  grid->addWidget(filepath_le, row, start_col);

  filepath_pb = new QPushButton("File", parent);
  filepath_pb->setFixedWidth(40);
  connect(filepath_pb, SIGNAL(clicked()), this, SLOT(fileButtonClicked()));
  grid->addWidget(filepath_pb, row, start_col+1);
}



// FileLoad::setEnabled()

void FileLoad::setEnabled(bool f) {
  if (label) label->setEnabled(f);
  filepath_le->setEnabled(f);
  filepath_pb->setEnabled(f);
}



// FileLoad::filepath()

const JString FileLoad::filepath() { 
  return Q2J(filepath_le->text()); 
}



// FileLoad::setFilepath()

void FileLoad::setFilepath(const JString& fp) { 
  filepath_le->setText(J2Q(fp)); 
}



// FileLoad::actionFileButtonClicked()

void FileLoad::fileButtonClicked() {
  JString fp;
  if (!is_save_file) {
    QString qs = QFileDialog::getOpenFileName(this, J2Q(caption_text), "", "");
    fp = Q2J(qs);
  } else {
    QString qs = QFileDialog::getSaveFileName(this, J2Q(caption_text), "", "");
    fp = Q2J(qs);
  }
  setFilepath(fp);
}

