/*
About this program:
- This program counts words.
- The specific words that will be counted are passed in as command-line
  arguments.
- The program reads words (one word per line) from standard input until EOF or
  an input line starting with a dot '.'
- The program prints out a summary of the number of times each word has
  appeared.
- Various command-line options alter the behavior of the program.

E.g., count the number of times 'cat', 'nap' or 'dog' appears.
> ./main cat nap dog
Given input:
 cat
 .
Expected output:
 Looking for 3 words
 Result:
 cat:1
 nap:0
 dog:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smp0_tests.h"

#define LENGTH(s) (sizeof(s) / sizeof(*s))

/* Structures */
typedef struct {
  char *word;
  int counter;
} WordCountEntry;


int process_stream(WordCountEntry entries[], int entry_count)
{
  short line_count = 0;
  char buffer[30];
  char *word_token;

  while (fgets(buffer, 30, stdin)) {

    //Enhancements - Q4
    // removing newline character from buffer char array which fgets stores in it
    int buffer_length = strlen(buffer); 
    if(buffer_length > 0 && buffer[buffer_length -1] == '\n'){
      buffer[buffer_length -1] = '\0';
    }

    // Q4 - Or function strcspn can be used
    // buffer[strcspn(buffer,"\n")] = '\0';

    if (*buffer == '.')
      break;

    /* Enhancements Q5 */
    // Separates out first word
    word_token = strtok(buffer, " ");

    while(word_token!=NULL){
      /* Compare against each entry */
      for(int i = 0; i < entry_count; i++){
        /*Debugging- Q5 */
        if (!strcmp(entries[i].word, word_token)){
          entries[i].counter++;
        }
      }
      // Separates the next word
      word_token = strtok(NULL, " ");
    }
    line_count++;
  }
  return line_count;
}


void print_result(WordCountEntry entries[], int entry_count)
{
  fprintf(stdout,"Result:\n");
  int i = 0;
  while (entry_count-- > 0) {
    // Changed for Debugging - Q5
    fprintf(stdout,"%s:%d\n", entries[i].word, entries[i].counter);
    i++;
  }
}


void printHelp(const char *name)
{
  fprintf(stderr, "usage: %s [-h] <word1> ... <wordN>\n", name);
}

int main(int argc, char **argv)
{
  const char *prog_name = *argv;

  // WordCountEntry entries[5] ;
  /* Another way to add support for matching arbitrary numbers of words */
  // WordCountEntry entries[argc] ;
  /* Enhancements - Q3 */
  WordCountEntry *entries = (WordCountEntry *)malloc(sizeof(WordCountEntry)*argc) ;
  int entryCount = 0;

  // To support a writing to a filename written in the command line
  FILE *outputFile = NULL;
  char *filename = malloc( sizeof(*filename) );

  /* Entry point for the testrunner program */
  if (argc > 1 && !strcmp(argv[1], "-test")) {
    run_smp0_tests(argc - 1, argv + 1);
    return EXIT_SUCCESS;
  }
  while (*argv != NULL) {
      //if an argv starts with -
    if (**argv == '-') {
      switch ((*argv)[1]) {
        case 'h':
          printHelp(prog_name);
          //return fail code added for Debugging - Q3. Test case requires break
          // return EXIT_FAILURE;
          break;
        case 'f':

          strcpy(filename, (*(argv)) + 2);

          //open the specified file and redirect stdout to it
          outputFile = freopen(filename, "w", stdout);

          break;
        default:

          fprintf(stderr,"%s: Invalid option %s. Use -h for help.\n",
                 prog_name, *argv);
          //return fail code for debugging question 3. Test case requires break
          // return EXIT_FAILURE;
          break;
      }
    } else {
      if (entryCount < argc && *argv!=prog_name) {
        /* Debugging - Q4*/
        // before: nothing in entries[4].word but something in entries[4].counter
        entries[entryCount].word = *argv;
        entries[entryCount].counter = 0; 
        entryCount++;

      }
    }
    argv++;
  }
  // //Added variable to count number of words - Different method
  // int count_words = entryCount - 1;
  if (entryCount == 0) {
    fprintf(stderr,"%s: Please supply at least one word. Use -h for help.\n",
           prog_name);
    return EXIT_FAILURE;
  }
  if (entryCount == 1) {
    fprintf(stdout, "Looking for a single word\n");
  } else {
    fprintf(stdout, "Looking for %d words\n", entryCount);
  }

  process_stream(entries, entryCount);
  print_result(entries, entryCount);

  // Check if output file was created and close it
  if(outputFile == fopen(filename, "w")){
    fclose(outputFile);
  }
  free( entries);

  return EXIT_SUCCESS;
}
