
#include "chart_hocr_time.h"
#include "fe/textdrawitem2d.h"
#include "fe/textchartnumbergen2d.h"
#include "wl/sensor_hocr.h"
#include "math/datautil.h"
#include "base/jsortable.h"



Chart_HOCR_Time::Chart_HOCR_Time(const JString& title_postfix, chart_type ct, bool show_key, QWidget* p) : 
  Chart_Time(p),
  ct(ct) {
  
  chart.setBackgroundColour(JColour::White);
  chart.setTickLength(4);
  chart.setMargins(5,15,12,5);
  chart.setNumbersMargin(4);
  chart.setTitleItem(new TextDrawItem2D("HOCR "+dataName()+" Time"+title_postfix));
 
  TextDrawItem2D* ti = new TextDrawItem2D(yUnits());
  ti->setRotation(270);
  chart.yLeftAxis().setLabelItem(ti);
  chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Time (s)"));

  chart.yLeftAxis().setNumberGen(new TextChartNumberGen2D);
  chart.xBottomAxis().setNumberGen(new TextChartNumberGen2D);

  chart.yLeftAxis().setDisplayNumbers(true);
    // example range
  chart.yLeftAxis().setValueRange(0, 2);
    // no more than 10 ticks and no ticks closer than 0.0001
  chart.yLeftAxis().setTickSubdivision(0.0001, 10);
  chart.yRightAxis().setTickLength(0);
  chart.xBottomAxis().setDisplayNumbers(true);
  chart.xBottomAxis().setValueRange(0, 300);
  chart.xTopAxis().setTickLength(0);

  line_blue.setColour(JColour::Blue);
  line_green.setColour(JColour::Green);
  line_red.setColour(JColour::Red);

  chart.registerData0(line_blue);
  chart.registerData0(line_green);
  chart.registerData0(line_red);

  key = new ChartKey2D();
  key->addItem(new TextDrawItem2D("band 50"),JColour::Blue);
  key->addItem(new TextDrawItem2D("band 100"),JColour::Green);
  key->addItem(new TextDrawItem2D("band 150"),JColour::Red);
  if (show_key) chart.setKey(key);
}



void Chart_HOCR_Time::newRecordList(const RecordList* rec_list) {

  line_blue.clearData();
  line_green.clearData();
  line_red.clearData();
  chart.deleteLabelItems();

    // no record list anymore
  if (!rec_list) { newRecordSelected(0); return; }

  int rec_count = 0;
  int processed_rec_count = 0;

  JListIterator<Record> iter(*rec_list);
  Record* rec = iter.first();
  while (rec) {

    const Record_HOCR* rec_hocr = dynamic_cast<const Record_HOCR*>(rec);    
    if (rec->isComplete() && !rec->hasProblem() && isChartType(rec_hocr)) {
      rec_count++;
      if (!isEs()) { if (rec_hocr->hasProcessedEdAndLu()) processed_rec_count++; }
      else if (rec_hocr->hasProcessedEs()) processed_rec_count++;
    }
    rec = iter.next();
  }

  if (!rec_count) return;

  if (!isRaw()) {
    rec_count = processed_rec_count;
    if (!rec_count) { chart.addLabelItem(new ChartLabel2D(new TextDrawItem2D("Not Processed"))); return; }
  }

  vector<double> x_data(rec_count);
  vector<double> blue_data(rec_count);
  vector<double> green_data(rec_count);
  vector<double> red_data(rec_count);

    // a list for sorting the entries and auto-ranging
  JList<JSortableDouble> sort_list;
  sort_list.setAutoDelete(true);

  rec_count = 0;
  
  rec = iter.first();
  while (rec) {

    const Record_HOCR* rec_hocr = dynamic_cast<const Record_HOCR*>(rec);    
    if (rec->isComplete() && !rec->hasProblem() && isChartType(rec_hocr)) {

      x_data(rec_count) = rec->timeSinceStartMsec() / 1000.0;

      double vb;
      double vg;
      double vr;

      if (isRaw()) {
	vb = rec_hocr->rawData(50) / 1000.0;
	vg = rec_hocr->rawData(100) / 1000.0;
        vr = rec_hocr->rawData(150) / 1000.0;
      } else if (isEs()) {
	vb = rec_hocr->esData()(50);
	vg = rec_hocr->esData()(100);
	vr = rec_hocr->esData()(150);
      } else if (isEd()) {
	vb = rec_hocr->edData()(50);
	vg = rec_hocr->edData()(100);
	vr = rec_hocr->edData()(150);
      } else {
	vb = rec_hocr->luData()(50);
	vg = rec_hocr->luData()(100);
	vr = rec_hocr->luData()(150);
      }

      blue_data(rec_count) = vb;
      green_data(rec_count) = vg;
      red_data(rec_count) = vr;

      sort_list.append(new JSortableDouble(vb));
      sort_list.append(new JSortableDouble(vg));
      sort_list.append(new JSortableDouble(vr));

      rec_count++;
    }
    rec = iter.next();
  }

    // fix the middle 80% to the middle 50% of the plot
  const double frac = 0.8;
  const double disp_frac = 0.5;
  double y_min;
  double y_max;
  DataUtil::autoRange(sort_list, frac, disp_frac, y_min, y_max);

  if (jlog::vb(5)) jlog::ls << "hocr time y_min " << y_min << " y_max " << y_max << "\n"; 

  chart.yLeftAxis().setValueRange(y_min, y_max);

  chart.xBottomAxis().setValueRange(0, x_data(rec_count-1));

  line_blue.setData(x_data, blue_data);
  line_green.setData(x_data, green_data);
  line_red.setData(x_data, red_data);
 
  drawToBuffer();
  update();
}

