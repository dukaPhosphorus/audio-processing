/* **** warning ****
STRAIGHT is always under construction. Please use it with your own risk.
**** end of warning ****

The procedures and data, so-called STRAIGHT-suite, in this directory
are under the copyright of ATR Human Information Processing Research
laboratories, Wakayama University, CREST and Prof. Hideki Kawahara. No
programs and data in this directory should be distributed without the
written permission by Prof. Hideki Kawahara.

Hideki Kawahara, Professor
Media Design Informatics Group 
Design and Information Science Dept.
Faculty of Systems Engineering, Wakayama University
Sakaedani, Wakayama, Wakayama 640-8510, Japan
Phone: +81-734-57-8461 Fax: +81-734-57-8112
E-mail: kawahara@sys.wakayama-u.ac.jp

However, the copyright holders do not wish to restrict the use of
STRAIGHT-suite for research purpose. Permission for research use
may be provided without any problem, if requested. If you wish to
apply STRAIGHT-suite for commercial use, please contact Dr. Katagiri
of ATR Human Information Processing Research laboratories.

Shigeru Katagiri, Dr.
Head, 1st department
ATR Human Information Processing Research Laboratories
2-2 Hikaridai, Seika-cho, Soraku-gun,
Kyoto 619-0288, Japan
Phone: +81-774-95-1054  Fax: +81-774-95-1008
E-mail: katagiri@hip.atr.co.jp

NO WARRANTY: STRAIGHT-suite is always under construction. The
copyright holders do not have any responsibilities for possible damage
or loss of assets which may be caused by improper or proper use of
STRAIGHT-suite. Please use it at your own risk.

STRAIGHT support page:
Please do not disclose these URL. They are subject to change.
http://www.sys.wakayama-u.ac.jp/~kawahara/puzzlet/STRAIGHTtipse/

Last update: 23 December, 2000.
*/

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
/*          Author :  Tomoki Toda (tomoki@ics.nitech.ac.jp)          */
/*          Date   :  June 2004                                      */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/*  TEMPO V30k18 (matlab)                                            */
/*                                                                   */
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../include/fileio.h"
#include "../../include/option.h"
#include "../../include/voperate.h"
#include "../../sub/anasyn_sub.h"
#include "tempo.h"

typedef struct CONDITION_STRUCT {
    double maxf0;
    double minf0;
    double f0ceil;
    double f0floor;
    double fs;
    double shiftm;
    double f0shiftm;
    char *cf0file;
    XBOOL fast_flag;
    XBOOL raw_flag;
    XBOOL msg_flag;
    XBOOL help_flag;
} CONDITION;

CONDITION cond = {800.0, 40.0, 800.0, 40.0, 16000.0, 1.0, 5.0,
		  NULL, XFALSE, XFALSE, XTRUE, XFALSE};

#define NUM_ARGFILE 2
ARGFILE argfile_struct[] = {
    {"[inputfile]", NULL},
    {"[outputfile]", NULL},
};

#define NUM_OPTION 12
OPTION option_struct[] = {
    {"-maxf0", NULL, "maximum of output f0 [Hz]", "maxf0", 
	 NULL, TYPE_DOUBLE, &cond.maxf0, XFALSE},
    {"-minf0", NULL, "minimum of output f0 [Hz]", "minf0", 
	 NULL, TYPE_DOUBLE, &cond.minf0, XFALSE},
    {"-uf0", NULL, "f0 upper limit [Hz]", "upperf0", 
	 NULL, TYPE_DOUBLE, &cond.f0ceil, XFALSE},
    {"-lf0", NULL, "f0 lower limit [Hz]", "lowerf0", 
	 NULL, TYPE_DOUBLE, &cond.f0floor, XFALSE},
    {"-f", NULL, "sampling frequency [Hz]", "samp_freq", 
	 NULL, TYPE_DOUBLE, &cond.fs, XFALSE},
    {"-shift", NULL, "frame shift [ms], (<= 5 ms)", "shift", 
	 NULL, TYPE_DOUBLE, &cond.shiftm, XFALSE},
    {"-f0shift", NULL, "f0 frame shift [ms]", "f0shift", 
	 NULL, TYPE_DOUBLE, &cond.f0shiftm, XFALSE},
    {"-cf0file", NULL, "candidate f0 file", "cf0file", 
	 NULL, TYPE_STRING, &cond.cf0file, XFALSE},
    {"-fast", NULL, "fast version", NULL, 
	 NULL, TYPE_BOOLEAN, &cond.fast_flag, XFALSE},
    {"-raw", NULL, "input raw file (16bit short)", NULL, 
	 NULL, TYPE_BOOLEAN, &cond.raw_flag, XFALSE},
    {"-nmsg", NULL, "no message", NULL, 
	 NULL, TYPE_BOOLEAN, &cond.msg_flag, XFALSE},
    {"-help", "-h", "display this message", NULL, 
	 NULL, TYPE_BOOLEAN, &cond.help_flag, XFALSE},
};

OPTIONS options_struct = {
    NULL, 1, NUM_OPTION, option_struct, NUM_ARGFILE, argfile_struct,
};

// main
int main(int argc, char *argv[])
{
    int i;
    int fc;
    SVECTOR xs = NODATA;
    DVECTOR xd = NODATA;
    DVECTORS f0info = NODATA;

    // Copyright
    fprintf(stderr, "F0 Extractor in STRAIGHTV30kr16\n");
    fprintf(stderr, "Copyright (C) 1997-2004\n");
    fprintf(stderr, " ATR Human Information Processing Research laboratories,\n");
    fprintf(stderr, " Wakayama University, JST and Prof. Hideki Kawahara.\n");
    fprintf(stderr, "All rights reserved.\n");

    // get program name
    options_struct.progname = xgetbasicname(argv[0]);

    // get option
    for (i = 1, fc = 0; i < argc; i++)
	if (getoption(argc, argv, &i, &options_struct) == UNKNOWN)
	    getargfile(argv[i], &fc, &options_struct);

    // display message
    if (cond.help_flag == XTRUE)
	printhelp(options_struct, "Time-domain Excitation extractor using Minimum Perturbation Operator");
    if (fc != options_struct.num_file)
	printerr(options_struct, "not enough files");
    
    // read wave data
    if (cond.raw_flag == XFALSE) {
	xd = xread_wavfile(options_struct.file[0].name, &cond.fs,
			  cond.msg_flag);
    } else {
	if ((xs = xreadssignal(options_struct.file[0].name, 0, 0)) == NODATA) {
	    fprintf(stderr, "Can't read wave data\n");
	    exit(1);
	} else {
	    if (cond.msg_flag == XTRUE)
		fprintf(stderr, "read signal: %s\n",
			options_struct.file[0].name);
	    xd = xsvtod(xs);
	    xsvfree(xs);
	}
    }

    if ((f0info = tempo(xd, cond.maxf0, cond.minf0, cond.f0ceil,
			cond.f0floor, cond.fs, cond.shiftm, cond.f0shiftm,
			cond.cf0file, XFALSE, XFALSE, cond.msg_flag,
			cond.fast_flag)) == NODATA) {
	fprintf(stderr, "Failed pitch extraction\n");
	exit(1);
    }
    
    // write f0 file
    writedvector_txt(options_struct.file[1].name, f0info->vector[0]);

    // memory free
    xdvfree(xd);
    xdvsfree(f0info);

    if (cond.msg_flag == XTRUE) fprintf(stderr, "done\n");

    return 0;
}
