#!/bin/bash

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
	--proc  $DATADIR/$DATE'_'$PERIOD'_PROCSTAT_'$HOST'.stat' \
	--system $DATADIR/$DATE'_'$PERIOD'_SYSTEMSTAT_'$HOST'.stat'
#	--nic	$DATADIR/$DATE'_'$PERIOD'_NICSTAT_'$HOST'.stat'

# NICSTAT Generation
CUT=/usr/bin/cut
GREP=/usr/bin/grep
DATETIME=`/usr/bin/date '+%Y-%m-%d %H:%M'`
NICFILE=$DATADIR/$DATE'_'$PERIOD'_NICSTAT_'$HOST'.stat'
NICCMD=/sbin/ifconfig

echo -e "DATETIME,deviceId,NICSTAT_queue,NICSTAT_ipkts,NICSTAT_ierrs,NICSTAT_opkts,NICSTAT_oerrs,NICSTAT_collis" > $NICFILE

INTFLST=($($NICCMD -s | $CUT -d ' ' -f 1 | $GREP -v 'Iface'))
for INTF in "${INTFLST[@]}"
do
	INTF_DETAILS=`$NICCMD $INTF`
	QUEUE=`echo $INTF_DETAILS | sed -n "s/^.*txqueuelen\s*\(\S*\).*$/\1/p"`
	IPKTS=`echo $INTF_DETAILS | sed -n "s/^.*RX packets\s*\(\S*\).*$/\1/p"`
	IERRS=`echo $INTF_DETAILS | sed -n "s/^.*RX errors\s*\(\S*\).*$/\1/p"`
	OPKTS=`echo $INTF_DETAILS | sed -n "s/^.*TX packets\s*\(\S*\).*$/\1/p"`
	OERRS=`echo $INTF_DETAILS | sed -n "s/^.*TX errors\s*\(\S*\).*$/\1/p"`
	COLL=`echo $INTF_DETAILS | sed -n "s/^.*collisions\s*\(\S*\).*$/\1/p"`
	echo -e "$DATETIME,$INTF,$QUEUE,$IPKTS,$IERRS,$OPKTS,$OERRS,$COLL" >> $NICFILE
done

# End of NICSTAT Generation

chmod 644 $DATADIR/*
tail -c 1K /hardute/error.log | cat > /hardute/error.log 2>/dev/null
