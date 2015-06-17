
#include "mergemenu.h"
#include "wltool.h"



// MergeMenu::(constructor)

MergeMenu::MergeMenu(const WLTool& wltool) :
  QMenu("Merge"),
  wltool(wltool) { }



void MergeMenu::addChannelInclude(int channel_index, bool is_ed, bool is_lu, bool is_es) {

  JString s("Include Channel "+JString::number(channel_index+1));
  if (is_ed) s += "/1 (HOCR Ed)";
  else if (is_lu) s += "/2 (HOCR Lu)";
  else if (is_es) s += "/3 (HOCR Es)";
  else s += " (" + wltool.sensorConfig().sensor(channel_index).sensorType() + ")";
  
  QAction* act = new QAction(tr(J2Q(s)), this);
  act->setStatusTip(tr("Include this data in the merged file"));
  //act->setData(channel_index);
  act->setCheckable(true);
  act->setChecked(true);
  addAction(act);
  act_list.append(act);
}



// MergeMenu::rebuild()

void MergeMenu::rebuild() {
 
  clear();
  act_list.clear();
 
  if (!wltool.archiveFile()) { setEnabled(false); return; }

  bool enab = false;

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (wltool.archiveFile()->channel(i).recordCount() > 0 && wltool.sensorConfig().sensor(i).sensorType() != "RawData") {

      if (wltool.sensorConfig().sensor(i).sensorType() == "HOCR") {
	addChannelInclude(i, true, false, false);
	addChannelInclude(i, false, true, false);
	addChannelInclude(i, false, false, true);
      } else {
	addChannelInclude(i, false, false, false);
      }

      enab = true;
    }
  }

  setEnabled(enab);  
  if (!enab) return;

  addSeparator();

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (wltool.archiveFile()->channel(i).recordCount() > 0 && wltool.sensorConfig().sensor(i).sensorType() != "RawData") {

      JString s("Do Merge On Channel "+JString::number(i+1)+" (" + wltool.sensorConfig().sensor(i).sensorType() + ")");
      
      QAction* act = new QAction(tr(J2Q(s)), this);
      act->setStatusTip(tr("Perform merge using this channel as the reference"));
      act->setData(i);
      //connect(act, SIGNAL(triggered()), this, SLOT(slotDoMerge()));
      connect(act, SIGNAL(triggered()), this, SLOT(slotDoMergeTriggered()));
      addAction(act);
      enab = true;
    }
  }
}



void MergeMenu::guiToParser(JParser& prm) const {

  JListIterator<QAction> iter(act_list);
  QAction* curr_act = iter.first();

  JString inc_str;

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (wltool.archiveFile()->channel(i).recordCount() > 0 && wltool.sensorConfig().sensor(i).sensorType() != "RawData") {

      bool f = false;

      if (wltool.sensorConfig().sensor(i).sensorType() == "HOCR") {

	JString label_base("chan"+JString::number(i+1)+"_hocr_l2s");

	if (curr_act) {
	  prm.setLabelAndValue(label_base+"_ed_output", curr_act->isChecked() ? "yes" : "no");
	  if (curr_act->isChecked()) f = true;
	}
	curr_act = iter.next();
	if (curr_act) {
	  prm.setLabelAndValue(label_base+"_lu_output", curr_act->isChecked() ? "yes" : "no");
	  if (curr_act->isChecked()) f = true;
	}
	curr_act = iter.next();
	if (curr_act) {
	  prm.setLabelAndValue(label_base+"_es_output", curr_act->isChecked() ? "yes" : "no");
	  if (curr_act->isChecked()) f = true;
	}

      } else {
	if (curr_act->isChecked()) f = true;
	curr_act = iter.next();
      }

      if (f) {
	if (!inc_str.isEmpty()) inc_str += ",";
	inc_str += JString::number(i+1);
      }
    }
  }

  prm.setLabelAndValue("merge_file_channels", inc_str);
}
 


void MergeMenu::slotDoMergeTriggered() {

  if (jlog::vb(5)) jlog::ls << "slotDoMergeTriggered()\n";

  QAction* act(dynamic_cast<QAction*>(QObject::sender()));
  if (!act) return;

  emit signalDoMerge(act->data().toInt());
}

