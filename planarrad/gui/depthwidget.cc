
// depthwidget.h

#include "depthwidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>



// DepthWidget::(constructor)

DepthWidget::DepthWidget(QWidget* parent) : 
  QWidget(parent) {

  QVBoxLayout *vl = new QVBoxLayout(this);

  QWidget* w = new QWidget(this);
  vl->addWidget(w);

  QGridLayout* grid = new QGridLayout(w);
  grid->setSpacing(2);
  grid->setMargin(2);
  grid->setColStretch(3, 100);

    // depth
  QLabel* label = new QLabel("Depth: ", w);
  grid->addWidget(label, 0, 0);
  depth_le = new QLineEdit(this);
  depth_le->setMaxLength(20);
  depth_le->setFixedWidth(60);
  grid->addWidget(depth_le, 0, 1);
  label = new QLabel(" (m)", this);
  grid->addWidget(label, 0, 2);
 
    // sample interval
  label = new QLabel("Sample interval: ", w);
  grid->addWidget(label, 1, 0);
  sample_interval_le = new QLineEdit(this);
  sample_interval_le->setMaxLength(20);
  sample_interval_le->setFixedWidth(60);
  grid->addWidget(sample_interval_le, 1, 1);
  label = new QLabel(" (m)", this);
  grid->addWidget(label, 1, 2);

    // delta func distance
  label = new QLabel("Delta evaluation distance: ", w);
  grid->addWidget(label, 2, 0);
  delta_eval_interval_le = new QLineEdit(w);
  delta_eval_interval_le->setMaxLength(20);
  delta_eval_interval_le->setFixedWidth(60);
  grid->addWidget(delta_eval_interval_le, 2, 1);
  label = new QLabel(" (m)", this);
  grid->addWidget(label, 2, 2);

  vl->addStretch(100);
}



// DepthWidget::parserToGui()

void DepthWidget::parserToGui(const JString& prefix, const JParser& prm, prm_context pc) {

  JString s;

  depth_le->setText(J2Q(prm.getStringOrEmpty(prefix+"depth")));
  sample_interval_le->setText(J2Q(prm.getStringOrEmpty(prefix+"sample_point_distance")));
  delta_eval_interval_le->setText(J2Q(prm.getStringOrEmpty(prefix+"sample_point_delta_distance")));
  
}



// DephtWidget::guiToParser()

void DepthWidget::guiToParser(const JString& prefix, JParser& prm, prm_context pc) const {

  JString s;

  prm.setLabelAndValue(prefix+"depth", Q2J(depth_le->text()));
  prm.setLabelAndValue(prefix+"sample_point_distance", Q2J(sample_interval_le->text()));
  prm.setLabelAndValue(prefix+"sample_point_delta_distance", Q2J(delta_eval_interval_le->text()));
}

