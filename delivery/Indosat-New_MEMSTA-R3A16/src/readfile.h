/*
 * read_file_to_string(FILE, char *, ..., NULL)
 * Read from the file until either EOF or one of the strings is found in the file.
 * Return the characters including the matched string as a dynamically allocated string
 * If you leave out the NULL as the last parameter, this function will barf.
 */
char *read_file_to_string_arr(FILE *f, char *token_array[]);
char *read_file_to_string_arg(FILE *f, ...);
