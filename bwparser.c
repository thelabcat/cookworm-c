/* BookWorm Deluxe wordlist parser
 *
 * wordlist.txt contents are piped to stdin,
 * and parsed words are printed to stdout, one per line.
 *
 * Copyright 2026, Wilbur Jaywright dba Marswide BGL.
 *
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Read directly from input
#define wordlist stdin

int main() {
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
    printf("%s\n", next_word);

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
  
  return 0;
}
