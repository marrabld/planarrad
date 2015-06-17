
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

  setCaption("About PlanarRad");

  QVBoxLayout* vl = new QVBoxLayout(this);
  vl->setSpacing(0);

  QLabel* label = new QLabel("PlanarRad 0.9 beta", this);
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

  label = new QLabel("This program comes with ABSOLUTELY NO WARRANTY.", this);
  vl->addWidget(label);
  vl->addSpacing(10);
  label = new QLabel("This is free software, and you are welcome to redistribute it under the terms of\nthe GPL v3 license.", this);
  vl->addWidget(label);
  vl->addSpacing(10);

  QHBoxLayout* hl = new QHBoxLayout(vl);
  label = new QLabel(this);
  label->setPixmap(QPixmap(":/resources/nerc_logo_272x96.png"));
  hl->addWidget(label);
  hl->addSpacing(10);
  label = new QLabel(this);
  label->setPixmap(QPixmap(":/resources/crtr_logo_100x96.png"));
  hl->addWidget(label);

  vl->addSpacing(10);

  label = new QLabel("Development of this software was supported by the NERC and the World Bank/GEF\nCoral Reef Targeted Research Program.", this);
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

  hl = new QHBoxLayout;
  vl->insertLayout(-1,hl);
  hl->insertStretch(100);
  hl->addWidget(ok_pb);

  connect(ok_pb, SIGNAL(pressed()), this, SLOT(accept()));
}

