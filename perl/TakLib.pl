#!/bin/perl

# GetOpt module
use Getopt::Long;

# print "use TakLib.pl\n";

# directories
$SPTK       = "/home/is/shinnosuke-t/Research/HTS/SPTK";
$HTS        = "/home/is/shinnosuke-t/Research/HTS/htk";
$HTS_ENGINE = "/home/is/shinnosuke-t/Research/HTS/hts_engine_API";

# HTS commands
$HCOMPV	   = "$HTS/bin/HCompV";
$HLIST	   = "$HTS/bin/HList";
$HINIT	   = "$HTS/bin/HInit";
$HREST	   = "$HTS/bin/HRest";
$HEREST	   = "$HTS/bin/HERest";
$HHED		   = "$HTS/bin/HHEd";
$HSMMALIGN	= "$HTS/bin/HSMMAlign";
$HMGENS	   = "$HTS/bin/HMGenS";
$HVITE      = "$HTS/bin/HVite";
$HCOPY      = "$HTS/bin/HCopy";
$HLED       = "$HTS/bin/HLEd";
$HPARSE     = "$HTS/bin/HParse";
$ENGINE	   = "$HTS_ENGINE/bin/hts_engine";

# SPTK commands
$BCP		   = "$SPTK/bin/bcp";
$BCUT		   = "$SPTK/bin/bcut";
$B2MC		   = "$SPTK/bin/b2mc";
$C2ACR	   = "$SPTK/bin/c2acr";
$DFS		   = "$SPTK/bin/dfs";
$EXCITE	   = "$SPTK/bin/excite";
$FREQT	   = "$SPTK/bin/freqt";
$GLOGSP	   = "$SPTK/bin/glogsp";
$GRLOGSP	   = "$SPTK/bin/grlogsp";
$INTERPOLATE= "$SPTK/bin/interpolate";
$LSP2LPC	   = "$SPTK/bin/lsp2lpc";
$LSPCHECK	= "$SPTK/bin/lspcheck";
$MC2B		   = "$SPTK/bin/mc2b";
$MCEP       = "$SPTK/bin/mcep";
$MERGE	   = "$SPTK/bin/merge";
$MGLSADF	   = "$SPTK/bin/mglsadf";
$MGC2MGC	   = "$SPTK/bin/mgc2mgc";
$MGC2SP	   = "$SPTK/bin/mgc2sp";
$NAN		   = "$SPTK/bin/nan";
$PSGR		   = "$SPTK/bin/psgr";
$SOPR		   = "$SPTK/bin/sopr";
$SPEC		   = "$SPTK/bin/spec";
$SWAB		   = "$SPTK/bin/swab";
$VOPR		   = "$SPTK/bin/vopr";
$VSTAT	   = "$SPTK/bin/vstat";
$XGR		   = "$SPTK/bin/xgr";
$X2X		   = "$SPTK/bin/x2x";

# program
$SAMPFREQ   = "/home/is/shinnosuke-t/Research/HTS/src/C/wav/sampfreq/sampfreq";
$BIT        = "/home/is/shinnosuke-t/Research/HTS/src/C/wav/bit/bit";
$CHANNEL    = "/home/is/shinnosuke-t/Research/HTS/src/C/wav/channel/channel";
$HISTOGRAM  = "perl /home/is/shinnosuke-t/Research/HTS/src/perl/plot/Histogram.pl";
$EXTDIM     = "/home/is/shinnosuke-t/Research/HTS/src_toda/src/extdim/extdim";
$SYNTHESIS  = "/home/is/shinnosuke-t/Research/HTS/src_toda/src/synthesis/synthesis";
$F02LF0     = "perl /home/is/shinnosuke-t/Research/HTS/src/perl/HTS/f02lf0.pl";
$LF02F0     = "perl /home/is/shinnosuke-t/Research/HTS/src/perl/HTS/lf02f0.pl";
$FRAME      = "/home/is/shinnosuke-t/Research/HTS/src/C/frame2/frame2";
$ROTVEC     = "/home/is/shinnosuke-t/Research/HTS/src/C/RotVec/RotVec";
$FLITE_ANA  = "~/Research/HTS/flite+hts_engine_ana/bin/flite_hts_engine";
$CUTWAV     = "/home/is/shinnosuke-t/Research/HTS/src/C/CutWav/CutWav";
$FUL2MON    = "perl /home/is/shinnosuke-t/Research/HTS/src/perl/HTS/ful2mon.pl";

# print_error
sub print_error($){
   my ( $message ) = @_;
   
   print "ERROR: $message \n";
   exit(1);
}

# print_time
sub print_time ($) {
   my ($message) = @_;
   my ($ruler);

   $message .= `date`;

   $ruler = '';
   for ( $i = 0 ; $i <= length($message) + 10 ; $i++ ) {
      $ruler .= '=';
   }

   print "\n$ruler\n";
   print "Start @_ at " . `date`;
   print "$ruler\n\n";
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

sub ful2mon($){
   my (@full) = @_;
   my @mono;
   for (my $i = 0; $i <= $#full; $i++) { 
      # $full[$i] =~ s/^ *\d+ +\d+ +//g;                # delete deletable info (sttime, edtime)
      $full[$i] =~ /\w+\^\w+\-(\w+)\+\w+\=\w+\@.+/;   # extract monophone
      $mono[$i] = $1;
   }
   return @mono;
}

