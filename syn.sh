#!/bin/sh

outdir=$1
ndim_mgc=$2
mkdir -p ${outdir}/wav

data=`cat data/scp/test.scp`
while read line
do
	src/synthesis/synthesis \
		-apf ${outdir}/bap/${line}.bap \
		${outdir}/f0/${line}.f0 \
		${outdir}/mgc/${line}.mgc \
		-ord ${ndim_mgc} \
		${outdir}/wav/${line}.wav
done << FILE
$data
FILE
