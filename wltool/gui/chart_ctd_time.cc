
#include "chart_ctd_time.h"
#include "fe/textdrawitem2d.h"
#include "fe/textchartnumbergen2d.h"
#include "wl/record_ctd.h"
#include "base/jsortable.h"
#include "math/datautil.h"



// Chart_CTD_Time::(constructor)

Chart_CTD_Time::Chart_CTD_Time(const JString& title_postfix, chart_type ct, bool show_key, QWidget* p) : 
  Chart_Time(p),
  ct(ct) {
  
  chart.setBackgroundColour(JColour::White);
  chart.setTickLength(4);
  chart.setMargins(5,15,12,5);
  chart.setNumbersMargin(4);
  chart.setTitleItem(new TextDrawItem2D("CTD "+dataName()+" Time"+title_postfix));

  TextDrawItem2D* ti = new TextDrawItem2D(dataName()+" "+yUnits());
  ti->setRotation(270);
  chart.yLeftAxis().setLabelItem(ti);
  chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Time (s)"));

  chart.yLeftAxis().setNumberGen(new TextChartNumberGen2D);
  chart.xBottomAxis().setNumberGen(new TextChartNumberGen2D);

  chart.yLeftAxis().setDisplayNumbers(true);
    // example range
  chart.yLeftAxis().setValueRange(0, 6);
    // no more than 10 ticks and no ticks closer than 0.0001
  chart.yLeftAxis().setTickSubdivision(0.0001, 10);
  chart.yRightAxis().setTickLength(0);
  chart.xBottomAxis().setDisplayNumbers(true);
  chart.xBottomAxis().setValueRange(0, 300);
  chart.xTopAxis().setTickLength(0);

  line.setColour(lineColour());

  chart.registerData0(line);
}



// Chart_CTD_Time::newRecordList()

void Chart_CTD_Time::newRecordList(const RecordList* rec_list) {

  line.clearData();

    // no record list anymore
  if (!rec_list) { newRecordSelected(0); return; }

  int rec_count = 0;

  JListIterator<Record> iter(*rec_list);
  Record* rec = iter.first();
  while (rec) {
    const Record_CTD* rec_ctd = dynamic_cast<const Record_CTD*>(rec);    
    if (rec_ctd && rec->isComplete() && !rec->hasProblem()) rec_count++;
    rec = iter.next();
  }

  if (rec_count==0) return;

  vector<double> x_data(rec_count);
  vector<double> y_data(rec_count);

    // a list for sorting the entries and auto-ranging
  JList<JSortableDouble> sort_list;
  sort_list.setAutoDelete(true);

  rec_count = 0;
  
  rec = iter.first();
  while (rec) {
 
    const Record_CTD* rec_ctd = dynamic_cast<const Record_CTD*>(rec);    
    if (rec_ctd && rec->isComplete() && !rec->hasProblem()) {

      x_data(rec_count) = rec->timeSinceStartMsec() / 1000.0;

      switch (ct) {
        case PRES : y_data(rec_count) = rec_ctd->pressure(); break;
        case TEMP : y_data(rec_count) = rec_ctd->temperature(); break;
        default : y_data(rec_count) = rec_ctd->salinity();
      }
 
      sort_list.append(new JSortableDouble(y_data(rec_count)));
    
      rec_count++;
    }
    rec = iter.next();
  }

    // ctd data is less noisy so fix the middle 90% to the middle 80% of the plot
  const double frac = 0.9;
  const double disp_frac = 0.8;
  double y_min;
  double y_max;
  DataUtil::autoRange(sort_list, frac, disp_frac, y_min, y_max);
    // keep depth minimum at zero
  //if (ct == PRES && y_min < 0) y_min = 0;

  if (jlog::vb(5)) jlog::ls << "ctd y_min " << y_min << " y_max " << y_max << "\n"; 

  chart.yLeftAxis().setValueRange(y_min, y_max);

  chart.xBottomAxis().setValueRange(0, x_data(rec_count-1));

  line.setData(x_data, y_data);
 
  drawToBuffer();
  update();
}

