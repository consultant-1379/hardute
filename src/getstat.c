
#include "platform.h"
#include "readfile.h"

#include "statcpu.h"
#include "statcrontab.h"
#include "statdisk.h"
#include "statmem.h"
#include "statmnt.h"
#include "statnic.h"
#include "statproc.h"
#include "statsystem.h"

#include "getstat.h"



/*
 * typedef enum {
 * 	GETSTAT_DATA_CPU = 1,
 * 	GETSTAT_DATA_MEM,
 * 	GETSTAT_DATA_DISK,
 * 	GETSTAT_DATA_NIC,
 * 	GETSTAT_DATA_SYSTEM,
 * 	GETSTAT_DATA_PROCESS,
 * 	GETSTAT_DATA_CRONTAB
 * } getstat_data_t;
 * 
 * typedef struct {
 * 	getstat_data_t info_type;
 * 	char *info_key;
 * 	char **info_values;
 * } getstat_info_t;
 */

 

static getstat_info_t *getstat_data;
static int getstat_data_count;
static time_t getstat_measurement_time;

/*
 * add a collection of info_values to the list of stats.  Do not free
 * the strings in info_values after calling this function!
 */
//#define DEBUG
void getstat_add(getstat_data_t info_type, char *info_values[] ) {
	getstat_info_t *r;


	if(!getstat_data || !getstat_data_count) {
		getstat_data_count = 1;
		getstat_data = malloc(getstat_data_count * sizeof(getstat_info_t));
	} else {
		getstat_data_count += 1;
		getstat_data = realloc(getstat_data, getstat_data_count * sizeof(getstat_info_t));
	}
	
	if(getstat_data) {
		int i;

		r = getstat_data + getstat_data_count - 1;
		r->info_type = info_type;
#if defined(DEBUG)
printf("getstat_data[%d]={ %d", getstat_data_count - 1, info_type);
#endif
		r->info_values = calloc(GETSTAT_MAX_VALUES + 1, sizeof(char *));
		for(i = 0; i < GETSTAT_MAX_VALUES; i++) {
			int j;

			for(j = 0; info_values[i] && info_values[i][j]; j++) {
				if(info_values[i][j] == ',') {
					info_values[i][j] = '.';
				}
			}
#if defined(DEBUG)
if(info_values[i]) printf(", \"%s\"", info_values[i]); else printf(", NULL");
#endif
			r->info_values[i] = info_values[i];
		}
#if defined(DEBUG)
printf("}\n");
#endif
	}
}
#undef DEBUG

//#define DEBUG
void getstat_read_header(FILE *p, char *tokens[], int row_terminator, char *header_names[], int header_start, int header_fields[]) {
	char *parsed_string = NULL;
	int i, terminator = 0;
	
	do {
		if(parsed_string) {
			free(parsed_string);
			parsed_string = 0;
		}

		parsed_string = read_file_to_string_arr(p, tokens);

		if(parsed_string && strlen(parsed_string) > 0) {
			terminator = parsed_string[strlen(parsed_string) - 1];
			parsed_string[strlen(parsed_string) - 1] = 0;
			
			while(strlen(parsed_string) > 0 && isspace(parsed_string[strlen(parsed_string) - 1])) {
				parsed_string[strlen(parsed_string) - 1] = 0;
			}
		} else {
			terminator = 0;
		}
		
		if(parsed_string && strlen(parsed_string) > 0) {
#if defined(DEBUG)
printf("parsed_string = \"%s\"\n", parsed_string);
#endif
			for(i = 0; i < GETSTAT_MAX_VALUES && header_names[i]; i++) {
#if defined(DEBUG)
if(header_names[i]) printf("header_names[%d]=\"%s\"\n", i, header_names[i]); else printf("header_names[%d]=NULL\n", i);
#endif
				if(header_names[i] && !strcmp(header_names[i], parsed_string)) {
#if defined(DEBUG)
printf("header_fields[%d]=%d\n", header_start, i+1);
#endif
					header_fields[header_start] = i + 1; 
				}
			}

			header_start++;
		}
	} while(!feof(p) && !ferror(p) && terminator != row_terminator);
}
#undef DEBUG

/*
 * Given a set of headers and a set of fieldstrings, when the headers match 
 * the fieldstrings the data is put in that column in the gstat data set.
 * We assume the data has been parsed to the right place and the headers filled in.
 */
//#define DEBUG
void getstat_read_data(getstat_data_t info_type, FILE *p, char *tokens[], int row_terminator, char *data_terminator, int header_fields[]) {
	int terminator, field, end_of_data = 0;
	char *parsed_string = NULL;
	char *info_values[GETSTAT_MAX_VALUES + 1];
	
	bzero(info_values, sizeof(info_values));

	while(!feof(p) && !ferror(p) && !end_of_data) {
		field = 0;
		int found_data = 0;
			
		do {
			if(parsed_string) {
				free(parsed_string);
				parsed_string = 0;
			}

			parsed_string = read_file_to_string_arr(p, tokens);
		
			if(parsed_string && strlen(parsed_string) > 0) {
				terminator = parsed_string[strlen(parsed_string) - 1];
				parsed_string[strlen(parsed_string) - 1] = 0;
			} else {
				terminator = 0;
			}

			if(parsed_string && strlen(parsed_string) > 0) {
#if defined(DEBUG)
printf("parsed_string=\"%s\", header_fields[%d]=%d\n", parsed_string, field, header_fields[field]);
#endif
				if( header_fields[field] > 0) {
					found_data = 1;
#if defined(DEBUG)
printf("info_values[%d]=\"%s\"\n", header_fields[field]-1, parsed_string);
#endif
					info_values[header_fields[field]-1] = strdup(parsed_string);
				}
				field++;
			}
		} while(!feof(p) && !ferror(p) && terminator != row_terminator);
		
		if(found_data) {
			if(data_terminator && info_values[0]
					&& !strcmp(data_terminator, info_values[0])) {
				end_of_data = 1;
			} else {
#if defined(DEBUG)
printf("storing data...\n");
#endif
				getstat_add(info_type, info_values);
			}
		}

		if(parsed_string) {
			free(parsed_string);
			parsed_string = 0;
		}
	}
}
#undef DEBUG


/* 
 * This reads the getstat data from the system.  It should be
 * called once when needed, and then the data it produces can be 
 * used as required.
 */


static void getstat_read(void) {
	time(&getstat_measurement_time);
	getstat_read_cpustat();
	getstat_read_cronstat();
	getstat_read_diskstat();
	getstat_read_memstat();
	getstat_read_mntstat();
	getstat_read_nicstat();
	getstat_read_procstat();
	getstat_read_systemstat();
}

 
/*
 * These three are the interface to the getstat info.  The first time one of
 * these is called, all the measurements are performed.  After this they 
 * read the results of the original measurement.
 */ 
/* get the index'th getstat_info that was read. */
const getstat_info_t *getstat_info(int index) {
	if(!getstat_data || getstat_data_count == 0) {
		getstat_read();
	}

	if(index < 0 || index >= getstat_data_count) {
		return(NULL);
	}
	
	return((const getstat_info_t *) getstat_data + index);
}
/* get a count of the getstat_info's read */
int getstat_info_count(void) {
	if(!getstat_data || getstat_data_count == 0) {
		getstat_read();
	}
	
	return(getstat_data_count);
}

/* get the time at which the getstat_info was measured */
void getstat_info_measuretime(time_t *t) {
	if(!getstat_data || getstat_data_count == 0) {
		getstat_read();
	}
	
	memcpy(t, &getstat_measurement_time, sizeof(time_t));
}

