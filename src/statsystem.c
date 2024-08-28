

#include "getstat.h"
#include "hardute.h"
#include "readfile.h"
#include "platform.h"


#ifndef TEST_HARNESS
/*
 * This reads the getstat data for system uptime
 */
void getstat_read_systemstat(void) {
	FILE *p;
	char *parse_line = 0;
	char *info_values[GETSTAT_MAX_VALUES + 1];

	
	bzero(info_values, sizeof(info_values));
	
	p = popen(SYSTEMSTAT_COMMAND, "r");
	
	if(!p) {
		perror("Cannot open pipe to \"" SYSTEMSTAT_COMMAND "\"\n");
		return;
	}

	parse_line = read_file_to_string_arg(p, "up ", 0);
/* find and parse uptime */
	if(parse_line) {
		free(parse_line);
		
		parse_line = read_file_to_string_arg(p, "user,", "users,", 0);
		
		if(parse_line) {
			int numstart;
/* work backwards from the word 'user[s]' */
			if(!strcmp(parse_line + strlen(parse_line) - 5, "user,"))
			{
				parse_line[strlen(parse_line) - 5] = 0;
			}
			else if(!strcmp(parse_line + strlen(parse_line) - 6, "users,"))
			{
				parse_line[strlen(parse_line) - 6] = 0;
			}

/* remove trailing whitespace */
			while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}

/* now find start of the number at the end of the string */
			for(numstart = strlen(parse_line) - 1; numstart >= 0 && isdigit(parse_line[numstart]); numstart--)
				;

			if(isdigit(parse_line[numstart + 1]))
			{
				info_values[1] = strdup(parse_line + numstart + 1);
				parse_line[numstart + 1] = 0;
			}

			/* remove trailing whitespace */
			while(*parse_line && parse_line[strlen(parse_line) - 1] <= ' ')
			{
				parse_line[strlen(parse_line) - 1] = 0;
			}


/* work back to last comma */
			if(parse_line[strlen(parse_line) - 1] == ',') {
				parse_line[strlen(parse_line) - 1] = 0;
			}
			
			if(strlen(parse_line) > 0) {
				int i;

				info_values[0] = strdup(parse_line);
				
				for(i = 0; i < strlen(info_values[0]); i++) {
					if(info_values[0][i] == ',') {
						info_values[0][i] = ' ';
					}
				}
			}
		}
	}

	if(parse_line) {
		free(parse_line);
		parse_line = 0;
	}

/* skip ahead to start of the load averages */
	parse_line = read_file_to_string_arg(p, "average: ", 0);
	if(parse_line) {
		free(parse_line);
		parse_line = 0;
	}


	parse_line = read_file_to_string_arg(p, ",", 0);
	if(parse_line) {
/* find first digit character */
		while(strlen(parse_line) > 1 && !isdigit(parse_line[0])) {
			memmove(parse_line, parse_line+1, strlen(parse_line) * sizeof(char));
		}			

/* remove last comma */
		if(parse_line[strlen(parse_line) - 1] == ',') {
			parse_line[strlen(parse_line) - 1] = 0;
		}
		
		if(strlen(parse_line) > 0) {
			info_values[2] = strdup(parse_line);
		}

		free(parse_line);
		parse_line =0;
	}

	if(info_values[0] && info_values[1] && info_values[2]) {
		getstat_add(GETSTAT_DATA_SYSTEM, info_values);
	}
}
#endif

static char *regular_uptime(const char *uptime_string)
{
#define UPTIME_MAX	(9+10+10+10+1)
	static char uptime[UPTIME_MAX]; /* static since it will be the return value */
	int days = 0, hours = 0, mins = 0;
	char *s;


/* format the uptime string correctly -- there are four cases:- */
	s = strstr(uptime_string, "day(s)");
	if(s)
	{
/*   - "[d]* day(s), [m]m min(s)" */
/*   - "[d]* day(s), [h]h:mm" */
		while(s > uptime_string && !isdigit(*s))
		{
			s--;
		}

		while(s > uptime_string && isdigit(*(s-1)))
		{
			s--;
		}

		sscanf(s, "%d", &days);
	} /* else days default to zero */

	s = strstr(uptime_string, "min(s)");
	if(s)
	{
/*   - "[d]* day(s), [m]m min(s)" */
/*   - "[m]m min(s)" */
		while(s > uptime_string && !isdigit(*s))
		{
			s--;
		}

		while(s > uptime_string && isdigit(*(s-1)))
		{
			s--;
		}

		sscanf(s, "%d", &mins);
		/* default hours is zero */
	}

	s = strstr(uptime_string, "hr(s)");
	if(s) 
	{
/*   - "[d]* day(s), [h]h hr(s)" */
/*   - "[h]h hr(s)" */
		while(s > uptime_string && !isdigit(*s))
		{
			s--;
		}

		while(s > uptime_string && isdigit(*(s-1)))
		{
			s--;
		}

		sscanf(s, "%d", &hours);
		/* default mins is zero */
	}


/* if the hours and the mins are zero, look for the [h]h:mm format */
	s = strchr(uptime_string, ':');

	if(s && !hours && !mins)
	{ 
/*   - "[d]* day(s), [h]h:mm" */
/*   - "[h]h:mm" */
		sscanf(s + 1, "%d", &mins);

		while(s > uptime_string && isdigit(*(s-1)))
		{
			s--;
		}

		sscanf(s, "%d", &hours);
	}

	snprintf(uptime, UPTIME_MAX, "%d day(s) %d:%02d", days, hours, mins);

	return(uptime);
}

#ifndef TEST_HARNESS
/*
 * print_systemstat
 * print out network utilisation stats
 */
void print_systemstat(FILE *f, int header) {
	int i, count;


	if(header) {
		print_date(f, header);
		fprintf(f, ",");
		fprintf(f, "uptime,users,load\n");
		return;
	}
	
	count = getstat_info_count();

	for(i = 0; i < count; i++) {
		const getstat_info_t *gstat = getstat_info(i);
		
		if(gstat && gstat->info_type == GETSTAT_DATA_SYSTEM) {
			print_date(f, header);
			fprintf(f, ",%s,%s,%s\n", 
				regular_uptime(gstat->info_values[0]), 
				gstat->info_values[1], 
				gstat->info_values[2]); 
		}
	}
}
#endif

#ifdef TEST_HARNESS
int main(void)
{
	char *uptimes[] =
	{
		"",
		"2 hr(s)",
		"3 min(s)",
		"12:34",
		"89 day(s)",
		"123 day(s)  4 hr(s)",
		"234 day(s)  56 min(s)",
		"12 day(s)  3:45",
		0
	};
	int i;

	for(i = 0; uptimes[i]; i++)
	{
		printf("regular_uptime(\"%s\") returns \"%s\"\n", uptimes[i], regular_uptime(uptimes[i]));
	}

	return(0);
}
#endif
