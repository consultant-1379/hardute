#!/bin/sh
HARDUTE=/hardute/bin/hardute
DATADIR=/var/opt/ericsson/eniqpc
DATE=`date +%Y%m%d%H%M`
PERIOD='15'
HOST=`hostname`

$HARDUTE \
	--roptime $PERIOD \
	--disk	$DATADIR/$DATE'_'$PERIOD'_DISKSTAT_'$HOST'.stat'

chmod 644 $DATADIR/$DATE'_'$PERIOD'_DISKSTAT_'$HOST'.stat'

