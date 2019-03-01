#!/bin/perl
#################################################################
#                          FeatExtract.pl                          #
#################################################################
#             Coded by Shinnosuke Takamichi (NAIST)             #
#                           2012.10.21                          #
#################################################################

use File::Basename;

$name		= $ARGV[0];

# work directory
$prjdir		= '/home/tamaru/20180413_tamaru_aso_VC';

# data directory
$datdir		= "$prjdir/data";

# file list
$test_scp	= "$datdir/scp/test.scp";	# test file list

# tools
$TEMPO		= "$prjdir/src/straight/tempo/tempo";
$STRAIGHT	= "$prjdir/src/straight/analysis/straight_ana";
$AP_ANA		= "$prjdir/src/straight/analysis/ap_ana";
$NORMPOW	= "$prjdir/src/normpow/normpow/normpow";
$F02LF0		= "perl $prjdir/perl/f02lf0.pl";
$VLPF		= "$prjdir/src/vlpf/vlpf";
$UTIL_F0	= "python $prjdir/scripts/util_f0.py";
$CONTF0		= "$prjdir/src/ContF0/ContF0";
$UV		= "$prjdir/src/UV/UV";
$SYNTHESIS	= "$prjdir/src/synthesis/synthesis";

# SPTK tools
$X2X		= "$prjdir/hts-installer/SPTK/bin/x2x";

# settings
$sr		= 16000; # sampling rate [Hz] (must be 16000)
$minf0_name	= $ARGV[1];
$maxf0_name	= $ARGV[2];
$shift		= 5;     # frame shift length [msec]
$mceporder	= 39;    # order of mel-cepstrum
$winsize 	= 3;     # the number of window file for calculating delta

# make dir
shell("mkdir -p $datdir/wav_rev/$name");
shell("mkdir -p $datdir/f0/$name");
shell("mkdir -p $datdir/lf0/$name");
shell("mkdir -p $datdir/clf0/$name");
shell("mkdir -p $datdir/mgc/$name");
shell("mkdir -p $datdir/bap/$name");
shell("mkdir -p $datdir/uv/$name");

# test data
foreach $base (split("\n", `cat $test_scp`)){

	$wav_name = "$datdir/wav/$name/$base.wav";
	print "processing $base ($name) \n";

	# normpow: Normalize power
	shell("$NORMPOW -nmsg -f $sr $wav_name gomi_rev.wav");

	# tempo: Extract fundamental frequency
	$opt = "-nmsg -uf0 $maxf0_name -lf0 $minf0_name -maxf0 $maxf0_name -minf0 $minf0_name -shift $shift -f0shift $shift";
	shell("$TEMPO $opt gomi_rev.wav gomi.f0");
	
	# straight: Extract spectral features
	$opt = "-nmsg -mcep -order ".($mceporder)." -pow -shift $shift -f0shift $shift";
	shell("$STRAIGHT $opt -f0file gomi.f0 gomi_rev.wav gomi.mgc");

	# bndap: Extract Band-aperiodic features
	$opt = "-nmsg -bndap -shift $shift -f0shift $shift ";
	shell("$AP_ANA $opt -f0file gomi.f0 gomi_rev.wav gomi.bap");

	# vlpf: trajectory smoothing (lowpass filtering toward spectral features)
	shell("$VLPF -nmsg -dim ".($mceporder+1)." -cf 50.0 -double -zp gomi.mgc > gomi.mgc.vlpf");

	# synthesis and play wav
	$opt = "-nmsg -ord ".($mceporder)." -apf gomi.bap gomi.f0 gomi.mgc.vlpf gomi.wav";
	shell("$SYNTHESIS $opt");
	shell("aplay gomi.wav");
	shell("rm gomi_rev.wav");
	shell("rm gomi.f0");
	shell("rm gomi.mgc");
	shell("rm gomi.bap");
	shell("rm gomi.mgc.vlpf");
	shell("rm gomi.wav");

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

