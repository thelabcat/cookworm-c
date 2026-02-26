#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
  int copy_chars = 0;
  int copy_chars_strlen = 0;
  FILE *wordlist = fopen("wordlist.txt", "r");
  FILE *wordlist_parsed = fopen("wordlist_parsed.txt", "w");
  char entry[99] = "";
  char word_ending[99] = "";
  char next_word[99] = "";
  char last_word[99] = "";
  fgets(entry, 99, wordlist);

  // Repeat until the string value read into the buffer is empty
  while (strlen(entry) > 0) {
    // printf("ENTRY: %s\n", entry);
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
    // word_ending[strlen(entry)] = '\0';

    // printf("COPY CHARS: %d\tWORD ENDING: %s\n", copy_chars, word_ending);

    // Do character copying
    if (copy_chars != 0) {

      // Erroneous presence of copy characters with nothing to copy
      if (strlen(last_word) == 0) {
        printf("ERROR: Copy characters was nonzero at start of file.\n");
        printf("INFO: Ignoring.\n");
      }

      // Not enough characters to copy
      else if (strlen(last_word) < copy_chars) {
        printf("ERROR: Entry '%s' has copy character characters quirement %d but previous entry '%s' is too short!",
               entry, copy_chars, last_word);
        return 1;
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

    // For next iteration
    memset(last_word, 0, sizeof(last_word));
    strcpy(last_word, next_word);
    memset(next_word, 0, sizeof(next_word));
    memset(entry, 0, sizeof(entry));
    memset(word_ending, 0, sizeof(word_ending));
    fgets(entry, 99, wordlist);
  }

}
