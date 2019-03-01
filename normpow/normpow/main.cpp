/*
 *	main.c : main function of straight
 *
 *		coded by H.Banno 	1996/12/25
 *		modified by T. Toda	2001/2/12
 *			V30k18 (matlab)
 *
 *	STRAIGHT Analysis applied to concatenative TTS
 *		coded by T. Toda
 *
 *	Tomoki Toda (tomoki.toda@atr.co.jp)
 *			From Mar. 2001 to Sep. 2003
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/fileio.h"
#include "../include/option.h"
#include "../include/voperate.h"

#include "../sub/straight_sub.h"
#include "../sub/straight_body_sub.h"
#include "../sub/straight_vconv_sub.h"
#include "../sub/straight_synth_sub.h"

typedef struct CONDITION_STRUCT {
    double fs;		/* Sampling frequency [Hz] */
    XBOOL wav_flag;	/* wav file */
    XBOOL msg_flag;	/* print message */
    XBOOL help_flag;
} CONDITION;

CONDITION cond = {16000.0, XTRUE, XTRUE, XFALSE};

#define NUM_ARGFILE 2
ARGFILE argfile_struct[] = {
    {"[inputfile]", NULL},
    {"[outputfile]", NULL},
};

#define NUM_OPTION 4
OPTION option_struct[] = {
    {"-f", NULL, "sampling frequency [Hz]", "samp_freq", 
	 NULL, TYPE_DOUBLE, &cond.fs, XFALSE},
    {"-raw", NULL, "input raw file (16bit short)", NULL, 
	 NULL, TYPE_BOOLEAN, &cond.wav_flag, XFALSE},
    {"-nmsg", NULL, "no message", NULL, 
	 NULL, TYPE_BOOLEAN, &cond.msg_flag, XFALSE},
    {"-help", "-h", "display this message", NULL, 
	 NULL, TYPE_BOOLEAN, &cond.help_flag, XFALSE},
};

OPTIONS options_struct = {
    NULL, 1, NUM_OPTION, option_struct, NUM_ARGFILE, argfile_struct,
};

/* main */
int main(int argc, char *argv[])
{
    int i, fc;
    SVECTOR x = NODATA;
    DVECTOR xd = NODATA;

    /* get program name */
    options_struct.progname = xgetbasicname(argv[0]);

    /* get option */
    for (i = 1, fc = 0; i < argc; i++)
	if (getoption(argc, argv, &i, &options_struct) == UNKNOWN)
	    getargfile(argv[i], &fc, &options_struct);
    
    /* display message */
    if (cond.help_flag == XTRUE)
	printhelp(options_struct, "Speech Transformation and Representation using Adaptive Interpolaiton of weiGHTed spectrogram");
    if (fc != options_struct.num_file)
	printerr(options_struct, "not enough files");
    
    if (cond.wav_flag == XTRUE) {
	xd = xread_wavfile(options_struct.file[0].name, &cond.fs,
			   cond.msg_flag);
    } else {
	/* read wave data */
	if ((x = xreadssignal(options_struct.file[0].name, 0, 0)) == NODATA) {
	    fprintf(stderr, "Can't read wave data\n");
	    exit(1);
	} else {
	    if (cond.msg_flag == XTRUE)
		fprintf(stderr, "read wave: %s\n",
			options_struct.file[0].name);
	    xd = xsvtod(x);
	    xsvfree(x);
	}
    }

    double max;
    ss_waveampcheck_tb06(xd, cond.fs, 15.0);
    if ((max = MAX(dvmax(xd, NULL), -1.0 * dvmin(xd, NULL))) > 32000.0)
	printf("### Warning ### MAX = %f\n", max);
    
    /* write wave data */
    x = xdvtos(xd);
    if (cond.wav_flag == XTRUE) {
	writessignal_wav(options_struct.file[1].name, x, cond.fs);
    } else {
	writessignal(options_struct.file[1].name, x, 0);
    }

    /* memory free */
    xsvfree(x);
    xdvfree(xd);

    if (cond.msg_flag == XTRUE) fprintf(stderr, "done\n");

    return 0;
}
