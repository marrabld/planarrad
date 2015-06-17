
// specpanel.cc

#include "fe/specpanel.h"
//#include "fe/specview.h"
//#include "fe/specplot.h"
#include "fe/hsdview.h"
#include "fe/specpanelbar.h"
//#include <qvbox.h>
//#include <qhbox.h>
#include <qslider.h>
#include <qlabel.h>
//#include "S2/trans2d.h"
//#include "base/jimage.h"
#include "widgetstack.h"
#include <qlayout.h>

//#include "guidefaults.h"
//#include "runopts.h"

using namespace jude::fe;



// SpecPanel::(constructor)

SpecPanel::SpecPanel(QWidget *parent) :
  BasePanel(parent), 
  spec_proj(0),
  spec_view(0),
  spec_plot(0),
  auto_set_sens(true),
  upward_spec(0),
  downward_spec(0) {

  QVBoxLayout* vbox = new QVBoxLayout(this); 
  vbox->setSpacing(4);

  display_stk = new QWidgetStack(this);
  vbox->addWidget(display_stk);

  spec_proj = new HSDView(display_stk);
  //spec_view = new SpecView(display_stk); 
  //spec_plot = new SpecPlot(display_stk);

  ////spec_view->setFixedSize(GuiDefaults::sensorPanelContentSize(), GuiDefaults::sensorPanelContentSize());

  display_stk->addWidget(spec_proj, 1);
  //display_stk->addWidget(spec_view, 2);
  //display_stk->addWidget(spec_plot, 3);

  //display_stk->raiseWidget(spec_proj);

  bar = new SpecPanelBar(this);

  vbox->addWidget(bar);
  bar->setDrawDown(true);
  bar->setDrawProj(true);
  //bar->signalButtonsChanged.connect(this, slotButtonsChanged);
 
  QHBoxLayout* hbox = new QHBoxLayout(vbox);
  QLabel* label = new QLabel("Sens:", this);
  hbox->addWidget(label);

  sensitivity_slider = new QSlider(Qt::Horizontal, this);
  sensitivity_slider->setMinValue(0);
  sensitivity_slider->setMaxValue(1000);
  connect(sensitivity_slider, SIGNAL(valueChanged(int)), this, SLOT(sensitivityChanged(int)));
  //sensitivity_slider->signalValueChanged.connect(this, slotSensitivityChanged); 
  sensitivity_slider->setFixedHeight(16);
  hbox->addWidget(sensitivity_slider);
}


void SpecPanel::setBandSpec(BandSpec* bs) {
  spec_proj->setBandSpec(bs);
}


// SpecPanel::setProjSquare()

void SpecPanel::setProjSquare(int sz) {
  if (spec_proj) spec_proj->setFixedSize(sz, sz);
}
 


// SpecPanel::setUpwardSpec()

void SpecPanel::setUpwardSpec(const RadDirecImp<SBandData>* s, double max_E) {
  upward_spec = s;
  max_Eu = max_E;
  rebuild();
}



// SpecPanel::setDownwardSpec()

void SpecPanel::setDownwardSpec(const RadDirecImp<SBandData>* s, double max_E) {
  downward_spec = s;
  max_Ed = max_E;
  rebuild();
}



// SpecPanel::setTitleDepth()

void SpecPanel::setTitleDepth(double d) {
  title_depth = d;
}



// SpecPanel::clearAdditionalData()

void SpecPanel::clear() {
  //spec_plot->clear();
}



// SpecPanel::addAdditionalData()

void SpecPanel::addAdditionalData(const SBandData& sd) {
  //spec_plot->addSpectralData(sd, JColour(128,128,128));
}



// SpecPanel::rebuild()

void SpecPanel::rebuild() {

  //spec_plot->clear();

  if (bar->drawDown()) {
    spec_proj->setSourceData(downward_spec);
    //spec_view->setSpec(downward_spec);
    //spec_plot->setTitle(true, title_depth);
    //spec_plot->setYAxisAnchor(bar->axisAnchor(), max_Ed);
    //spec_plot->addSpec(downward_spec, JColour::Red);
  } else {
    spec_proj->setSourceData(upward_spec);
    //spec_view->setSpec(upward_spec);
    //spec_plot->setTitle(false, title_depth);
    //spec_plot->setYAxisAnchor(bar->axisAnchor(), max_Eu);
    //spec_plot->addSpec(upward_spec, JColour::Red);
  }
  
    // make sure right widget is shown
  if (bar->drawProj()) display_stk->raiseWidget(spec_proj);
  //else if (bar->drawView()) display_stk->raiseWidget(spec_view);
  // else if (bar->drawChart()) display_stk->raiseWidget(spec_plot);

  double sens_k = spec_proj->autoSetSensK();
  sensitivity_slider->setValue((int)(10 * sens_k));
  spec_proj->repaint(false);

  //spec_plot->drawToBuffer();
  //spec_plot->repaint(false);
}



// SpecPanel::resizeEvent()
 
//void SpecPanel::resizeEvent(JResizeEvent* e) { 
  //updateBufferImage();
//}



// SpecPanel::autoSetSensitivity()

void SpecPanel::autoSetSensitivity() {
  //spec_view->autoSetSensitivity();
  //sensitivity_slider->setValue((int)(10 * camera_view->sensitivity()));
    // will have just called actionSensitivityChanged() do need to set auot flag again
  //auto_set_sens = true;
}



// SpecPanel::actionButtonsChanged()

void SpecPanel::buttonsChanged() {
  rebuild();
}



// SpecPanel::updateBufferImage()

//void SpecPanel::updateBufferImage() {

//  if (auto_set_sens) autoSetSensitivity();

//  spec_view->repaint(false);
//}



// SpecPanel::actionSensitivityChanged()

void SpecPanel::sensitivityChanged(int v) {
  double s = ((double)v)/1000;
  //printf("sens_k = %f\n",s);
  spec_proj->setRelativeSensK(s);
 
  auto_set_sens = false;

  spec_proj->repaint(false);
 
  //update();
}



// SpecPanel::update()

//void SpecPanel::update() {
//  updateBufferImage();
//}

