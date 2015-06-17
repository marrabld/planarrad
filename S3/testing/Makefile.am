

noinst_PROGRAMS = test_S3_free




test_S3_free_SOURCES = \
	main.cc

test_S3_free_CPPFLAGS = \
	-I$(top_srcdir)

test_S3_free_LDADD = \
	$(top_builddir)/base/libjudebase2_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/S3/libjudeS3_free.la

if WIN32
test_S3_free_LDFLAGS = -mconsole
endif




