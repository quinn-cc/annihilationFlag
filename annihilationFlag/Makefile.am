lib_LTLIBRARIES = annihilationFlag.la

annihilationFlag_la_SOURCES = annihilationFlag.cpp
annihilationFlag_la_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
annihilationFlag_la_LDFLAGS = -module -avoid-version -shared
annihilationFlag_la_LIBADD = $(top_builddir)/plugins/plugin_utils/libplugin_utils.la

AM_CPPFLAGS = $(CONF_CPPFLAGS)
AM_CFLAGS = $(CONF_CFLAGS)
AM_CXXFLAGS = $(CONF_CXXFLAGS)

EXTRA_DIST = \
	README.annihilationFlag.txt \
	annihilationFlag.sln \
	annihilationFlag.vcxproj	\
	annihilationFlag.vcxproj.filters

MAINTAINERCLEANFILES =	\
	Makefile.in
