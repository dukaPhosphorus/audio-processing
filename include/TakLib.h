/*       TakLib.h                */
/*  made by Shinnosuke Takamichi */

#include<stdio.h>
#include<string.h>
#include<stdarg.h>

#include "defs.h"
#include "vector.h"
#include "memory.h"
#include "matrix.h"

#define VOICE 1
#define UNVOICE 0
#define USYMBOL -1e+10
#define UVis(f) ((f) == USYMBOL ? UNVOICE:VOICE)

//typedef enum {FALSE=0, TRUE=1} Boolean; /* Boolean type definition */
extern void message(int m, char *message, ...);
extern void print_error(const char *message, ...);
extern void print_sub(char *message, ...);
extern void print_info(char *message, char *parm);
// extern char *basename(char *fn);
extern FILE *Getfp(const char *name, const char *opt);
extern int AnyFileGetLength(const char *name, const char *opt);
//Boolean FileExist(char *fname); 
extern void chomp(char *str);
extern int Index(char *str, char *key);
extern void Uniq(char *str, char s);
XBOOL CopyUV(FVECTOR ref, FVECTOR rev, XBOOL ignU2V);
void F0InterP(DVECTOR infile, int *vs, int *ve);
double SplineInterP(double xp[], double yp[], int N, double x);
void LF0stats(double *mean, double *variance, DVECTOR LF0);
void PrtHTKHeader(long nframe, long dim, FILE *fp);
DVECTOR ext_vframe(DVECTOR lf0);
DMATRIX xreadmatrix(char *fn, long d, XBOOL dbl);
void writematrix(char *fn, DMATRIX x, XBOOL dbl);
DVECTOR xreadvector(char *fn, XBOOL dbl);
void writevector(char *fn, DVECTOR x, XBOOL dbl);
