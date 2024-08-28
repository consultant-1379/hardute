
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

#include <sys/varargs.h>

#define DEFAULT_ROP_TIME	(60*15)

#define CPUSTAT_COMMAND		"sar -u"
#define CPUSTAT_NAMES		{ "cpu", "usr","sys","wio","idle", 0}
#define CPUSTAT_HTOKENS		{ "%", "\n", 0 }
#define CPUSTAT_DTOKENS		{ " ", "\n", 0 }
#define CPUSTAT_HEADERTOKEN	"%"
#define CPUSTAT_DATATOKEN	"Average"

#define CPUCORESTAT_COMMAND		"mpstat -q"
#define CPUCORESTAT_NAMES		{ "CPU", "usr","sys", "wt", "idl", 0}
#define CPUCORESTAT_HTOKENS		{ " ", "\n", 0 }
#define CPUCORESTAT_DTOKENS		{ " ", "\n", 0 }

#define DISKSTAT_COMMAND	"sar -d"
#define DISKSTAT_NAMES		{ "ice", "%busy", "avque", "r+w/s", "blks/s", "avwait", "avserv", 0 }
#define DISKSTAT_HTOKENS	{ " ", "\n", 0}
#define DISKSTAT_DTOKENS	{ " ", "\n", 0}
#define DISKSTAT_HEADERTOKEN "dev"
#define DISKSTAT_DATATOKEN	"Average"

/* last E missing from "Average" because there are 3 header fields and "Average" is the first data field */
#define MEMSTAT_COMMAND		"sar -r"
#define MEMSTAT_NAMES		{ "freemem","freeswap", 0}
#define MEMSTAT_HTOKENS		{ " ", "\n", 0 }
#define MEMSTAT_DTOKENS		{ " ", "\n", 0 }
#define MEMSTAT_HEADERTOKEN	"\n\n"
#define MEMSTAT_DATATOKEN	"Averag"

#define MNTSTAT_COMMAND		"df -k"
#define MNTSTAT_NAMES		{ "Filesystem", "kbytes", "used", "avail", "Mounted", 0 }
#define MNTSTAT_HTOKENS		{ " ", "\n", 0}
#define MNTSTAT_DTOKENS		{ " ", "\n", 0}

#define NICSTAT_COMMAND		"netstat -in -f inet"
#define NICSTAT_NAMES		{ "Name", "Ipkts", "Ierrs", "Opkts", "Oerrs", "Collis", "Queue", 0 }
#define NICSTAT_HTOKENS		{ " ", "\n", 0}
#define NICSTAT_DTOKENS		{ " ", "\n", 0}

// SYSTEMSTAT

#define PROCSTAT_COMMAND	"prstat -n 10000 0 1"
#define PROCSTAT_NAMES		{ "PID", "PROCESS", "TIME", "CPU", "SIZE", "RSS", "NLWP", 0}
#define PROCSTAT_HTOKENS	{ "/", " ", "\n", 0 }
#define PROCSTAT_DTOKENS	{ "/", " ", "\n", 0 }

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

#define INT_CHAR_MAX		24		/* should be good for signed 64-bit integers */
