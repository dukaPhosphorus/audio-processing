#!/bin/bash

dir=$1
mkdir ${dir}/LAB
for alpha in {A..J}
do
	echo ${dir}
	mkdir ${dir}/LAB/${alpha}
	fnames=${dir}/TXT/SD/${alpha}/*.txt
	for f in ${fnames}
	do	
		echo $f
		base=`echo $f | cut -d'/' -f 9 | cut -d'.' -f 1`
		./bin/open_jtalk -x dic $f > ${dir}/LAB/${alpha}/${base}.lab
		sleep 0.05s
	done
done
