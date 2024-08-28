
/*
 * 8.1	Measurement Information
 * This Tech Pack manages the following measurements: CPU utilization, Disk Utilization, Interface, and Process statistics. They are described further below.
 * 8.1.1	CPU utilization
 * It collects the average CPU utilization as portion of time the CPU is running in user mode (%usr), system mode (%sys), and idle (%idle). It uses the output of “sar” command.
 * Per-CPU metrics will also be available comparing the percentage of CPU utilization between user and kernel. It uses the output of “mpstat” command.
 * 8.1.2	Memory Utilization
 * It collects the average free memory and free swap space in the node. It uses the output of “sar” command.
 * 8.1.3	Disk Utilization
 * It collects the following information:
 * *	File system space, used, available, and mount point. It uses the output of “df” command.
 * *	Disk statistics: %busy, average queue, read/write per second, blocks per second, average wait time, average service time. It uses the output of “sar” command
 * 
 * 8.1.4	Network Interface Card Statistics
 * It collects the following information based on the output of “netstat” command:
 * *	Amount of inward packets and inward packet errors
 * *	Amount of outward packets and outward packet errors
 * *	Amount of packet collision and queue
 * 
 * 8.1.5	System Uptime
 * It collects the length of time the system has been up since last restart based on the output of “uptime” command.
 * 8.1.6	Process Statistics
 * It collects the following process-related information:
 * *	Process Count
 * *	Process per Second
 * *	CPU Time
 * *	Average CPU / sec
 * *	Process use of memory (MB)
 * *	RSS
 * *	Threads
 * *	File descriptors
 * 8.1.7	Crontab Statistics
 * It collects the following information based on the information in the cron log:
 * *	How many times each command is executed
 * *	Minimum, maximum, and average execution time of each command
 * 
 * 8.2	Measurement File Format & Structure
 * The measurement file format will be comma separated ASCII. The line of the file is the header and followed by the respective values on the next lines.
 * More information will be included in later stage.
 * 8.3	Measurement File Naming Rules
 * The measurement file name format will be:
 * <date>_<period duration>_<moc>_<hostname>.stat
 * *	<date> is the time when the measurement file is produced. The format is yyyymmddhhmm
 * *	<period duration> is the measurement granularity in minutes. It can be 15, 30, or 60 minutes
 * *	<moc> is the measured object group of the hardware utilization statistics, e.g. CPU, DISK, MEM, etc.
 * *	<hostname> is the name of the host where the statistic files are coming from
 * For example:
 * 201105091000_60_CPU_moip1.stat
 * More information will be included in later stage.
 * 8.4	Measurement Object Format
 * The measured object is identified through <hostname> information in the file name. In addition, the following information will also be used as measured object identifier (keys):
 * *	CPU ID for CPU related statistics, e.g. “0”, “1”, etc.
 * *	Device ID for Disk related statistics, e.g. “md10”, “md11”, etc.
 * *	Network Interface ID and address for network interface related statistics, e.g. “nge0” and “localhost” respectively
 * *	 Process ID (PID) and Process Name for process related statistics
 * *	Command (CMD) for crontab related statistics
 * 8.5	Measurement Granularity
 * The measurement granularity is 60 minutes.
 * 8.6	Topology
 * Topology information will be populated using an ASCII file which contains the following information from “uname” and “prtconf” command:
 * *	System
 * *	Hostname
 * *	Hostid
 * *	Release
 * *	Kernel ID
 * *	Machine type
 * *	CPU Type 
 * *	Number of CPU
 * *	Memory size
 * *	Total disk size
 * The ASCII file should be formatted in the following way:
 * system,hostname,hostid,release,kernelid,mach,cputype,nocpu,memsize,disksize
 * <value>,<value>,<value>,<value>,<value>,<value>,<value>,<value>,<value>,<value>
 * 
 * And there should be a line break (empty line) at the end of the file.
 * This file should be populated for every new node name introduced in the network.
 * The following file name will be used for this topology: “hwutiltopology.txt”
 * 
 */


#include "getstat.h"

#include "statcpu.h"
#include "statcrontab.h"
#include "statdisk.h"
#include "statmem.h"
#include "statmnt.h"
#include "statnic.h"
#include "statproc.h"
#include "statsystem.h"

#include "platform.h"

#define HARDUTE_C
#include "hardute.h"

command_params_t command_params;


/*
 * Print out the date.
 * If the value header is nonzero, print out the header value instead of data value
 */
void print_date(FILE *f, int header) {
	time_t timestamp;
	struct tm *tm;

	getstat_info_measuretime(&timestamp);
	tm = localtime(&timestamp);
	if(header) {
		fprintf(f, "DATETIME");
	} else {
		fprintf(f, "%04d-%02d-%02d %02d:%02d", tm->tm_year + 1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
	}
}




static int command_matches(char *string, char *test1, char *test2) {
	if(!strcasecmp(string, test1)) {
		return(1);
	}
	
	if(!strcasecmp(string, test2)) {
		return(2);
	}
	
	return(0);
}

/*
 * Scan the command parameters and populate the values in command_params
 * If the parameters are wrong, report the problem and return nonzero.
 */
static int command_scan(int arg_count, char *args[]) {
	int i, error = 0;
	
	bzero(&command_params, sizeof(command_params));
	command_params.roptime = DEFAULT_ROP_TIME;
	
	if(arg_count < 3) {
		return(1);
	}
		
	for(i = 1; i < arg_count; i++) {
		if(command_matches(args[i], "-h", "--help")) {
			return(1);
		} else if(command_matches(args[i], "-c", "--cpu") && i + 1 < arg_count) {
			command_params.cpu = args[++i];
		} else if(command_matches(args[i], "-k", "--crontab") && i + 1 < arg_count) {
			command_params.crontab = args[++i];
		} else if(command_matches(args[i], "-d", "--disk") && i + 1 < arg_count) {
			command_params.disk = args[++i];
		} else if(command_matches(args[i], "-m", "--mem") && i + 1 < arg_count) {
			command_params.mem = args[++i];
		} else if(command_matches(args[i], "-f", "--mount") && i + 1 < arg_count) {
			command_params.mnt = args[++i];
		} else if(command_matches(args[i], "-n", "--nic") && i + 1 < arg_count) {
			command_params.nic = args[++i];
		} else if(command_matches(args[i], "-p", "--proc") && i + 1 < arg_count) {
			command_params.proc = args[++i];
		} else if(command_matches(args[i], "-r", "--roptime") && i + 1 < arg_count) {
			int argn = 0;
			sscanf(args[++i], "%d", &argn);
			if(argn > 0) command_params.roptime = argn * 60;
		} else if(command_matches(args[i], "-s", "--system") && i + 1 < arg_count) {
			command_params.system = args[++i];
		} else {
			fprintf(stderr, "%s: Unrecognised parameter \"%s\"\n", args[0], args[i]);
			error = 2;
		}
	}
	
	return(error);
}

static void command_help(FILE *out) {
	fprintf(out, " --cpu <pathname>\n");
	fprintf(out, " --crontab <pathname>\n");
	fprintf(out, " --disk <pathname>\n");
	fprintf(out, " --mem <pathname>\n");
	fprintf(out, " --mount <pathname>\n");
	fprintf(out, " --nic <pathname>\n");
	fprintf(out, " --proc <pathname>\n");
	fprintf(out, " --roptime <minutes>\n");	
	fprintf(out, " --system <pathname>\n");
}

static FILE *file_open(char *filename) {
	if(strlen(filename) == 1 && filename[0] == '-') {
		return(fdopen(0, "a"));
	}

	return(fopen(filename, "a"));
}

int main(int arg_count, char *args[]) {
	int command_line_error;
	
	command_line_error = command_scan(arg_count, args);
	
	if(command_line_error) {
		command_help(stderr);
		return(command_line_error);
	}

	if(command_params.cpu) {
		FILE *outfile;
		
		outfile = file_open(command_params.cpu);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_cpustat(outfile, 1);
			}
			print_cpustat(outfile, 0);
		}
	}
	
	if(command_params.crontab) {
		FILE *outfile;
		
		outfile = file_open(command_params.crontab);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_crontabstat(outfile, 1);
			}
			print_crontabstat(outfile, 0);
		}
	}
	
	if(command_params.disk) {
		FILE *outfile;
		
		outfile = file_open(command_params.disk);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_diskstat(outfile, 1);
			}
			print_diskstat(outfile, 0);
		}
	}
	
	
	if(command_params.mem) {
		FILE *outfile;
		
		outfile = file_open(command_params.mem);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_memstat(outfile, 1);
			}
			print_memstat(outfile, 0);
		}
	}
	
	if(command_params.mnt) {
		FILE *outfile;
		
		outfile = file_open(command_params.mnt);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_mntstat(outfile, 1);
			}
			print_mntstat(outfile, 0);
		}
	}
	
	if(command_params.nic) {
		FILE *outfile;
		
		outfile = file_open(command_params.nic);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_nicstat(outfile, 1);
			}
			print_nicstat(outfile, 0);
		}
	}
	
	if(command_params.proc) {
		FILE *outfile;
		
		outfile = file_open(command_params.proc);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_procstat(outfile, 1);
			}
			print_procstat(outfile, 0);
		}
	}
	
	if(command_params.system) {
		FILE *outfile;
		
		outfile = file_open(command_params.system);
		
		if(outfile) {
			if(!ftell(outfile)) {
				print_systemstat(outfile, 1);
			}
			print_systemstat(outfile, 0);
		}
	}
	
	return(0);
}
