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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

void printhelp() {
  // Print the program help and exit
  printf("Cookworm-C BookWorm Parser - Parse and unparse the BookWorm Deluxe wordlist\n"
         "\n"
         "Usage: bwparser [-u] [infile.txt] [outfile.txt]\n"
         "       bwparser -h\n"
         "\n"
         "Options:\n"
         "\n"
         "  -u\tChange into unparsing mode. Defaults to parsing.\n"
         "  infile.txt\tThe text file to read from. Defaults to stdin.\n"
         "  outfile.txt\tThe text file to write to. Defaults to stdout.\n"
         "\n"
         "  -h\tInstead of operating, print this help and exit.\n"
         "\n"
         "S.D.G.\n"
  );
  exit(EXIT_SUCCESS);
}

void parse(FILE *wordlist, FILE *output) {
  /* Parse a compressed wordlist into plain words.
   * Will parse words as they arrive, so stdio is accepted.
   * Args:
   *  FILE *wordlist: Pointer to the input file.
   *  FILE *output: Pointer to the output.
   */

  int copy_chars = 0;  // Characters to copy from previous word
  int copy_chars_strlen = 0;  // How long IN characters the copy characters specifier of each entry was
  char entry[99] = "";  // Each line of the file at a time
  char word_ending[99] = "";  // The part of each entry that is the end of the new word
  char next_word[99] = "";  // Each new word as it is parsed
  char last_word[99] = "";  // Each previous parsed word

  // Repeat until we reach the end of the file
  while (feof(wordlist) == 0) {

    // Read another line
    fgets(entry, 99, wordlist);

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
    for (int j=copy_chars_strlen; j<strlen(entry) && entry[j] != '\n'; j++) {
      word_ending[j - copy_chars_strlen] = entry[j];
    }

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

    // Add the string part of the word entry
    strcat(next_word, word_ending);

    // Output
    fprintf(output, "%s\n", next_word);

    // Save the last word
    strcpy(last_word, next_word);

    // These strings will be set by character iteration, so we must fully clear them.
    memset(next_word, '\0', sizeof(next_word));
    memset(word_ending, '\0', sizeof(word_ending));

    /* fgets won't write anything to the entry buffer when it reaches EOF,
     * not even a null terminator. So, we must quick clear that.
     */
    entry[0] = '\0';
  }
}

void unparse(FILE *plainwords, FILE *wordlist) {
  /* Unparse a list of plain words into a compressed wordlest.
   * Will unparse words as they arrive, so stdio is accepted.
   * Args:
   *  FILE *plainwords: Pointer to the input file.
   *  FILE *wordlist: Pointer to the output file.
   */
  fprintf(stderr, "Unparse not implemented\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  errno = 0;

  bool unparse_mode = false;
  FILE *infile = stdin;
  FILE *outfile = stdout;

  // Parse all arguments
  for (int i=0; i<argc; i++) {

    // Skip the first option
    if (i == 0) {
      continue;
    }

    // Search through known flags, and lastly positional arguments
    // unparse flag set
    if (!strcmp(argv[i], "-u")) {
      unparse_mode = true;
    }

    // print help and exit
    else if (!strcmp(argv[i], "-h")) {
      printhelp();
    }

    // not a known flag argument

    // Unrecognized flag argument
    else if (argv[i][0] == '-') {
      fprintf(stderr, "Unrecognized option, '%s'\n", argv[i]);

      // It is a lone hyphen
      if (strlen(argv[i]) == 1) {
        fprintf(stderr, "Did you know the program defaults to stdio?\n");
      }

      exit(EXIT_FAILURE);
    }

    // Positional arguments:

    //infile is not set yet
    else if (infile == stdin) {
      infile = fopen(argv[i], "r");
      if (infile == NULL) {
        fprintf(stderr, "Could not open input file '%s', error %d:\n\t'%s'\n",
                argv[i], errno, strerror(errno));
        exit(EXIT_FAILURE);
      }
    }

    // outfile is not set yet
    else if (outfile == stdout) {
      outfile = fopen(argv[i], "w");
      if (outfile == NULL) {
        fprintf(stderr, "Could not open output file '%s', error %d:\n\t'%s'\n",
                argv[i], errno, strerror(errno));
        exit(EXIT_FAILURE);
      }
    }

    // both files are set, no recognized options
    else {
      fprintf(stderr, "Unrecognized positional option, '%s'\n", argv[i]);
      exit(EXIT_FAILURE);
    }
  }

  // Options are set, do our thing!
  if (!unparse_mode) {
    parse(infile, outfile);
  }
  else {
    unparse(infile, outfile);
  }

  fclose(infile);
  fclose(outfile);

  return 0;
}
