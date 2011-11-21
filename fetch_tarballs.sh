#!/bin/sh
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

if [ -z "$TARFILE_LOCATION" ]; then
    echo "ERROR: no destination defined! please set TARFILE_LOCATION!"
    exit
fi

if [ ! -d "$TARFILE_LOCATION" ]; then
    mkdir $TARFILE_LOCATION
fi
if [ ! -d "$TARFILE_LOCATION" ]; then
    echo "ERROR: can't create"
    exit
fi

if [ -z "$1" ]; then
    echo "ERROR: parameter missing!"
    echo "usage: $0 <fetch list>"
    echo "first line must define the base url."
    exit
fi

# check for wget and md5sum
wget=
md5sum=
curl=

for i in wget /usr/bin/wget /usr/local/bin/wget /usr/sfw/bin/wget /opt/sfw/bin/wget /opt/local/bin/wget; do
    eval "$i --version" > /dev/null 2>&1
    ret=$?
    if [ $ret -eq 0 ]; then
        wget=$i
        echo found wget: $wget
        break 2
    fi
done

if [ -z "$wget" ]; then
    for i in curl /usr/bin/curl /usr/local/bin/curl /usr/sfw/bin/curl /opt/sfw/bin/curl /opt/local/bin/curl; do
    # mac curl returns "2" on --version 
    #    eval "$i --version" > /dev/null 2>&1
    #    ret=$?
    #    if [ $ret -eq 0 ]; then
        if [ -x $i ]; then
            curl=$i
            echo found curl: $curl
            break 2
        fi
    done
fi

if [ -z "$wget" -a -z "$curl" ]; then
    echo "ERROR: neither  wget nor curl found!"
    exit
fi

for i in md5 md5sum /usr/local/bin/md5sum gmd5sum /usr/sfw/bin/md5sum /opt/sfw/bin/gmd5sum /opt/local/bin/md5sum; do
    if [ "$i" = "md5" ]; then
        eval "$i -x" > /dev/null 2>&1
    else
        eval "$i --version" > /dev/null 2>&1
    fi
    ret=$?
    if [ $ret -eq 0 ]; then
        md5sum=$i
        echo found md5sum: $md5sum
        break 2
    fi
done

if [ "$md5sum" = "md5" ]; then
    md5special=-r
fi

if [ -z "$md5sum" ]; then
    echo "Warning: no md5sum: found!"
fi

start_dir=`pwd`
logfile=$TARFILE_LOCATION/fetch.log
date >> $logfile

# Create and go to a temporary directory under the tar file destination.
mkdir -p $TARFILE_LOCATION/tmp
cd $TARFILE_LOCATION/tmp

if [ -n "$DMAKE_URL" -a ! -x "$SOLARENV/$OUTPATH/bin/dmake$EXEEXT" ]; then
    # Determine the name of the downloaded file.
    dmake_package_name=`echo $DMAKE_URL | sed "s/^\(.*\/\)//"`

    if [ ! -f "../$dmake_package_name" ]; then
        # Fetch the dmake source
        if [ ! -z "$wget" ]; then
            echo fetching $DMAKE_URL with wget to $TARFILE_LOCATION/tmp
            $wget -nv -N $DMAKE_URL 2>&1 | tee -a $logfile
        else
            echo fetching $DMAKE_URL with curl to $TARFILE_LOCATION/tmp
            $curl $file_date_check -O $DMAKE_URL 2>&1 | tee -a $logfile
        fi
        wret=$?

        # When the download failed then remove the remains, otherwise
        # move the downloaded file up to TARFILE_LOCATION
        if [ $wret -ne 0 ]; then
            echo "download failed. removing $dmake_package_name"
            rm "$dmake_package_name"
            failed="$failed $i"
            wret=0
        else
            mv "$dmake_package_name" ..
            echo "successfully downloaded $dmake_package_name"
        fi
    else
        echo "found $dmake_package_name, no need to download it again"
    fi
fi



cd $TARFILE_LOCATION/tmp
filelist=`cat $1`
echo $$ > fetch-running
for i in $filelist ; do
#    echo $i
    if [ "$i" != `echo $i | sed "s/^http:\///"` ]; then
        tarurl=$i
    # TODO: check for comment    
    else
        if [ "$tarurl" != "" ]; then
            if [ ! -f "../$i" ]; then
                echo $i
                if [ ! -z "$wget" ]; then
                    $wget -nv -N $tarurl/$i 2>&1 | tee -a $logfile
                else
                    echo fetching $i
                    $curl $file_date_check -O $tarurl/$i 2>&1 | tee -a $logfile
                fi
                wret=$?
                if [ $wret -ne 0 ]; then
                    mv $i ${i}_broken
                    failed="$failed $i"
                    wret=0
                fi
                if [ -f $i -a -n "$md5sum" ]; then
                    sum=`$md5sum $md5special $i | sed "s/ .*//"`
                    sum2=`echo $i | sed "s/-.*//"`
                    if [ "$sum" != "$sum2" ]; then
                        echo checksum failure for $i 2>&1 | tee -a $logfile
                        failed="$failed $i"
                        mv $i ${i}_broken
                    else
                        mv $i ..
                    fi
                else
                    mv $i ..
                fi
            fi
        fi
    fi
done
rm $TARFILE_LOCATION/tmp/*-*
cd $start_dir

if [ ! -z "$failed" ]; then
    echo
    echo ERROR: failed on:
    for i in $failed ; do
        echo $i
    done
    exit 1
fi

