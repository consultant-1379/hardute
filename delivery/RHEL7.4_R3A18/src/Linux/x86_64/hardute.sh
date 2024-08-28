#!/bin/sh
HARDUTE=/hardute/bin/hardute
DATADIR=/var/opt/ericsson/eniqpc
DATE=`date +%Y%m%d%H%M`
PERIOD='15'
HOST=`hostname`

$HARDUTE \
	--roptime $PERIOD \
	--cpu	$DATADIR/$DATE'_'$PERIOD'_CPUSTAT_'$HOST'.stat' \
	--mount	$DATADIR/$DATE'_'$PERIOD'_MNTSTAT_'$HOST'.stat' \
	--mem	$DATADIR/$DATE'_'$PERIOD'_MEMSTAT_'$HOST'.stat' \
	--nic	$DATADIR/$DATE'_'$PERIOD'_NICSTAT_'$HOST'.stat' \
	--proc  $DATADIR/$DATE'_'$PERIOD'_PROCSTAT_'$HOST'.stat' \
	--system $DATADIR/$DATE'_'$PERIOD'_SYSTEMSTAT_'$HOST'.stat'

chmod 644 $DATADIR/*
tail -c 1K /hardute/error.log | cat > /hardute/error.log 2>/dev/null
