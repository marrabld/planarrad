

noinst_PROGRAMS = test_base_free




test_base_free_SOURCES = \
	jimagesave_testing.cc \
	jstring_testing.cc \
	jtypes_testing.cc \
	main.cc

test_base_free_CPPFLAGS = \
	-I$(top_srcdir)

test_base_free_LDADD = \
	$(top_builddir)/base/libjudebase2_free.la

test_base_free_DEPENDENCIES = test_base_files

if WIN32
test_base_free_LDFLAGS = -mconsole
endif






test_base_files: Makefile
	cp $(srcdir)/cross40x40_ref.ppm ./
	cp $(srcdir)/cross20x50_ref.ppm ./

