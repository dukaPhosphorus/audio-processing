#!/bin/perl
#################################################################
#                          make_f0hist.pl                       #
#################################################################
#                    Coded by Yuki Saito (UT)                   #
#                           2018.04.13                          #
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
$NORMPOW	= "$prjdir/src/normpow/normpow/normpow";
$F0HIST		= "python $prjdir/python/f0hist.py";

# SPTK tools
$X2X		= "$prjdir/hts-installer/SPTK/bin/x2x";

# settings
$sr		= 16000; # sampling rate [Hz] (must be 16000)
$shift		= 5;     # frame shift length [msec]

# make dir
shell("mkdir -p $datdir/wav_rev/$name");
shell("mkdir -p $datdir/f0def/$name");

# train data
foreach $base (split("\n", `cat $train_scp`)){

	$wav_name = "$datdir/wav/$name/$base.wav";
	print "processing $base ($name) \n";

	# normpow: Normalize power
	shell("$NORMPOW -nmsg -f $sr $wav_name $datdir/wav_rev/$name/$base.wav");

	# tempo: Extract fundamental frequency
	$opt = "-nmsg -shift $shift -f0shift $shift";
	shell("$TEMPO $opt $datdir/wav_rev/$name/$base.wav $datdir/f0def/$name/$base.f0");
}

# f0hist.py: generate histogram of F0 as .png file
shell("$F0HIST $train_scp $name $datdir/f0def/$name/tr_hist.png");

# test data
foreach $base (split("\n", `cat $test_scp`)){

	$wav_name = "$datdir/wav/$name/$base.wav";
	print "processing $base ($name) \n";

	# normpow: Normalize power
	shell("$NORMPOW -nmsg -f $sr $wav_name $datdir/wav_rev/$name/$base.wav");

	# tempo: Extract fundamental frequency
	$opt = "-nmsg -shift $shift -f0shift $shift";
	shell("$TEMPO $opt $datdir/wav_rev/$name/$base.wav $datdir/f0def/$name/$base.f0");
}

# f0hist.py: generate histogram of F0 as .png file
shell("$F0HIST $test_scp $name $datdir/f0def/$name/ev_hist.png");

# shell
sub shell($) {
   my ($command) = @_;
   my ($exit);

   $exit = system($command);

   if ( $exit / 256 != 0 ) {
      die "Error in $command\n";
   }
}
