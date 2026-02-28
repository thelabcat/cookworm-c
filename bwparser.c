#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
  int copy_chars = 0;  // Characters to copy from previous word
  int copy_chars_strlen = 0;  // How long IN characters the copy characters specifier of each entry was
  FILE *wordlist = fopen("wordlist.txt", "r");  // The input file
  // FILE *wordlist_parsed = fopen("wordlist_parsed.txt", "w");
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

    // printf("ENTRY: '%s'\n", entry);

    // Look for numbers at the beginning of the string
    copy_chars_strlen = 0;
    for (int i=0; i<strlen(entry) && isdigit(entry[i]); i++) {
      // The character is an integer if we are even in this loop
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

    // printf("COPY CHARS: %d\tWORD ENDING: '%s'\n", copy_chars, word_ending);

    // Do character copying
    if (copy_chars != 0) {

      // Erroneous presence of copy characters with nothing to copy
      // The start of last_word is null terminator
      if (*last_word == '\0') {
        printf("WARNING: Copy characters was nonzero at start of file. Ignoring.\n");
      }

      // Not enough characters to copy
      else if (strlen(last_word) < copy_chars) {
        printf("ERROR: Entry '%s' has copy character characters quirement %d but previous entry '%s' is too short!",
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

    // Reset stuff for next iteration
    strcpy(last_word, next_word);
    memset(next_word, '\0', sizeof(next_word));
    memset(entry, '\0', sizeof(entry));
    memset(word_ending, '\0', sizeof(word_ending));
    // fgets(entry, 99, wordlist);
  }
  
  return 0;
}
