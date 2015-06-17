

bin_PROGRAMS = calctool_free




calctool_free_SOURCES = \
	$(top_srcdir)/tool_common/main_template.cc \
	ct_table.cc \
	calctool.cc

calctool_free_CPPFLAGS = \
	-I$(top_srcdir) \
	-DTOOLOBJECT=CalcTool

calctool_free_LDADD = \
	$(top_builddir)/file/libjudefile_free.la \
	$(top_builddir)/tool/libjudetool_free.la \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/table/libjudetable_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la





BUILT_SOURCES = tool_header.h

tool_header.h :
	$(LN_S) -f ${srcdir}/calctool.h tool_header.h

