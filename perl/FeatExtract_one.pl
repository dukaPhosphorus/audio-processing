#!/bin/perl
#################################################################
#                          FeatExtract.pl                          #
#################################################################
#             Coded by Shinnosuke Takamichi (NAIST)             #
#                           2012.10.21                          #
#################################################################

use File::Basename;

# work directory
$prjdir		= $ARGV[0];
$name		= $ARGV[1];

# data directory
$datdir		= "$prjdir/data";

# file list
$train_scp	= "$datdir/scp/train.scp";	# train file list
$test_scp	= "$datdir/scp/test.scp";	# test file list

# tools
$TEMPO		= "$prjdir/src/straight/tempo/tempo";
$STRAIGHT	= "$prjdir/src/straight/analysis/straight_ana";
$AP_ANA		= "$prjdir/src/straight/analysis/ap_ana";
$NORMPOW	= "$prjdir/src/normpow/normpow/normpow";
$F02LF0		= "perl $prjdir/perl/f02lf0.pl";
$VLPF		= "$prjdir/src/vlpf/vlpf";
$UTIL_F0	= "python $prjdir/python/util_f0.py";
$CONTF0		= "$prjdir/src/ContF0/ContF0";
$UV		= "$prjdir/src/UV/UV";

# SPTK tools
$X2X		= "$prjdir/hts-installer/SPTK/bin/x2x";

# settings
$sr		= $ARGV[2]; # sampling rate [Hz] (must be 16000)
$minf0_name	= $ARGV[3];
$maxf0_name	= $ARGV[4];
$shift		= $ARGV[5];     # frame shift length [msec]
$mceporder	= $ARGV[6];    # order of mel-cepstrum

# make dir
shell("mkdir -p $datdir/wav_rev/$name");
shell("mkdir -p $datdir/f0/$name");
shell("mkdir -p $datdir/lf0/$name");
shell("mkdir -p $datdir/clf0/$name");
shell("mkdir -p $datdir/mgc/$name");
shell("mkdir -p $datdir/bap/$name");
shell("mkdir -p $datdir/uv/$name");

#=pod

# train data
foreach $base (split("\n", `cat $train_scp`)){

	$wav_name = "$datdir/wav/$name/$base.wav";
	print "processing $base ($name) \n";

	# normpow: Normalize power
	shell("$NORMPOW -nmsg -f $sr $wav_name $datdir/wav_rev/$name/$base.wav");

	# tempo: Extract fundamental frequency
	$opt = "-nmsg -uf0 $maxf0_name -lf0 $minf0_name -maxf0 $maxf0_name -minf0 $minf0_name -shift $shift -f0shift $shift";
	shell("$TEMPO $opt $datdir/wav_rev/$name/$base.wav $datdir/f0/$name/$base.f0");
	
	# straight: Extract spectral features
	$opt = "-nmsg -mcep -order ".($mceporder)." -pow -shift $shift -f0shift $shift";
	shell("$STRAIGHT $opt -f0file $datdir/f0/$name/$base.f0 $datdir/wav_rev/$name/$base.wav $datdir/mgc/$name/$base.mgc");

	# bndap: Extract Band-aperiodic features
	$opt = "-nmsg -bndap -shift $shift -f0shift $shift ";
	shell("$AP_ANA $opt -f0file $datdir/f0/$name/$base.f0 $datdir/wav_rev/$name/$base.wav $datdir/bap/$name/$base.bap");

	# vlpf: trajectory smoothing (lowpass filtering toward spectral features)
	shell("$VLPF -nmsg -dim ".($mceporder+1)." -cf 50.0 -double -zp $datdir/mgc/$name/$base.mgc > $datdir/mgc/$name/$base.mgc.vlpf");

	# x2x: Convert file format from "binary double" to "binary float"
	shell("$X2X +df $datdir/mgc/$name/$base.mgc.vlpf > $datdir/mgc/$name/$base.mgc");
	shell("$X2X +df $datdir/bap/$name/$base.bap > $datdir/bap/$name/$base.bap.temp");
	shell("$X2X +af $datdir/f0/$name/$base.f0 > $datdir/f0/$name/$base.f0.temp");
	#shell("mv $datdir/lmgc/$base.mgc.temp $datdir/lmgc/$base.mgc");
	shell("rm $datdir/mgc/$name/$base.mgc.vlpf");
	shell("mv $datdir/bap/$name/$base.bap.temp $datdir/bap/$name/$base.bap");

	# f02lf0.pl: Convert F0 into log-scaled F0
	shell("$F02LF0 $datdir/f0/$name/$base.f0.temp > $datdir/lf0/$name/$base.lf0");
	shell("rm $datdir/f0/$name/$base.f0.temp");

	# util_f0.py: Estimates a continuous logF0 sequance and Unvoiced / Voiced regions
	shell("$UTIL_F0 $datdir/lf0/$name/$base.lf0 $datdir/clf0/$name/$base.clf0 $datdir/uv/$name/$base.uv");	
}

#=cut

# test data
foreach $base (split("\n", `cat $test_scp`)){

	$wav_name = "$datdir/wav/$name/$base.wav";
	print "processing $base ($name) \n";

	# normpow: Normalize power
	shell("$NORMPOW -nmsg -f $sr $wav_name $datdir/wav_rev/$name/$base.wav");

	# tempo: Extract fundamental frequency
	$opt = "-nmsg -uf0 $maxf0_name -lf0 $minf0_name -maxf0 $maxf0_name -minf0 $minf0_name -shift $shift -f0shift $shift";
	shell("$TEMPO $opt $datdir/wav_rev/$name/$base.wav $datdir/f0/$name/$base.f0");
	
	# straight: Extract spectral features
	$opt = "-nmsg -mcep -order ".($mceporder)." -pow -shift $shift -f0shift $shift";
	shell("$STRAIGHT $opt -f0file $datdir/f0/$name/$base.f0 $datdir/wav_rev/$name/$base.wav $datdir/mgc/$name/$base.mgc");

	# bndap: Extract Band-aperiodic features
	$opt = "-nmsg -bndap -shift $shift -f0shift $shift ";
	shell("$AP_ANA $opt -f0file $datdir/f0/$name/$base.f0 $datdir/wav_rev/$name/$base.wav $datdir/bap/$name/$base.bap");

	# vlpf: trajectory smoothing (lowpass filtering toward spectral features)
	shell("$VLPF -nmsg -dim ".($mceporder+1)." -cf 50.0 -double -zp $datdir/mgc/$name/$base.mgc > $datdir/mgc/$name/$base.mgc.vlpf");

	# x2x: Convert file format from "binary double" to "binary float"
	shell("$X2X +df $datdir/mgc/$name/$base.mgc.vlpf > $datdir/mgc/$name/$base.mgc");
	shell("$X2X +df $datdir/bap/$name/$base.bap > $datdir/bap/$name/$base.bap.temp");
	shell("$X2X +af $datdir/f0/$name/$base.f0 > $datdir/f0/$name/$base.f0.temp");
	#shell("mv $datdir/lmgc/$base.mgc.temp $datdir/lmgc/$base.mgc");
	shell("rm $datdir/mgc/$name/$base.mgc.vlpf");
	shell("mv $datdir/bap/$name/$base.bap.temp $datdir/bap/$name/$base.bap");

	# f02lf0.pl: Convert F0 into log-scaled F0
	shell("$F02LF0 $datdir/f0/$name/$base.f0.temp > $datdir/lf0/$name/$base.lf0");
	shell("rm $datdir/f0/$name/$base.f0.temp");

	# util_f0.py: Estimates a continuous logF0 sequance and Unvoiced / Voiced regions
	shell("$UTIL_F0 $datdir/lf0/$name/$base.lf0 $datdir/clf0/$name/$base.clf0 $datdir/uv/$name/$base.uv");	
}

# shell
sub shell($) {
   my ($command) = @_;
   my ($exit);

   $exit = system($command);

   if ( $exit / 256 != 0 ) {
      die "Error in $command\n";
   }
}
