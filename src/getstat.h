/*
 * This library reads the stats from the appropriate OS utilities 
 * and puts them into a big data structure for the rest of the 
 * code to use.
 */

#if !defined(GETSTAT_MAX_VALUES)

#include "platform.h"

typedef enum {
	GETSTAT_DATA_CPU = 1,
	GETSTAT_DATA_CRONTAB,
	GETSTAT_DATA_DISK,
	GETSTAT_DATA_MEM,
	GETSTAT_DATA_MNT,
	GETSTAT_DATA_NIC,
	GETSTAT_DATA_SYSTEM,
	GETSTAT_DATA_PROCESS
} getstat_data_t;

/* The maximum number of values is for the disc statistics */
#define GETSTAT_MAX_VALUES 	(10)

typedef struct {
	getstat_data_t info_type;
	char **info_values;
} getstat_info_t;

/*
 * These three are the interface to the getstat info.  The first time one of
 * these is called, all the measurements are performed.  After this they 
 * read the results of the original measurement.
 */ 
void getstat_read_header(FILE *p, char *tokens[], int row_terminator, char *header_names[], int header_start, int header_fields[]);
void getstat_read_data(getstat_data_t info_type, FILE *p, char *tokens[], int row_terminator, char *data_terminator, int header_fields[]);
void getstat_add(getstat_data_t info_type, char *info_values[] );
/* get the index'th getstat_info that was read. */
const getstat_info_t *	getstat_info(int index);
/* get a count of the getstat_info's read */
int						getstat_info_count(void);
/* get the time at which the getstat_info was measured */
void					getstat_info_measuretime(time_t *t);

#endif
