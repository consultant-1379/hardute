

#include "getstat.h"
#include "hardute.h"
#include "readfile.h"
#include "platform.h"
//#include "statproc.h"

/*
 * This reads the getstat data from the process tables
 */
void getstat_read_procstat(void) {
	FILE *p;
	char *header_names[] = PROCSTAT_NAMES;
	char *header_tokens[] = PROCSTAT_HTOKENS;
	char *data_tokens[] = PROCSTAT_DTOKENS;
	int header_pattern[GETSTAT_MAX_VALUES + 1];
#if defined(PROCSTAT_OPENFILES_COMMAND)
	int i;
#endif
	
	bzero(header_pattern, sizeof(header_pattern));
	
	
/* open the remote process */
	p = popen(PROCSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" PROCSTAT_COMMAND "\"\n");
		return;
	}

#if defined (PROCSTAT_HEADERTOKEN)
	{
		char *parse_line = 0;

		parse_line = read_file_to_string_arg(p, PROCSTAT_HEADERTOKEN, 0);

		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_header(p, header_tokens, '\n', header_names, 0, header_pattern);
#if defined (PROCSTAT_DATATOKEN)
	{
		char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, PROCSTAT_DATATOKEN, 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_data(GETSTAT_DATA_PROCESS, p, data_tokens, '\n', "Total:", header_pattern);

	pclose(p);

#if defined(PROCSTAT_OPENFILES_COMMAND)
/* now for each process recorded, read the number of open files */	
	for(i = 0; i < getstat_info_count(); i++) {
		getstat_info_t *gstat = (getstat_info_t *) getstat_info(i); /* naughty casting to get rid of const */
		char *parse_line = 0;

		if(gstat && gstat->info_type == GETSTAT_DATA_PROCESS) {
			char command[PROCSTAT_OPENFILES_BUFSIZE];
			int count = 0;
			
			sprintf(command, PROCSTAT_OPENFILES_COMMAND, gstat->info_values[0]);
			
			p = popen(command, "r");
			
			if(!p) {
#if defined(PROCSTAT_OPENFILES_COUNTFIELD)
				gstat->info_values[PROCSTAT_OPENFILES_COUNTFIELD] = strdup("?");
#endif
#if defined(PROCSTAT_OPENFILES_MAXFIELD)
				gstat->info_values[PROCSTAT_OPENFILES_MAXFIELD] = strdup("?");
#endif
				break;
			}
#if defined(PROCSTAT_OPENFILES_MAXSTART)
			if(parse_line) {
				free(parse_line);
				parse_line = 0;
			}
			
			parse_line = read_file_to_string_arg(p, PROCSTAT_OPENFILES_MAXSTART, 0);
			
			if(parse_line) {
				free(parse_line);
				parse_line = 0;
			}
			
			parse_line = read_file_to_string_arg(p, " ", 0);
			
#if defined(PROCSTAT_OPENFILES_MAXFIELD)
			if(parse_line) {
				while(strlen(parse_line) > 1 && parse_line[strlen(parse_line) - 1] <= ' ') {
					parse_line[strlen(parse_line) - 1] = 0;
				}
				
				if(strlen(parse_line) > 0) {
					getstat_data[i].info_values[PROCSTAT_OPENFILES_MAXFIELD] = parse_line;
				} else {
					free(parse_line);
					getstat_data[i].info_values[PROCSTAT_OPENFILES_MAXFIELD] = strdup("?");
				}
				parse_line = 0;
			}
#endif
#endif			
			if(parse_line) {
				free(parse_line);
				parse_line = 0;
			}
#if defined(PROCSTAT_OPENFILES_STARTCOUNT)
			parse_line = read_file_to_string_arg(p, PROCSTAT_OPENFILES_STARTCOUNT, 0);
#endif
			
			while(!feof(p) && !ferror(p)) {
				if(parse_line) {
					free(parse_line);
					parse_line = 0;
				}	
			
				parse_line = read_file_to_string_arg(p, "\n", 0);
				
				count++;
			}
			
			if(parse_line) {
				free(parse_line);
				parse_line = 0;
			}

			pclose(p);
			
#if defined(PROCSTAT_OPENFILES_COUNTFIELD) && defined(PROCSTAT_OPENFILES_LINESPERFILE)
			if(count >= PROCSTAT_OPENFILES_LINESPERFILE) {
				sprintf(command, "%d", count / PROCSTAT_OPENFILES_LINESPERFILE -2);
			} else {
				sprintf(command, "0");
			}
			
			gstat->info_values[PROCSTAT_OPENFILES_COUNTFIELD] = strdup(command);
#endif
		}
	}
#endif
}


/*
 * print_procstat
 * print out network utilisation stats
 */

void print_procstat(FILE *f, int header) {
	int i, count;
	
	if(header) {
		print_date(f, header);
		fprintf(f, ",");
#if defined(PROCSTAT_OPENFILES_MAXFIELD)
		fprintf(f, "PID,PROCESS,TIME,CPU,SIZE,RSS,NLWP,OFILES,MAXFILES\n");
#elif defined(PROCSTAT_OPENFILES_COUNTFIELD)
		fprintf(f, "PID,PROCESS,TIME,CPU,SIZE,RSS,NLWP,OFILES\n");
#else
		fprintf(f, "PID,PROCESS,TIME,CPU,SIZE,RSS,NLWP\n");
#endif
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		
		if(gstat && gstat->info_type == GETSTAT_DATA_PROCESS) {
			print_date(f, header);
#if defined(PROCSTAT_OPENFILES_MAXFIELD)
			fprintf(f, ",%s,%s,%s,%s%,%sK,%sK,%s,%s,%s\n", 
				gstat->info_values[0], 
				gstat->info_values[6], 
				gstat->info_values[1], 
				gstat->info_values[2], 
				gstat->info_values[3], 
				gstat->info_values[4], 
				gstat->info_values[5], 
				gstat->info_values[7], 
				gstat->info_values[8]); 
#elif defined(PROCSTAT_OPENFILES_COUNTFIELD)
			fprintf(f, ",%s,%s,%s,%s%,%sK,%sK,%s,%s\n", 
				gstat->info_values[0], 
				gstat->info_values[6], 
				gstat->info_values[1], 
				gstat->info_values[2], 
				gstat->info_values[3], 
				gstat->info_values[4], 
				gstat->info_values[5], 
				gstat->info_values[7]); 
#else
			fprintf(f, ",%s,%s,%s,%s%,%sK,%sK,%s\n", 
				gstat->info_values[0], 
				gstat->info_values[6], 
				gstat->info_values[1], 
				gstat->info_values[2], 
				gstat->info_values[3], 
				gstat->info_values[4], 
				gstat->info_values[5]); 
#endif
		}
	}
}
