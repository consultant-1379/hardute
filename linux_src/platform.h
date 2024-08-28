 
/*
 * includes all the system library stuff.  If the system library is lacking,
 * this should also include prototypes for the missing bits ... which should
 * then be implemented in platform.c.
 *
 * If platform.h and platform.c are correctly implemented (with conditional
 * compilation as necessary) then the other code should all build regardless
 * of platform.
 */
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>

//#include <sys/varargs.h>
#include <stdarg.h>

#define DEFAULT_ROP_TIME	(60*15)

#define CPUSTAT_COMMAND		"sar -u"
#define CPUSTAT_NAMES		{ "cpu", "usr","sys","wio","idle", 0}
#define CPUSTAT_HTOKENS		{ "%", "\n", 0 }
#define CPUSTAT_DTOKENS		{ " ", "\n", 0 }
#define CPUSTAT_HEADERTOKEN	"%"
#define CPUSTAT_DATATOKEN	"Average"

#define CPUCORESTAT_COMMAND		"mpstat 60 4"
#define CPUCORESTAT_NAMES		{ "CPU", "%usr","%sys", "%iowait", "%idle", 0 }
#define CPUCORESTAT_HTOKENS		{ " ", "\n", 0 }
#define CPUCORESTAT_DTOKENS		{ " ", "\n", 0 }
#define CPUCORESTAT_HEADERTOKEN ":"

#define DISKSTAT_COMMAND	"sar -d -p"
#define DISKSTAT_NAMES		{ "DEV", "tps", "rd_sec/s", "wr_sec/s", "avgqu-sz", "await", "svctm", "%util", 0 }
#define DISKSTAT_HTOKENS	{ " ", "\n", 0}
#define DISKSTAT_DTOKENS	{ " ", "\n", 0}
#define DISKSTAT_HEADERTOKEN "00:00:01"
#define DISKSTAT_DATATOKEN	"Average:"

/* last E missing from "Average" because there are 3 header fields and "Average" is the first data field */
#define MEMSTAT_COMMAND		"free -k"
#define MEMSTAT_NAMES		{ "free", 0}
#define MEMSTAT_HTOKENS		{ " ", "\n", 0 }
#define MEMSTAT_DTOKENS		{ " ", "\n", 0 }
#define MEMSTAT_HEADERTOKEN	" "
#define MEMSTAT_DATATOKEN	"Mem:"
#define MEMSTAT_DATATOKENS	"Swap:"

#define MNTSTAT_COMMAND		"df -k"
#define MNTSTAT_NAMES		{ "Filesystem", "1K-blocks", "Used", "Available", "Mounted", 0 }
#define MNTSTAT_HTOKENS		{ " ", "\n", 0}
#define MNTSTAT_DTOKENS		{ " ", "\n", 0}

#define NICSTAT_COMMAND		"/sbin/ifconfig"

// SYSTEMSTAT

#define PROCSTAT_COMMAND	"ps -eo pid,time,pcpu,vsz,rss,nlwp,args,lwp,pmem,psr,start_time,tty"
#define PROCSTAT_NAMES		{ "PID", "TIME", "%CPU", "VSZ", "RSS", "NLWP", "COMMAND","LWP", "%MEM", "PSR", "START", "TT", 0}
#define PROCSTAT_HTOKENS	{ " ", "\n", 0 }
#define PROCSTAT_DTOKENS	{ " ", "\n", 0 }

//#define PROCSTAT_OPENFILES_COMMAND		"pfiles %s"
#define PROCSTAT_OPENFILES_COMMAND		"ls -l /proc/%s/fd"
#define PROCSTAT_OPENFILES_BUFSIZE 		(100)
//#define PROCSTAT_OPENFILES_MAXSTART 	"rlimit: "
#define PROCSTAT_OPENFILES_STARTCOUNT	"total"
#define PROCSTAT_OPENFILES_LINESPERFILE	(1)
#define PROCSTAT_OPENFILES_COUNTFIELD	(7)
//#define PROCSTAT_OPENFILES_MAXFIELD		(8)


#define SYSTEMSTAT_COMMAND	"uptime"

#define CRONSTAT_COMMAND	"cat /var/cron/log"

#define INT_CHAR_MAX		24		// should be good for sign