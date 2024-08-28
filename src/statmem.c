
#include "getstat.h"
#include "hardute.h"
#include "readfile.h"
#include "platform.h"

static int get_pageSize(void) {
	static int pageSize = 0;
	if(!pageSize){
		FILE*p;
		p = popen("pagesize", "r");
		
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

/*
 * This reads the mem data from the sar -r command
 */
void getstat_read_memstat(void) {
	FILE *p;
	char *header_names[] = MEMSTAT_NAMES;
	char *header_tokens[] = MEMSTAT_HTOKENS;
	char *data_tokens[] = MEMSTAT_DTOKENS;
	int header_pattern[GETSTAT_MAX_VALUES + 1];
	
	bzero(header_pattern, sizeof(header_pattern));
	
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
	getstat_read_data(GETSTAT_DATA_MEM, p, data_tokens, '\n', 0, header_pattern);

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
			fprintf(f, "freemem,freeswap\n");
		}
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		
		if(gstat && gstat->info_type == GETSTAT_DATA_MEM) {
			print_date(f, header);
			if(get_pageSize()){
				unsigned long long freemembytes;
				
				sscanf(gstat->info_values[0], "%llu", &freemembytes);
				freemembytes *= get_pageSize();
				
				fprintf(f, ",%s,%s,%llu,%llu\n", 
					gstat->info_values[0], 
					gstat->info_values[1],
					freemembytes,
					get_freeswapKB());
			}else{
				fprintf(f, ",%s,%s\n", 
					gstat->info_values[0], 
					gstat->info_values[1]);
			}
		}
	}
}

