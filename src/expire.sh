#!/bin/sh
HARDUTE=/hardute/bin/hardute
DATADIR=/var/opt/ericsson/eniqpc
EXPIRY_DAYS=3

find $DATADIR -type file -mtime +$EXPIRY_DAYS -exec rm \{\} \;
