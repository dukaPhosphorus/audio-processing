/***************************************************************/
/*                            ContF0                           */
/***************************************************************/
/*            Coded by Shinnosuke Takamichi (NAIST)            */
/*                          2013.01.24                         */
/***************************************************************/
/*This program calculate interpolate F0 in unvoiced region.    */
/***************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "../include/defs.h"
#include "../include/memory.h"
#include "../include/basic.h"
#include "../include/fileio.h"
#include "../include/option.h"
#include "../include/vector.h"
#include "../include/voperate.h"
#include "../include/matrix.h"
#include "../include/window.h"
#include "../include/filter.h"
#include "../include/fft.h"
#include "../include/TakLib.h"

double fcut_mvav(int p, double t);
double np_mvav(double f, double t);
int np_mvav_rev(double f, double t);
void mvavfiltering(DVECTOR sig, int nPoint, XBOOL f0mode);


typedef struct CONDITION_STRUCT {
   double coff;
   double shift;
   XBOOL extp;
   XBOOL FlagNlpf;
   char *zmean;
   XBOOL FlagNmsg;
   XBOOL FlagHelp;
} CONDITION;

CONDITION cond = {10.0, 5.0, 0, XFALSE, NULL, XFALSE, XFALSE};

#define NUM_ARGFILE 1
ARGFILE argfile_struct[] = {
    {"[input lf0 file]", NULL}
};

#define NUM_OPTION 7
OPTION opt[] ={
   {"-cf",     NULL, "cut-off freq [Hz]",                      "f",  NULL, TYPE_DOUBLE,  &cond.coff,     XFALSE},  
   {"-shift",  NULL, "frame shift [ms]",                       "f",  NULL, TYPE_DOUBLE,  &cond.shift,    XFALSE},
   {"-extp",   NULL, "extrapolation (0:no, 1:mean, 2:near V, 3:near V + noise)", "n", NULL, TYPE_INT, &cond.extp, XFALSE},
   {"-nolpf",  NULL, "not apply lpf after inter/extrapolation",NULL, NULL, TYPE_BOOLEAN, &cond.FlagNlpf, XFALSE},
   {"-zmean",  NULL, "set org F0 mean to 0 and print mean",    "s",  NULL, TYPE_STRING,  &cond.zmean,    XFALSE},
   {"-nmsg",   NULL, "no message",                             NULL, NULL, TYPE_BOOLEAN, &cond.FlagNmsg, XFALSE},  
   {"-help",   "-h", "print this message",                     NULL, NULL, TYPE_BOOLEAN, &cond.FlagHelp, XFALSE}
}; 

OPTIONS opts = {
    NULL, 1, NUM_OPTION, opt, NUM_ARGFILE, argfile_struct,
};

void dvsetsc(DVECTOR x, int st, int ed, double v) {
   int s = MAX(st, 0);
   int e = MIN(ed, x->length);
   int t;

   for (t = s; t < e; t++)
      x->data[t] = v;
}

// random
double uniform_noise( void ){
   return ((double)rand()+1.0)/((double)RAND_MAX+2.0);
}

// gauss noise
double gauss_noise( double mu, double sigma ){
   double z=sqrt( -2.0*log(uniform_noise()) ) * sin( 2.0*M_PI*uniform_noise() );
   return mu + sigma*z;
}

void add_noise(DVECTOR x, int st, int ed, double var) {
   int s = MAX(st, 0);
   int e = MIN(ed, x->length);
   int t;

   for (t = s; t < e; t++)
      x->data[t] += gauss_noise(0, var);
}

// main
int main(int argc, char *argv[])
{

   int      i, nFile, vstart, vend, nPoint;
   DVECTOR  f0, orgf0;
   double   fcut, shift, lf0mean, lf0var;
   char     fn_mean[256];
   FILE     *fp_mean = NULL;

   // Get argments
   opts.progname = xgetbasicname(argv[0]);
   nFile = GetOptions(argc, argv, &opts); 
   if (cond.FlagHelp == XTRUE)
      printhelp(opts, (char *)"cont F0");
   if (nFile != opts.num_file)
      printerr(opts,  (char *)"not enough files");
   if ((f0 = xreadf2dsignal(opts.file[0].name, 0, 0)) == NODATA)
      print_error("Can't open %s.", opts.file[0].name);
   if (cond.zmean != NULL) {
      strcpy(fn_mean, cond.zmean);
      fp_mean = Getfp(fn_mean, "w");
   }
   
   // Set mvavfiltering parm.
   fcut  = MAX(cond.coff, 0.0);
   shift = MAX(cond.shift, 1e-10) * 0.001;
   nPoint= np_mvav_rev(fcut, shift);
   if (!cond.FlagNmsg)
      fprintf(stderr, "filter: %d-point filter (cut off: %f)\n", nPoint, fcut_mvav(nPoint, shift));

   // Generate cont F0
   LF0stats(&lf0mean, &lf0var, f0); // (not revised)
   if (fp_mean != NULL) { // set mean to zero
      for (i = 0; i < f0->length; i++) 
         if (UVis(f0->data[i]) == VOICE) 
            f0->data[i] -= lf0mean;
   }   
   orgf0 = xdvreal(f0);
   mvavfiltering(f0, nPoint, XTRUE); // lpf
   F0InterP(f0, &vstart, &vend); // interpolation (not revised)

   // Extrapolation
   if (cond.extp != 0) {
      if (!cond.FlagNmsg)
         fprintf(stderr, "extrapolation: 0-%d & %d-%ld frames(%f,%f)\n", vstart, vend, orgf0->length, orgf0->data[vend - 1], orgf0->data[vend]);
         
      switch (cond.extp) {
         case 1: // mean
            dvsetsc(orgf0, 0,    vstart,        lf0mean); 
            dvsetsc(orgf0, vend-1, orgf0->length, lf0mean);
            break;
         case 2: // nearest V
            dvsetsc(orgf0, 0,    vstart,        orgf0->data[vstart]);
            dvsetsc(orgf0, vend-1, orgf0->length, orgf0->data[vend - 2]);
            break;
         case 3: // nearest V + noise
            dvsetsc(orgf0, 0,    vstart,        orgf0->data[vstart]);
            dvsetsc(orgf0, vend-1, orgf0->length, orgf0->data[vend - 2]);
            add_noise(orgf0, 0, vstart, lf0var / 30.0);
            add_noise(orgf0, vend-1, orgf0->length, lf0var / 30.0);
            break;
         default:
            print_error("Not supported extraction method (%d)",cond.extp);
      }
   }
   
   // Copy original f0 value in V region
   for (i = 0; i < f0->length; i++)
      if (UVis(orgf0->data[i]) == VOICE) 
         f0->data[i] = orgf0->data[i]; 
         
   // lpf
   if (!cond.FlagNlpf)
      mvavfiltering(f0, nPoint, XTRUE);
  
   // Print result
   fwrited2f(f0->data, f0->length, 0, stdout);
   if (fp_mean != NULL) {
      fprintf(fp_mean, "%f", lf0mean);
      fclose(fp_mean);
   }

   xdvfree(f0);
   xdvfree(orgf0);

   if (!cond.FlagNmsg) 
      fprintf(stderr, "done.\n");

   return 0;
}

