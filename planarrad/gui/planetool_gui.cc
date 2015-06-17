
#include "planetool_gui.h"
#include "parser.h"
#include "jcombobox.h"
#include "jmenubar.h"
#include "jpopupmenu.h"
#include "jvboxlayout.h"
#include "jhboxlayout.h"
#include "jlabel.h"
#include "jwidgetstack.h"
#include "jgridlayout.h"
#include "jlineedit.h"
#include "jcheckbox.h"
#include "jpushbutton.h"
#include "fileload.h"
#include "specload.h"
#include "jprogressbar.h"
#include "jfiledialog.h"
#include "jgroupbox.h"
#include "bandspeccombobox.h"
#include "direcspeccombobox.h"
#include "specpanel.h"
#include "globalfuncs.h"
#include "bandspec.h"
#include "profilepanel.h"
#include "depthswipepanel.h"
#include "spectralprofiledata.h"
#include "jtabwidget.h"
#include "dataselector.h"
#include "log.h"
#include "jlog.h"

static bool fit_1024x768 = true;



PlaneTool_Gui::PlaneTool_Gui(Parser& prm, JWidget* parent) :
  JWidget(parent),
  prm(prm),
  tool_thread(tool, PlaneTool_Gui::prm) {

  JLog::init();

  setCaption("PlanarRad");

  temp_dir = prm.isDefined("temp_dir") ? prm.getString("temp_dir") : "/space/aqrad_files/temp";

  JLabel* label;

  JVBoxLayout *top_vbox = new JVBoxLayout(this);
 
  JPopupMenu* file_barmenu = new JPopupMenu(this);
  file_barmenu->insertItem("Load Solution Files", this, slotFileMenu_LoadSolnFile);
  file_barmenu->insertItem("Save Solution Files", this, slotFileMenu_SaveSolnFile);
  file_barmenu->insertItem("Load Profile Data", this, slotFileMenu_LoadProfileData);
  file_barmenu->insertItem("Quit", this, slotFileMenu_Quit);
 
  view_barmenu = new JPopupMenu(this);
  view_menu_modelinput_id = view_barmenu->insertItem("Model Input", this, slotViewMenu_ModelInput);
  view_menu_profile_id = view_barmenu->insertItem("Profile", this, slotViewMenu_Profile);
  view_menu_depthswipe_id = view_barmenu->insertItem("Depth Swipe", this, slotViewMenu_DepthSwipe);

  if (!fit_1024x768) {
    view_barmenu->setItemChecked(view_menu_modelinput_id, true);
    view_barmenu->setItemChecked(view_menu_profile_id, true);
    view_barmenu->setItemChecked(view_menu_depthswipe_id, true);
  } else {
    view_barmenu->setItemChecked(view_menu_modelinput_id, true);
    view_barmenu->setItemChecked(view_menu_profile_id, true);
    view_barmenu->setItemChecked(view_menu_depthswipe_id, false);
  }

  JMenuBar* mb = new JMenuBar(this);
  mb->insertItem( "&File", file_barmenu);
  mb->insertItem( "&View", view_barmenu);
  top_vbox->addWidget(mb); 

  //top_vbox->addSpacing(34);
  top_vbox->addSpacing(4);

  JHBoxLayout *top_hbox = new JHBoxLayout(top_vbox);
  top_hbox->setSpacing(4);
  top_hbox->addSpacing(4);

  tabs = new JTabWidget(this);
  top_hbox->addWidget(tabs);

    // create model and data tabs
  model_tab = new JWidget(tabs);
  tabs->insertTab(model_tab,"Model");
  data_tab = new JWidget(tabs);
  tabs->insertTab(data_tab,"Data");

    // model tab margin and spacing
  JHBoxLayout *tab_hbox = new JHBoxLayout(model_tab);
  tab_hbox->addSpacing(10);
  JVBoxLayout *tab_vbox = new JVBoxLayout(tab_hbox);
  tab_vbox->addSpacing(10);
  tab_vbox->setSpacing(20);

  JHBoxLayout *hbox = new JHBoxLayout(tab_vbox);
  hbox->setSpacing(4);

  label = new JLabel("Bands: ",model_tab);
  hbox->addWidget(label);
  target_bandspec_cm = new BandSpecComboBox(false, model_tab);
  hbox->addWidget(target_bandspec_cm);
  hbox->addSpacing(20);
  label = new JLabel("Directional Structure: ",model_tab);
  hbox->addWidget(label);
  target_direcspec_cm = new DirecSpecComboBox(false, model_tab);
  hbox->addWidget(target_direcspec_cm);
  hbox->addStretch(100);
 
  tab_vbox->addWidget(buildInputOptions(model_tab));
  tab_vbox->addWidget(buildIntegrationOptions(model_tab));

    // space between options and bottom row
  tab_vbox->addStretch(100);

    // bottom row
  hbox = new JHBoxLayout(tab_vbox);
  hbox->setSpacing(4);

  process_pb = new JPushButton("Process", model_tab);
  process_pb->signalClicked.connect(this, slotProcessClicked);
  hbox->addWidget(process_pb);
  progress_bar = new JProgressBar(model_tab);
  progress_bar->setTotalSteps(100);
  hbox->addWidget(progress_bar);
  load_params_pb = new JPushButton(" Load Params ", model_tab);
  load_params_pb->signalClicked.connect(this, slotLoadParamsClicked);
  hbox->addWidget(load_params_pb);
  save_params_pb = new JPushButton(" Save Params ", model_tab);
  save_params_pb->signalClicked.connect(this, slotSaveParamsClicked);
  hbox->addWidget(save_params_pb);

    // model tab final margin
  tab_vbox->addSpacing(10);
  tab_hbox->addSpacing(10);

    // data tab margin
  tab_hbox = new JHBoxLayout(data_tab);
  tab_hbox->addSpacing(10);
  tab_vbox = new JVBoxLayout(tab_hbox);
  tab_vbox->addSpacing(10);
    // main widget 
  data_selector = new DataSelector(data_tab);
  tab_vbox->addWidget(data_selector);
  data_selector->signalFileListChanged.connect(this, slotDataFileListChanged);
    // margin again
  tab_vbox->addSpacing(10);
  tab_hbox->addSpacing(10);

  // stuff to the right hand side

  profile_panel = new ProfilePanel(this);
  profile_panel->setMinimumWidth(260);
  top_hbox->addWidget(profile_panel);

  top_hbox->addSpacing(10);
  depth_swipe_panel = new DepthSwipePanel(this);
  top_hbox->addWidget(depth_swipe_panel);
  top_hbox->addSpacing(4);

  depth_swipe_panel->signalNewDepth.connect(profile_panel, ProfilePanel::slotNewDepth);

  if (fit_1024x768) {
    //profile_panel->hide();
    depth_swipe_panel->hide();
  }

  progress_timer.signalTimeout.connect(this, slotUpdateProgress);

  parserToGui();

    // the solution radiance profile
  rad_profile_group.addRadProfile(soln_rad_profile);
  rad_profile_group.addRadProfile(data_selector->radProfile());

  if (prm.isDefined("load_data1_filepath")) loadDataFiles(prm);

  if (prm.isDefined("load_soln_files")) loadSolutionFiles(prm.getString("load_soln_files"));


  //setMinimumSize(400,400);
}



// PlaneTool_Gui::(destructor)

PlaneTool_Gui::~PlaneTool_Gui() {
  // nothing to do
}



// PlaneTool_Gui::sizePolicy()

//JSizePolicy PlaneTool_Gui::sizePolicy() const {
//  return JSizePolicy(JSizePolicy::Minimum, JSizePolicy::Minimum);
//}



// PlaneTool_Gui::parserToGui()

void PlaneTool_Gui::parserToGui() {

  JString s;
 
    // target band spec
  s = prm.getStringOrEmpty("band_spec");
  target_bandspec_cm->setCurrentBandSpecFromFilepath(s);

    // target direc spec
  s = prm.getStringOrEmpty("direc_spec");
  target_direcspec_cm->setCurrentDirecSpecFromFilepath(s);

    // sky file
  sky_file_fl->setFilepath(prm.getStringOrEmpty("sky"));

    // water surface files
  surface_r00_file_fl->setFilepath(prm.getStringOrEmpty("surf_r0"));
  surface_r11_file_fl->setFilepath(prm.getStringOrEmpty("surf_r1"));
  surface_t01_file_fl->setFilepath(prm.getStringOrEmpty("surf_t01"));
  surface_t10_file_fl->setFilepath(prm.getStringOrEmpty("surf_t10"));

    // total and diffuse irradiance files
  atten_sl->setSourceFilepath(prm.getStringOrEmpty("atten_srcfile"));
  absorp_sl->setSourceFilepath(prm.getStringOrEmpty("absorp_srcfile"));
  atten_sl->setBandSpecFilepath(prm.getStringOrEmpty("atten_bandspec"));
  absorp_sl->setBandSpecFilepath(prm.getStringOrEmpty("absorp_bandspec"));

    // phase function file
  phase_func_fl->setFilepath(prm.getStringOrEmpty("phase_func"));

    // set depth or default
  depth_le->setText(prm.isDefined("depth") ? prm.getString("depth") : "5");
    // set sample steps or default
  sample_interval_le->setText(prm.isDefined("dft_sample_distance") ? prm.getString("dft_sample_distance") : "1");
    // set sample steps or default
  delta_eval_interval_le->setText(prm.isDefined("dft_sample_delta_distance") ? prm.getString("dft_sample_delta_distance") : "0.01");
 
    // diffuse substrate reflectance
  subs_diffuse_r0_sl->setSourceFilepath(prm.getStringOrEmpty("subs_diffuse_r0_srcfile"));
  subs_diffuse_r0_sl->setBandSpecFilepath(prm.getStringOrEmpty("subs_diffuse_r0_bandspec"));
    // brdf substrate reflectance
  subs_brdf_r0_fl->setFilepath(prm.getStringOrEmpty("subs_brdf_r0"));
    // which one to use 
  if (prm.isDefinedAndTrueBool("subs_is_brdf")) actionBRDFSubstrateClicked();
  else actionLambertianSubstrateClicked();
 
    // default solution method if none specified
  if (!prm.isDefined("integ_soln")) prm.setLabelAndValue("integ_soln","runga4");
    // solution method
  if (prm.valueCheck("integ_soln", "euler")) {
    soln_method_cm->setCurrentItem(EULER_CODE);
    soln_method_stack->raiseWidget(EULER_CODE);
  } else if (prm.valueCheck("integ_soln", "midpoint")) {
    soln_method_cm->setCurrentItem(MIDPOINT_CODE);
    soln_method_stack->raiseWidget(MIDPOINT_CODE);
  } else if (prm.valueCheck("integ_soln", "runga4")) {
    soln_method_cm->setCurrentItem(RUNGA4_CODE);
    soln_method_stack->raiseWidget(RUNGA4_CODE);
  } else {
    soln_method_cm->setCurrentItem(RUNGA4ADAP_CODE);
    soln_method_stack->raiseWidget(RUNGA4ADAP_CODE);
  }

  soln_euler_steps_per_od_le->setText(prm.isDefined("dft_euler_steps_per_optical_depth") ? prm.getString("dft_euler_steps_per_optical_depth") : "250");
  soln_midpoint_steps_per_od_le->setText(prm.isDefined("dft_midpoint_steps_per_optical_depth") ? prm.getString("dft_midpoint_steps_per_optical_depth") : "100");
  soln_runga4_steps_per_od_le->setText(prm.isDefined("dft_runga4_steps_per_optical_depth") ? prm.getString("dft_runga4_steps_per_optical_depth") : "20");

  soln_runga4adap_min_steps_per_od_le->setText(prm.isDefined("dft_runga4adap_min_steps_per_optical_depth") ? prm.getString("dft_runga4adap_min_steps_per_optical_depth") : "5");
  soln_runga4adap_max_steps_per_od_le->setText(prm.isDefined("dft_runga4adap_max_steps_per_optical_depth") ? prm.getString("dft_runga4adap_max_steps_per_optical_depth") : "40");
  soln_runga4adap_min_error_le->setText(prm.isDefined("dft_runga4adap_min_error") ? prm.getString("dft_runga4adap_min_error") : "0.01");
  soln_runga4adap_max_error_le->setText(prm.isDefined("dft_runga4adap_max_error") ? prm.getString("dft_runga4adap_max_error") : "0.05");

}



// PlaneTool_Gui::guiToParser()

void PlaneTool_Gui::guiToParser() {

  JString s;

  prm.clear();

  BandSpec& bs = *target_bandspec_cm->currentBandSpec();
 
    // band_spec and direc_spec 
  if (!(s = target_bandspec_cm->currentBandSpecFilepath()).isEmpty()) prm.setLabelAndValue("band_spec", s);
  if (!(s = target_direcspec_cm->currentDirecSpecFilepath()).isEmpty()) prm.setLabelAndValue("direc_spec", s);
 
    // sky file
  if (!(s = sky_file_fl->filepath()).isEmpty()) prm.setLabelAndValue("sky", s);

    // water surface functions
  if (!(s = surface_r00_file_fl->filepath()).isEmpty()) prm.setLabelAndValue("surf_r0", s);
  if (!(s = surface_r11_file_fl->filepath()).isEmpty()) prm.setLabelAndValue("surf_r1", s);
  if (!(s = surface_t01_file_fl->filepath()).isEmpty()) prm.setLabelAndValue("surf_t01", s);
  if (!(s = surface_t10_file_fl->filepath()).isEmpty()) prm.setLabelAndValue("surf_t10", s);

  JString atten_fp;
  JString absorp_fp;

    // attenuation and scattering
  if (!(s = atten_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue("atten_srcfile", s);
  if (!(s = absorp_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue("absorp_srcfile", s);
  if (!(s = atten_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue("atten_bandspec", s);
  if (!(s = absorp_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue("absorp_bandspec", s);
  if (!(atten_fp = atten_sl->compatibleFilepath(bs)).isEmpty()) prm.setLabelAndValue("atten", atten_fp);
  if (!(absorp_fp = absorp_sl->compatibleFilepath(bs)).isEmpty()) prm.setLabelAndValue("absorp", absorp_fp);

    // build scattering
  if (!atten_fp.isEmpty() && !absorp_fp.isEmpty()) {
    Vector atten_vec;
    atten_vec.readFromTextFile(atten_fp);
    Vector absorp_vec;
    absorp_vec.readFromTextFile(absorp_fp);
    Vector scat_vec;
    scat_vec.subtract(atten_vec, absorp_vec);
    scat_vec.display();
      // write to temporary file
    JString scat_fp(tempDir()+"/resampled_scat" + "_" + bs.code());
    Log::stream() << "Writing temporary file: " << scat_fp << "\n";
    scat_vec.writeToTextFile(scat_fp); 
    prm.setLabelAndValue("scat",scat_fp);
  }

    // phase func file
  if (!(s = phase_func_fl->filepath()).isEmpty()) prm.setLabelAndValue("phase_func", s);

    // depth
  if (!(s = depth_le->text()).isEmpty()) {
    prm.setLabelAndValue("depth", s);
    prm.setLabelAndValue("total_thickness", s);
  }

    // sample interval
  if (!(s = sample_interval_le->text()).isEmpty()) prm.setLabelAndValue("dft_sample_distance", s);
  else prm.setLabelAndValue("dft_sample_distance", "1");

    // 1 cm delta distance for evaluating kd at each smaple point
  if (!(s = delta_eval_interval_le->text()).isEmpty()) prm.setLabelAndValue("dft_sample_delta_distance",s);
  else prm.setLabelAndValue("dft_sample_delta_distance","0.1");

    // which type of substrate reflectance
  prm.setLabelAndValue("subs_is_brdf", subs_is_brdf_cb->isChecked() ? "true" : "false"); 
    // diffuse substrate reflectance
  if (!(s = subs_diffuse_r0_sl->sourceFilepath()).isEmpty()) prm.setLabelAndValue("subs_diffuse_r0_srcfile", s);
  if (!(s = subs_diffuse_r0_sl->bandSpecFilepath()).isEmpty()) prm.setLabelAndValue("subs_diffuse_r0_bandspec", s);
  if (!(s = subs_diffuse_r0_sl->compatibleFilepath(bs)).isEmpty()) prm.setLabelAndValue("subs_diffuse_r0", s);
    // brdf substrate reflectance
  if (!(s = subs_brdf_r0_fl->filepath()).isEmpty()) prm.setLabelAndValue("subs_brdf_r0", s);

    // solution method 
  int integ_soln = soln_method_cm->currentItem();

  if (integ_soln == EULER_CODE) {
    prm.setLabelAndValue("integ_soln", "euler");

  } else if (integ_soln == MIDPOINT_CODE) {
    prm.setLabelAndValue("integ_soln", "midpoint");

  } else if (integ_soln == RUNGA4_CODE) {
    prm.setLabelAndValue("integ_soln", "runga4");

  } else if (integ_soln == RUNGA4ADAP_CODE) {
    prm.setLabelAndValue("integ_soln", "runga4adap");
  }

  if (!(s = soln_euler_steps_per_od_le->text()).isEmpty()) prm.setLabelAndValue("dft_euler_steps_per_optical_depth", s);
  else prm.setLabelAndValue("dft_euler_steps_per_optical_depth", "250");
 
  if (!(s = soln_midpoint_steps_per_od_le->text()).isEmpty()) prm.setLabelAndValue("dft_midpoint_steps_per_optical_depth", s);
  else prm.setLabelAndValue("dft_midpoint_steps_per_optical_depth", "100");
 
  if (!(s = soln_runga4_steps_per_od_le->text()).isEmpty()) prm.setLabelAndValue("dft_runga4_steps_per_optical_depth", s);
  else prm.setLabelAndValue("dft_runga4_steps_per_optical_depth", "20");
 
  if (!(s = soln_runga4adap_min_steps_per_od_le->text()).isEmpty()) prm.setLabelAndValue("dft_runga4adap_min_steps_per_optical_depth", s);
  else prm.setLabelAndValue("dft_runga4adap_min_steps_per_optical_depth", "5");

  if (!(s = soln_runga4adap_max_steps_per_od_le->text()).isEmpty()) prm.setLabelAndValue("dft_runga4adap_max_steps_per_optical_depth", s);
  else prm.setLabelAndValue("dft_runga4adap_max_steps_per_optical_depth", "5");

  if (!(s = soln_runga4adap_min_error_le->text()).isEmpty()) prm.setLabelAndValue("dft_runga4adap_min_error", s);
  else prm.setLabelAndValue("dft_runga4adap_min_error", "0.01");

  if (!(s = soln_runga4adap_max_error_le->text()).isEmpty()) prm.setLabelAndValue("dft_runga4adap_max_error", s);
  else prm.setLabelAndValue("dft_runga4adap_max_error", "0.05");

    // cunrrently only dft is supported through gui - alternative is integ (no dft optinization)
  prm.setLabelAndValue("solver", "dft");
}




// PlaneTool_Gui::buildInputOptions() [private]

JWidget* PlaneTool_Gui::buildInputOptions(JWidget* parent) {

  JGroupBox* g = new JGroupBox("Input", parent);
  JWidget* top_widget = new JWidget(g);
  JVBoxLayout *vbox = new JVBoxLayout(top_widget);
  vbox->setSpacing(4);

    // sky input
  vbox->addSpacing(8);
  sky_file_fl = new FileLoad("Downwelling sky radiance: ", top_widget, vbox, true, "Load sky downwelling radiance file");
  vbox->addSpacing(8);

    // title for surface functions
  JWidget* label = new JLabel("Water surface reflectance and transmittance functions:", top_widget);
  vbox->addWidget(label);
    // grid for surface functions
  JWidget* w = new JWidget(top_widget);
  vbox->addWidget(w);
  JGridLayout *grid = new JGridLayout(w,4,5,2,2);
  grid->setColStretch(4,100);
    // suface functions
  surface_r00_file_fl = new FileLoad("air > air: ", w, grid, 0, 0, "Load water surface air > air reflectance file");
  surface_r11_file_fl = new FileLoad("water > water: ", w, grid, 1, 0, "Load water surface water > water reflectance file");
  surface_t01_file_fl = new FileLoad("air > water: ", w, grid, 2, 0, "Load water surface air > water transmittance file");
  surface_t10_file_fl = new FileLoad("water > air: ", w, grid, 3, 0, "Load water surface water > air transmittance file");

  vbox->addSpacing(8);

    // grid for attenuation and scattering
  w = new JWidget(top_widget);
  vbox->addWidget(w);
  grid = new JGridLayout(w,3,5,2,2);
  grid->setColStretch(4,100);
    // attenuation and scattering
  atten_sl = new SpecLoad("Attenuation (c) (m-1): ", tempDir()+"/resampled_atten", w, grid, 0, 0, "Load spectral beam attenuation coefficients");
  absorp_sl = new SpecLoad("Absorption (a) (m-1): ", tempDir()+"/resampled_absorp", w, grid, 1, 0, "Load spectral absorption coefficients");
  phase_func_fl = new FileLoad("Phase function: ", w, grid, 3, 0, "Load phase function file");

  vbox->addSpacing(6);

    // grid for depth and sample interval
  w = new JWidget(top_widget);
  vbox->addWidget(w);
  grid = new JGridLayout(w,3,3,2,2);
  grid->setColStretch(2,100);
    // depth
  label = new JLabel("Depth (m): ", w);
  grid->addWidget(label,0,0);
  depth_le = new JLineEdit(w);
  depth_le->setMaxLength(20);
  depth_le->setFixedWidth(60);
  grid->addWidget(depth_le,0,1);
    // sample interval
  label = new JLabel("Sample interval (m): ", w);
  grid->addWidget(label,1,0);
  sample_interval_le = new JLineEdit(w);
  sample_interval_le->setMaxLength(20);
  sample_interval_le->setFixedWidth(60);
  grid->addWidget(sample_interval_le,1,1);
    // delta func distance
  label = new JLabel("Delta evaluation distance (m): ", w);
  grid->addWidget(label,2,0);
  delta_eval_interval_le = new JLineEdit(w);
  delta_eval_interval_le->setMaxLength(20);
  delta_eval_interval_le->setFixedWidth(60);
  grid->addWidget(delta_eval_interval_le,2,1);
  if (fit_1024x768) {
    label->hide();
    delta_eval_interval_le->hide();
  }

  vbox->addSpacing(8);

  vbox->addWidget(buildSubstrateOptions(top_widget));

  return g;
}



// PlaneTool_Gui::buildSubstrateOptions()

JWidget* PlaneTool_Gui::buildSubstrateOptions(JWidget* parent) {

  JWidget* top_widget = new JWidget(parent);
  JVBoxLayout *vbox = new JVBoxLayout(top_widget);
  vbox->setSpacing(4);

  JHBoxLayout *hbox = new JHBoxLayout(vbox);
  hbox->setSpacing(4);

  JWidget* w = new JWidget(top_widget);
  vbox->addWidget(w);
  JGridLayout *grid = new JGridLayout(w,2,6,2,2);
  grid->setColStretch(5,100);
 
  subs_is_lamb_cb = new JCheckBox("",w);
  subs_is_lamb_cb->signalClicked.connect(this, slotLambertianSubstrateClicked);
  grid->addWidget(subs_is_lamb_cb, 0, 0);
  subs_is_brdf_cb = new JCheckBox("",w);
  subs_is_brdf_cb->signalClicked.connect(this, slotBRDFSubstrateClicked);
  grid->addWidget(subs_is_brdf_cb, 1, 0);

  subs_diffuse_r0_sl = new SpecLoad(" Lambertian substrate: ", tempDir()+"/resampled_subs_diffuse_r0", w, grid, 0, 1, "Load substrate diffuse spectral reflectance");
  subs_brdf_r0_fl = new FileLoad(" BRDF substrate: ", w, grid, 1, 1, "Load substrate BRDF function file");

  return top_widget;
}



// PlaneTool_Gui::buildIntegrationOptions() [private]

JWidget* PlaneTool_Gui::buildIntegrationOptions(JWidget* parent) {

  JGroupBox* g = new JGroupBox("Integration", parent);
  JWidget* top_widget = new JWidget(g);
  JVBoxLayout *vbox = new JVBoxLayout(top_widget);
  vbox->setSpacing(4);

  JHBoxLayout* hbox = new JHBoxLayout(vbox);
  hbox->setSpacing(4);

    // integration method
  JLabel* label = new JLabel("Method:", top_widget);
  hbox->addWidget(label);

  soln_method_cm = new JComboBox(top_widget);
  soln_method_cm->insertItem("Euler", EULER_CODE);
  soln_method_cm->insertItem("Mid-Point", MIDPOINT_CODE);
  soln_method_cm->insertItem("Runga4", RUNGA4_CODE);
  if (!fit_1024x768) {
    soln_method_cm->insertItem("Runga4 Adaptive", RUNGA4ADAP_CODE);
  }
  soln_method_cm->signalActivated.connect(this, slotSolnMethodActivated);
  hbox->addWidget(soln_method_cm);
  hbox->addStretch(100);

  vbox->addSpacing(4);

    // stack of widgets for input parameterers for each type
  soln_method_stack = new JWidgetStack(top_widget);
  vbox->addWidget(soln_method_stack);

  JWidget* w;
  JVBoxLayout *vb;
  JHBoxLayout *hb;

    // euler method
  w = new JWidget(soln_method_stack);
  vb = new JVBoxLayout(w);
  hb = new JHBoxLayout(vb);
  label = new JLabel("Steps per optical depth: ",w);
  hb->addWidget(label);
  hb->addSpacing(4);
  soln_euler_steps_per_od_le = new JLineEdit(w);
  soln_euler_steps_per_od_le->setMaxLength(20);
  soln_euler_steps_per_od_le->setFixedWidth(60);
  hb->addWidget(soln_euler_steps_per_od_le);
  hb->addStretch(100);
  vb->addStretch(100);
  soln_method_stack->addWidget(w, EULER_CODE);
 
    // midpoint method
  w = new JWidget(soln_method_stack);
  vb = new JVBoxLayout(w);
  hb = new JHBoxLayout(vb);
  label = new JLabel("Steps per optical depth: ",w);
  hb->addWidget(label);
  hb->addSpacing(4);
  soln_midpoint_steps_per_od_le = new JLineEdit(w);
  soln_midpoint_steps_per_od_le->setMaxLength(20);
  soln_midpoint_steps_per_od_le->setFixedWidth(60);
  hb->addWidget(soln_midpoint_steps_per_od_le);
  hb->addStretch(100);
  vb->addStretch(100);
  soln_method_stack->addWidget(w, MIDPOINT_CODE);

    // runga4 method
  w = new JWidget(soln_method_stack);
  vb = new JVBoxLayout(w);
  hb = new JHBoxLayout(vb);
  label = new JLabel("Steps per optical depth: ",w);
  hb->addWidget(label);
  hb->addSpacing(4);
  soln_runga4_steps_per_od_le = new JLineEdit(w);
  soln_runga4_steps_per_od_le->setMaxLength(20);
  soln_runga4_steps_per_od_le->setFixedWidth(60);
  hb->addWidget(soln_runga4_steps_per_od_le);
  hb->addStretch(100);
  vb->addStretch(100);
  soln_method_stack->addWidget(w, RUNGA4_CODE);

    // runga4adap
    // removed for demo on 1024 x 768
  w = new JWidget(soln_method_stack);
  vb = new JVBoxLayout(w);

  JWidget* gw = new JWidget(w);
  vb->addWidget(gw);
    
  JGridLayout *grid = new JGridLayout(gw,4,3,2,2);
  grid->setColStretch(3,100);
    
  label = new JLabel("Min. steps per optical depth: ",gw);
  grid->addWidget(label, 0, 0);
  soln_runga4adap_min_steps_per_od_le = new JLineEdit(gw);
  soln_runga4adap_min_steps_per_od_le->setMaxLength(20);
  soln_runga4adap_min_steps_per_od_le->setFixedWidth(60);
  grid->addWidget(soln_runga4adap_min_steps_per_od_le, 0, 1);

  label = new JLabel("Max. steps per optical depth: ",gw);
  grid->addWidget(label, 1, 0);
  soln_runga4adap_max_steps_per_od_le = new JLineEdit(gw);
  soln_runga4adap_max_steps_per_od_le->setMaxLength(20);
  soln_runga4adap_max_steps_per_od_le->setFixedWidth(60);
  grid->addWidget(soln_runga4adap_max_steps_per_od_le, 1, 1);

  label = new JLabel("Low error limit: ",gw);
  grid->addWidget(label, 2, 0);
  soln_runga4adap_min_error_le = new JLineEdit(gw);
  soln_runga4adap_min_error_le->setMaxLength(20);
  soln_runga4adap_min_error_le->setFixedWidth(60);
  grid->addWidget(soln_runga4adap_min_error_le, 2, 1);

  label = new JLabel("High error limit: ",gw);
  grid->addWidget(label, 3, 0);
  soln_runga4adap_max_error_le = new JLineEdit(gw);
  soln_runga4adap_max_error_le->setMaxLength(20);
  soln_runga4adap_max_error_le->setFixedWidth(60);
  grid->addWidget(soln_runga4adap_max_error_le, 3, 1);
  
  vb->addStretch(100);

  if (!fit_1024x768) {
    soln_method_stack->addWidget(w, RUNGA4ADAP_CODE);
  }

  return g;
}



// PlaneTool_Gui::init()

void PlaneTool_Gui::init() {
  // nothing to do
}



// PlaneTool_Gui::loadSolutionFiles()

void PlaneTool_Gui::loadSolutionFiles(const JString dir) {

  if (dir.isEmpty()) return;

  Log::stream() << "Load solution from '" << dir << "'\n";

  prm.clear();
  if (prm.readFile(dir+"/params.txt")) {
     Log::stream() << "Run parameters ok.\n";
     parserToGui();
  }

    // init number of bands
  tool.initBands(prm);
 
    // has saved the number of sample points in the parameters also
  int point_count = prm.isDefined("soln_sample_point_count") ? prm.getInt("soln_sample_point_count") : 0;
  double point_interval = prm.getDouble("dft_sample_distance");
 
    // read solution data
  soln_rad_profile.readFromDir(dir, point_count, point_interval);

  newSolution();
}



// PlaneTool_Gui::addDataFiles()

void PlaneTool_Gui::loadDataFiles(const Parser& prm) {

  int i = 1;
  while (true) {

    JString prefix = JString("load_data") + JString::number(i);

    if (!prm.isDefined(prefix+"_filepath")) break;

    JString fp = prm.getString(prefix+"_filepath");
    int dc = prm.getInt(prefix+"_depth_column") - 1;
    double doff = prm.getDouble(prefix+"_depth_offset");
    int sc = prm.getInt(prefix+"_start_column") - 1;
    JString type = prm.getString(prefix+"_type");
    JString bs_fp = prm.getString(prefix+"_bandspec_filepath");
    
    data_selector->addFile(fp, dc, doff, sc, type, bs_fp);
    i++;
  }
}



// PlaneTool_Gui::processComplete() [private]

void PlaneTool_Gui::processComplete() {

  tool.getSolutionData(soln_rad_profile.samplePointData());
  soln_rad_profile.lazyInvalidate();

  Log::stream() << "processing complete.\n";

  newSolution();
}



// PlaneTool_Gui::newSolution() [private]

void PlaneTool_Gui::newSolution() {

  Log::stream() << "new solution.\n";

  BandSpec* bs = target_bandspec_cm->currentBandSpec();
  data_selector->radProfile().setBandSpec(bs);

  //profile_panel->setLoadedData(&data_selector->dataList(), bs);
  profile_panel->setSamplePointData(&soln_rad_profile.samplePointData());


  //depth_swipe_panel->setLoadedData(&data_selector->dataList(), bs);
  //depth_swipe_panel->setSamplePointData(&soln_data);
  depth_swipe_panel->setRadProfileGroup(&rad_profile_group);

}



// PlaneTool_Gui::actionSolnMethodActivated()

void PlaneTool_Gui::actionSolnMethodActivated(int code) {
  soln_method_stack->raiseWidget(code);
}



// PlaneTool_Gui::actionLambertianSubstrateClicked()

void PlaneTool_Gui::actionLambertianSubstrateClicked() {
  subs_is_lamb_cb->setChecked(true);
  subs_is_brdf_cb->setChecked(false);
  subs_diffuse_r0_sl->setEnabled(true);
  subs_brdf_r0_fl->setEnabled(false);
}



// PlaneTool_Gui::actionBRDFSubstrateClicked()

void PlaneTool_Gui::actionBRDFSubstrateClicked() {
  subs_is_lamb_cb->setChecked(false);
  subs_is_brdf_cb->setChecked(true);
  subs_diffuse_r0_sl->setEnabled(false);
  subs_brdf_r0_fl->setEnabled(true);
}

 

// PlaneTool_Gui::actionProcessClicked()

void PlaneTool_Gui::actionProcessClicked() {

  Log::stream() << "process clicked.\n";

    // make sure prm is set up
  guiToParser();
    // set logging level
  prm.setLabelAndValue("verbose", "2");
    // a progress update every 100 ms
  progress_timer.start(100);
    // start the tool processing
  tool_thread.start();

  // non-threaded running
  // tool.run(prm);
}



// PlaneTool_Gui::actionLoadParamsClicked()

void PlaneTool_Gui::actionLoadParamsClicked() {

  Log::stream() << "load params clicked.\n";

  JString filepath = JFileDialog::getOpenFileName(JString::null, JString::null, 0, 0, "Load parameters file");

  if (filepath.isEmpty()) return;
  if (!GlobalFuncs::fileCanBeRead(filepath)) return;

  prm.clear();
  prm.readFile(filepath);
  parserToGui();
}



// PlaneTool_Gui::actionSaveParamsClicked()

void PlaneTool_Gui::actionSaveParamsClicked() {
  
  Log::stream() << "save params clicked.\n";

  JString filepath = JFileDialog::getSaveFileName(JString::null, JString::null, 0, 0, "Save parameters file");
 
  guiToParser();

  if (!filepath.isEmpty()) prm.writeFile(filepath);
  else prm.displayList(Log::stream());
}



// PlaneTool_Gui::actionFileMenu_LoadSolnFile()

void PlaneTool_Gui::actionFileMenu_LoadSolnFile() {

  Log::stream() << "load solution file.\n";

  JString dir = JFileDialog::getExistingDirectory(JString::null, 0, 0, "Load solution files from from directory");

  loadSolutionFiles(dir);
}



// PlaneTool_Gui::actionFileMenu_SaveSolnFile()

void PlaneTool_Gui::actionFileMenu_SaveSolnFile() {
  
  Log::stream() << "save solution file.\n";

  JString dir = JFileDialog::getExistingDirectory(JString::null, 0, 0, "Save solution files to directory");

  if (dir.isEmpty()) return;

  guiToParser();

    // will save the number of sample points in the parameters also
  prm.setLabelAndValue("soln_sample_point_count",JString::number(soln_rad_profile.samplePointData().pointCount()));
    // write parameters
  prm.writeFile(dir+"/params.txt");
    // write soln data
  soln_rad_profile.writeToDir(dir);
}




// PlaneTool_Gui::actionFileMenu_LoadProfileData()

void PlaneTool_Gui::actionFileMenu_LoadProfileData() {

  Log::stream() << "load profile data.\n";
}




// PlaneTool_Gui::actionFileMenu_Quit()

void PlaneTool_Gui::actionFileMenu_Quit() {  
  Log::stream() << "quit.\n";
  exit(0);
}



// PlaneTool_Gui::actionViewMenu_ModelInput()

void PlaneTool_Gui::actionViewMenu_ModelInput() {
  //printf("PlaneTool_Gui::actionViewMenu_ModelInput()\n");
  bool f = view_barmenu->isItemChecked(view_menu_modelinput_id);
  //view_barmenu->setItemChecked(view_menu_modelinput_id, f);
  if (f) tabs->show();
  else tabs->hide();
}



// PlaneTool_Gui::actionViewMenu_Profile()

void PlaneTool_Gui::actionViewMenu_Profile() {
  //printf("PlaneTool_Gui::actionViewMenu_Profile()\n");
  //printf("%d %d\n", view_menu_profile_id, view_barmenu->isItemChecked(view_menu_profile_id));
  bool f = view_barmenu->isItemChecked(view_menu_profile_id);
  //view_barmenu->setItemChecked(view_menu_profile_id, f);
  if (f) profile_panel->show();
  else profile_panel->hide();
}



// PlaneTool_Gui::actionViewMenu_DepthSwipe()

void PlaneTool_Gui::actionViewMenu_DepthSwipe() {
  //printf("PlaneTool_Gui::actionViewMenu_DepthSwipe()\n");
  bool f = view_barmenu->isItemChecked(view_menu_depthswipe_id);
  //view_barmenu->setItemChecked(view_menu_depthswipe_id, f);
  if (f) depth_swipe_panel->show();
  else depth_swipe_panel->hide();
}



// PlaneTool_Gui::actionDataFileListChanged()

void PlaneTool_Gui::actionDataFileListChanged() {

  Log::stream() << "file list changed.\n";

  //printf("count %d\n", data_selector->dataList().count());
 
  //BandSpec* bs = target_bandspec_cm->currentBandSpec();
  //profile_panel->setLoadedData(&data_selector->dataList(), bs);
}



// PlaneTool_Gui::actionUpdateProgress()

void PlaneTool_Gui::actionUpdateProgress() {

  progress_bar->setProgress((int) tool.pctComplete());
  
  //printf("called %f\n",tool.pctComplete());
 
  if (!tool.processingComplete()) return;

  progress_timer.stop();
  processComplete();
}

