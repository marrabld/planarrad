

noinst_PROGRAMS = test_rad_free




test_rad_free_SOURCES = \
	bandspec_testing.cc \
	main.cc

test_rad_free_CPPFLAGS = \
	-I$(top_srcdir)

test_rad_free_LDADD = \
	$(top_builddir)/rad/libjuderad_free.la \
	$(top_builddir)/ds/libjudeds_free.la \
	$(top_builddir)/S3/libjudeS3_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la

test_rad_free_DEPENDENCIES = test_rad_files




test_rad_files: Makefile
	cp $(srcdir)/ps1 ./
	cp $(srcdir)/ps2 ./

