

bin_PROGRAMS = wltool_free




wltool_free_SOURCES = \
	$(top_srcdir)/tool_common/main_template.cc \
	wltool.cc

wltool_free_CPPFLAGS = \
	-I$(top_srcdir) \
	-DTOOLOBJECT=WLTool

wltool_free_LDADD = \
	$(top_builddir)/wl/libjudewl_free.la \
	$(top_builddir)/file/libjudefile_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/table/libjudetable_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la

if WIN32
wltool_free_LDFLAGS = -mconsole
endif




BUILT_SOURCES = tool_header.h

tool_header.h :
	$(LN_S) -f ${srcdir}/wltool.h tool_header.h

