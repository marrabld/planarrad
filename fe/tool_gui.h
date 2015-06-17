
// tool_gui.h

#ifndef _JUDE_FE_TOOLGUI_H
#define _JUDE_FE_TOOLGUI_H

#include <QMainWindow>
#include "base/jstring.h"
#include "base/jparser.h"
#include "base/jiodevicestore.h"

#include <QSettings>

using namespace jude::base;

namespace jude {
namespace fe {



/*! Base class for all GUI context tools. */

class Tool_Gui : public QMainWindow {

protected:

  JParser& outer_prm;
  JParser inner_prm;

  void reportError(); 

private:

  JString temp_dir;
  JString files_dir;
  JString band_spec_dir;
  JString direc_spec_dir;
 
  JIODeviceStore es_store;

public:

    /*! Convienience function for writing parser entries to persistent settings. */
  static void parserToSettings(const JParser& prm, const JString& label, QSettings& settings) {
    if (!prm.getStringOrEmpty(label).isEmpty()) settings.setValue((JString("params/")+label).latin1(), prm.getStringOrEmpty(label).latin1());
  }

    /*! Convienience function for reading parser entries from persistent settings. */
  static void settingsToParser(JParser& prm, const JString& label, const QSettings& settings) {
    prm.setLabelAndValue(label, Q2J(settings.value((JString("params/")+label).latin1()).toString()));
  }

  static void parserAllToSettings(const JParser& prm, const JString& group, QSettings& settings);

  static void settingsToParserAll(JParser& prm, const JString& group, const QSettings& settings);

  Tool_Gui(JParser& prm, QWidget* parent=0);

  QSizePolicy sizePolicy() const;

  void saveAllSettings();
  void restoreAllSettings();
  void clearInitialisationFlag();

  //void closeEvent(QCloseEvent *event);

  virtual const char* toolID() const = 0;

    /*! Where to save temporary files. */
  const JString& tempDir() const { return temp_dir; }

    /*! Where to find input data files. */
  const JString& filesDir() const { return files_dir; }

    /*! Where to find directional structure specification files. */
  const JString& bandSpecDir() const { return band_spec_dir; }

    /*! Where to find band specification files. */
  const JString& direcSpecDir() const { return direc_spec_dir; }

  void initBandSpecList();
  void initDirecSpecList();

  virtual void guiToParser();

  virtual void saveSettings(QSettings& settings) const;
  virtual void restoreSettings(const QSettings& settings);
};


} } // namespace jude::fe

#endif // _JUDE_FE_TOOLGUI_H

