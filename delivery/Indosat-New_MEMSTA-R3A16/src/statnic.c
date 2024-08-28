

#include "getstat.h"
#include "hardute.h"
#include "readfile.h"
#include "platform.h"
//#include "statnic.h"


/*
 * This reads the getstat data from the ifconfig command
 */
void getstat_read_nicstat(void) {
	FILE *p;
	char *parse_line = 0;
	char *info_values[GETSTAT_MAX_VALUES + 1];
	int end_of_data = 0;
		
//	bzero(header_pattern, sizeof(header_pattern));
	memset(info_values,0,sizeof(info_values));
	
	
/* open the remote process */
	p = popen(NICSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" NICSTAT_COMMAND "\"\n");
		return;
	}
	
	while(!feof(p) && !ferror(p) && !end_of_data) {
		
	parse_line = read_file_to_string_arg(p, " ", 0);
	
		
	if (feof(p))
		{
			end_of_data=1;
		}
	
	while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}
			
		
	info_values[0] = strdup(parse_line);
	
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "RX packets:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	parse_line = read_file_to_string_arg(p, " ", 0);
	while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}

	info_values[1] = strdup(parse_line);
	
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "errors:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	parse_line = read_file_to_string_arg(p, " ", 0);
	while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}

	info_values[2] = strdup(parse_line);
	
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "TX packets:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	parse_line = read_file_to_string_arg(p, " ", 0);
	while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}

	info_values[3] = strdup(parse_line);
	
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "errors:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	parse_line = read_file_to_string_arg(p, " ", 0);
	while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}

	info_values[4] = strdup(parse_line);
	
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "collisions:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	parse_line = read_file_to_string_arg(p, " ", 0);
	while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}

	info_values[5] = strdup(parse_line);
	
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "txqueuelen:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	parse_line = read_file_to_string_arg(p, " ", 0);
	while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}

	info_values[6] = strdup(parse_line);
/* Redundant Code
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "TX bytes:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "Mb)", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
Redundant code */
	parse_line = read_file_to_string_arg(p, "\n\n", 0);
	
	if(info_values[0] && info_values[1] && info_values[2] && info_values[3] && info_values[4] && info_values[5] && info_values[6] && !end_of_data) {
		getstat_add(GETSTAT_DATA_NIC, info_values);
		}
		
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
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
