
#include <stdio.h>
#include "getstat.h"
/*
 * This reads the mem data from the sar -r command
 */
void getstat_read_memstat(void);
void getstat_spc_mem(getstat_data_t info_type, FILE *p, char *tokens[], int row_terminator, char *data_terminator, int header_fields[]);
void print_memstat(FILE *f, int header);
