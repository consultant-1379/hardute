
#include "getstat.h"
#include "hardute.h"
#include "readfile.h"

#include "platform.h"

static char *months[12] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };

static int getstat_findmonth(char *s) {
	int i;
	
	for(i = 0; i < 12; i++) {
		if(!strncasecmp(months[i], s, strlen(months[i]))) {
			return(i + 1);
		}
	}
	
	return(0);
}

/* 
 * Fix for TR HP36452
 * It's noddy but it's consistent, unlike Solaris mktime, which is affected by timezone changes :-(
 */
#define ISLEAP(x) ((!((x) % 4)) && (((x) % 100) || !((x) % 400)))

static time_t getstat_mktime(struct tm *t)
{
	unsigned long long seconds = 0;
	int mdays[12]={31,28,31,30,31,30,31,31,30,31,30,31}, x;

	if(t->tm_year > 70)
	{
		for(x = 1970; x < t->tm_year + 1900; x++)
		{
			if(ISLEAP(x))
			{
				seconds += 86400L * 366L;
			}
			else
			{
				seconds += 86400L * 365L;
			}
		}
	}
	else if(t->tm_year < 70)
	{
		return((time_t) 0);
	}

	if(ISLEAP(t->tm_year + 1900))
	{
		mdays[1] = 29;
	}

	for(x = 0; x < t->tm_mon; x++)
	{
		seconds += mdays[x] * 86400L;
	}

	seconds += (((t->tm_mday - 1)* 24L + t->tm_hour ) * 60 + t->tm_min ) * 60L + t->tm_sec;
	
	return((time_t) seconds);
}

#ifndef TEST_HARNESS
/*
 * Return time since the date given by datestring, in seconds
 */
static int getstat_check_date(char *datestring) {
	char *datebuf = 0;
	int month, mday, hour, min, sec, year;
	time_t check_date;
	struct tm check_date_tm;
	time_t getstat_measurement_time;

	datebuf  = strdup(datestring); // buffer as big as datestring ... cannot overflow sscanf
	sscanf(datestring, "%s %d %d:%d:%d %d", datebuf, &mday, &hour, &min, &sec, &year);
	
	for(month = 0; month < 12 && strcasecmp(months[month], datebuf); month++) {
	}

	if(month  == 12) {
		perror("getstat_check_date: unrecognised month");
		fprintf(stderr, "datebuf = \"%s\"\n", datebuf);
		return(0);
	}

	free(datebuf);
	datebuf = 0;

	bzero(&check_date, sizeof(check_date));
	check_date_tm.tm_year = year - 1900;
	check_date_tm.tm_mon = month;
	check_date_tm.tm_mday = mday;
	check_date_tm.tm_hour = hour;
	check_date_tm.tm_min = min;
	check_date_tm.tm_sec = sec;
	
	errno = 0;
	check_date = getstat_mktime(&check_date_tm);
	
	if(errno) {
		perror("getstat_check_date: mktime failed");
		fprintf(stderr, "datestring = \"%s\"\n", datestring);
		return(0);
	}

	getstat_info_measuretime(&getstat_measurement_time);
	return(difftime(getstat_measurement_time, check_date));
}

/*
 * This reads the cron log to get the statistics.  
 * It uses a different approach to the other getstats, as the cron log is
 * meant for computers to read.
 */
void getstat_read_cronstat(void) {
	FILE *p;
	char *line = 0;
	char **cron_command_string = 0;
	int *cron_command_time = 0;
	int *cron_command_min = 0;
	int *cron_command_max = 0;
	int *cron_command_calls = 0;
	int cron_command_count = 0;
	char *info_values[GETSTAT_MAX_VALUES + 1];
	int cron_command_index = 0;
	int start_time = 0;

	bzero(info_values, sizeof(info_values));
	
	p = popen(CRONSTAT_COMMAND, "r");

	if(!p || feof(p) || ferror(p))
	{
		perror("Cannot open \"" CRONSTAT_COMMAND "\"\n");
	}
	
	while(p && !ferror(p) && !feof(p)) {
		line = read_file_to_string_arg(p, "\n", 0);
		while(strlen(line) && line[strlen(line) - 1] <= ' ') {
			line[strlen(line) - 1] = 0;
		}
		
		if(line) {
			if(strlen(line) > 8 && !strncmp(line, ">  CMD: ", 8)) {

			/* search for the index to the command in line */
				for(cron_command_index = 0; cron_command_index < cron_command_count 
						&& strcmp(line + 8, cron_command_string[cron_command_index]); cron_command_index++) {
				}
				
				/* if the index was not found, create a new entry */
				if(cron_command_index == cron_command_count) {
					if(cron_command_count <= 0) {
						cron_command_count = 1;
						cron_command_string = malloc(sizeof(char *));
						cron_command_time = malloc(sizeof(int));
						cron_command_min = malloc(sizeof(int));
						cron_command_max = malloc(sizeof(int));
						cron_command_calls = malloc(sizeof(int));
					} else {
						cron_command_count++;
						cron_command_string = realloc(cron_command_string, cron_command_count * sizeof(char *));
						cron_command_time = realloc(cron_command_time, cron_command_count * sizeof(int));
						cron_command_min = realloc(cron_command_min, cron_command_count * sizeof(int));
						cron_command_max = realloc(cron_command_max, cron_command_count * sizeof(int));
						cron_command_calls = realloc(cron_command_calls, cron_command_count * sizeof(int));
					}

				/* new entry needs to be initialised */
					cron_command_string[cron_command_index] = strdup(line + 8);
					cron_command_time[cron_command_index] = 0;
					cron_command_min[cron_command_index] = command_params.roptime + 1;
					cron_command_max[cron_command_index] = 0;
					cron_command_calls[cron_command_index] = 0;
				}
			} else if(line[0] == '>') {
				char *s;
				
				/* find month to start date */
				for(s = line; *s && !getstat_findmonth(s); s++) {
				}

				start_time = getstat_check_date(s);
				
				if(!start_time)
				{
					fprintf(stderr, "getstat_read_cronstat: cannot get start_time: line=\"%s\"\n\n", line);
				}

			} else if(line[0] == '<') {
				char *s;
				int end_time;
				
				/* find month to start date */
				for(s = line; *s && !getstat_findmonth(s); s++) {
				}

				end_time = getstat_check_date(s);

				if(!end_time)
				{
					fprintf(stderr, "getstat_read_cronstat: cannot get end_time: line=\"%s\"\n\n", line);
				}

				if(cron_command_index < cron_command_count 
						&& end_time > 0 && start_time > 0 && end_time <= command_params.roptime) {
					int duration = start_time - end_time;
					

					cron_command_time[cron_command_index] += duration;
					if(cron_command_max[cron_command_index] < duration) {
						cron_command_max[cron_command_index] = duration;
					}
					if(cron_command_min[cron_command_index] > duration) {
						cron_command_min[cron_command_index] = duration;
					}
					cron_command_calls[cron_command_index]++;					
				}
			}
		
			free(line);
			line = 0;
		}
	}

/* write out the data we have collected, and free the storage */
	if(cron_command_string) {
		for(cron_command_index = 0; cron_command_index < cron_command_count; cron_command_index++) {
			if(cron_command_string[cron_command_index]) {
				if(cron_command_calls[cron_command_index]) {
					char vbuf[INT_CHAR_MAX];
					info_values[0] = strdup(cron_command_string[cron_command_index]);
					sprintf(vbuf, "%d", cron_command_calls[cron_command_index]);
					info_values[1] =  strdup(vbuf);
					sprintf(vbuf, "%d", cron_command_min[cron_command_index]);
					info_values[2] =  strdup(vbuf);
					sprintf(vbuf, "%d", cron_command_max[cron_command_index]);
					info_values[3] =  strdup(vbuf);
					sprintf(vbuf, "%d", cron_command_time[cron_command_index]/cron_command_calls[cron_command_index]);
					info_values[4] =  strdup(vbuf);
					getstat_add(GETSTAT_DATA_CRONTAB, info_values);
				}
				free(cron_command_string[cron_command_index]);
				cron_command_string[cron_command_index] = 0;
			}
		}
		
		cron_command_count = 0;
		free(cron_command_string);
		cron_command_string =  0;
		free(cron_command_time);
		cron_command_time =  0;
		free(cron_command_min);
		cron_command_min =  0;
		free(cron_command_max);
		cron_command_max =  0;
		free(cron_command_calls);
		cron_command_calls =  0;
	}
	
	if(p) {
		pclose(p);
		p = 0;
	}
}


/*
 * print_crontabstat
 * print out crontab stats
 */
void print_crontabstat(FILE *f, int header) {
	int i, count;
	
	if(header) {
		print_date(f, header);
		fprintf(f, ",");
		fprintf(f, "command,calls,min,max,ave\n");
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		
		if(gstat && gstat->info_type == GETSTAT_DATA_CRONTAB) {
			
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
#endif



#ifdef TEST_HARNESS
/* test getconfig_mktime */
int main(void)
{
	int i, errors = 0;
	struct
	{
		unsigned long long epoch;
		int year;
		int mon;
		int mday;
		int hour;
		int min;
		int sec;
	} 
	testdata[] =
	{
		{0L, 			1970,  1,  1,  0,  0,  0, },
		{311861181L,    1979, 11, 19, 12, 06, 21, },
		{489979590L,    1985,  7, 12,  1, 26, 30, },
		{825551999L,    1996,  2, 28, 23, 59, 59, },
		{825638401L,    1996,  3,  1,  0,  0,  1, },
		{920246399l,    1999,  2, 28, 23, 59, 59, },
		{920246401L,    1999,  3,  1,  0,  0,  1, },
		{951782399L,    2000,  2, 28, 23, 59, 59, },
		{951868801L,    2000,  3,  1,  0,  0,  1, },
		{1075135200L,   2004,  1, 26, 16, 40,  0, },
		{1229791200L,   2008, 12, 20, 16, 40,  0, },
		{1878513990L,   2029,  7, 12,  1, 26, 30, },
		{4107542399UL,  2100,  2, 28, 23, 59, 59, },
		{4107542401UL,  2100,  3,  1,  0,  0,  1, },
		{4294967295UL, 	2106,  2,  7,  6, 28, 15, },
		{0L, 			0,     0,  0,  0,  0,  0, }, /* zero finishes */
	};

	for(i = 0; testdata[i].mon; i++)
	{
		time_t t;
		struct tm tms;
		
		bzero(&tms, sizeof(tms));

		tms.tm_year = testdata[i].year - 1900;
		tms.tm_mon = testdata[i].mon - 1;
		tms.tm_mday = testdata[i].mday;
		tms.tm_hour = testdata[i].hour;
		tms.tm_min = testdata[i].min;
		tms.tm_sec = testdata[i].sec;
		
		t = getstat_mktime(&tms);
		
		printf("test: %d/%02d%/%02d %2d:%02d:%02d",
				testdata[i].year, testdata[i].mon, testdata[i].mday,
				testdata[i].hour, testdata[i].min, testdata[i].sec);

		if(((unsigned long) t) == ((unsigned long) testdata[i].epoch))
		{
			printf(" passed\n");
		}
		else
		{
			printf(" gave %ld not %ld\n",
					(long) t, (long) testdata[i].epoch);
			errors++;
		}
	}

	printf("%d error(s)\n", errors);

	return(errors);
}
#endif
