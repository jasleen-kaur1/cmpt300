/* calc.c - Multithreaded calculator */

#include "calc.h"

pthread_t adderThread;
pthread_t degrouperThread;
pthread_t multiplierThread;
pthread_t readerThread;
pthread_t sentinelThread;

// buffer array holds the inputted expressions, separated by semi-colons. No need to implement semicolon.
char buffer[BUF_SIZE];
int num_ops;


/* Utiltity functions provided for your convenience */

/* int2string converts an integer into a string and writes it in the
   passed char array s, which should be of reasonable size (e.g., 20
   characters).  */
char *int2string(int i, char *s)
{
    sprintf(s, "%d", i);
    return s;
}

/* string2int just calls atoi() */
int string2int(const char *s)
{
    return atoi(s);
}

/* isNumeric just calls isdigit() */
int isNumeric(char c)
{
    return isdigit(c);
}

/* End utility functions */


void printErrorAndExit(char *msg)
{
    msg = msg ? msg : "An unspecified error occured!";
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int timeToFinish()
{
    /* be careful: timeToFinish() also accesses buffer */
    return buffer[0] == '.';
}

/* Looks for an addition symbol "+" surrounded by two numbers, e.g. "5+6"
   and, if found, adds the two numbers and replaces the addition subexpression 
   with the result ("(5+6)*8" becomes "(11)*8")--remember, you don't have
   to worry about associativity! */
void *adder(void *arg)
{
    int bufferlen;
    int value1, value2;
    int startOffset, remainderOffset;
    int i;
    char total_value[20];
    int val1_length;
    int val2_length;

    // return NULL; /* remove this line */

    while (1) {
		startOffset = remainderOffset = -1;
		value1 = value2 = -1;
		val1_length = val2_length = -1;


		// See . symbol and stop
		if (timeToFinish()) {
		    return NULL;
		}

		//&buffer[i] returns from i to the end of the string?  --- HELP

		/* storing this prevents having to recalculate it in the loop */
		bufferlen = strlen(buffer);
		// printf("bufferlen %d \n", bufferlen);

		for (i = 0; i < bufferlen; i++) {
			// char * str = &buffer[i];
			// printf("buffer: %c , i: %d, bufferlen: %d, str: %s \n", buffer[i], i , bufferlen, str);
 

 			// PLEASE HELP! WHY DOES &buffer[i] return everything from i to end of string?
 			// char * str = &buffer[i];
 			// printf("&buffer[i]: %s, buffer[i]: %c \n", str, buffer[i]);

 			// int value = string2int(&buffer[i]);
 			// printf("Integer value at &buffer[i]: %d, Value at buffer[i]: %c \n", value, buffer[i]);

			if (isNumeric(buffer[i])){

			  	value1 = string2int(&buffer[i]);

			  	// length of first number
			  	char string_value1[20];
				val1_length = strlen(int2string(value1, string_value1));

				// printf("Next number: %c \n", buffer[i+val1_length+1]);
				// break;
				if(buffer[i+val1_length] == '+' && isNumeric(buffer[i+val1_length+1])){
					startOffset = i;
					value2 = string2int(&(buffer[i+val1_length+1]));

					// length of last number
					char string_value2[20];
					val2_length = strlen(int2string(value2, string_value2));
					remainderOffset = startOffset + val1_length + val2_length;

					// printf("remainderOffset: %d \n", remainderOffset);
					
					// convert total value to string to place back in buffer
					int2string(value1+value2, total_value);
					// printf("total_value: %s \n", total_value);
					
					// copy and replace v1+v2 with total value in buffer
					strcpy(&buffer[i], total_value);
					int total_value_length = strlen(total_value);

					// shift remainder buffer after total value
					strcpy(&buffer[startOffset + total_value_length], &buffer[remainderOffset +1] );
		  			

					// printf("i value: %d, Integer value at buffer[i]: %s \n", i, &buffer[i]);

				}

			}
			 // do we have value1 already?  If not, is this a "naked" number?
		    // if we do, is the next character after it a '+'?
		    // if so, is the next one a "naked" number?

		    // once we have value1, value2 and start and end offsets of the
		    // expression in buffer, replace it with v1+v2
		}

		// something missing?

    }

}

/* Looks for a multiplication symbol "*" surrounded by two numbers, e.g.
   "5*6" and, if found, multiplies the two numbers and replaces the
   mulitplication subexpression with the result ("1+(5*6)+8" becomes
   "1+(30)+8"). */
void *multiplier(void *arg)
{
    int bufferlen;
    int value1, value2;
    int startOffset, remainderOffset;
    int i;
    char total_value[20];
    int val1_length;
    int val2_length;

    // return NULL; /* remove this line */

    while (1) {
		startOffset = remainderOffset = -1;
		value1 = value2 = -1;
		val1_length = val2_length = -1;

		if (timeToFinish()) {
		    return NULL;
		}

		/* storing this prevents having to recalculate it in the loop */
		bufferlen = strlen(buffer);

		for (i = 0; i < bufferlen; i++) {
		    // same as adder, but v1*v2
		
			if (isNumeric(buffer[i])){

			  	value1 = string2int(&buffer[i]);

			  	// length of first number
			  	char string_value1[20];
				val1_length = strlen(int2string(value1, string_value1));

				// printf("Next number: %c \n", buffer[i+val1_length+1]);
				// break;
				if(buffer[i+val1_length] == '*' && isNumeric(buffer[i+val1_length+1])){
					startOffset = i;
					value2 = string2int(&(buffer[i+val1_length+1]));

					// length of last number
					char string_value2[20];
					val2_length = strlen(int2string(value2, string_value2));
					remainderOffset = startOffset + val1_length + val2_length;

					// printf("remainderOffset: %d \n", remainderOffset);
					
					// convert total value to string to place back in buffer
					int2string(value1*value2, total_value);
					// printf("total_value: %s \n", total_value);
					
					// copy and replace v1+v2 with total value in buffer
					strcpy(&buffer[i], total_value);
					int total_value_length = strlen(total_value);
					// printf("startOffset: %d, &buffer[i]: %s, &buffer[remainderOffset +1]: %s \n",i, &buffer[i], &buffer[remainderOffset +1]);

					// shift remainder buffer after total value
					strcpy(&buffer[startOffset + total_value_length], &buffer[remainderOffset +1] );
		  			

					// printf("i value: %d, Integer value at buffer[i]: %s \n", i, &buffer[i+2]);
					break;
				}

			}
	// something missing?
		}
		// if(value1!= -1){
		// 	break;
		// }
    }
         // printf("total %s \n", total_value);

}


/* Looks for a number immediately surrounded by parentheses [e.g.
   "(56)"] in the buffer and, if found, removes the parentheses leaving
   only the surrounded number. */
void *degrouper(void *arg)
{
    int bufferlen;
    int i;

    // int flag=0;

    // return NULL; /* remove this line */

    while (1) {

    	// number_length = -1;

		if (timeToFinish()) {
		    return NULL;
		}

		/* storing this prevents having to recalculate it in the loop */
		bufferlen = strlen(buffer);

		for (i = 0; i < bufferlen; i++) {
			 if (buffer[i] == '(' && isNumeric(buffer[i+1])) {

			 	// grab number in paranthesis
			 	int number = string2int(&buffer[i+1]);

			 	// length of naked number
			  	char string_value[20];
				int number_length = strlen(int2string(number, string_value));

				// check if after naked number is a ')'
				if(buffer[i + number_length +1] == ')'){

					int closing_bracket_index = i + number_length +1;
					// printf("buffer: %s \n", &buffer[i + number_length +1]);

					/* remove '(' and shift the remainder of the string to the left */
		    		printf("&buffer[i]: %s, &buffer[i+1]: %s \n", &buffer[i], &buffer[i+1]);
					strcpy(&buffer[i], &buffer[i + 1]);

					/* remove ')' and shift the remainder of the string to the left */
					strcpy(&buffer[closing_bracket_index], &buffer[closing_bracket_index +1]);

					printf("Number: %d, Number length: %d, &buffer[i]: %s, i: %d \n", number, number_length, &buffer[i], i);
					// flag = 1


				}

			 	// break;

			 }

		    // check for '(' followed by a naked number followed by ')'
		    // remove ')' by shifting the tail end of the expression
		    // remove '(' by shifting the beginning of the expression
		}
		// if(flag == 1){
		// 	break;
		// }
	// something missing?
    }
}


/* sentinel waits for a number followed by a ; (e.g. "453;") to appear
   at the beginning of the buffer, indicating that the current
   expression has been fully reduced by the other threads and can now be
   output.  It then "dequeues" that expression (and trailing ;) so work can
   proceed on the next (if available). */
void *sentinel(void *arg)
{
    char numberBuffer[20];
    int bufferlen;
    int i;

    return NULL; /* remove this line */

    while (1) {

	if (timeToFinish()) {
	    return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++) {
	    if (buffer[i] == ';') {
		if (i == 0) {
		    printErrorAndExit("Sentinel found empty expression!");
		} else {
		    /* null terminate the string */
		    numberBuffer[i] = '\0';
		    /* print out the number we've found */
		    fprintf(stdout, "%s\n", numberBuffer);
		    /* shift the remainder of the string to the left */
		    strcpy(buffer, &buffer[i + 1]);
		    break;
		}
	    } else if (!isNumeric(buffer[i])) {
		break;
	    } else {
		numberBuffer[i] = buffer[i];
	    }
	}

	// something missing?
    }
}

/* reader reads in lines of input from stdin and writes them to the
   buffer */
void *reader(void *arg)
{
    while (1) {
	char tBuffer[100];
	int currentlen;
	int newlen;
	int free;

	fgets(tBuffer, sizeof(tBuffer), stdin);

	/* Sychronization bugs in remainder of function need to be fixed */

	newlen = strlen(tBuffer);
	currentlen = strlen(buffer);

	/* if tBuffer comes back with a newline from fgets, remove it */
	if (tBuffer[newlen - 1] == '\n') {
	    /* shift null terminator left */
	    tBuffer[newlen - 1] = tBuffer[newlen];
	    newlen--;
	}

	/* -1 for null terminator, -1 for ; separator */
	free = sizeof(buffer) - currentlen - 2;

	while (free < newlen) {
		// spinwaiting
	}

	/* we can add another expression now */
	strcat(buffer, tBuffer);
	strcat(buffer, ";");

	/* Stop when user enters '.' */
	if (tBuffer[0] == '.') {
	    return NULL;
	}
    }
}


/* Where it all begins */
int smp3_main(int argc, char **argv)
{
    void *arg = 0;		/* dummy value */

    /* let's create our threads */
    if (pthread_create(&multiplierThread, NULL, multiplier, arg)
	|| pthread_create(&adderThread, NULL, adder, arg)
	|| pthread_create(&degrouperThread, NULL, degrouper, arg)
	|| pthread_create(&sentinelThread, NULL, sentinel, arg)
	|| pthread_create(&readerThread, NULL, reader, arg)) {
	printErrorAndExit("Failed trying to create threads");
    }

    /* you need to join one of these threads... but which one? */
    pthread_detach(multiplierThread);
    pthread_detach(adderThread);
    pthread_detach(degrouperThread);
    pthread_detach(sentinelThread);
    // pthread_detach(readerThread);
    pthread_join(readerThread, NULL);

    /* everything is finished, print out the number of operations performed */
    fprintf(stdout, "Performed a total of %d operations\n", num_ops);
    return EXIT_SUCCESS;
}
