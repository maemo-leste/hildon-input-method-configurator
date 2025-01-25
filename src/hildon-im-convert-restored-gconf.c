#include "config.h"

#include <gconf/gconf-client.h>
#include <expat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SCHEMA_FILE GCONF_SCHEMA_DIR "/hildon-input-method-configuration.schema"

typedef enum
{
  UNDEFINED = 0,
  CURRENT_LANGUAGE = 1,
  PRIMARY_LANGUAGE = 2,
  SECONDARY_LANGUAGE = 3,
  PRIMARY_COMPLETION_LANGUAGE = 4,
  SECONDARY_COMPLETION_LANGUAGE = 5,
  WORD_COMPLETION = 6,
  AUTO_CAPITALIZATION = 7,
  NEXT_WORD_PREDICTION = 8,
  SPACE_AFTER = 9
} TAG_TYPES;

static const char *versions[] =
{
  "OSSO1.1",
  "2008SE",
  "DIABLO"
};

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define swrite(s, fp) fwrite(s, 1, sizeof(s) - 1, fp)

#define _strncmp(s1, s2) strncmp(s1, s2, sizeof(s2) - 1)

typedef struct
{
  int current_language;
  gboolean word_completion;
  gboolean auto_capitalisation;
  gboolean next_word_prediction;
  gboolean insert_space_after_word;
  char *primary_language;
  char *secondary_language;
  char *primary_completion_language;
  char *secondary_completion_language;
  FILE *fp;
  gboolean tag_open;
  TAG_TYPES element_type;
  gboolean duplicate_data;
  gboolean stringvalue;
} gconf_data;

static const char *
format_string_entry(const char *name, const char *value)
{
  static char string_buf[1024];

  if (value)
  {
    snprintf(string_buf, sizeof(string_buf),
             "\t<entry name=\"%s\" mtime=\"%ld\" type=\"string\">\n\t\t<stringvalue>%s</stringvalue>\n\t</entry>\n",
             name, time(0), value);
  }
  else
  {
    snprintf(string_buf, sizeof(string_buf),
             "\t<entry name=\"%s\" mtime=\"%ld\" type=\"string\">\n\t\t<stringvalue/>\n\t</entry>\n",
             name, time(0));
  }

  return string_buf;
}

static const char *
format_bool_entry(const char *name, gboolean val)
{
  static char bool_buf[1024];

  snprintf(bool_buf, sizeof(bool_buf),
           "\t<entry name=\"%s\" mtime=\"%ld\" type=\"bool\" value=\"%s\"></entry>\n",
           name, time(0), val ? "true" : "false");

  return bool_buf;
}

static const char *
format_int_entry(const char *name, int val)
{
  static char int_buf[1024];

  snprintf(int_buf, sizeof(int_buf),
          "\t<entry name=\"%s\" mtime=\"%ld\" type=\"int\" value=\"%d\"></entry>\n",
          name, time(0), val);

  return int_buf;
}

static void
char_data_handler(gconf_data *userData, const XML_Char *s, int len)
{
  if (userData->tag_open)
  {
    char *tmp = strndup(s, len);

    fputs(tmp, userData->fp);
    free(tmp);
    return;
  }
  else if (!userData->stringvalue)
    return;

  switch ( userData->element_type )
  {
    case PRIMARY_LANGUAGE:
      if (userData->primary_language)
      {
        userData->duplicate_data = TRUE;
        eprintf("Primary language is already defined: %s",
                userData->primary_language);
      }
      else
        userData->primary_language = strndup(s, len);

      break;
    case SECONDARY_LANGUAGE:
      if (userData->secondary_language)
      {
        userData->duplicate_data = TRUE;
        eprintf("Secondary language is already defined: %s",
                userData->secondary_language);
      }
      else
        userData->secondary_language = strndup(s, len);

      break;
    case PRIMARY_COMPLETION_LANGUAGE:
      if (userData->primary_completion_language)
      {
        userData->duplicate_data = TRUE;
        eprintf("Primary completion language is already defined: %s",
                userData->primary_completion_language);
      }
      else
        userData->primary_completion_language = strndup(s, len);

      break;
    case SECONDARY_COMPLETION_LANGUAGE:
      if (userData->secondary_completion_language)
      {
        userData->duplicate_data = TRUE;
        eprintf("Secondary completion language is already defined: %s",
                userData->secondary_completion_language);
      }
      else
        userData->secondary_completion_language = strndup(s, len);

      break;
    default:
      return;
  }
}

static int
write_gconf_schema(gboolean primary, const char *dir, gconf_data *data)
{
  char *lang_dir;
  char *fname;
  FILE *fp;
  const char *lang;

  if (primary && !data->primary_language)
    return FALSE;

  if (!primary && !data->secondary_language)
    return FALSE;

  lang = primary ? data->primary_language : data->secondary_language;

  lang_dir = (char *)malloc(strlen(dir) + strlen(lang) + 1);

  if (!lang_dir)
  {
    swrite("Unable to allocate memory. (xfilename#0)\n", stderr);
    return 1;
  }

  sprintf(lang_dir, "%s%s", dir, lang);
  mkdir(lang_dir, 0755);
  free(lang_dir);

  fname = (char *)malloc(strlen(dir)+ strlen(lang) + strlen("%gconf.xml") + 2);

  if (!fname)
  {
    swrite("Unable to allocate memory. (xfilename#1)\n", stderr);
    return 1;
  }

  sprintf(fname, "%s%s/%s", dir, lang, "%gconf.xml");

  fp = fopen(fname, "w");
  if (!fp)
  {
      eprintf("Unable open the file %s for writing: %s\n", fname,
              strerror(errno));
      return 1;
  }

  free(fname);
  swrite("<?xml version=\"1.0\"?>\n<gconf>\n", fp);
  fputs(format_bool_entry("word-completion", data->word_completion), fp);
  fputs(format_bool_entry(
          "auto-capitalisation", data->auto_capitalisation), fp);

  fputs(format_bool_entry(
          "next-word-prediction", data->next_word_prediction), fp);
  fputs(format_bool_entry(
          "insert-space-after-word", data->insert_space_after_word), fp);
  fputs(format_string_entry(
          "dictionary", primary ? data->primary_completion_language :
                                  data->secondary_completion_language), fp);
  swrite("</gconf>\n", fp);
  fclose(fp);

  return 0;
}

static void
start_handler(gconf_data *userData, const char *name, const char **attrs)
{
  int i;
  gboolean tag_open;

  userData->stringvalue = FALSE;

  if (!_strncmp(name, "entry"))
  {
    userData->element_type = UNDEFINED;
    tag_open = FALSE;

    for (i = 0; attrs[i]; i += 2)
    {
      const char *attr_name = attrs[i];
      const char *attr_val = attrs[i + 1];

      if (!_strncmp(attr_name, "name"))
      {
        userData->tag_open = FALSE;

        if (!_strncmp(attr_val, "current_language"))
          userData->element_type = CURRENT_LANGUAGE;
        else if (!_strncmp(attr_val, "primary_language"))
          userData->element_type = PRIMARY_LANGUAGE;
        else if (!_strncmp(attr_val, "secondary_language"))
          userData->element_type = SECONDARY_LANGUAGE;
        else if (!_strncmp(attr_val, "primary_completion_language"))
          userData->element_type = PRIMARY_COMPLETION_LANGUAGE;
        else if (!_strncmp(attr_val, "secondary_completion_language"))
          userData->element_type = SECONDARY_COMPLETION_LANGUAGE;
        else if (!_strncmp(attr_val, "word_completion"))
          userData->element_type = WORD_COMPLETION;
        else if (!_strncmp(attr_val, "auto_capitalization"))
          userData->element_type = AUTO_CAPITALIZATION;
        else if (!_strncmp(attr_val, "next_word_prediction"))
          userData->element_type = NEXT_WORD_PREDICTION;
        else if (!_strncmp(attr_val, "space_after"))
          userData->element_type = SPACE_AFTER;
        else
        {
          userData->tag_open = TRUE;

          if (!tag_open)
          {
            fprintf(userData->fp, "<%s", name);
            tag_open = TRUE;
          }

          if (!_strncmp(attr_val, "dual_dictionary"))
            fprintf(userData->fp, " %s=\"dual-dictionary\"", attr_name);
          else
            fprintf(userData->fp, " %s=\"%s\"", attr_name, attr_val);
        }
      }
      else if (!_strncmp(attr_name, "value"))
      {
        switch (userData->element_type)
        {
          case CURRENT_LANGUAGE:
            userData->current_language = atoi(attr_val);
            userData->element_type = UNDEFINED;
            break;
          case WORD_COMPLETION:
            userData->word_completion = !_strncmp(attr_val, "true");
            userData->element_type = UNDEFINED;
            break;
          case AUTO_CAPITALIZATION:
            userData->auto_capitalisation = !_strncmp(attr_val, "true");
            userData->element_type = UNDEFINED;
            break;
          case NEXT_WORD_PREDICTION:
            userData->next_word_prediction = !_strncmp(attr_val, "true");
            userData->element_type = UNDEFINED;
            break;
          case SPACE_AFTER:
            userData->insert_space_after_word = !_strncmp(attr_val, "true");
            userData->element_type = UNDEFINED;
            break;
          default:
            if (userData->tag_open)
              fprintf(userData->fp, " %s=\"%s\"", attr_name, attr_val);
            break;
        }
      }
      else if (userData->tag_open)
      {
        if (!tag_open)
        {
          fprintf(userData->fp, "<%s", name);
          tag_open = TRUE;
        }

        fprintf(userData->fp, " %s=\"%s\"", attr_name, attr_val);
      }
    }
  }
  else
  {
    if (!_strncmp(name, "stringvalue"))
      userData->stringvalue = TRUE;

    if (userData->tag_open)
    {
      fprintf(userData->fp, "<%s", name);

      for (i = 0; attrs[i]; i += 2)
        fprintf(userData->fp, " %s=\"%s\"", attrs[i], attrs[i + 1]);
    }
  }

  if (userData->tag_open)
    fputc('>', userData->fp);

}

static void
end_handler(gconf_data *userData, const char *name)
{
  if (userData->tag_open)
    fprintf(userData->fp, "</%s>", name);

  userData->element_type = UNDEFINED;
}

int
main(int argc, char *argv[])
{
  size_t i;
  FILE *fp_in;
  char *gconfname;
  FILE *fp_xml;
  int tempfile;
  struct XML_ParserStruct *parser;
  int len;
  int isFinal;
  char *langdirname;
  char *himgconf;
  FILE *fp_gconf;
  char *tempname;
  char *gconfdir;
  char xml_buf[1024];
  char buf[1024];
  gconf_data data;

  if (argc != 3)
  {
    printf("Usage: %s file-name backup-version\n", *argv);
    exit(1);
  }

  for (i = 0; i < sizeof(versions) / sizeof (versions[0]); i++)
  {
    if (strstr(argv[2], versions[i]) != NULL)
      break;
  }

  if (i == sizeof (versions[0]))
  {
    unlink(SCHEMA_FILE);
    unlink("/etc/hildon-input-method.configured");
    exit(0);
  }

  fp_in = fopen(argv[1], "r");

  if (!fp_in)
  {
    eprintf("File %s couldn't be opened\n", argv[1]);
    exit(1);
  }

  do
  {
    if (!fgets(buf, sizeof(buf), fp_in))
    {
      fclose(fp_in);
      return 0;
    }
  }
  while (!strstr(buf, "/apps/osso/inputmethod/%gconf.xml"));

  buf[strlen(buf) - 1] = 0;

  gconfname = strdup(buf);

  gconfdir = dirname(gconfname);

  tempname = (char *)malloc(strlen(buf) + 8);

  sprintf(tempname, "%s.XXXXXX", buf);
  memset(&data, 0, sizeof(data));

  fp_xml = fopen(buf, "r");

  if (fp_xml == NULL)
  {
    eprintf("Can't open %s for reading: %s\n", buf, strerror(errno));
    free(tempname);
    return 1;
  }

  tempfile = mkstemp(tempname);
  data.fp = fdopen(tempfile, "w");
  data.tag_open = TRUE;

  if (!data.fp)
  {
    eprintf("Can't open %s for writing: %s\n", buf, strerror(errno));
    free(tempname);
    fclose(fp_xml);
    return 1;
  }

  parser = XML_ParserCreate(NULL);

  if (!parser)
  {
    swrite("Can't initialize the XML parser\n", stderr);
    goto err;
  }

  swrite("<?xml version=\"1.0\"?>\n", data.fp);

  XML_SetUserData(parser, &data);
  XML_SetElementHandler(parser, (XML_StartElementHandler)start_handler,
                        (XML_EndElementHandler)end_handler);
  XML_SetCharacterDataHandler(parser,
                              (XML_CharacterDataHandler)char_data_handler);

  do
  {
    len = fread(xml_buf, 1, sizeof(xml_buf), fp_xml);
    isFinal = feof(fp_xml);

    if (XML_Parse(parser, xml_buf, len, isFinal) == XML_STATUS_ERROR)
    {
      eprintf("Parsing error on line %lu: %s\n",
              XML_GetCurrentLineNumber(parser),
              XML_ErrorString(XML_GetErrorCode(parser)));
      goto err;
    }
  }
  while (!isFinal);

  XML_ParserFree(parser);
  swrite("\n", data.fp);

  langdirname = (char *)malloc(strlen(gconfdir) +
                               sizeof("hildon-im-languages/") + 1);

  if (!langdirname)
  {
    swrite("Unable to allocate memory for langdirname\n", stderr);
    goto err;
  }

  sprintf(langdirname, "%s/%s", gconfdir, "hildon-im-languages/");
  mkdir(langdirname, 0755);
  himgconf = (char *)malloc(strlen(langdirname) + 12);

  if (!himgconf)
  {
    swrite("Unable to allocate memory for xfilename\n", stderr);
    goto err;
  }

  sprintf(himgconf, "%s%s", langdirname, "%gconf.xml");
  fp_gconf = fopen(himgconf, "w");

  if (!fp_gconf)
  {
    eprintf("Unable open the file %s for writing: %s\n", himgconf,
            strerror(errno));
    goto err;
  }

  swrite("<?xml version=\"1.0\"?>\n<gconf>\n", fp_gconf);

  fputs(format_int_entry("current", data.current_language), fp_gconf);
  fputs(format_string_entry("language-0", data.primary_language), fp_gconf);
  fputs(format_string_entry("language-1", data.secondary_language), fp_gconf);

  swrite("</gconf>\n", fp_gconf);
  fclose(fp_gconf);
  free(himgconf);

  write_gconf_schema(TRUE, langdirname, &data);
  write_gconf_schema(FALSE, langdirname, &data);

  free(gconfdir);
  free(langdirname);
  unlink(buf);

  if (rename(tempname, buf))
  {
    eprintf("Unable to move %s to %s: %s\n", tempname, buf,
            strerror(errno));
    goto err;
  }
  else
    chmod(buf, 0644);

  return 0;

err:
  free(tempname);
  fclose(data.fp);
  fclose(fp_xml);

  return 1;
}
