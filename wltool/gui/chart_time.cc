
#include "chart_time.h"



// Chart_Time::(constructor)

Chart_Time::Chart_Time(QWidget* p) :
  WLChart(p) {

  time_line.setColour(JColour(128,128,128));
  chart.registerData0(time_line);
}



// Chart_Time::newRecordSelected()

void Chart_Time::newRecordSelected(const Record* rec) {

  time_line.clearData();

  if (rec) {
 
    double x = rec->timeSinceStartMsec() / 1000.0;

    vector<double> x_data_line(2);
    vector<double> y_data_line(2);

    x_data_line(0) = x;
    x_data_line(1) = x;
    y_data_line(0) = chart.yLeftAxis().minValue();
    y_data_line(1) = chart.yLeftAxis().maxValue();
    time_line.setData(x_data_line, y_data_line);

    //jlog::ls << "y min " << chart.yLeftAxis().minValue() << "\n";
  }

  drawToBuffer();
  update();
}

