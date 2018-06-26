#include <glib-object.h>

#include <stdlib.h>

#define SCHEMA_FILE "/etc/gconf/schemas/hildon-input-method-configuration.schema"
#define INSTALL_SCHEMA_FILE_CMD "gconftool-2 --makefile-install-rule " SCHEMA_FILE

static gboolean
generate_schema_file()
{
  char *slide_layout = get_slide_layout();
  const char *int_kb_model = get_int_kb_model();
  GString *s = g_string_new("<gconfschemafile>\n\t<schemalist>\n");
  const gchar *have_int_kb;
  char *hw;
  gboolean rv = FALSE;
  gint exit_status;

  g_debug("int keyboard model is [%s] --- \n", int_kb_model);

  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/have-internal-keyboard</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/have-internal-keyboard</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>bool</type>\n"
                      "\t\t\t<default>");

  if (!get_have_int_kb())
    have_int_kb = "true";
  else
    have_int_kb = "false";

  s = g_string_append(s, have_int_kb);
  s = g_string_append(s, "</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Have internal keyboard</short>\n"
                      "\t\t\t\t<long>Have internal keyboard</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");
  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/slide-layout</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/slide-layout</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>string</type>\n"
                      "\t\t\t<default>");

  if (slide_layout)
  {
    gchar *kb_layout = get_int_kb_layout(slide_layout);

    s = g_string_append(s, slide_layout);
    s = g_string_append(s, "</default>\n"
                        "\t\t\t<locale name=\"C\">\n"
                        "\t\t\t\t<short>Slide keyboard layout</short>\n"
                        "\t\t\t\t<long>Slide keyboard layout</long>\n"
                        "\t\t\t</locale>\n"
                        "\t\t</schema>\n");
    s = g_string_append(s, "\t\t<schema>\n"
                        "\t\t\t<key>/schemas/apps/osso/inputmethod/int_kb_layout</key>\n"
                        "\t\t\t<applyto>/apps/osso/inputmethod/int_kb_layout</applyto>\n"
                        "\t\t\t<owner>hildon-input-method</owner>\n"
                        "\t\t\t<type>string</type>\n"
                        "\t\t\t<default>");

    if (kb_layout)
      s = g_string_append(s, kb_layout);
    else
      s = g_string_append(s, "us");

    g_free(kb_layout);

    s = g_string_append(s, "</default>\n"
                        "\t\t\t<locale name=\"C\">\n"
                        "\t\t\t\t<short>Slide keyboard layout abbreviation name</short>\n"
                        "\t\t\t\t<long>Slide keyboard layout abbreviation name</long>\n"
                        "\t\t\t</locale>\n"
                        "\t\t</schema>\n");
    g_free(slide_layout);
  }
  else
  {
    g_warning("Error getting keyboard value, on sbox eh?");
    s = g_string_append(s, "English");
    s = g_string_append(s, "</default>\n"
                        "\t\t\t<locale name=\"C\">\n"
                        "\t\t\t\t<short>Slide keyboard layout</short>\n"
                        "\t\t\t\t<long>Slide keyboard layout</long>\n"
                        "\t\t\t</locale>\n"
                        "\t\t</schema>\n");
    s = g_string_append(s, "\t\t<schema>\n"
                        "\t\t\t<key>/schemas/apps/osso/inputmethod/int_kb_layout</key>\n"
                        "\t\t\t<applyto>/apps/osso/inputmethod/int_kb_layout</applyto>\n"
                        "\t\t\t<owner>hildon-input-method</owner>\n"
                        "\t\t\t<type>string</type>\n"
                        "\t\t\t<default>");
    s = g_string_append(s, "us");
    s = g_string_append(s, "</default>\n"
                        "\t\t\t<locale name=\"C\">\n"
                        "\t\t\t\t<short>Slide keyboard layout abbreviation name</short>\n"
                        "\t\t\t\t<long>Slide keyboard layout abbreviation name</long>\n"
                        "\t\t\t</locale>\n"
                        "\t\t</schema>\n");
  }

  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/int_kb_model</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/int_kb_model</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>string</type>\n"
                      "\t\t\t<default>");

  if (int_kb_model)
    s = g_string_append(s, int_kb_model);
  else
  {
    g_warning("Error getting keyboard model, on sbox eh?");
    s = g_string_append(s, "nokiarx44");
  }

  s = g_string_append(s, "</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Slide keyboard model name</short>\n"
                      "\t\t\t\t<long>Slide keyboard model name</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");
  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/hildon-im-languages/translation-library/function</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/hildon-im-languages/translation-library/function</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>string</type>\n"
                      "\t\t\t<default>libi18n_locale_resolve_simple</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Endonyms resolver</short>\n"
                      "\t\t\t\t<long>Endonyms resolver</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");
  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/hildon-im-languages/translation-library/name</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/hildon-im-languages/translation-library/name</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>string</type>\n"
                      "\t\t\t<default>/usr/lib/libi18n-locale-resolver.so.0</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Endonyms resolver</short>\n"
                      "\t\t\t\t<long>Endonyms resolver</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");

  hw = osso_get_product_info(OSSO_PRODUCT_HARDWARE);

  if (hw)
  {
    if (g_ascii_strcasecmp(hw, "RX-48"))
    {
      s = g_string_append(s, "\t\t<schema>\n"
                          "\t\t\t<key>/schemas/apps/osso/inputmethod/hildon-im-languages/language-0</key>\n"
                          "\t\t\t<applyto>/apps/osso/inputmethod/hildon-im-languages/language-0</applyto>\n"
                          "\t\t\t<owner>hildon-input-method</owner>\n"
                          "\t\t\t<type>string</type>\n"
                          "\t\t\t<default>en_GB</default>\n"
                          "\t\t\t<locale name=\"C\">\n"
                          "\t\t\t\t<short>Default language</short>\n"
                          "\t\t\t\t<long>Default language</long>\n"
                          "\t\t\t</locale>\n"
                          "\t\t</schema>\n");
    }
    else
    {
      s = g_string_append(s, "\t\t<schema>\n"
                          "\t\t\t<key>/schemas/apps/osso/inputmethod/hildon-im-languages/language-0</key>\n"
                          "\t\t\t<applyto>/apps/osso/inputmethod/hildon-im-languages/language-0</applyto>\n"
                          "\t\t\t<owner>hildon-input-method</owner>\n"
                          "\t\t\t<type>string</type>\n"
                          "\t\t\t<default>");
      s = g_string_append(s, "en_US");
      s = g_string_append(s, "</default>\n"
                          "\t\t\t<locale name=\"C\">\n"
                          "\t\t\t\t<short>Default language</short>\n"
                          "\t\t\t\t<long>Default language</long>\n"
                          "\t\t\t</locale>\n"
                          "\t\t</schema>\n");
    }

    g_free(hw);
  }
  else
  {
    s = g_string_append(s, "\t\t<schema>\n"
                        "\t\t\t<key>/schemas/apps/osso/inputmethod/hildon-im-languages/language-0</key>\n"
                        "\t\t\t<applyto>/apps/osso/inputmethod/hildon-im-languages/language-0</applyto>\n"
                        "\t\t\t<owner>hildon-input-method</owner>\n"
                        "\t\t\t<type>string</type>\n"
                        "\t\t\t<default>en_GB</default>\n"
                        "\t\t\t<locale name=\"C\">\n"
                        "\t\t\t\t<short>Default language</short>\n"
                        "\t\t\t\t<long>Default language</long>\n"
                        "\t\t\t</locale>\n"
                        "\t\t</schema>\n");
  }

  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/default-plugins/hw-keyboard</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/default-plugins/hw-keyboard</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>string</type>\n"
                      "\t\t\t<default>hildon_keyboard_assistant</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Default plugin for HW keyboard</short>\n"
                      "\t\t\t\t<long>The default plugin for hardware keyboard trigger</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");
  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/default-plugins/stylus</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/default-plugins/stylus</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>string</type>\n"
                      "\t\t\t<default>hildon_western_fkb</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Default plugin for stylus</short>\n"
                      "\t\t\t\t<long>The default plugin for stylus trigger</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");
  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/default-plugins/finger</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/default-plugins/finger</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>string</type>\n"
                      "\t\t\t<default>hildon_western_fkb</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Default plugin for finger</short>\n"
                      "\t\t\t\t<long>The default plugin for finger trigger</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");
  s = g_string_append(s, "\t</schemalist>\n</gconfschemafile>\n");

  if (g_file_set_contents(SCHEMA_FILE, s->str, s->len, NULL))
  {
    setenv("GCONF_CONFIG_SOURCE", "xml::/etc/gconf/gconf.xml.defaults", 1);

    if (g_spawn_command_line_sync(INSTALL_SCHEMA_FILE_CMD,NULL, NULL,
                                  &exit_status, NULL))
    {
      rv = TRUE;
    }
    else
      g_warning("Error installing schema.");
  }
  else
    g_warning("Unable to generate the schema file.");

  g_string_free(s, TRUE);

  return rv;
}

int main()
{
#if !GLIB_CHECK_VERSION(2,35,0)
  g_type_init();
#endif

  if (generate_schema_file())
    return 0;

  return -1;
}
