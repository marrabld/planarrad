
# stops autoreconf complaining about the pattern rule being a GNU make extension
AUTOMAKE_OPTIONS = -Wno-portability

%.moc.cc: %.h
	$(MOC) -o $@ $<

pkglibdir = ${prefix}/lib
pkgincludedir = ${prefix}/include/fe

pkglib_LTLIBRARIES = libjudefe_free.la



libjudefe_free_la_CPPFLAGS = -I${top_srcdir} -DQT_MAJOR=$(QT_MAJOR)

libjudefe_free_la_SOURCES = \
	bandspeccombobox.cc \
	bufferwidget.cc \
	button.cc \
	buttongroup.cc \
	direcspeccombobox.cc \
	fileload.cc \
	getimage.cc \
	lognumberdrawitem2d.cc \
	hsdview.cc \
	panelscroll.cc \
	specload.cc \
	specpanel.cc \
	specpanelbar.cc \
	specpanelsupplier.cc \
	textchartlognumbergen2d.cc \
	textchartnumbergen2d.cc \
	textdrawitem2d.cc \
	toolthread.cc \
	tool_gui.cc \
	$(libjudefe_free_la_MOC)

libjudefe_free_la_MOC = \
	bufferwidget.moc.cc \
	button.moc.cc \
	fileload.moc.cc \
	panelscroll.moc.cc \
	specpanel.moc.cc \
	specpanelbar.moc.cc \
	toolthread.moc.cc

#libjudefe_free_la_LDFLAGS = -no-undefined -L$(top_builddir)/judebase -L$(QTDIR)/lib
libjudefe_free_la_LDFLAGS = -no-undefined -L$(QTDIR)/lib

BUILT_SOURCES = $(libjudefe_free_la_MOC) 






pkginclude_HEADERS = \
	bandspeccombobox.h \
	basepanel.h \
	bufferwidget.h \
	button.h \
	buttongroup.h \
	direcspeccombobox.h \
	fileload.h \
	getimage.h \
	hsdview.h \
	lognumberdrawitem2d.h \
	panelscroll.h \
	specload.h \
	specpanel.h \
	specpanelbar.h \
	specpanelsupplier.h \
	textchartlognumbergen2d.h \
	textchartnumbergen2d.h \
	toolthread.h \
	tool_gui.h




AM_CXXFLAGS = $(QT_CXXFLAGS)

CLEANFILES = $(BUILT_SOURCES)

