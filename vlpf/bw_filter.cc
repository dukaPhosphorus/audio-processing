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


void bwf2_coef(double fs, double fc, double *ar, double *ma) {
   double w0, dm;

   w0    = tan(PI * fc / fs) / (2.0 * PI);
   dm    = 1.0 + (2.0 * sqrt(2.0) * PI * w0) + 4.0 * PI * PI * w0 * w0;
   ma[0] = (4.0 * PI * PI * w0 * w0) / dm;
   ma[1] = (8.0 * PI * PI * w0 * w0) / dm;
   ma[2] = (4.0 * PI * PI * w0 * w0) / dm;
   ar[0] = 1.0;
   ar[1] = (8.0 * PI * PI * w0 * w0 - 2.0) / dm;
   ar[2] = (1.0 - (2.0 * sqrt(2) * PI * w0) + 4 * PI * PI * w0 * w0) / dm;
}


double dfs(double x, double *a, int m, double *b, int n, double *buf, int *bufp)
{
   double y = 0.0;
   int i, p;
   int max;

   n++;
   m++;

   (m < n) ? (max = n) : (max = m);

   x = x * a[0];
   for (i = 1; i < m; i++) {
      if ((p = *bufp + i) >= max)
         p -= max;
      x -= buf[p] * a[i];
   }
   buf[*bufp] = x;
   for (i = 0; i < n; i++) {
      if ((p = *bufp + i) >= max)
         p -= max;
      y += buf[p] * b[i];
   }

   if (--*bufp < 0)
      *bufp += max;

   return (y);
}


DVECTOR xdvbwfilter(DVECTOR x, double fs, double fc) {
   DVECTOR y = xdvzeros(x->length); 
   double *ar, *ma;
   DVECTOR buf = xdvzeros(2048);
   int t, bufp = 0;
   int ord = 3;

   ar = xalloc(ord, double);
   ma = xalloc(ord, double);
   bwf2_coef(fs, fc, ar, ma);
   
   for (t = 0; t < x->length; t++)
      y->data[t] = dfs(x->data[t], ar, ord, ma, ord, buf->data, &bufp);

   xdvfree(buf);
   xfree(ar);
   xfree(ma);
   return y;
}
