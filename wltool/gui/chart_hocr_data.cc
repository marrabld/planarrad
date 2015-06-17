
#include "chart_hocr_data.h"
#include "fe/textdrawitem2d.h"
#include "fe/textchartnumbergen2d.h"
#include "wl/sensor_hocr.h"
#include "base/jsortable.h"
#include "math/datautil.h"



Chart_HOCR_Data::Chart_HOCR_Data(const JString& title_postfix, chart_type ct, bool show_key, QWidget* p) : 
  WLChart(p),
  ct(ct) {

  chart.setBackgroundColour(JColour::White);
  chart.setTickLength(4);
  chart.setMargins(5,15,12,5);
  chart.setNumbersMargin(4);
  chart.setTitleItem(new TextDrawItem2D("HOCR "+dataName()+" Spectra"+title_postfix));
 
  TextDrawItem2D* ti = new TextDrawItem2D(yUnits());
  ti->setRotation(270);
  chart.yLeftAxis().setLabelItem(ti);

  if (isRaw()) chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Band Index"));
  else chart.xBottomAxis().setLabelItem(new TextDrawItem2D("Wavelength (nm)"));

  chart.yLeftAxis().setNumberGen(new TextChartNumberGen2D);
  chart.xBottomAxis().setNumberGen(new TextChartNumberGen2D);

  chart.yLeftAxis().setDisplayNumbers(true);
    // example range
  chart.yLeftAxis().setValueRange(0, 6);
    // no more than 10 ticks and no ticks closer than 0.0001
  chart.yLeftAxis().setTickSubdivision(0.0001, 10);
  chart.yRightAxis().setTickLength(0);
  chart.xBottomAxis().setDisplayNumbers(true);
 
  if (isRaw()) chart.xBottomAxis().setValueRange(0, 200);
  else chart.xBottomAxis().setValueRange(350, 950);

  chart.xTopAxis().setTickLength(0);

  if (isEd()) data_line.setColour(JColour::Blue);
  else data_line.setColour(JColour::Red);

  chart.registerData0(data_line);
}



void Chart_HOCR_Data::newRecordList(const RecordList* rec_list) {

    // no record list anymore
  if (!rec_list) { newRecordSelected(0); return; }

    // a list for sorting the entries and auto-ranging
  JList<JSortableDouble> sort_list;
  sort_list.setAutoDelete(true);

  JListIterator<Record> iter(*rec_list);
  Record* rec = iter.first();
  while (rec) {
 
    const Record_HOCR* rec_hocr = dynamic_cast<const Record_HOCR*>(rec);    
    if (rec->isComplete() && !rec->hasProblem() && isChartType(rec_hocr)) {
      
      double max_v = 0;
      double min_v = 0;

      if (isRaw()) {
	max_v = rec_hocr->rawData(0);
	min_v = max_v;
	for (int i=1; i<rec_hocr->rawDataBandCount(); i++) { 
	  double v = rec_hocr->rawData(i);
	  if (v > max_v) max_v = v; 
	  if (v < min_v) min_v = v; 
	}
	max_v /= 1000; 
	min_v /= 1000; 

	sort_list.append(new JSortableDouble(max_v));
	sort_list.append(new JSortableDouble(min_v));

      } else if (isEs()) {

	if (rec_hocr->hasProcessedEs()) { 

	  max_v = norm_max(rec_hocr->esData());
	  min_v = norm_min(rec_hocr->esData());

	  sort_list.append(new JSortableDouble(max_v));
	  sort_list.append(new JSortableDouble(min_v));
	}

      } else {

	if (rec_hocr->hasProcessedEdAndLu()) { 
	  if (isEd()) { 
	    max_v = norm_max(rec_hocr->edData());
	    min_v = norm_min(rec_hocr->edData());
	  } else {
	    max_v = norm_max(rec_hocr->luData());
	    min_v = norm_min(rec_hocr->luData());
	  }
	  sort_list.append(new JSortableDouble(max_v));
	  sort_list.append(new JSortableDouble(min_v));
	}
      }
    }

    rec = iter.next();
  }

  if (sort_list.count() == 0) return;

    // gets the range of the values
  const double frac = 0.8;
  const double disp_frac = 0.6;
  double y_min;
  double y_max;
  DataUtil::autoRange(sort_list, frac, disp_frac, y_min, y_max);

  chart.yLeftAxis().setValueRange(y_min, y_max);

  drawToBuffer();
  update();
}
 


// Chart_HOCR_Data::setRecord()

void Chart_HOCR_Data::newRecordSelected(const Record* rec) {

    // no record selected
  if (!rec) {
    data_line.clearData();
    chart.deleteLabelItems();
    drawToBuffer();
    update();
    return;
  }

  const Record_HOCR* rec_hocr = dynamic_cast<const Record_HOCR*>(rec);
  if (!isChartType(rec_hocr) || !rec->isComplete() || rec->hasProblem()) return;

  if (!isRaw() && ((!rec_hocr->hasProcessedEdAndLu() && !isEs()) || (!rec_hocr->hasProcessedEs() && isEs()))) {
    chart.addLabelItem(new ChartLabel2D(new TextDrawItem2D("Not Processed")));
    return;
  }

  data_line.clearData();
  chart.deleteLabelItems();

  if (isRaw()) {

    vector<double> x_data(rec_hocr->rawDataBandCount());
    vector<double> y_data(rec_hocr->rawDataBandCount());

    for (int i=0; i<rec_hocr->rawDataBandCount(); i++) { 
      x_data(i) = i;
      y_data(i) = rec_hocr->rawData(i) / 1000.0;
    }
    data_line.setData(x_data, y_data);

  } else {
 
    const Sensor_HOCR* sens_hocr = dynamic_cast<const Sensor_HOCR*>(rec_hocr->sensor());
    if (!sens_hocr) return;

    if (isEs()) data_line.setData(sens_hocr->EsBandWavelength(), rec_hocr->esData());
    else if (isEd()) data_line.setData(sens_hocr->EdBandWavelength(), rec_hocr->edData());
    else data_line.setData(sens_hocr->LuBandWavelength(), rec_hocr->luData());
  } 

  drawToBuffer();
  update();
}

