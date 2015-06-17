

noinst_PROGRAMS = test_ds_free




test_ds_free_SOURCES = \
	ds_testing.cc \
	main.cc

test_ds_free_CPPFLAGS = \
	-I$(top_srcdir)

test_ds_free_LDADD = \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la

if WIN32
test_ds_free_LDFLAGS = -mconsole
endif



