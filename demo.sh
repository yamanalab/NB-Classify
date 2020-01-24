#!/bin/bash

# kind of datasets: "", "22", "51", "92"
SAMPLE_KIND=""

TOPDIR=`pwd`
BINDIR=${TOPDIR}/build/demo
DATDIR=${TOPDIR}/datasets
TESTDIR=${TOPDIR}/testdata
KEYOPT=-g

while getopts s OPT; do
    case $OPT in
	"s" ) KEYOPT="" ;;
    esac
done

INFOFILE=${DATDIR}/sample${SAMPLE_KIND}_info.csv
TESTFILE=${DATDIR}/sample${SAMPLE_KIND}_test.csv
MODELFILE=${DATDIR}/sample${SAMPLE_KIND}_model.csv
CONFFILE=${TOPDIR}/demo/ta/config.txt
EXPCFILE=${TESTDIR}/sample${SAMPLE_KIND}_expect_result.txt

xterm -T "TA"       -e "/bin/bash -c 'cd ${BINDIR}/ta     && ./ta ${KEYOPT} -t ${CONFFILE};          exec /bin/bash -i'"&
xterm -T "CS"       -e "/bin/bash -c 'cd ${BINDIR}/cs     && ./cs;                                   exec /bin/bash -i'"&
xterm -T "MP"       -e "/bin/bash -c 'cd ${BINDIR}/mp     && ./mp -i ${INFOFILE} -m ${MODELFILE};    exec /bin/bash -i'"&
xterm -T "Client#1" -e "/bin/bash -c 'cd ${BINDIR}/client && time ./client -i ${INFOFILE} -t ${TESTFILE} -v ${EXPCFILE}; exec /bin/bash -i'"&
