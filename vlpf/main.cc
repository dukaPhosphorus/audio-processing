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


DVECTOR xdvbwfilter(DVECTOR x, double fs, double fc);

void xdmbwfilter(DMATRIX mat, double fs, double fc){
   int d = 0; 
   DVECTOR vec, vec2;

   for (d = 0; d < mat->col; d++) {
      vec = xdmcutcol(mat, d, 0, mat->row);
      vec2= xdvbwfilter(vec, fs, fc);
      dmpastecol(mat, d, vec2, 0, mat->row, 0);
      xdvfree(vec); xdvfree(vec2);
   }
}

typedef struct CONDITION_STRUCT {
   long     dim;
   double   coff;
   double   shift;
   XBOOL    flag_dfmt;
   XBOOL    flag_nmsg;
   XBOOL    flag_zero;
   XBOOL    flag_help;
} CONDITION;

CONDITION cond = {25, 70.0, 5.0, XFALSE, XFALSE, XFALSE, XFALSE};

#define NUM_ARGFILE 1
ARGFILE argfile_struct[] = {
    {"[mat]", NULL}
};

#define NUM_OPTION 7
OPTION opt[] ={
   {"-dim",    NULL, "vector dimension",     "f",  NULL, TYPE_LONG,    &cond.dim,         XFALSE},  
   {"-cf",     NULL, "cut-off freq [Hz]",    "f",  NULL, TYPE_DOUBLE,  &cond.coff,        XFALSE},  
   {"-shift",  NULL, "frame shift [ms]",     "f",  NULL, TYPE_DOUBLE,  &cond.shift,       XFALSE},
   {"-double", NULL, "double in/out",        NULL, NULL, TYPE_BOOLEAN, &cond.flag_dfmt,   XFALSE},
   {"-zp",     NULL, "zero phase filter",    NULL, NULL, TYPE_BOOLEAN, &cond.flag_zero,   XFALSE},
   {"-nmsg",   NULL, "no message",           NULL, NULL, TYPE_BOOLEAN, &cond.flag_nmsg,   XFALSE},  
   {"-help",   "-h", "print this message",   NULL, NULL, TYPE_BOOLEAN, &cond.flag_help,   XFALSE}
}; 

OPTIONS opts = {
    NULL, 1, NUM_OPTION, opt, NUM_ARGFILE, argfile_struct,
};

// main
int main(int argc, char *argv[])
{
   int      nfile, t, d, T;
   long     dim;
   double   fc, fs;
   char     *fname = (char *)"stdin";
   DMATRIX  mat = NODATA, mat2 = NODATA;

   // Get argments
   opts.progname  = xgetbasicname(argv[0]);
   nfile = GetOptions(argc, argv, &opts); 
   dim   = MAX(cond.dim, 1);                       // vector dim
   fs    = MAX(1000.0 / cond.shift, 1.0);          // sampling freq
   fc    = MAX(1e-10, MIN(cond.coff, fs / 2.0));   // cut-off freq
   
   if (nfile == 1)
      fname = opts.file[0].name;
   if (cond.flag_help == XTRUE || nfile == 0)
      printhelp(opts, (char *)"vector lpf");
   if (!cond.flag_nmsg)
      fprintf(stderr, "%fHz-sampled, %fHz-cut\n", fs, fc);
   
   mat = xreadmatrix(fname, dim, cond.flag_dfmt);

   // low pass filter
   xdmbwfilter(mat, fs, fc);

   // zero phase filter
   if (cond.flag_zero) {
      T = mat->row;
      mat2 = xdmzeros(T, mat->col);
      for (t = 0; t < T; t++)
         for (d = 0; d < mat->col; d++)
            mat2->data[t][d] = mat->data[T - t - 1][d];

      xdmbwfilter(mat2, fs, fc);

      for (t = 0; t < T; t++)
         for (d = 0; d < mat->col; d++)
            mat->data[t][d] = mat2->data[T - t - 1][d];
      
      xdmfree(mat2);
   }

   writematrix((char *)"stdout", mat, cond.flag_dfmt);

   if (!cond.flag_nmsg) 
      fprintf(stderr, "done.\n");

   return 0;
}

