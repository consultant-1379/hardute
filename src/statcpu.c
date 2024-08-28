
#include "getstat.h"
#include "hardute.h"
#include "readfile.h"
#include "platform.h"

/*
 * This reads the cpu data from the mpstat command
 */
static void getstat_read_cpustat_percore(void) {
	FILE *p;
	char *header_names[] = CPUCORESTAT_NAMES;
	char *header_tokens[] = CPUCORESTAT_HTOKENS;
	char *data_tokens[] = CPUCORESTAT_DTOKENS;
	int header_pattern[20];
	
	bzero(header_pattern, sizeof(header_pattern));
	
//printf("getstat_read_cpustat begins\n");	
/* open the remote process */
	p = popen(CPUCORESTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" CPUCORESTAT_COMMAND "\"\n");
		return;
	}

#if defined (CPUCORESTAT_HEADERTOKEN)
	{
		char *parse_line = 0;
//printf(">>%s\n", CPUCORESTAT_HEADERTOKEN);
		parse_line = read_file_to_string_arg(p, CPUCORESTAT_HEADERTOKEN, 0);
//printf("parse_line = \"%s\"\n", parse_line);		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_header(p, header_tokens, '\n', header_names, 0, header_pattern);
#if defined (CPUCORESTAT_DATATOKEN)
	{
		char *parse_line = 0;
//printf(">>%s\n", CPUCORESTAT_DATATOKEN);
		parse_line = read_file_to_string_arg(p, CPUCORESTAT_DATATOKEN, 0);
//printf("parse_line = \"%s\"\n", parse_line);		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_data(GETSTAT_DATA_CPU, p, data_tokens, '\n', 0, header_pattern);

	pclose(p);
}

/*
 * This reads the cpu data from the sar command
 */
static void getstat_read_cpustat_average(void) {
	FILE *p;
	char *header_names[] = CPUSTAT_NAMES;
	char *header_tokens[] = CPUSTAT_HTOKENS;
	char *data_tokens[] = CPUSTAT_DTOKENS;
	int header_pattern[GETSTAT_MAX_VALUES + 1];
	
	bzero(header_pattern, sizeof(header_pattern));
	
//printf("getstat_read_cpustat begins\n");	
/* open the remote process */
	p = popen(CPUSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" CPUSTAT_COMMAND "\"\n");
		return;
	}

#if defined (CPUSTAT_HEADERTOKEN)
	{
		char *parse_line = 0;
//printf(">>%s\n", CPUSTAT_HEADERTOKEN);
		parse_line = read_file_to_string_arg(p, CPUSTAT_HEADERTOKEN, 0);
//printf("parse_line = \"%s\"\n", parse_line);		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_header(p, header_tokens, '\n', header_names, 0, header_pattern);
#if defined (CPUSTAT_DATATOKEN)
	{
		char *parse_line = 0;
//printf(">>%s\n", CPUSTAT_DATATOKEN);
		parse_line = read_file_to_string_arg(p, CPUSTAT_DATATOKEN, 0);
//printf("parse_line = \"%s\"\n", parse_line);		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_data(GETSTAT_DATA_CPU, p, data_tokens, '\n', 0, header_pattern);

	pclose(p);
}
void getstat_read_cpustat(void) {
	getstat_read_cpustat_percore();
	getstat_read_cpustat_average();
}
/*
 * print_cpustat
 * print out cpu stats
 */
void print_cpustat(FILE *f, int header) {
	int i, count;
	
	if(header) {
		print_date(f, header);
		fprintf(f, ",");
		fprintf(f, "cpu,CPU_usr,CPU_sys,CPU_idle\n");
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		
		if(gstat && gstat->info_type == GETSTAT_DATA_CPU) {
			char *cpu;
			
			cpu = gstat->info_values[0];
			
			if(!cpu) cpu = "average";

			print_date(f, header);
			fprintf(f, ",%s,%s,%s,%s\n",
				cpu, 
				gstat->info_values[1], 
				gstat->info_values[2], 
				gstat->info_values[4]); 
		}
	}
}
