
#include "getstat.h"
#include "hardute.h"
#include "readfile.h"
#include "platform.h"
#include "statmem.h"


static int get_pageSize(void) {
	static int pageSize = 0;
	if(!pageSize){
		FILE*p;
		p = popen("getconf PAGESIZE", "r");
		
		if(p){
			int i;
			fscanf(p, "%d", &pageSize);
			for(i=1; i && i!=pageSize; i<<=1)
				;
				
			pageSize = i;
			pageSize = pageSize/1024;
			pclose(p);
		}
	}
	return pageSize;
}

/*
static unsigned long long get_freeswapKB(void) {
	unsigned long long freeswapKB = -1;
	FILE*p;
	p = popen("swap -s", "r");
	if(p){
		fscanf(p, "total: %*dk bytes allocated + %*dk reserved = %*dk used, %lluk available", &freeswapKB);
		pclose(p);
	}
	return freeswapKB;
}
*/
//#define DEBUG
void getstat_spc_mem(getstat_data_t info_type, FILE *p, char *tokens[], int row_terminator, char *data_terminator, int header_fields[]) {
	int terminator, field, spc_cnt = 0, end_of_data = 0, end_of_datas = 0;
	char *parsed_string = NULL;
	char *info_values[GETSTAT_MAX_VALUES + 1];
	
//	bzero(info_values, sizeof(info_values));
memset(info_values,0,sizeof(info_values));

#if defined (MEMSTAT_DATATOKEN)
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, MEMSTAT_DATATOKEN, 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif

while(!end_of_data) {
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
					info_values[spc_cnt] = strdup(parsed_string);
				}
				field++;
			}
		} while(terminator != row_terminator);
		
		if(found_data) {
			
				end_of_data = 1;
			
#if defined(DEBUG)
printf("storing data...\n");
#endif
				spc_cnt++;
			}
#if defined(DEBUG)
printf("Value of count =%d \n",spc_cnt);
#endif
		}

		if(parsed_string) {
			free(parsed_string);
			parsed_string = 0;
		}
	


#if defined (MEMSTAT_DATATOKENS)
	{
    char *parse_line = 0;
		
		parse_line = read_file_to_string_arg(p, MEMSTAT_DATATOKENS, 0);
		
		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif

while(!end_of_datas) {
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
					info_values[spc_cnt] = strdup(parsed_string);
				}
				field++;
			}
		} while(!feof(p) && !ferror(p) && terminator != row_terminator);
		
		if(found_data) {
			
				end_of_datas = 1;
			
#if defined(DEBUG)
printf("storing data...\n");
#endif
				spc_cnt++;
			}
#if defined(DEBUG)
printf("Value of count =%d \n",spc_cnt);
#endif
		}

		if(parsed_string) {
			free(parsed_string);
			parsed_string = 0;
		}
	
	if(spc_cnt == 2){
	getstat_add(info_type, info_values);
	}

}
#undef DEBUG

/*
 * This reads the mem data from the sar -r command
 */
void getstat_read_memstat(void) {
	FILE *p;
	char *header_names[] = MEMSTAT_NAMES;
	char *header_tokens[] = MEMSTAT_HTOKENS;
	char *data_tokens[] = MEMSTAT_DTOKENS;
	int header_pattern[GETSTAT_MAX_VALUES + 1];
	
	//bzero(header_pattern, sizeof(header_pattern));
	memset(header_pattern,0,sizeof(header_pattern));
	
/* open the remote process */
	p = popen(MEMSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" MEMSTAT_COMMAND "\"\n");
		return;
	}

#if defined (MEMSTAT_HEADERTOKEN)
	{
		char *parse_line = 0;

		parse_line = read_file_to_string_arg(p, MEMSTAT_HEADERTOKEN, 0);

		if(parse_line) {
			free(parse_line);
			parse_line = 0;
		}
	}
#endif
	getstat_read_header(p, header_tokens, '\n', header_names, 0, header_pattern);
	
	getstat_spc_mem(GETSTAT_DATA_MEM, p, data_tokens, '\n', 0, header_pattern);
	
	pclose(p);
}



/*
 * print_memstat
 * print out network utilisation stats
 */
void print_memstat(FILE *f, int header) {
	int i, count;
	
	if(header) {
		print_date(f, header);
		fprintf(f, ",");
		if(get_pageSize()){
			fprintf(f, "freemem,freeswap,freememKB,freeswapKB\n");
		}else{
			fprintf(f, "freememKB,freeswapKB\n");
		}
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
				
		if(gstat && gstat->info_type == GETSTAT_DATA_MEM){
            print_date(f, header);
			if(get_pageSize()){
				unsigned long long freemembytes;
				
				sscanf(gstat->info_values[0], "%llu", &freemembytes);
				freemembytes /= get_pageSize();
				
				fprintf(f, ",%llu,%s,%s,%s\n", 
				freemembytes,
				gstat->info_values[1],
				gstat->info_values[0],
				gstat->info_values[1]
                );
			}else{
				fprintf(f, ",%s,%s\n", 
					gstat->info_values[0],
					gstat->info_values[1]
                );
			}
    
	}
}
}
