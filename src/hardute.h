
typedef struct {
	char *root_dir;
	char *cpu, *crontab, *disk, *mem, *mnt, *nic, *proc, *system;
	int roptime;
} command_params_t;

#ifndef HARDUTE_C
extern command_params_t command_params;
#endif

void print_date(FILE *f, int header);
