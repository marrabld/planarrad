

noinst_PROGRAMS = test_table_free




test_table_free_SOURCES = \
	table_testing.cc \
	main.cc	

test_table_free_CPPFLAGS = \
	-I$(top_srcdir)

test_table_free_LDADD = \
	$(top_builddir)/table/libjudetable_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la

if WIN32
test_table_free_LDFLAGS = -mconsole
endif



