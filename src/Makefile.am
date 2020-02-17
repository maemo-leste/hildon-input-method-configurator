bin_PROGRAMS = hildon-input-method-configurator hildon-im-convert-restored-gconf

hildon_input_method_configurator_CFLAGS = $(GLIB_CFLAGS)
hildon_input_method_configurator_LDADD = $(GLIB_LIBS)
hildon_input_method_configurator_SOURCES = configurator.c

hildon_im_convert_restored_gconf_CFLAGS = $(GLIB_CFLAGS) $(EXPAT_CFLAGS)
hildon_im_convert_restored_gconf_LDADD = $(GLIB_LIBS) $(EXPAT_LIBS)
hildon_im_convert_restored_gconf_SOURCES = hildon-im-convert-restored-gconf.c

MAINTAINERCLEANFILES = Makefile.in
