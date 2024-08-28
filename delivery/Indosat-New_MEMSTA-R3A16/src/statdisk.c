
#include "getstat.h"
#include "hardute.h"
#include "readfile.h"
#include "platform.h"

/*
 * This reads the disk data from the sar command
 */
void getstat_read_diskstat(void) {
	FILE *p;
	char *header_names[] = DISKSTAT_NAMES;
	char *header_tokens[] = DISKSTAT_HTOKENS;
	char *data_tokens[] = DISKSTAT_DTOKENS;
	int header_pattern[GETSTAT_MAX_VALUES + 1];
	
	bzero(header_pattern, sizeof(header_pattern));
	
//printf("getstat_read_cpustat begins\n");	
/* open the remote process */
	p = popen(DISKSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" DISKSTAT_COMMAND "\"\n");
		return;
	}

#if defined (DISKSTAT_HEADERTOKEN)

{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, "00:", 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
	{
		char *parse_line = 0;
//printf("HeaderToken>>%s\n", DISKSTAT_HEADERTOKEN);
		parse_line = read_file_to_string_arg(p, DISKSTAT_HEADERTOKEN, 0);
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_header(p, header_tokens, '\n', header_names, 0, header_pattern);
#if defined (DISKSTAT_DATATOKEN)
	{
		char *parse_line = 0;
//printf("DataToken>>%s\n", DISKSTAT_DATATOKEN);
		parse_line = read_file_to_string_arg(p, DISKSTAT_DATATOKEN, 0);
//printf("parse_line = \"%s\"\n", parse_line);		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_data_disk(GETSTAT_DATA_DISK, p, data_tokens, '\n', 0, header_pattern);
	pclose(p);
}


/*
 * print_diskstat
 * print out disk stats
 */
void print_diskstat(FILE *f, int header) {
	//printf("printdiskstat function\n");
	int i, count;
	int read=0,write=0,rd_wrt=0;
	if(header) {
		print_date(f, header);
		fprintf(f, ",");
		fprintf(f, "device,busy,avque,r+w/s,blks/s,avwait,avserv\n");
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		//printf("\ni=%d\tgstat->info_type=%d\n",i,gstat->info_type);
		if(gstat && gstat->info_type == GETSTAT_DATA_DISK) {
			char *disk;
			int i;
			
			disk = strdup(gstat->info_values[0]);
			//printf("\nTest inside if control\n");
			for(i = 0; disk[i]; i++) {
				if(disk[i] == ',') {
					disk[i] = '.';
				}
			}
			read=atoi(gstat->info_values[2]);
			write=atoi(gstat->info_values[3]);
			rd_wrt=read+write;
			//printf("\n\n\t\t\tBlks/sec=%d\n\n\n",rd_wrt);
			//printf("\nHeader=%d\n",header);
			print_date(f, header);
			//printf("\nPrinting statistics in file\n");
			fprintf(f, ",%s,%s,%s,%s,%d,%s,%s\n",
				gstat->info_values[0], 
				gstat->info_values[7], 
				gstat->info_values[4], 
				gstat->info_values[1], 
				rd_wrt, 
				gstat->info_values[5], 
				gstat->info_values[6]); 
			
			free(disk);
			disk = 0;
		}
	}
}
