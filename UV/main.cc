/***************************************************************/
/***************************************************************/
/*          Coded by Yuki Saito (University of Tokyo)          */
/*                          2016.10.03                         */
/***************************************************************/
/*This program generates unvoiced / voiced region of F0.       */
/***************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "../include/toda/defs.h"
#include "../include/toda/memory.h"
#include "../include/toda/basic.h"
#include "../include/toda/fileio.h"
#include "../include/toda/option.h"
#include "../include/toda/vector.h"
#include "../include/toda/voperate.h"
#include "../include/toda/matrix.h"
#include "../include/toda/window.h"
#include "../include/toda/filter.h"
#include "../include/toda/fft.h"
#include "../include/toda/TakLib.h"

typedef struct CONDITION_STRUCT {
   XBOOL FlagNmsg;
   XBOOL FlagHelp;
} CONDITION;

CONDITION cond = {XFALSE, XFALSE};

#define NUM_ARGFILE 1
ARGFILE argfile_struct[] = {
    {"[input lf0 file]", NULL}
};

#define NUM_OPTION 2
OPTION opt[] = {
   {"-nmsg",   NULL, "no message",                             NULL, NULL, TYPE_BOOLEAN, &cond.FlagNmsg, XFALSE},
   {"-help",   "-h", "print this message",                     NULL, NULL, TYPE_BOOLEAN, &cond.FlagHelp, XFALSE}
};

OPTIONS opts = {
    NULL, 1, NUM_OPTION, opt, NUM_ARGFILE, argfile_struct,
};

// main
int main(int argc, char *argv[])
{
   int      i, nFile;
   int*     UV;
   DVECTOR  f0;

   // Get argments
   opts.progname = xgetbasicname(argv[0]);
   nFile = GetOptions(argc, argv, &opts);
   if (cond.FlagHelp == XTRUE)
      printhelp(opts, (char *)"Unvoiced / Voiced region of F0");
   if (nFile != opts.num_file)
      printerr(opts,  (char *)"not enough files");
   if ((f0 = xreadf2dsignal(opts.file[0].name, 0, 0)) == NODATA)
      print_error("Can't open %s.", opts.file[0].name);

   UV = new int[f0->length];

   // Generate U/V
   for (i = 0; i < f0->length; i++) {
    UV[i] = UVis(f0->data[i]) == VOICE ? 1 : 0;
   }

   fwrite(UV, sizeof(UV), 1, stdout);

   xdvfree(f0);
   delete[] UV;

   if (!cond.FlagNmsg)
      fprintf(stderr, "done.\n");

   return 0;
}
