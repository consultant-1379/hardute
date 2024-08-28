#!/bin/sh
HARDUTE=/hardute/bin/hardute
DATADIR=/var/opt/ericsson/eniqpc
PERIOD='15'
HOST=`hostname`
TEMP='/hardute/data/TEMP'
MINSET='45'

$HARDUTE \
	--roptime $PERIOD \
	--cpu	$TEMP'_CPUSTAT_'$HOST'.stat' \
	--disk	$TEMP'_DISKSTAT_'$HOST'.stat' \
	--mem	$TEMP'_MEMSTAT_'$HOST'.stat'

#
# Since the information was collected at hh:59/hh:14/hh:29/hh:44, wait for the next minute
# before setting the date on these temporary files.
#
sleep 60
DATE=`date +%Y%m%d%H`
mv $TEMP'_CPUSTAT_'$HOST'.stat' $DATADIR/$DATE$MINSET'_'$PERIOD'_CPUSTAT_'$HOST'.stat' 
mv $TEMP'_DISKSTAT_'$HOST'.stat' $DATADIR/$DATE$MINSET'_'$PERIOD'_DISKSTAT_'$HOST'.stat' 
mv $TEMP'_MEMSTAT_'$HOST'.stat' $DATADIR/$DATE$MINSET'_'$PERIOD'_MEMSTAT_'$HOST'.stat' 
