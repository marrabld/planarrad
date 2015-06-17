

noinst_PROGRAMS = test_math_free




test_math_free_SOURCES = \
	rand_testing.cc \
	main.cc

test_math_free_CPPFLAGS = \
	-I$(top_srcdir)

test_math_free_LDADD = \
	$(top_builddir)/math/libjudemath2_free.la \
	$(top_builddir)/base/libjudebase2_free.la

if WIN32
test_math_free_LDFLAGS = -mconsole
endif




