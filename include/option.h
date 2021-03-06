/*********************************************************************/
/*                                                                   */
/*            Nagoya Institute of Technology, Aichi, Japan,          */
/*       Nara Institute of Science and Technology, Nara, Japan       */
/*                                and                                */
/*             Carnegie Mellon University, Pittsburgh, PA            */
/*                      Copyright (c) 2003-2004                      */
/*                        All Rights Reserved.                       */
/*                                                                   */
/*  Permission is hereby granted, free of charge, to use and         */
/*  distribute this software and its documentation without           */
/*  restriction, including without limitation the rights to use,     */
/*  copy, modify, merge, publish, distribute, sublicense, and/or     */
/*  sell copies of this work, and to permit persons to whom this     */
/*  work is furnished to do so, subject to the following conditions: */
/*                                                                   */
/*    1. The code must retain the above copyright notice, this list  */
/*       of conditions and the following disclaimer.                 */
/*    2. Any modifications must be clearly marked as such.           */
/*    3. Original authors' names are not deleted.                    */
/*                                                                   */    
/*  NAGOYA INSTITUTE OF TECHNOLOGY, NARA INSTITUTE OF SCIENCE AND    */
/*  TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, AND THE CONTRIBUTORS TO  */
/*  THIS WORK DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,  */
/*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, */
/*  IN NO EVENT SHALL NAGOYA INSTITUTE OF TECHNOLOGY, NARA           */
/*  INSTITUTE OF SCIENCE AND TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, */
/*  NOR THE CONTRIBUTORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR      */
/*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM   */
/*  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,  */
/*  NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN        */
/*  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.         */
/*                                                                   */
/*********************************************************************/
/*                                                                   */
/*          Author :  Hideki Banno                                   */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/*  Slightly modified by Tomoki Toda (tomoki@ics.nitech.ac.jp)       */
/*  June 2004                                                        */
/*  Integrate as a Voice Conversion module                           */
/*                                                                   */
/*-------------------------------------------------------------------*/

#ifndef __OPTION_H
#define __OPTION_H

#include "defs.h"

#define TYPE_BOOLEAN 0
#define TYPE_STRING 1
#define TYPE_INT 2
#define TYPE_SHORT 3
#define TYPE_LONG 4
#define TYPE_FLOAT 5
#define TYPE_DOUBLE 6
#define TYPE_CHAR 7
#define TYPE_OTHERS 8
#define TYPE_NONE 9
#define TYPE_XRM 10
#define TYPE_STRING_S 11

#define USAGE_LABEL_STRING "???"

#define eqtype(type1, type2) ((type1 == type2) ? 1 : 0)
#define str2bool(value) (((*(value) == 'T') || streq(value, "ON") || streq(value, "On")) ? XTRUE : XFALSE)
#define bool2str(value) ((*(XBOOL *)(value) == XTRUE) ? "True" : "False")

typedef struct ARGFILE_STRUCT {
    const char *label;	/* label for help message */
    char *name;		/* filename */
} ARGFILE, *ARGFILE_P;

typedef struct OPTION_STRUCT {
    const char *flag;		/* option flag */
    const char *subflag;	/* option subflag */
    const char *desc;		/* description for help */
    const char *label;	/* label for setup file */
    const char *specifier;	/* specifier for using X11 */
    int type;		/* type of value */
    void *value;	/* option value */
    XBOOL changed;	/* true if value changed */
} OPTION, *OPTION_P;

typedef struct OPTIONS_STRUCT {
    char *progname;	/* program name */
    int section;	/* section number */
    int num_option;	/* number of option */
    OPTION *option;	/* option structure */
    int num_file;	/* number of file */
    ARGFILE *file;	/* file structure */
} OPTIONS, *OPTIONS_P;

extern char *getbasicname(char *name);
extern char *xgetbasicname(char *name);
extern char *xgetdirname(char *name);
extern char *xgetexactname(char *name);
extern int flageq(char *flag, OPTIONS options);
extern int convoptvalue(char *value, OPTION *option);
extern int setoptvalue(char *value, OPTION *option);
extern int getoption(int argc, char *argv[], int *ac, OPTIONS *options);
extern void setchanged(int argc, char *argv[], OPTIONS *options);
extern int getargfile(char *filename, int *fc, OPTIONS *options);
#ifdef VARARGS
extern void printhelp();
extern void printerr();
#else
extern void printhelp(OPTIONS options, char *format, ...);
extern void printerr(OPTIONS options, char *format, ...);
#endif
extern int labeleq(char *label, OPTIONS *options);
extern void readsetup(char *filename, OPTIONS *options);
extern void writesetup(char *filename, OPTIONS options);
extern void usage(OPTIONS options);
extern int GetOptions(int argc, char *argv[], OPTIONS* opt);

#endif /* __OPTION_H */
