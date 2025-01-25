#include "config.h"

#include <glib-object.h>
#include <gconf/gconf-client.h>

#include <hildon-im-ui.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SCHEMA_FILE GCONF_SCHEMA_DIR "/hildon-input-method-configuration.schema"
#define INSTALL_SCHEMA_FILE_CMD "gconftool-2 --makefile-install-rule " SCHEMA_FILE

#define HILDON_IM_GCONF_INT_KB_MODEL HILDON_IM_GCONF_DIR "/int_kb_model"
#define HILDON_IM_GCONF_SLIDE_LAYOUT HILDON_IM_GCONF_DIR "/slide-layout"

#define XKBMODEL "XKBMODEL="
#define XKBLAYOUT "XKBLAYOUT="

static char *get_slide_layout()
{
  GConfClient *gconf = gconf_client_get_default();
  char *slide_layout;

  /* slide_layout = osso_get_product_info(OSSO_PRODUCT_KEYBOARD); */
  slide_layout = g_strdup("English, Dutch");

  if (slide_layout)
  {
    gconf_client_set_string(gconf, HILDON_IM_GCONF_SLIDE_LAYOUT,
                            slide_layout, NULL);
  }

  g_object_unref(gconf);

  return slide_layout;
}

static gchar *
get_keyboard_val(const char *key)
{
  char *val = NULL;
  char buf[256];
  FILE *fp = fopen("/etc/default/keyboard", "r");
  int len = strlen(key);

  if (!fp)
    return NULL;

  while (fgets(buf, sizeof(buf), fp))
  {
    if (!memcmp(buf, key, len))
    {
      val = strtok(&buf[len], "\"");
      break;
    }
  }

  fclose(fp);

  if (val)
    val = g_strdup(val);

  return val;
}

static gchar *
get_int_kb_model()
{
  char *model = get_keyboard_val(XKBMODEL);
  GConfClient *gconf = gconf_client_get_default();

  if (model)
    gconf_client_set_string(gconf, HILDON_IM_GCONF_INT_KB_MODEL, model, NULL);

  g_object_unref(gconf);

  return model;
}

static gchar *
get_int_kb_layout()
{
  return get_keyboard_val(XKBLAYOUT);
}

static gboolean
get_have_int_kb(const gchar *int_kb_model)
{

  if (!int_kb_model)
    return FALSE;

  /* add more supported, droid4 for example */
  return !g_ascii_strcasecmp(int_kb_model, "nokiarx51") || !g_ascii_strcasecmp(int_kb_model, "motoroladroid4");
}

static gboolean
generate_schema_file()
{
  gchar *slide_layout = get_slide_layout();
  gchar *int_kb_model = get_int_kb_model();
  GString *s = g_string_new("<gconfschemafile>\n\t<schemalist>\n");
  const gchar *have_int_kb;
  gboolean rv = FALSE;
  gint exit_status;

  g_debug("int keyboard model is [%s] --- \n", int_kb_model);

  s = g_string_append(s, "\t\t<schema>\n"
                      "\t\t\t<key>/schemas/apps/osso/inputmethod/have-internal-keyboard</key>\n"
                      "\t\t\t<applyto>/apps/osso/inputmethod/have-internal-keyboard</applyto>\n"
                      "\t\t\t<owner>hildon-input-method</owner>\n"
                      "\t\t\t<type>bool</type>\n"
                      "\t\t\t<default>");

  if (get_have_int_kb(int_kb_model))
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
    gchar *kb_layout = get_int_kb_layout();

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
    s = g_string_append(s, "nokiarx44");

  g_free(int_kb_model);

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
                      "\t\t\t<default>iso_codes_locale_resolve_simple</default>\n"
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
                      "\t\t\t<default>libiso-codes-locale-resolver.so.0</default>\n"
                      "\t\t\t<locale name=\"C\">\n"
                      "\t\t\t\t<short>Endonyms resolver</short>\n"
                      "\t\t\t\t<long>Endonyms resolver</long>\n"
                      "\t\t\t</locale>\n"
                      "\t\t</schema>\n");

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
