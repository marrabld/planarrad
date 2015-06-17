
#include "chart_ecobb3_time.h"
#include "fe/textdrawitem2d.h"
#include "fe/textchartnumbergen2d.h"
#include "wl/sensor_ecobb3.h"
#include "base/jsortable.h"
#include "math/datautil.h"



Chart_ECOBB3_Time::Chart_ECOBB3_Time(const JString& title_postfix, bool show_key, QWidget* p) : 
  Chart_Time(p) {
  
  chart.setBackgroundColour(JColour::White);
  chart.setTickLength(4);
  chart.setMargins(5,15,12,5);
  chart.setNumbersMargin(4);
  chart.setTitleItem(new TextDrawItem2D("ECOBB3 Time"+title_postfix));
 
  TextDrawItem2D* ti = new TextDrawItem2D("Counts x 1000");
  ti->setRotation(270);
  chart.yLeftAxis().setLabelItem(ti);
  chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Time (s)"));

  chart.yLeftAxis().setNumberGen(new TextChartNumberGen2D);
  chart.xBottomAxis().setNumberGen(new TextChartNumberGen2D);

  chart.yLeftAxis().setDisplayNumbers(true);
    // example range
  chart.yLeftAxis().setValueRange(0, 2);
    // no more than 10 ticks and no ticks closer than 0.0001 (tenth of a count)
  chart.yLeftAxis().setTickSubdivision(0.0001, 10);
  chart.yRightAxis().setTickLength(0);
  chart.xBottomAxis().setDisplayNumbers(true);
  chart.xBottomAxis().setValueRange(0, 300);
  chart.xTopAxis().setTickLength(0);

  JColour cb(110,160,190);
  JColour cg(90,180,90);
  JColour cr(230,110,90);

  line_470.setColour(cb);
  line_530.setColour(cg);
  line_650.setColour(cr);

  chart.registerData0(line_470);
  chart.registerData0(line_530);
  chart.registerData0(line_650);

  key = new ChartKey2D();
  key->addItem(new TextDrawItem2D("470 nm"),cb);
  key->addItem(new TextDrawItem2D("530 nm"),cg);
  key->addItem(new TextDrawItem2D("650 nm"),cr);
  if (show_key) chart.setKey(key);
}



void Chart_ECOBB3_Time::newRecordList(const RecordList* rec_list) {

  line_470.clearData();
  line_530.clearData();
  line_650.clearData();

    // no record list anymore
  if (!rec_list) { newRecordSelected(0); return; }

  int rec_count = 0;

  JListIterator<Record> iter(*rec_list);
  Record* rec = iter.first();
  while (rec) {

    const Record_ECOBB3* rec_ecobb3 = dynamic_cast<const Record_ECOBB3*>(rec);    
    if (rec_ecobb3 && rec->isComplete() && !rec->hasProblem()) rec_count++;

    rec = iter.next();
  }

  if (!rec_count) return;

  vector<double> x_data(rec_count);
  vector<double> y470_data(rec_count);
  vector<double> y530_data(rec_count);
  vector<double> y650_data(rec_count);

    // a list for sorting the entries and auto-ranging
  JList<JSortableDouble> sort_list;
  sort_list.setAutoDelete(true);

  rec_count = 0;
  
  rec = iter.first();
  while (rec) {

    const Record_ECOBB3* rec_ecobb3 = dynamic_cast<const Record_ECOBB3*>(rec);    
    if (rec_ecobb3 && rec->isComplete() && !rec->hasProblem()) {

      double v470 = rec_ecobb3->sig470() / 1000.0;
      double v530 = rec_ecobb3->sig530() / 1000.0;
      double v650 = rec_ecobb3->sig650() / 1000.0;

      x_data(rec_count) = rec->timeSinceStartMsec() / 1000.0;
      y470_data(rec_count) = v470;
      y530_data(rec_count) = v530;
      y650_data(rec_count) = v650;

      sort_list.append(new JSortableDouble(v470));
      sort_list.append(new JSortableDouble(v530));
      sort_list.append(new JSortableDouble(v650));

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

  if (jlog::vb(5)) jlog::ls << "ecobb3 y_min " << y_min << " y_max " << y_max << "\n"; 

  chart.yLeftAxis().setValueRange(y_min, y_max);

  chart.xBottomAxis().setValueRange(0, x_data(rec_count-1));

  line_470.setData(x_data, y470_data);
  line_530.setData(x_data, y530_data);
  line_650.setData(x_data, y650_data);

  drawToBuffer();
  update();
}

