
#include "wlpanelscroll.h"
#include "chart_acs_data.h"
#include "chart_acs_time.h"
#include "chart_ecobb3_time.h"
#include "chart_hocr_time.h"
#include "chart_hocr_data.h"
#include "chart_ctd_time.h"



WLPanelScroll::WLPanelScroll(QWidget* p) :
  PanelScroll(p),
  show_key(true) { 

  setBackgroundColor(QColor(255,255,255));
}




// WLPanelScroll::createPanel() [private]

WLPanel* WLPanelScroll::createPanel(const JString& code, int channel_index) {
  JString tp(JString(" (")+JString::number(channel_index+1)+")");
  if (code == "vis_acs_spectra") return new WLPanel(code, channel_index, new Chart_ACS_Data(tp, show_key));
  if (code == "vis_acs_a_time") return new WLPanel(code, channel_index, new Chart_ACS_Time(tp, Chart_ACS_Time::A, show_key));
  if (code == "vis_acs_c_time") return new WLPanel(code, channel_index, new Chart_ACS_Time(tp, Chart_ACS_Time::C, show_key));
  if (code == "vis_ctd_pressure") return new WLPanel(code, channel_index, new Chart_CTD_Time(tp, Chart_CTD_Time::PRES, show_key));
  if (code == "vis_ctd_temperature") return new WLPanel(code, channel_index, new Chart_CTD_Time(tp, Chart_CTD_Time::TEMP, show_key));
  if (code == "vis_ctd_salinity") return new WLPanel(code, channel_index, new Chart_CTD_Time(tp, Chart_CTD_Time::SALIN, show_key));
  if (code == "vis_ecobb3_time") return new WLPanel(code, channel_index, new Chart_ECOBB3_Time(tp, show_key));
  if (code == "vis_hocr_ed_raw_time") return new WLPanel(code, channel_index, new Chart_HOCR_Time(tp, Chart_HOCR_Time::ED_RAW, show_key));
  if (code == "vis_hocr_lu_raw_time") return new WLPanel(code, channel_index, new Chart_HOCR_Time(tp, Chart_HOCR_Time::LU_RAW, show_key));
  if (code == "vis_hocr_ed_dark_time") return new WLPanel(code, channel_index, new Chart_HOCR_Time(tp, Chart_HOCR_Time::ED_DARK, show_key));
  if (code == "vis_hocr_lu_dark_time") return new WLPanel(code, channel_index, new Chart_HOCR_Time(tp, Chart_HOCR_Time::LU_DARK, show_key));
  if (code == "vis_hocr_ed_time") return new WLPanel(code, channel_index, new Chart_HOCR_Time(tp, Chart_HOCR_Time::ED, show_key));
  if (code == "vis_hocr_lu_time") return new WLPanel(code, channel_index, new Chart_HOCR_Time(tp, Chart_HOCR_Time::LU, show_key));
  if (code == "vis_hocr_ed_raw_spectra") return new WLPanel(code, channel_index, new Chart_HOCR_Data(tp, Chart_HOCR_Data::ED_RAW, show_key));
  if (code == "vis_hocr_lu_raw_spectra") return new WLPanel(code, channel_index, new Chart_HOCR_Data(tp, Chart_HOCR_Data::LU_RAW, show_key));
  if (code == "vis_hocr_ed_spectra") return new WLPanel(code, channel_index, new Chart_HOCR_Data(tp, Chart_HOCR_Data::ED, show_key));
  if (code == "vis_hocr_lu_spectra") return new WLPanel(code, channel_index, new Chart_HOCR_Data(tp, Chart_HOCR_Data::LU, show_key));
  if (code == "vis_hocr_es_time") return new WLPanel(code, channel_index, new Chart_HOCR_Time(tp, Chart_HOCR_Time::ES, show_key));
  if (code == "vis_hocr_es_spectra") return new WLPanel(code, channel_index, new Chart_HOCR_Data(tp, Chart_HOCR_Data::ES, show_key));
  return 0;
}



// WLPanelScroll::checkPanel() [private]

void WLPanelScroll::checkPanel(const JString& code, int channel_index, int& curr_panel_index, const JParser& prm) {

  jlog::ls << "check panel: " << code << "\n";

  bool want_f(prm.isDefinedAndTrueBool(code));

  WLPanel* curr_panel = dynamic_cast<WLPanel*>(panelList().at(curr_panel_index));
  bool got_f = (curr_panel) ? (curr_panel->typeText()==code && curr_panel->channelIndex()==channel_index) : false;

  jlog::ls << "got: " << got_f << " want: " << want_f << " index " << curr_panel_index << "\n";

  if (want_f && !got_f) {

    WLPanel* p = createPanel(code, channel_index);
    if (!p) return;
    insertPanel(p, curr_panel_index);
    
    //if (wltool.archiveFile()) p->newRecordList(&wltool.archiveFile()->recordList());
    //if (selected_rec) p->newRecordSelected(selected_rec);

    curr_panel_index++;

  } else if (!want_f && got_f) {

    deletePanel(curr_panel);

  } else if (want_f && got_f) curr_panel_index++;
}



// WLPanelScroll::rebuild()

void WLPanelScroll::rebuild(const SensorConfig& sc, const JParser& prm) {

  int curr_panel_index = 0;

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
  
    JString tt(sc.sensor(i).typeText());

    jlog::ls << "Sensor: " << tt << "\n";

    if (tt == "RawData") continue;

    if (tt == "ACS") {
      checkPanel("vis_acs_spectra", i, curr_panel_index, prm);
      checkPanel("vis_acs_a_time", i, curr_panel_index, prm);
      checkPanel("vis_acs_c_time", i, curr_panel_index, prm);
    
    } else if (tt == "CTD") {
      checkPanel("vis_ctd_pressure", i, curr_panel_index, prm);
      checkPanel("vis_ctd_temperature", i, curr_panel_index, prm);
      checkPanel("vis_ctd_salinity", i, curr_panel_index, prm);
     
    } else if (tt == "ECOBB3") {
      checkPanel("vis_ecobb3_time", i, curr_panel_index, prm);
    
    } else if (tt == "HOCR") {
      checkPanel("vis_hocr_ed_raw_time", i, curr_panel_index, prm);
      checkPanel("vis_hocr_ed_dark_time", i, curr_panel_index, prm);
      checkPanel("vis_hocr_ed_time", i, curr_panel_index, prm);
      checkPanel("vis_hocr_lu_raw_time", i, curr_panel_index, prm);
      checkPanel("vis_hocr_lu_dark_time", i, curr_panel_index, prm);
      checkPanel("vis_hocr_lu_time", i, curr_panel_index, prm);
      checkPanel("vis_hocr_ed_raw_spectra", i, curr_panel_index, prm);
      checkPanel("vis_hocr_ed_spectra", i, curr_panel_index, prm);
      checkPanel("vis_hocr_lu_raw_spectra", i, curr_panel_index, prm);
      checkPanel("vis_hocr_lu_spectra", i, curr_panel_index, prm);
      checkPanel("vis_hocr_es_time", i, curr_panel_index, prm);
      checkPanel("vis_hocr_es_spectra", i, curr_panel_index, prm);
    }
  }
}



// WLPanelScroll::slotNewRecordList()

void WLPanelScroll::slotNewRecordList(const RecordList* rec_list) {
  for (int i=0; i<panelCount(); i++) {
    WLPanel* p(dynamic_cast<WLPanel*>(panel(i)));
    if (p) p->slotNewRecordList(rec_list);
  }  
}



// WLPanelScroll::slotNewRecordSelected() 

void WLPanelScroll::slotNewRecordSelected(const Record* rec) {
  for (int i=0; i<panelCount(); i++) {
    WLPanel* p(dynamic_cast<WLPanel*>(panel(i)));
    if (p) p->slotNewRecordSelected(rec);
  }  
}



// WLPanelScroll::slotShowKey() 

void WLPanelScroll::slotSetShowKey(bool f) {

  show_key = f;

  for (int i=0; i<panelCount(); i++) {
    WLPanel* p(dynamic_cast<WLPanel*>(panel(i)));
    if (p) {
      p->slotSetShowKey(f);
      p->update();
    }
  }  
}

