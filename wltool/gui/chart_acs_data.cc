
#include "chart_acs_data.h"
#include "fe/textdrawitem2d.h"
#include "fe/textchartnumbergen2d.h"
#include "wl/sensor_acs.h"
#include "base/jsortable.h"
#include "math/datautil.h"



Chart_ACS_Data::Chart_ACS_Data(const JString& title_postfix, bool show_key, QWidget* p) : 
  WLChart(p) {

  chart.setBackgroundColour(JColour::White);
  chart.setTickLength(4);
  chart.setMargins(5,15,12,5);
  chart.setNumbersMargin(4);
  chart.setTitleItem(new TextDrawItem2D("ACS Spectra"+title_postfix));
 
  TextDrawItem2D* ti = new TextDrawItem2D("a or c (m-1)");
  ti->setRotation(270);
  chart.yLeftAxis().setLabelItem(ti);
  chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Wavelength (nm)"));

  chart.yLeftAxis().setNumberGen(new TextChartNumberGen2D);
  chart.xBottomAxis().setNumberGen(new TextChartNumberGen2D);

  chart.yLeftAxis().setDisplayNumbers(true);
    // example range
  chart.yLeftAxis().setValueRange(0, 6);
    // no more than 10 ticks and no ticks closer than 0.0001
  chart.yLeftAxis().setTickSubdivision(0.0001, 10);
  chart.yRightAxis().setTickLength(0);
  chart.xBottomAxis().setDisplayNumbers(true);
  chart.xBottomAxis().setValueRange(400, 750);
  chart.xTopAxis().setTickLength(0);

  a_line.setColour(JColour::Red);
  c_line.setColour(JColour::Blue);

  chart.registerData0(a_line);
  chart.registerData0(c_line);

  key = new ChartKey2D();
  key->addItem(new TextDrawItem2D("c"),JColour::Blue);
  key->addItem(new TextDrawItem2D("a"),JColour::Red);
  if (show_key) chart.setKey(key);
}



void Chart_ACS_Data::newRecordList(const RecordList* rec_list) {

    // no record list anymore
  if (!rec_list) { newRecordSelected(0); return; }

   // a list for sorting the entries and auto-ranging
  JList<JSortableDouble> sort_list;
  sort_list.setAutoDelete(true);

  JListIterator<Record> iter(*rec_list);
  Record* rec = iter.first();
  while (rec) {
 
    const Record_ACS* rec_acs = dynamic_cast<const Record_ACS*>(rec);    
    if (rec_acs && rec->isComplete() && !rec->hasProblem()) {
      
      const Sensor_ACS* sens_acs = dynamic_cast<const Sensor_ACS*>(rec_acs->sensor());
      if (sens_acs) {
      
	if (sens_acs->hasDeviceFile()) {
	  if (rec_acs->cCorrected().size() > 0) {
	    sort_list.append(new JSortableDouble(norm_max(rec_acs->cCorrected())));
	    sort_list.append(new JSortableDouble(norm_min(rec_acs->cCorrected())));
	  }
	  if (rec_acs->aCorrected().size() > 0) {
	    sort_list.append(new JSortableDouble(norm_max(rec_acs->aCorrected())));
	    sort_list.append(new JSortableDouble(norm_min(rec_acs->aCorrected())));
	  }
	}
      }
    }

    rec = iter.next();
  }

  if (sort_list.count() == 0) return;

    // gets the range of the maximum values
  const double frac = 0.8;
  const double disp_frac = 0.8;
  double y_min;
  double y_max;
  DataUtil::autoRange(sort_list, frac, disp_frac, y_min, y_max);

  chart.yLeftAxis().setValueRange(y_min, y_max);

  drawToBuffer();
  update();
}
 


// Chart_ACS_Data::setRecord()

void Chart_ACS_Data::newRecordSelected(const Record* rec) {

    // no record selected
  if (!rec) {
    a_line.clearData();
    c_line.clearData();
    chart.deleteLabelItems();
    drawToBuffer();
    update();
    return;
  }

  const Record_ACS* rec_acs = dynamic_cast<const Record_ACS*>(rec);

  if (!rec_acs || !rec->isComplete() || rec->hasProblem()) return;

  a_line.clearData();
  c_line.clearData();
  chart.deleteLabelItems();

  const Sensor_ACS* sens_acs = dynamic_cast<const Sensor_ACS*>(rec_acs->sensor());

  if (!sens_acs) return;

  if (!sens_acs->hasDeviceFile()) {
    chart.addLabelItem(new ChartLabel2D(new TextDrawItem2D("No Device File")));
  } else {
    a_line.setData(sens_acs->aBandWavelength(), rec_acs->aCorrected());
    c_line.setData(sens_acs->cBandWavelength(), rec_acs->cCorrected());
  } 

  drawToBuffer();
  update();
}

