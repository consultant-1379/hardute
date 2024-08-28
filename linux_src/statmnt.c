
#include "getstat.h"
#include "hardute.h"
#include "platform.h"
#include "statmnt.h"


/*
 * This reads the getstat data from the df command
 */
 
 
 
void getstat_read_mntstat(void) {
	FILE *p;
	char *header_names[] = MNTSTAT_NAMES;
	char *header_tokens[] = MNTSTAT_HTOKENS;
	char *data_tokens[] = MNTSTAT_DTOKENS;
	int header_pattern[GETSTAT_MAX_VALUES + 1];
	
//	bzero(header_pattern, sizeof(header_pattern));
	memset(header_pattern,0,sizeof(header_pattern));
	
/* open the remote process */
	p = popen(MNTSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" MNTSTAT_COMMAND "\"\n");
		return;
	}

#if defined (MNTSTAT_HEADERTOKEN)
	{
		char *parse_line = 0;

		parse_line = read_file_to_string_arg(p, MNTSTAT_HEADERTOKEN, 0);

		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_header(p, header_tokens, '\n', header_names, 0, header_pattern);
#if defined (MNTSTAT_DATATOKEN)
	{
		char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, MNTSTAT_DATATOKEN, 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_data(GETSTAT_DATA_MNT, p, data_tokens, '\n', 0, header_pattern);

	pclose(p);
}



/*
 * print_mntstat
 * print out network utilisation stats
 */
void print_mntstat(FILE *f, int header) {
	int i, count;
	
	if(header) {
		print_date(f, header);
		fprintf(f, ",");
		fprintf(f, "Filesystem,size,used,avail,Mounted\n");
		return;
	}
	
	count = getstat_info_count();
	
	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		
		if(gstat && gstat->info_type == GETSTAT_DATA_MNT) {
			print_date(f, header);
			fprintf(f, ",%s,%s,%s,%s,%s\n", 
				gstat->info_values[0], 
				gstat->info_values[1], 
				gstat->info_values[2], 
				gstat->info_values[3], 
				gstat->info_values[4]); 
		}
	}
}
