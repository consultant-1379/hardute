#!/bin/sh
HARDUTE=/hardute/bin/hardute
DATADIR=/var/opt/ericsson/eniqpc
PERIOD='15'
HOST=`hostname`
TEMP='/hardute/data/TEMP'
MINSET='00'

$HARDUTE \
	--roptime $PERIOD \
	--disk	$TEMP'_DISKSTAT_'$HOST'.stat' 
	
sleep 240 
DATE=`date +%Y%m%d%H`
mv $TEMP'_DISKSTAT_'$HOST'.stat' $DATADIR/$DATE$MINSET'_'$PERIOD'_DISKSTAT_'$HOST'.stat'
chmod 644 $DATADIR/$DATE$MINSET'_'$PERIOD'_DISKSTAT_'$HOST'.stat'
