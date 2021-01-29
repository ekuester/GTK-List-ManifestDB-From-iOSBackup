ACLOCAL_AMFLAGS = -I m4

installdir = $(datadir)/$(PACKAGE)
install_PROGRAMS = ManifestDBView

ManifestDBView_SOURCES = about.xpm \
main.cpp \
manifestDBwindow.h manifestDBwindow.cpp

ManifestDBView_CPPFLAGS = $(GTKMM_CFLAGS) $(CAIRO_CFLAGS) \
-DPROGRAMNAME_LOCALEDIR=\"$(PROGRAMNAME_LOCALEDIR)\" \
-DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\" \
-DNLS=1

ManifestDBView_LDFLAGS = -lsqlite3

ManifestDBView_LDADD = $(GTKMM_LIBS) $(CAIRO_LIBS)

