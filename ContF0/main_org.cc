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
void mvavfiltering(FVECTOR sig, int nPoint, XBOOL f0mode);


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
OPTION option_struct[] ={
   {"-cf",     NULL, "cut-off freq [Hz]",                      "f",  NULL, TYPE_DOUBLE,  &cond.coff,     XFALSE},  
   {"-shift",  NULL, "frame shift [ms]",                       "f",  NULL, TYPE_DOUBLE,  &cond.shift,    XFALSE},
   {"-extp",   NULL, "extrapolation (0:not apply, 1:mean, 2;nearest V)", "n", NULL, TYPE_INT, &cond.extp, XFALSE},
   {"-nolpf",  NULL, "not apply lpf after inter/extrapolation",NULL, NULL, TYPE_BOOLEAN, &cond.FlagNlpf, XFALSE},
   {"-zmean",  NULL, "set org F0 mean to 0 and print mean",    "s",  NULL, TYPE_STRING,  &cond.zmean,    XFALSE},
   {"-nmsg",   NULL, "no message",                             NULL, NULL, TYPE_BOOLEAN, &cond.FlagNmsg, XFALSE},  
   {"-help",   "-h", "print this message",                     NULL, NULL, TYPE_BOOLEAN, &cond.FlagHelp, XFALSE}
}; 

OPTIONS options_struct = {
    NULL, 1, NUM_OPTION, option_struct, NUM_ARGFILE, argfile_struct,
};

// main
int main(int argc, char *argv[])
{
   int      i, fc, vstart, vend, nPoint;
   FVECTOR  infile, org;
   double   fcut, shift, dif1, dif2, lf0mean;
   char     fn_mean[256];
   FILE     *fp_mean = NULL;

   options_struct.progname = xgetbasicname(argv[0]);
   fc = GetOptions(argc, argv, &options_struct); 

   // Print message and read files
   if (cond.FlagHelp == XTRUE)
      printhelp(options_struct, "cont F0");
   if (fc != options_struct.num_file)
      printerr(options_struct, "not enough files");
   if ((infile = xreadfsignal(options_struct.file[0].name,0,0)) == NODATA)
      print_error("Can't open %s.", options_struct.file[0].name);
   if (cond.zmean != NULL) {
      strcpy(fn_mean, cond.zmean);
      fp_mean = Getfp(fn_mean, "w");
   }
   
   fcut  = MAX(cond.coff, 0.0);
   shift = MAX(cond.shift, 1e-10) * 0.001;
   nPoint= round(np_mvav(fcut, shift));
   if (nPoint %2 != 1){
      dif1  = fabs(fcut_mvav((int)(nPoint+1), shift) - fcut);
      dif2  = fabs(fcut_mvav((int)(nPoint-1), shift) - fcut);
      nPoint= ARGMIN(dif1, (int)(nPoint-1), dif2, (int)(nPoint-1));
   }   
   fcut  = fcut_mvav(nPoint, shift);

   if (!cond.FlagNmsg)
      fprintf(stderr, "filter: %d-point filter (cut off: %f)\n", nPoint, fcut);

   // Main process
   LF0stats(&lf0mean, NULL, infile);
   if (fp_mean != NULL) { // set mean to zero
      for (i = 0; i < infile->length; i++) 
         if (UVis(infile->data[i]) == VOICE) infile->data[i] -= lf0mean;
   }   
   org = xfvreal(infile);
   
   mvavfiltering(infile, nPoint, XTRUE);
   F0InterP(infile, &vstart, &vend); 

   if (cond.extp != 0) {
      if (!cond.FlagNmsg)
         fprintf(stderr, "extrapolation: 0-%d & %d-last frames\n", vstart, vend);
      switch (cond.extp) {
         case 1: // mean
            for (i = 0; i < infile->length; i++)
               if (i < vstart || i >= vend) org->data[i] = (fp_mean != NULL)? 0.0:lf0mean;
            break;
         case 2: // nearest V
            for (i = 0; i < vstart; i++) org->data[i] = org->data[vstart];
            for (i = vend; i < org->length; i++) org->data[i] = org->data[vend-1];
            break;
         default:
            print_error("Not supported extraction method (%d)",cond.extp);
      }
   }
   
   for (i = 0; i < infile->length; i++) // copy org v
      if (UVis(org->data[i]) == VOICE) infile->data[i] = org->data[i]; 
   if (!cond.FlagNlpf) {
      if (!cond.FlagNmsg)
         fprintf(stderr, "postfilter: %d-point filter (cut off: %f)\n", nPoint, fcut);      
      mvavfiltering(infile, nPoint, XTRUE);
   }

   // Print result
   for (i = 0; i < infile->length; i++) 
      fwrite(&(infile->data[i]), sizeof(float), (size_t)1, stdout);
   if (fp_mean != NULL) {
      fprintf(fp_mean, "%f", lf0mean);
      // fwrite(&(infile->data[i]), sizeof(float), (size_t)1, fp_mean);
      fclose(fp_mean);
   }

   xfvfree(infile);
   xfvfree(org);

   if (!cond.FlagNmsg) 
      fprintf(stderr, "done.\n");

   return 0;
}

