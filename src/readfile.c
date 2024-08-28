
#include "platform.h"

#include "readfile.h"

#define BUFFER_BLOCKSIZE	(10000)

/*
 * read_file_to_string_arr(FILE *f, char *matchstring_buffer[])
 * Read from the file until either EOF or one of the strings is found in the file.
 * Return the characters including the matched string as a dynamically allocated string
 * If you leave out the NULL as the last matchstring, this function will barf.
 */
//#define DEBUG	1
char *read_file_to_string_arr(FILE *f, char *matchstring_buffer[]) {
	static char *input_buffer;
	static int input_buffer_size;
	int length, matchstring_count = -1;
	
	if(input_buffer_size == 0 || input_buffer == NULL) {
		input_buffer_size += BUFFER_BLOCKSIZE;
		input_buffer = (char *) malloc (input_buffer_size * sizeof(char));
		
		if(!input_buffer) {
			perror("read_file_to_string: malloc failed to allocate input_buffer!");
			return(NULL);
		}
	}

#ifdef DEBUG
printf("read_file_to_string_arr(FILE");
#endif
	for(matchstring_count = 0; matchstring_buffer[matchstring_count]; matchstring_count++)
#ifdef DEBUG
printf(",\"%s\"",matchstring_buffer[matchstring_count]);
#endif
		;
#ifdef DEBUG
printf(")\n");
#endif

	if(matchstring_count < 1) {
		return(strdup(""));
	}
	
	for(length = 0; !feof(f) && !ferror(f); length++) {
		int x;
 
		for(x = 0; x < matchstring_count && (length < strlen(matchstring_buffer[x])
				|| strcmp(input_buffer + length - strlen(matchstring_buffer[x]), matchstring_buffer[x])); x++) {
		}
		
		if(x < matchstring_count) {
			break;
		}
		
		if(input_buffer_size <= length + 2) {
			input_buffer_size += BUFFER_BLOCKSIZE;
			input_buffer = (char *) realloc (input_buffer, input_buffer_size * sizeof(char));
		
			if(!input_buffer) {
				perror("read_file_to_string: realloc failed to allocate input_buffer!");
				return(NULL);
			}
		}

		x = fgetc(f);
		if(x != EOF) { 
#ifdef DEBUG
printf(".%c", x);
#endif
			input_buffer[length] = x;
			input_buffer[length + 1] = 0;
		} else {
#ifdef DEBUG
printf(".EOF");
#endif
			input_buffer[length] = 0;
		}
	}
	
//if(feof(f)) printf("End of file!\n");
//if(ferror(f)) printf("Error reading file!\n");
	input_buffer[length + 1] = 0;
//printf("read_file_to_string_arr() returns \"%s\"\n", input_buffer);
	return(strdup(input_buffer));
}
#undef DEBUG
/*
 * read_file_to_string_arg(FILE, char *, ..., NULL)
 * Read from the file until either EOF or one of the strings is found in the file.
 * Return the characters including the matched string as a dynamically allocated string
 * If you leave out the NULL as the last parameter, this function will barf.
 */
char *read_file_to_string_arg(FILE *f, ...) {
	static char **matchstring_buffer;
	static int matchstring_buffer_size;

	int matchstring_count = -1;
	va_list matchstrings;

	if(matchstring_buffer_size == 0 || matchstring_buffer == NULL) {
		matchstring_buffer_size += BUFFER_BLOCKSIZE;
		matchstring_buffer = (char **) malloc( matchstring_buffer_size * sizeof(char *));
		
		if(!matchstring_buffer) {
			perror("read_file_to_string: malloc failed to allocate matchstring_buffer!");
			return(NULL);
		}		
	}
	
	va_start(matchstrings, f);

	do {
		matchstring_buffer[++matchstring_count] = va_arg(matchstrings, char *);
		matchstring_buffer[matchstring_count + 1] = 0;
		if(matchstring_count + 2 >= matchstring_buffer_size) {
			matchstring_buffer_size += BUFFER_BLOCKSIZE;
			matchstring_buffer = (char **) realloc(matchstring_buffer, matchstring_buffer_size * sizeof(char *));

			if(!matchstring_buffer) {
				perror("read_file_to_string: realloc failed to allocate matchstring_buffer!");
				return(NULL);
			}		
		}
		
	} while(matchstring_buffer[matchstring_count] != NULL);
	
	va_end(matchstrings);

	if(matchstring_count < 1) {
		return(strdup(""));
	}
	
	return(read_file_to_string_arr(f, matchstring_buffer));
}
