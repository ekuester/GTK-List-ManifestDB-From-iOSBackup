SUBDIRS = src po
# rpm 16x16

ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS = manifestDBview.sh

LATERAL_FILES = COMMENTS LICENSE README.md

lateraldir = $(pkgdatadir)
lateral_DATA = $(LATERAL_FILES)

EXTRA_DIST = autogen.sh GTKManifestDBView.sh manifestDBview.sh \
 $(LATERAL_FILES)
# rpm

CLEANFILES =

# desktop files
desktopdir = $(datadir)/applications
desktop_DATA = manifestDBview.desktop

# convenience command for doing Makefile variable substitutions in non-Makefile
# files (scripts, service files, etc.)
do_subst = sed -e 's|@abs_top_srcdir[@]|$(abs_top_srcdir)|g' \
	-e 's|@abs_top_builddir[@]|$(abs_top_builddir)|g' \
	-e 's|@localedir[@]|$(localedir)|g' \
	-e 's|@bindir[@]|$(bindir)|g' \
	-e 's|@libexecdir[@]|$(libexecdir)|g' \
	-e 's|@pkglibdir[@]|$(pkglibdir)|g' \
	-e 's|@pkgdatadir[@]|$(pkgdatadir)|g' \
	-e 's|@have_libnotify[@]|$(HAVE_LIBNOTIFY)|g' \
	-e 's|@have_libsoup[@]|$(HAVE_LIBSOUP)|g' \
	-e 's|@have_cheese[@]|$(HAVE_CHEESE)|g'

manifestDBview.sh: manifestDBview.sh.in
	$(AM_V_GEN) $(do_subst) $< > $@
	chmod +x $@

manifestDBview.desktop: manifestDBview.desktop.in
	$(AM_V_GEN) $(do_subst) $< > $@
	chmod +x $@

CLEANFILES += manifestDBview.sh
EXTRA_DIST += manifestDBview.desktop.in manifestDBview.sh.in

# application icon 512x512
appicondir = $(datadir)/pixmaps
appicon_DATA = manifestDBview.svg

EXTRA_DIST += manifestDBview.desktop manifestDBview.svg

