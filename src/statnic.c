

#include "getstat.h"
#include "hardute.h"
#include "platform.h"

/*
 * This reads the getstat data from the netstat command
 */
void getstat_read_nicstat(void) {
	FILE *p;
	char *header_names[] = NICSTAT_NAMES;
	char *header_tokens[] = NICSTAT_HTOKENS;
	char *data_tokens[] = NICSTAT_DTOKENS;
	int header_pattern[GETSTAT_MAX_VALUES + 1];
	
	bzero(header_pattern, sizeof(header_pattern));
	
	
/* open the remote process */
	p = popen(NICSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" NICSTAT_COMMAND "\"\n");
		return;
	}

#if defined (NICSTAT_HEADERTOKEN)
	{
		char *parse_line = 0;

		parse_line = read_file_to_string_arg(p, NICSTAT_HEADERTOKEN, 0);

		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_header(p, header_tokens, '\n', header_names, 0, header_pattern);
#if defined (NICSTAT_DATATOKEN)
	{
		char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, NICSTAT_DATATOKEN, 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_data(GETSTAT_DATA_NIC, p, data_tokens, '\n', 0, header_pattern);

	pclose(p);
}

/*
 * print_nicstat
 * print out network utilisation stats
 */
void print_nicstat(FILE *f, int header) {
	int i, count;
	
	if(header) {
		print_date(f, header);
		fprintf(f, ",");
		fprintf(f, "deviceId,NICSTAT_ipkts,NICSTAT_ierrs,NICSTAT_opkts,NICSTAT_oerrs,NICSTAT_collis,NICSTAT_queue\n");
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		
		if(gstat && gstat->info_type == GETSTAT_DATA_NIC) {
			print_date(f, header);
			fprintf(f, ",%s,%s,%s,%s,%s,%s,%s\n", 
				gstat->info_values[0], 
				gstat->info_values[1], 
				gstat->info_values[2], 
				gstat->info_values[3], 
				gstat->info_values[4], 
				gstat->info_values[5], 
				gstat->info_values[6]); 
		}
	}
}

