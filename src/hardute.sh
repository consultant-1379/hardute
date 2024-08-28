#!/bin/sh
HARDUTE=/hardute/bin/hardute
DATADIR=/var/opt/ericsson/eniqpc
DATE=`date +%Y%m%d%H%M`
PERIOD='15'
HOST=`hostname`

$HARDUTE \
	--roptime $PERIOD \
	--crontab $DATADIR/$DATE'_'$PERIOD'_CRONSTAT_'$HOST'.stat' \
	--mount	$DATADIR/$DATE'_'$PERIOD'_MNTSTAT_'$HOST'.stat' \
	--nic	$DATADIR/$DATE'_'$PERIOD'_NICSTAT_'$HOST'.stat' \
	--proc	$DATADIR/$DATE'_'$PERIOD'_PROCSTAT_'$HOST'.stat' \
	--system $DATADIR/$DATE'_'$PERIOD'_SYSTEMSTAT_'$HOST'.stat' 

# moved to hardute-delay-sar.sh
#	--cpu	$DATADIR/$DATE'_'$PERIOD'_CPUSTAT_'$HOST'.stat' \
#	--disk	$DATADIR/$DATE'_'$PERIOD'_DISKSTAT_'$HOST'.stat' \
#	--mem	$DATADIR/$DATE'_'$PERIOD'_MEMSTAT_'$HOST'.stat' \
