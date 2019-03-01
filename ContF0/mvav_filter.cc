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

/* cut-off freq of moving-ave filter */
double fcut_mvav(int p, double t) {
   return 1.0/(2.0*p*t);
}

/* nPoint of mvav */
double np_mvav(double f, double t) {
   return 1.0/(2.0*f*t);
}

/* nPoint of mvav */
int np_mvav_rev(double f, double t) {
   double npd = 1.0 / (2.0 * f * t);
   int    np  = round(npd);
   double dif1, dif2;
   
   if (np % 2 != 0)
      return np;

   dif1  = fabs(fcut_mvav((int)(np+1), t) - f);
   dif2  = fabs(fcut_mvav((int)(np-1), t) - f);
   np    = ARGMIN(dif1, (int)(np-1), dif2, (int)(np-1));
   return np;
}


void mvavfiltering(DVECTOR sig, int nPoint, XBOOL f0mode){
   int t, k, k2;
   double *ave;
   int *num;

   ave = xalloc(sig->length, double);
   num = xalloc(sig->length, int);

   for (t = 0; t < sig->length; t++){
      ave[t] = 0.0;
      num[t] = 0;
      if (!(f0mode && UVis(sig->data[t]) == UNVOICE)) {
         for (k = -(nPoint-1)/2; k <= (nPoint+1)/2; k++) {
            k2 = MIN(sig->length-1,MAX(t + k,0));
            if ((f0mode && UVis(sig->data[k2]) == VOICE) || !f0mode) {
               ave[t] += sig->data[k2];
               num[t]++;
            }      
         }
      }
   }

   for (t = 0; t < sig->length; t++) {
      if (!(f0mode && UVis(sig->data[t]) == UNVOICE)) 
         sig->data[t] = ave[t]/num[t];
      
   }
   
   xfree(ave);
   xfree(num);
}


