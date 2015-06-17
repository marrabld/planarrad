
// aboutdialog.cc

#include "aboutdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QTextBrowser>




AboutDialog::AboutDialog(QWidget* p) : 
  QDialog(p) {

  setCaption("About WLTool");

  QVBoxLayout* vl = new QVBoxLayout(this);
  vl->setSpacing(0);

  QLabel* label = new QLabel("WLTool v. 1.1", this);
  label->setFont(QFont("arial", 14, QFont::Bold));
  vl->addWidget(label);
  vl->addSpacing(10);

  QFrame* w = new QFrame(this);
  w->setFrameStyle(QFrame::HLine | QFrame::Plain); 
  vl->addWidget(w);
  vl->addSpacing(10);

  label = new QLabel("Copyright (C) 2010 John Hedley", this);
  vl->addWidget(label);
  vl->addSpacing(10);

  QString s("This program comes with ABSOLUTELY NO WARRANTY.\n");
  s.append("This is free software, and you are welcome to redistribute it\nunder the terms of the GPL v3 license.");
  label = new QLabel(s, this);
  vl->addWidget(label);
  vl->addSpacing(10);

  label = new QLabel(this);
  label->setPixmap(QPixmap(":/resources/fsf_logo_390x96.png"));
  vl->addWidget(label);
  vl->addSpacing(10);

  label = new QLabel("Development of this software was supported by the UK's\nNERC Field Spectroscopy Facility.", this);
  vl->addWidget(label);
  vl->addSpacing(10);

  w = new QFrame(this);
  w->setFrameStyle(QFrame::HLine | QFrame::Plain); 
  vl->addWidget(w);
  vl->addSpacing(10);

  label = new QLabel("Distribution Licence Terms:", this);
  vl->addWidget(label);
  vl->addSpacing(10);

  QTextBrowser* tb = new QTextBrowser(this);
  vl->addWidget(tb);
  tb->setReadOnly(true);
  tb->setAcceptRichText(false);
  tb->setPlainText("");
  tb->setSource(QUrl("qrc:/../../resources/gpl_v3.html"));
  vl->addSpacing(10);

  QPushButton* ok_pb = new QPushButton(tr("OK"));

  QHBoxLayout* hl = new QHBoxLayout;
  vl->insertLayout(-1,hl);
  hl->insertStretch(100);
  hl->addWidget(ok_pb);

  connect(ok_pb, SIGNAL(pressed()), this, SLOT(accept()));
}

