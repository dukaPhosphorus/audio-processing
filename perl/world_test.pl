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
$prjdir		= '/home/ysaito/DVC';

# data directory
$datdir		= "$prjdir/data";

# file list
$test_scp	= "$datdir/scp/test.scp";	# test file list

# tools
$ANA		= "/home/ysaito/study_master/world/analysis/analysis";
$SYN		= "/home/ysaito/study_master/world/synthesis/synthesis";

# settings
$sr		= 16000; # sampling rate [Hz] (must be 16000)
$minf0		= $ARGV[1];
$maxf0		= $ARGV[2];
$shift		= 5;     # frame shift length [msec]
$mceporder	= 59;    # order of mel-cepstrum
$winsize 	= 3;     # the number of window file for calculating delta

# check errors
if (!(-d "$datdir/win")){  print_error("Can't find win dir."); }

# test data
foreach $base (split("\n", `cat $test_scp`)){

	$wav_name = "$datdir/wav/$name/$base.wav";
	print "processing $base ($name) \n";

	# analysis
	$opt = " -minf0 $minf0 -maxf0 $maxf0 -sp gomi.sp -lf0 gomi.lf0 -bap gomi.bap $wav_name";
	shell("$ANA $opt");

	# synthesis and play wav
	$opt = "-sp gomi.sp -lf0 gomi.lf0 -bap gomi.bap gomi.wav";
	shell("$SYN $opt");
	shell("aplay gomi.wav");
	shell("rm gomi.wav");
	shell("rm gomi.lf0");
	shell("rm gomi.sp");
	shell("rm gomi.bap");
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

