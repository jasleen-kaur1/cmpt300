/* SMP1: Simple Shell */

/* LIBRARY SECTION */
#include <ctype.h>              /* Character types                       */
#include <stdio.h>              /* Standard buffered input/output        */
#include <stdlib.h>             /* Standard library functions            */
#include <string.h>             /* String operations                     */
#include <sys/types.h>          /* Data types                            */
#include <sys/wait.h>           /* Declarations for waiting              */
#include <unistd.h>             /* Standard symbolic constants and types */

#include "smp1_tests.h"         /* Built-in test system                  */

/* DEFINE SECTION */
#define SHELL_BUFFER_SIZE 256   /* Size of the Shell input buffer        */
#define SHELL_MAX_ARGS 8        /* Maximum number of arguments parsed    */

/* VARIABLE SECTION */
enum { STATE_SPACE, STATE_NON_SPACE };	/* Parser states */
// Global variable to help prevent invoking further subshells
int subshell_count = 0;

int imthechild(const char *path_to_exec, char *const args[])
{
	// Step 5 - Q1
	/* execvp used to construct path name. If argument uses slash character, the 
	argument is used as path name for file. Else, path prefix of file comes from 
	the search of directories in the environment variable PATH
	Source: https://www.mkssoftware.com/docs/man3/execl.3.asp
	*/
	return execvp(path_to_exec, args) ? -1 : 0;
}

void imtheparent(pid_t child_pid, int run_in_background)
{
	int child_return_val, child_error_code;

	/* fork returned a positive pid so we are the parent */
	fprintf(stderr,
	        "  Parent says 'child process has been forked with pid=%d'\n",
	        child_pid);
	if (run_in_background) {
		fprintf(stderr,
		        "  Parent says 'run_in_background=1 ... so we're not waiting for the child'\n");
		return;
	}
	// wait(&child_return_val);
	/* Step 5 - Q4. Source used: https://www.tutorialspoint.com/unix_system_calls/waitpid.htm */
	waitpid(-1, &child_return_val, 0);

	/* Use the WEXITSTATUS to extract the status code from the return value */
	child_error_code = WEXITSTATUS(child_return_val);
	fprintf(stderr,
	        "  Parent says 'wait() returned so the child with pid=%d is finished.'\n",
	        child_pid);
	if (child_error_code != 0) {
		/* Error: Child process failed. Most likely a failed exec */
		fprintf(stderr,
		        "  Parent says 'Child process %d failed with code %d'\n",
		        child_pid, child_error_code);
	}
}

/* MAIN PROCEDURE SECTION */
int main(int argc, char **argv)
{
	//Added a pid_t variable to support fork for subshell
	pid_t shell_pid, pid_from_fork;
	int n_read, i, exec_argc, parser_state, run_in_background;
	/* buffer: The Shell's input buffer. */
	char buffer[SHELL_BUFFER_SIZE];
	/* exec_argv: Arguments passed to exec call including NULL terminator. */
	char *exec_argv[SHELL_MAX_ARGS + 1];
	// Variable to support Step 5 - Q2
	int counter = 1;
	// Char array storing last 9 buffers
	char store_buffer[9][SHELL_BUFFER_SIZE];

	/* Entrypoint for the testrunner program */
	if (argc > 1 && !strcmp(argv[1], "-test")) {
		return run_smp1_tests(argc - 1, argv + 1);
	}

	//  Allow the Shell prompt to display the pid of this process 
	shell_pid = getpid();

	while (1) {
	/* The Shell runs in an infinite loop, processing input. */

		fprintf(stdout, "Shell(pid=%d)%d> ", shell_pid, counter);
		fflush(stdout);

		/* Read a line of input. */
		if (fgets(buffer, SHELL_BUFFER_SIZE, stdin) == NULL)
			return EXIT_SUCCESS;
		n_read = strlen(buffer);
		run_in_background = n_read > 2 && buffer[n_read - 2] == '&';
		buffer[n_read - run_in_background - 1] = '\n';

		// Step 5 - Q3
		/* Checks if command is of type !(a digit) */
		if(buffer[0] == '!' && isdigit(buffer[1])){

			// Convert character digit to integer digit
			int command_number = atoi(&(buffer[1]));

			// Prints not valid to stderr when command wanted does not exist
			if( command_number >= counter){
				fprintf(stderr, "Not valid \n");
			}
			else{
				// Copy n'th command into buffer to execute
				strcpy(buffer, store_buffer[command_number-1]);

				#ifdef DEBUG
				fprintf(stderr, "Change commands: %s", buffer);
				#endif
			}
		}

		//Stores commands in an array
		strcpy(store_buffer[counter -1], buffer);

		/* Step 5 - Q2. Increments counter only when empty command is not 
		inputted */
		if (buffer[0] != '\n'){
			counter++;
		}

		/* Parse the arguments: the first argument is the file or command *
		 * we want to run.                                                */

		// Looking at if the input line has a space
		parser_state = STATE_SPACE;
		for (exec_argc = 0, i = 0;
		     (buffer[i] != '\n') && (exec_argc < SHELL_MAX_ARGS); i++) {

			//When not a white space character
			if (!isspace(buffer[i])) {
				if (parser_state == STATE_SPACE)
					exec_argv[exec_argc++] = &buffer[i];
				parser_state = STATE_NON_SPACE;
			} else {
				buffer[i] = '\0';
				parser_state = STATE_SPACE;
			}
		}

		/* run_in_background is 1 if the input line's last character *
		 * is an ampersand (indicating background execution).        */


		buffer[i] = '\0';	/* Terminate input, overwriting the '&' if it exists */

		/* If no command was given (empty line) the Shell just prints the prompt again */
		if (!exec_argc)
			continue;
		/* Terminate the list of exec parameters with NULL */
		exec_argv[exec_argc] = NULL;

		/* If Shell runs 'exit' it exits the program. */
		if (!strcmp(exec_argv[0], "exit")) {
			printf("Exiting process %d\n", shell_pid);
			return EXIT_SUCCESS;	/* End Shell program */

		} else if (!strcmp(exec_argv[0], "cd") && exec_argc > 1) {
		/* Running 'cd' changes the Shell's working directory. */
			/* Alternative: try chdir inside a forked child: if(fork() == 0) { */

			if (chdir(exec_argv[1]))
				/* Error: change directory failed */
				fprintf(stderr, "cd: failed to chdir %s\n", exec_argv[1]);	
			/* End alternative: exit(EXIT_SUCCESS);} */

		/* Step 5 - Q5 and Q6 */
			// Implementing a built-in command 'sub'
		} else if(!strcmp(exec_argv[0], "sub")){
			// Maximum of 2 sub sub shells are allowed to be invoked
			if (subshell_count < 2){
				// Increment subshell count once 'sub' command invoked
				subshell_count++;
				// forking to create sub shell
				pid_from_fork = fork();
				// Similar to else statement functionality
				if (pid_from_fork < 0) {
					/* Error: fork() failed.  Unlikely, but possible (e.g. OS *
					 * kernel runs out of memory or process descriptors).     */
					fprintf(stderr, "fork failed\n");
					continue;
				}
				if (pid_from_fork == 0){
					// Calling main here instead for sub shell with same argc and argv arguments as before
					main(argc, argv);
				}
				else{
					imtheparent(pid_from_fork, run_in_background);
					/* Parent will continue around the loop. */
				}
			} else{
				// If more than two sub shells invoked error message printed
				fprintf(stderr, "Too deep!\n");
			}
		}else{
			/* Execute Commands */
			/* Try replacing 'fork()' with '0'.  What happens? */
			pid_from_fork = fork();
			// pid_from_fork = 0;

			if (pid_from_fork < 0) {
				/* Error: fork() failed.  Unlikely, but possible (e.g. OS *
				 * kernel runs out of memory or process descriptors).     */
				fprintf(stderr, "fork failed\n");
				continue;
			}
			if (pid_from_fork == 0) {
				return imthechild(exec_argv[0], &exec_argv[0]);
				/* Exit from main. */
			} else {
				imtheparent(pid_from_fork, run_in_background);
				/* Parent will continue around the loop. */
			}
			
		} /* end if */
	} /* end while loop */

	return EXIT_SUCCESS;
} /* end main() */
