
#include "chart_acs_time.h"
#include "fe/textdrawitem2d.h"
#include "fe/textchartnumbergen2d.h"
#include "wl/sensor_acs.h"
#include "math/ipol.h"
#include "math/datautil.h"
#include "base/jsortable.h"

using namespace jude::math;



Chart_ACS_Time::Chart_ACS_Time(const JString& title_postfix, chart_type ct, bool show_key, QWidget* p) : 
  Chart_Time(p),
  ct(ct) {
  
  chart.setBackgroundColour(JColour::White);
  chart.setTickLength(4);
  chart.setMargins(5,15,12,5);
  chart.setNumbersMargin(4);
  chart.setTitleItem(new TextDrawItem2D("ACS "+dataName()+" Time"+title_postfix));

  TextDrawItem2D* ti = new TextDrawItem2D(dataName()+" (m-1)");
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

  line_470.setColour(JColour::Blue);
  line_530.setColour(JColour::Green);
  line_650.setColour(JColour::Red);

  chart.registerData0(line_470);
  chart.registerData0(line_530);
  chart.registerData0(line_650);

  key = new ChartKey2D();
  key->addItem(new TextDrawItem2D(dataName()+" 470 nm"),JColour::Blue);
  key->addItem(new TextDrawItem2D(dataName()+" 530 nm"),JColour::Green);
  key->addItem(new TextDrawItem2D(dataName()+" 650 nm"),JColour::Red);
  if (show_key) chart.setKey(key);
}



// Chart_ACS_Time::newRecordList()

void Chart_ACS_Time::newRecordList(const RecordList* rec_list) {

  line_470.clearData();
  line_530.clearData();
  line_650.clearData();
  chart.deleteLabelItems();

    // no record list anymore
  if (!rec_list) { newRecordSelected(0); return; }

  int rec_count = 0;

  const Record_ACS* first_rec_acs(0);

  JListIterator<Record> iter(*rec_list);
  Record* rec = iter.first();
  while (rec) {
    const Record_ACS* rec_acs = dynamic_cast<const Record_ACS*>(rec);    
    if (rec_acs && rec->isComplete() && !rec->hasProblem()) {
      rec_count++;
      if (first_rec_acs == 0) first_rec_acs = rec_acs;
    }
    rec = iter.next();
  }

  if (!first_rec_acs) return;

  const Sensor_ACS* sens_acs = dynamic_cast<const Sensor_ACS*>(first_rec_acs->sensor());
  if (!sens_acs) return;

  if (!sens_acs->hasDeviceFile()) {
    chart.addLabelItem(new ChartLabel2D(new TextDrawItem2D("No Device File")));
    drawToBuffer();
    update();
    return;
  }

  vector<double> x_data(rec_count);
  vector<double> c470_data(rec_count);
  vector<double> c530_data(rec_count);
  vector<double> c650_data(rec_count);

  int band_c470 = Ipol::closestIndex(sens_acs->cBandWavelength(), 470);
  int band_c530 = Ipol::closestIndex(sens_acs->cBandWavelength(), 530);
  int band_c650 = Ipol::closestIndex(sens_acs->cBandWavelength(), 650);

  if (jlog::vb(5)) {
    jlog::ls << "band_c470: " << band_c470 << "\n";
    jlog::ls << "band_c530: " << band_c530 << "\n";
    jlog::ls << "band_c650: " << band_c650 << "\n";
  }

    // a list for sorting the entries and auto-ranging
  JList<JSortableDouble> sort_list;
  sort_list.setAutoDelete(true);

  rec_count = 0;
  
  rec = iter.first();
  while (rec) {

    const Record_ACS* rec_acs = dynamic_cast<const Record_ACS*>(rec);    
    if (rec_acs && rec->isComplete() && !rec->hasProblem()) {

      const vector<double>& v(ct==A ? rec_acs->aCorrected() : rec_acs->cCorrected());

      x_data(rec_count) = rec->timeSinceStartMsec() / 1000.0;

      double v470((band_c470 > (int)v.size()) ? 0 : v(band_c470));
      double v530((band_c530 > (int)v.size()) ? 0 : v(band_c530));
      double v650((band_c650 > (int)v.size()) ? 0 : v(band_c650));

      c470_data(rec_count) = v470;
      c530_data(rec_count) = v530;
      c650_data(rec_count) = v650;

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

  if (jlog::vb(5)) jlog::ls << "acs time y_min " << y_min << " y_max " << y_max << "\n"; 

  chart.yLeftAxis().setValueRange(y_min, y_max);

  chart.xBottomAxis().setValueRange(0, x_data(rec_count-1));

  line_470.setData(x_data, c470_data);
  line_530.setData(x_data, c530_data);
  line_650.setData(x_data, c650_data);

  drawToBuffer();
  update();
}

