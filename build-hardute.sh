#!/bin/sh

OSSYSTEM=`uname -s`
OSREVISION=`uname -r`
ARCH=`uname -p`
DATE=`date +"%Y%m%d %H:%M"`
PRODUCT_NUMBER="CXP9019868"
BUILD_LABEL="${PRODUCT_NUMBER}-$1"
VOB_ROOT="/vobs/eniqpc"
SOURCE_CODE_ROOT="/vobs/eniqpc/hardute/src"
#DELIVERY_VOB_AREA="/vobs/eniq/delivery/plat/hardute"
DELIVERY_VOB_AREA="/vobs/eniqpc/hardute/delivery"

if [ $# != 2 ] || [ "$1" = "" ] || [ "$2" = "" ]
then
	echo "call build-hardute.sh with label rstate"
	exit
fi

SOURCE_LABEL=$1
RSTATE=$2


# make up the pkginfo file using standard template and locally known info
BUILD_ROOT=`pwd`
cd $SOURCE_CODE_ROOT
cat pkginfo.std >pkginfo
echo "ARCH=\""$ARCH"\"" >>pkginfo
echo "VERSION=\""$RSTATE"\"" >>pkginfo
echo "VSTOCK=\""${PRODUCT_NUMBER}"\"" >>pkginfo
echo "PSTAMP=\""$DATE"\"" >>pkginfo

make -f build.mk clean
make -f build.mk pkg BUILD=$OSSYSTEM-$OSREVISION-$ARCH-$RSTATE
make -f build.mk clean
cd $BUILD_ROOT

PACKAGE='hardute-'$OSSYSTEM'-'$OSREVISION'-'$ARCH'-'$RSTATE

#put the newly-built package in the delivery dir
# newgrp eei-epcusers == if you have problems, RUN THIS BY HAND before running the script
cleartool checkout -nc $DELIVERY_VOB_AREA
mv $BUILD_ROOT/$PACKAGE $DELIVERY_VOB_AREA
cleartool mkelem -nc $DELIVERY_VOB_AREA/$PACKAGE
cleartool checkin -nc $DELIVERY_VOB_AREA/$PACKAGE
cleartool checkin -nc $DELIVERY_VOB_AREA

#Apply build label to source code
#Create the lbtype
cleartool mklbtype -nc ${BUILD_LABEL}
#Apply label to the vob root
cleartool mklabel ${BUILD_LABEL} ${VOB_ROOT}
#Apply label to source code - /vobs/eniqpc/hardute
cleartool mklabel -r ${BUILD_LABEL} ${SOURCE_CODE_ROOT}
#Lock the label in the source code vob
#cleartool lock lbtype:${BUILD_LABEL}@${VOB_ROOT}
#Create the label type in the delivery vob
#cleartool mklbtype -nc ${BUILD_LABEL}@${DELIVERY_VOB_AREA} 
#Need to apply the label to the pkg created by the build
cleartool mklabel ${BUILD_LABEL} ${DELIVERY_VOB_AREA}/$OSSYSTEM-$OSREVISION-$ARCH-$RSTATE
cleartool lock lbtype:${BUILD_LABEL}@${VOB_ROOT}

