
// aqrad
// planetool_gui.h
// (c) John Hedley 2008

#ifndef _PLANETOOL_GUI_H_JDH
#define _PLANETOOL_GUI_H_JDH

#include "config.h"
#include "jwidget.h"
#include "planetool.h"
#include "toolthread.h"
#include "samplepointdata.h"
#include "jtimer.h"
#include "radprofilegroup.h"
#include "modelradprofile.h"

class Parser;
class JComboBox;
class JWidgetStack;
class JLineEdit;
class SpecLoad;
class JCheckBox;
class JPushButton;
class JProgressBar;
class BandSpecComboBox;
class DirecSpecComboBox;
class ProfilePanel;
class DepthSwipePanel;
class FileLoad;
class JPopupMenu;
class SpectralProfileData;
class JTabWidget;
class DataSelector;

template <typename SD> class SpecImp;



// PlaneTool_Gui

class PlaneTool_Gui : public JWidget {

  PlaneTool tool;
  Parser& prm;
 
  ToolThread tool_thread;

  JTabWidget* tabs;
  JWidget* model_tab;
  JWidget* data_tab;
  DataSelector* data_selector;

  BandSpecComboBox* target_bandspec_cm;
  DirecSpecComboBox* target_direcspec_cm;

  JPopupMenu* view_barmenu;
  int view_menu_modelinput_id;
  int view_menu_profile_id;
  int view_menu_depthswipe_id;

  FileLoad* sky_file_fl;

  FileLoad* surface_r00_file_fl;
  FileLoad* surface_r11_file_fl;
  FileLoad* surface_t01_file_fl;
  FileLoad* surface_t10_file_fl;
 
  SpecLoad* atten_sl;
  SpecLoad* absorp_sl;
  FileLoad* phase_func_fl;

  JLineEdit* depth_le;
  JLineEdit* sample_interval_le;
  JLineEdit* delta_eval_interval_le;
  
  JCheckBox* subs_is_lamb_cb;
  JCheckBox* subs_is_brdf_cb;

  SpecLoad* subs_diffuse_r0_sl;
  FileLoad* subs_brdf_r0_fl;
 
  JComboBox* soln_method_cm;
  JWidgetStack* soln_method_stack;

  JLineEdit* soln_euler_steps_per_od_le;
  JLineEdit* soln_midpoint_steps_per_od_le;
  JLineEdit* soln_runga4_steps_per_od_le;
  JLineEdit* soln_runga4adap_min_steps_per_od_le;
  JLineEdit* soln_runga4adap_max_steps_per_od_le;
  JLineEdit* soln_runga4adap_min_error_le;
  JLineEdit* soln_runga4adap_max_error_le;

  JPushButton* process_pb;
  JProgressBar* progress_bar;
  JPushButton* save_params_pb;
  JPushButton* load_params_pb;

  ProfilePanel* profile_panel;
  DepthSwipePanel* depth_swipe_panel;

  //SpecPanel* spec_panel;

  JTimer progress_timer;

  RadProfileGroup rad_profile_group;
  ModelRadProfile soln_rad_profile;

  JList<SpectralProfileData> loaded_data_List;

  JString temp_dir;

  void parserToGui();
  void guiToParser();

  JWidget* buildInputOptions(JWidget* parent);
  JWidget* buildSubstrateOptions(JWidget* parent);
  JWidget* buildIntegrationOptions(JWidget* parent);

  void loadSolutionFiles(const JString dir);
  void loadDataFiles(const Parser& prm);
  void processComplete();
  void newSolution();

  enum { EULER_CODE, MIDPOINT_CODE, RUNGA4_CODE, RUNGA4ADAP_CODE };

public:

  static void slotSolnMethodActivated(void* object, int code) { ((PlaneTool_Gui*)object)->actionSolnMethodActivated(code); }

  static void slotLambertianSubstrateClicked(void* object, int) { ((PlaneTool_Gui*)object)->actionLambertianSubstrateClicked(); }
  static void slotBRDFSubstrateClicked(void* object, int) { ((PlaneTool_Gui*)object)->actionBRDFSubstrateClicked(); }

  static void slotProcessClicked(void* object, int) { ((PlaneTool_Gui*)object)->actionProcessClicked(); }
  static void slotLoadParamsClicked(void* object, int) { ((PlaneTool_Gui*)object)->actionLoadParamsClicked(); }
  static void slotSaveParamsClicked(void* object, int) { ((PlaneTool_Gui*)object)->actionSaveParamsClicked(); }

  static void slotFileMenu_LoadSolnFile(void* object, int) { ((PlaneTool_Gui*)object)->actionFileMenu_LoadSolnFile(); }
  static void slotFileMenu_SaveSolnFile(void* object, int) { ((PlaneTool_Gui*)object)->actionFileMenu_SaveSolnFile(); }
  static void slotFileMenu_LoadProfileData(void* object, int) { ((PlaneTool_Gui*)object)->actionFileMenu_LoadProfileData(); }
  static void slotFileMenu_Quit(void* object, int) { ((PlaneTool_Gui*)object)->actionFileMenu_Quit(); }

  static void slotViewMenu_ModelInput(void* object, int) { ((PlaneTool_Gui*)object)->actionViewMenu_ModelInput(); }
  static void slotViewMenu_Profile(void* object, int) { ((PlaneTool_Gui*)object)->actionViewMenu_Profile(); }
  static void slotViewMenu_DepthSwipe(void* object, int) { ((PlaneTool_Gui*)object)->actionViewMenu_DepthSwipe(); }

  static void slotDataFileListChanged(void* object, int) { ((PlaneTool_Gui*)object)->actionDataFileListChanged(); }

  static void slotUpdateProgress(void* object, int) { ((PlaneTool_Gui*)object)->actionUpdateProgress(); }

  PlaneTool_Gui(Parser& prm, JWidget* parent=0);
  ~PlaneTool_Gui();

  const JString tempDir() { return temp_dir; }

  void init();

  //  JSizePolicy sizePolicy() const;

  void actionSolnMethodActivated(int code);

  void actionLambertianSubstrateClicked();
  void actionBRDFSubstrateClicked();
 
  void actionProcessClicked();
  void actionLoadParamsClicked();
  void actionSaveParamsClicked();

  void actionFileMenu_LoadSolnFile();
  void actionFileMenu_SaveSolnFile();
  void actionFileMenu_LoadProfileData();
  void actionFileMenu_Quit();

  void actionViewMenu_ModelInput();
  void actionViewMenu_Profile();
  void actionViewMenu_DepthSwipe();

  void actionDataFileListChanged();

  void actionUpdateProgress();

protected:

  //void resizeEvent(JResizeEvent*);
  //void mousePressEvent(JMouseEvent*);
};



#endif // _PLANETOOL_GUI_H_JDH

