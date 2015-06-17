

bin_PROGRAMS = skytool_free




skytool_free_SOURCES = \
	$(top_srcdir)/tool_common/main_template.cc \
	skytool.cc

skytool_free_CPPFLAGS = \
	-I$(top_srcdir) \
	-DTOOLOBJECT=SkyTool

skytool_free_LDADD = \
	$(top_builddir)/file/libjudefile_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/table/libjudetable_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la

if WIN32
skytool_free_LDFLAGS = -mconsole
endif





BUILT_SOURCES = tool_header.h

tool_header.h :
	$(LN_S) -f ${srcdir}/skytool.h tool_header.h

