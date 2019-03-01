#!/usr/bin/perl
#######################################################
#                 f02lf0.pl                           #
# this program convert f0 file to lf0 file.           #
#######################################################
#           2012.06.15  Shinnosuke Takamichi          #
#######################################################

open( IN, $ARGV[0] );
@STAT = stat(IN);
read( IN, $data, $STAT[7] );
close(IN);

$T    = $STAT[7] / 4;
@frq  = unpack( "f$T", $data );
for ($t = 0; $t < $T; $t++) {
   if ( $frq[$t] == 0.00000 ) {  $out[$t] = -1.0e+10; }
   else {   $out[$t] = log( $frq[$t] );   }
}
$data = pack( "f$T", @out );
print $data;
