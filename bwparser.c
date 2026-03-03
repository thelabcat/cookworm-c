/* BookWorm Deluxe wordlist parser
 *
 * wordlist.txt contents are piped to stdin,
 * and parsed words are printed to stdout, one per line.
 *
 * Copyright 2026 A.D. Wilbur Jaywright dba Marswide BGL
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file exce*pt in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Some code used from http://www.crasseux.com/books/ctutorial/argp-example.html
 *
 * S.D.G.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <argp.h>

#define STR_SIZE 99

const char *argp_program_version =
"bwparser 1.0.0";

const char *argp_program_bug_address =
"<zargulthewizard@outlook.com>";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  FILE *infile;
  FILE *outfile;
  bool unparse;
};

/*
 *  OPTIONS.  Field 1 in ARGP.
 *  Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
 */
static struct argp_option options[] =
{
  {"unparse", 'u', 0, 0, "Use unparsing mode instead of parsing"},
  {0},  // Terminates this structure, otherwise we get ghost options
};


/*
 *  PARSER. Field 2 in ARGP.
 *  Order of parameters: KEY, ARG, STATE.
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key)
  {
    // Unparse mode set
    case 'u':
      arguments->unparse = true;
      break;

    // Positional arguments
    case ARGP_KEY_ARG:
      switch (state->arg_num) {
        // Input file
        case 0:
          arguments->infile = fopen(arg, "r");
          if (arguments->infile == NULL) {
            fprintf(stderr, "Could not open input file '%s', error %d:\n\t'%s'\n",
                    arg, errno, strerror(errno));
            exit(EXIT_FAILURE);
          }
          break;

        // Output file
        case 1:
          arguments->outfile = fopen(arg, "w");
          if (arguments->outfile == NULL) {
            fprintf(stderr, "Could not open output file '%s', error %d:\n\t'%s'\n",
                    arg, errno, strerror(errno));
            exit(EXIT_FAILURE);
          }
          break;

        // Invalid number of positional arguments
        default:
          argp_usage(state);
      }

    // Reached the end of arguments, do wrap up
    case ARGP_KEY_END:
      // Nothing to do here, we don't need positional arguments
      /*
      if (state->arg_num < 2)
      {
        argp_usage (state);
      }
      */
      break;

    // Invalid argument
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/*
 *  ARGS_DOC. Field 3 in ARGP.
 *  A description of the non-option command-line arguments
 *    that we accept.
 */
static char args_doc[] = "[INFILE] [OUTFILE]";

/*
 * DOC.  Field 4 in ARGP.
 * Program documentation.
 */
static char doc[] =
"Cookworm-C BookWorm Parser - Parse and unparse the BookWorm Deluxe wordlist";

/*
 *  The ARGP structure itself.
 */
static struct argp argp = {options, parse_opt, args_doc, doc};

void parse(FILE *wordlist, FILE *output) {
  /* Parse a compressed wordlist into plain words.
   * Will parse words as they arrive, so stdio is accepted.
   * Args:
   *  FILE *wordlist: Pointer to the input file.
   *  FILE *output: Pointer to the output.
   */

  int copy_chars = 0;  // Characters to copy from previous word
  int copy_chars_strlen = 0;  // How long IN characters the copy characters specifier of each entry was
  char entry[STR_SIZE] = "";  // Each line of the file at a time
  char word_ending[STR_SIZE] = "";  // The part of each entry that is the end of the new word
  char next_word[STR_SIZE] = "";  // Each new word as it is parsed
  char last_word[STR_SIZE] = "";  // Each previous parsed word

  // Repeat until we reach the end of the file
  while (feof(wordlist) == 0) {

    /* fgets won't write anything to the entry buffer when it reaches EOF,
     * not even a null terminator. So, we must quick clear that.
     */
    entry[0] = '\0';

    // Read another line
    fgets(entry, STR_SIZE, wordlist);

    // Skip blank lines and the ending empty line
    if (entry[0] == '\r' || entry[0] == '\n' || strlen(entry) == 0) {
      continue;
    }

    // Remove trailing newline and carriage return for lines that aren't the last
    while (entry[strlen(entry) - 1] == '\n' || entry[strlen(entry) - 1] == '\r') {
      entry[strlen(entry) - 1] = '\0';
    }

    // Look for numbers at the beginning of the string
    copy_chars_strlen = 0;
    for (int i=0; i<strlen(entry) && isdigit(entry[i]); i++) {
      // NOTE: No part of this loop will execute if the first character is not an integer

      // This is the first character? reset copy chars
      if (i == 0) {
        copy_chars = 0;
      }

      // Do the place value math and add the next digit
      copy_chars = (copy_chars * 10) + (entry[i] - '0');
      copy_chars_strlen++;
    }

    // Get the rest of the string
    for (int j=copy_chars_strlen; j<strlen(entry); j++) {
      word_ending[j - copy_chars_strlen] = entry[j];
    }
    word_ending[strlen(entry) - copy_chars_strlen] = '\0';

    // Do character copying
    if (copy_chars != 0) {

      // Erroneous presence of copy characters with nothing to copy
      // The start of last_word is null terminator
      if (*last_word == '\0') {
        fprintf(stderr, "WARNING: Copy characters was nonzero at start of file. Ignoring.\n");
      }

      // Not enough characters to copy
      else if (strlen(last_word) < copy_chars) {
        fprintf(stderr, "ERROR: Entry '%s' has copy character characters requirement %d but previous entry '%s' is too short!\n",
                entry, copy_chars, last_word);
        exit(EXIT_FAILURE);
      }

      // All go for copy
      else {
        for (int i=0; i<copy_chars; i++) {
          next_word[i] = last_word[i];
        }
      }
    }

    // Copy characters or not, null terminate the current word beginning
    next_word[copy_chars] = '\0';

    // Add the string part of the word entry
    strcat(next_word, word_ending);

    // Output
    fprintf(output, "%s\n", next_word);

    // Save the last word
    strcpy(last_word, next_word);
  }
}

void unparse(FILE *plainwords, FILE *wordlist) {
  /* Unparse a list of plain words into a compressed wordlest.
   * Will unparse words as they arrive, so stdio is accepted.
   * Args:
   *  FILE *plainwords: Pointer to the input file.
   *  FILE *wordlist: Pointer to the output file.
   */

  int copy_chars = 0;  // Characters to copy from previous word
  int copy_chars_strlen = 0;  // How long IN characters the copy characters specifier of each entry was
  int last_copy_chars = 0;  // The last copy characters count
  char word[STR_SIZE];  // Each input word
  int word_len = 0;  // strlen(word) currently
  char last_word[STR_SIZE] = "";  // Each previous word
  char entry[STR_SIZE] = "";  // The next entry being built

  // Repeat until we reach the end of the file
  while (feof(plainwords) == 0) {
    /* fgets won't write anything to the word buffer when it reaches EOF,
     * not even a null terminator. So, we must quick clear that.
     */
    word[0] = '\0';

    // Read another line
    fgets(word, STR_SIZE, plainwords);
    word_len = strlen(word);

    // Skip blank lines and the ending empty line
    if (word[0] == '\r' || word[0] == '\n' || word_len == 0) {
      continue;
    }

    // Remove trailing newline and carriage return for lines that aren't the last
    while (word[word_len - 1] == '\n' || word[word_len - 1] == '\r') {
      word[word_len - 1] = '\0';
      word_len--;
    }

    // If this is the first word, write it and go on
    if (last_word[0] == '\0') {
      fprintf(wordlist, "%s\r\n", word);
      strcpy(last_word, word);
      continue;
    }

    // Find how many characters match the previous word
    for (int i=0; i<strlen(last_word); i++) {
      if (last_word[i] == word[i]) {
        copy_chars = i + 1;
      }
      else {
        break;
      }
    }

    // The copy characters are different
    if (copy_chars != last_copy_chars) {
      snprintf(entry, STR_SIZE, "%d", copy_chars);
      copy_chars_strlen = strlen(entry);
    }
    // They are not different
    else {
      copy_chars_strlen = 0;
    }

    // Note the non-copied characters
    for (int i=copy_chars; i<word_len; i++) {
      entry[i - copy_chars + copy_chars_strlen] = word[i];
    }
    entry[word_len - copy_chars + copy_chars_strlen] = '\0';

    fprintf(wordlist, "%s\r\n", entry);

    // Store stuff for next time, and reset
    last_copy_chars = copy_chars;
    copy_chars = 0;
    strcpy(last_word, word);
  }
}

int main(int argc, char *argv[]) {
  errno = 0;

  /* Set argument defaults */
  struct arguments arguments;
  arguments.unparse = false;
  arguments.infile = stdin;
  arguments.outfile = stdout;

  // Parse arguments
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  // Options are set, do our thing!
  if (arguments.unparse) {
    unparse(arguments.infile, arguments.outfile);
  }
  else {
    parse(arguments.infile, arguments.outfile);
  }

  fclose(arguments.infile);
  fclose(arguments.outfile);

  return 0;
}
