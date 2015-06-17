

bin_PROGRAMS = planarrad_free



planarrad_free_SOURCES = \
	$(top_srcdir)/tool_common/gui_template.cc \
	aboutdialog.cc \
	additionaloptionsdialog.cc \
	boundarywidget.cc \
	colourcombo.cc \
	depthwidget.cc \
	dataset.cc \
	datasetcombo.cc \
	datatypecombo.cc \
	datawidget.cc \
	datalistmodel.cc \
	hemiwidget.cc \
	interfacewidget.cc \
	iopwidget.cc \
	modelwidget.cc \
	planarrad.cc \
	plotpanelscroll.cc \
	profilechart.cc \
	profiledata.cc \
	profiledepthline.cc \
	profilewidget.cc \
	raddirecset.cc \
	res.cc \
	specset.cc \
	specchart.cc \
	specdata.cc \
	specwidget.cc \
	skywidget.cc \
	visualisationoptionsdialog.cc \
	../../skytool/skytool.cc \
	../../surftool/surftool.cc \
	../../surftool/coxsurface.cc \
	../../surftool/facet.cc \
	../../phasetool/phasetool.cc \
	../../slabtool/slabtool.cc \
	$(planarrad_free_MOC)

planarrad_free_MOC = \
	additionaloptionsdialog.moc.cc \
	boundarywidget.moc.cc \
	datawidget.moc.cc \
	datasetcombo.moc.cc \
	datatypecombo.moc.cc \
	hemiwidget.moc.cc \
	interfacewidget.moc.cc \
	iopwidget.moc.cc \
	modelwidget.moc.cc \
	planarrad.moc.cc \
	plotpanelscroll.moc.cc \
	profilechart.moc.cc \
	profiledata.moc.cc \
	profilewidget.moc.cc \
	specchart.moc.cc \
	specdata.moc.cc \
	specwidget.moc.cc \
	skywidget.moc.cc \
	visualisationoptionsdialog.moc.cc

planarrad_free_CPPFLAGS = \
	-I$(top_srcdir) \
	-I${srcdir}/.. \
	-DQT_MAJOR=$(QT_MAJOR) \
	-DTOOLOBJECT_GUI=PlanarRad

planarrad_free_LDADD = \
	$(top_builddir)/fe/libjudefe_free.la \
	$(top_builddir)/chartS2/libjudechartS2_free.la \
	$(top_builddir)/drawS2/libjudedrawS2_free.la \
	$(top_builddir)/file/libjudefile_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/table/libjudetable_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la \
	$(QTLIBS)

#planarrad_free_DEPENDENCIES = resources

if WIN32
planarrad_free_LDFLAGS = -mwindows
endif

BUILT_SOURCES = tool_header_gui.h $(planarrad_free_MOC) 






# stops autoreconf complaining about the pattern rule being a GNU make extension
AUTOMAKE_OPTIONS = -Wno-portability

%.moc.cc: %.h
	$(MOC) -o $@ $<

AM_CXXFLAGS = $(QT_CXXFLAGS)

tool_header_gui.h :
	$(LN_S) -f ${srcdir}/planarrad.h tool_header_gui.h

res.cc:
	$(QTDIR)/bin/rcc -o $(srcdir)/res.cc $(srcdir)/res.qrc

