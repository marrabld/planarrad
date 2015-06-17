

bin_PROGRAMS = wltool_free_gui



wltool_free_gui_SOURCES = \
	$(top_srcdir)/tool_common/gui_template.cc \
	wltool_gui.cc \
	../wltool.cc \
	aboutdialog.cc \
	chart_acs_data.cc \
	chart_acs_time.cc \
	chart_ctd_time.cc \
	chart_ecobb3_time.cc \
	chart_hocr_data.cc \
	chart_hocr_time.cc \
	chart_time.cc \
	mergelistmodel.cc \
	mergemenu.cc \
	mergetab.cc \
	packetlistmodel.cc \
	packettab.cc \
	recordlistmodel.cc \
	recordtab.cc \
	recordtext.cc \
	sensorconfigdialog.cc \
	visualisationconfigdialog.cc \
	wlchart.cc \
	wlpanel.cc \
	wlpanelscroll.cc \
	sensorselect.cc \
	$(wltool_free_gui_MOC)

wltool_free_gui_MOC = \
	mergemenu.moc.cc \
	wlchart.moc.cc \
	wlpanel.moc.cc \
	recordtext.moc.cc \
	sensorconfigdialog.moc.cc \
	sensorselect.moc.cc \
	visualisationconfigdialog.moc.cc \
	wlpanelscroll.moc.cc \
	wltool_gui.moc.cc

wltool_free_gui_CPPFLAGS = \
	-I$(top_srcdir) \
	-I${srcdir}/.. \
	-DQT_MAJOR=$(QT_MAJOR) \
	-DTOOLOBJECT=WLTool -DTOOLOBJECT_GUI=WLTool_Gui

wltool_free_gui_LDADD = \
	$(top_builddir)/fe/libjudefe_free.la \
	$(top_builddir)/chartS2/libjudechartS2_free.la \
	$(top_builddir)/drawS2/libjudedrawS2_free.la \
	$(top_builddir)/wl/libjudewl_free.la \
	$(top_builddir)/file/libjudefile_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/table/libjudetable_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la \
	$(QTLIBS)

#wltool_free_gui_DEPENDENCIES = resources

if WIN32
wltool_free_gui_LDFLAGS = -mwindows
endif

BUILT_SOURCES = tool_header_gui.h $(wltool_free_gui_MOC) 






# stops autoreconf complaining about the pattern rule being a GNU make extension
AUTOMAKE_OPTIONS = -Wno-portability

%.moc.cc: %.h
	$(MOC) -o $@ $<

AM_CXXFLAGS = $(QT_CXXFLAGS)

tool_header_gui.h :
	$(LN_S) -f ${srcdir}/wltool_gui.h tool_header_gui.h

res.cc:
	$(QTDIR)/bin/rcc -o $(srcdir)/res.cc $(srcdir)/res.qrc

