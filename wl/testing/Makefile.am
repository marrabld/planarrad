

noinst_PROGRAMS = test_wl_free test_wl_files




test_wl_free_SOURCES = \
	wl_testing.cc \
	main.cc

test_wl_free_CPPFLAGS = \
	-I$(top_srcdir)

test_wl_free_LDADD = \
	$(top_builddir)/wl/libjudewl_free.la \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la





test_wl_files: Makefile
	cp $(srcdir)/archive.042 ./
	cp $(srcdir)/archive.008 ./

