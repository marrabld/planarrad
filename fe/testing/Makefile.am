

noinst_PROGRAMS = \
	test_fe_helloworld_free \
	test_fe_bufferwidget_free \
	test_fe_drawing3dwidget_free




test_fe_helloworld_free_SOURCES = \
	main_helloworld.cc

test_fe_helloworld_free_CPPFLAGS = \
	-I$(top_srcdir) \
	-DQT_MAJOR=$(QT_MAJOR)

test_fe_helloworld_free_LDADD = \
	$(top_builddir)/fe/libjudefe_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/drawS3/libjudedrawS3_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/drawS2/libjudedrawS2_free.la \
	$(top_builddir)/S2/libjudeS2_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la \
	$(QTLIBS)

if WIN32
test_fe_helloworld_free_LDFLAGS = -mwindows
endif


test_fe_bufferwidget_free_SOURCES = \
	main_bufferwidget.cc

test_fe_bufferwidget_free_CPPFLAGS = \
	-I$(top_srcdir) \
	-DQT_MAJOR=$(QT_MAJOR)

test_fe_bufferwidget_free_LDADD = \
	$(top_builddir)/fe/libjudefe_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/drawS3/libjudedrawS3_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/drawS2/libjudedrawS2_free.la \
	$(top_builddir)/S2/libjudeS2_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la \
	$(QTLIBS)

if WIN32
test_fe_bufferwidget_free_LDFLAGS = -mwindows
endif


test_fe_drawing3dwidget_free_SOURCES = \
	main_drawing3dwidget.cc

test_fe_drawing3dwidget_free_CPPFLAGS = \
	-I$(top_srcdir) \
	-DQT_MAJOR=$(QT_MAJOR)

test_fe_drawing3dwidget_free_LDADD = \
	$(top_builddir)/fe/libjudefe_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/drawS3/libjudedrawS3_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/drawS2/libjudedrawS2_free.la \
	$(top_builddir)/S2/libjudeS2_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la \
	$(QTLIBS)

if WIN32
test_fe_drawing3dwidget_free_LDFLAGS = -mwindows
endif





AM_CXXFLAGS = $(QT_CXXFLAGS)

