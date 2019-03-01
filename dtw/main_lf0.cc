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
/*  DTW                                                              */
/*                                                                   */
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fileio.h"
#include "../include/option.h"
#include "../include/voperate.h"

#include "../sub/matope_sub.h"
#include "dtw_sub.h"

typedef struct CONDITION_STRUCT {
    double shiftm;
    long dim;
    long tdim;
    char *twfdat;
    char *intwf;
    char *dtwofile;
    char *dtwtfile;
    char *dtwotfile;
    XBOOL dtworg_flag;
    XBOOL rmuv_flag;
    XBOOL nmsg_flag;
    XBOOL help_flag;
} CONDITION;

CONDITION cond = {5.0, 40, 2, NULL, NULL, NULL, NULL, NULL,
		  XFALSE, XFALSE, XFALSE, XFALSE};

#define NUM_ARGFILE 2
ARGFILE argfile_struct[] = {
    {"[orgfile]", NULL},
    {"[tarfile]", NULL},
};

#define NUM_OPTION 12
OPTION option_struct[] ={
    {"-shift", NULL, "frame shift [ms]", "shift",
	 NULL, TYPE_DOUBLE, &cond.shiftm, XFALSE},
    {"-dim", NULL, "dimension", "dim",
	 NULL, TYPE_LONG, &cond.dim, XFALSE},
    {"-tdim", NULL, "target dimension", "tdim",
	 NULL, TYPE_LONG, &cond.tdim, XFALSE},
    {"-twfdat", NULL, "get time warping function plot file", "twfdat",
	 NULL, TYPE_STRING, &cond.twfdat, XFALSE},
    {"-intwf", NULL, "input time warping function file", "intwf",
	 NULL, TYPE_STRING, &cond.intwf, XFALSE},
    {"-dtwofile", NULL, "get DTWed original file", "dtwofile",
	 NULL, TYPE_STRING, &cond.dtwofile, XFALSE},
    {"-dtwtfile", NULL, "get DTWed target file", "dtwtfile",
	 NULL, TYPE_STRING, &cond.dtwtfile, XFALSE},
    {"-dtwotfile", NULL, "get DTWed joint (org-tar) file", "dtwotfile",
	 NULL, TYPE_STRING, &cond.dtwotfile, XFALSE},
    {"-dtworg", NULL, "DTW original vectors", NULL,
	 NULL, TYPE_BOOLEAN, &cond.dtworg_flag, XFALSE},
    {"-rmuv", NULL, "remove UV frames", NULL,
	 NULL, TYPE_BOOLEAN, &cond.rmuv_flag, XFALSE},
    {"-nmsg", NULL, "no message", NULL,
	 NULL, TYPE_BOOLEAN, &cond.nmsg_flag, XFALSE},
    {"-help", "-h", "display this message", NULL,
	 NULL, TYPE_BOOLEAN, &cond.help_flag, XFALSE},
};

OPTIONS options_struct = {
    NULL, 1, NUM_OPTION, option_struct, NUM_ARGFILE, argfile_struct,
};

// main
int main(int argc, char *argv[])
{
    int i, fc;
    long k, l, j, t;
    LMATRIX twf = NODATA;
    DMATRIX orgmat = NODATA;
    DMATRIX tarmat = NODATA;
    DMATRIX dtwomat = NODATA;
    DMATRIX dtwtmat = NODATA;
    DMATRIX dtwotmat = NODATA;
    DMATRIX tmpomat = NODATA;
    DMATRIX tmptmat = NODATA;
    FILE *fp;

    // get program name
    options_struct.progname = xgetbasicname(argv[0]);

    // get option
    for (i = 1, fc = 0; i < argc; i++)
	if (getoption(argc, argv, &i, &options_struct) == UNKNOWN)
	    getargfile(argv[i], &fc, &options_struct);

    // display message
    if (cond.help_flag == XTRUE)
	printhelp(options_struct, "Dynamic Time Warping");
    if (fc != options_struct.num_file)
	printerr(options_struct, "not enough files");

    // read inputfile
    if ((orgmat = xreaddmatrix(options_struct.file[0].name,
			       cond.dim, 0)) == NODATA) {
	fprintf(stderr, "DTW: Can't read Original File\n");
	exit(1);
    }
    if ((tarmat = xreaddmatrix(options_struct.file[1].name,
			       cond.tdim, 0)) == NODATA) {
	fprintf(stderr, "DTW: Can't read Target File\n");
	exit(1);
    }
    if (cond.nmsg_flag == XFALSE) {
	fprintf(stderr, "Original File [%ld][%ld]: %s\n",
		orgmat->row, orgmat->col, options_struct.file[0].name);
	fprintf(stderr, "Target File [%ld][%ld]: %s\n",
		tarmat->row, tarmat->col, options_struct.file[1].name);
    }

    if (strnone(cond.intwf)) {
	fprintf(stderr, "Error: need -intwf\n");
	exit(1);
    } else {
	// read input time warping function file
	if ((twf = xreadlmatrix(cond.intwf, 2, 0)) == NODATA) {
	    fprintf(stderr, "Can't read input time warping function file\n");
	    exit(1);
	}
    }
    if (strnone(cond.dtwofile) && strnone(cond.dtwtfile) &&
	strnone(cond.dtwotfile)) {
	// memory free
	xdmfree(orgmat);	xdmfree(tarmat);
    }

    // write time warping function plot file
    if (!strnone(cond.twfdat)) {
	// open file
	if ((fp = fopen(cond.twfdat, "wt")) == NULL) {
	    fprintf(stderr, "Can't open file\n");
	    exit(1);
	}
	// write file (x:original, y:target)
	for (k = 0; k < twf->row; k++)
	    for (j = 0; j < twf->data[k][1]; j++)
		if (twf->data[k][0] >= 0)
		    fprintf(fp, "%f %f\n", (double)(twf->data[k][0] + j)
			    * cond.shiftm, (double)k * cond.shiftm);
	if (cond.nmsg_flag == XFALSE)
	    fprintf(stderr, "write time warping function plot file: %s\n",
		    cond.twfdat);
	// close file
	fclose(fp);
    }

    // get cepstrum distortion
    if (!strnone(cond.dtwofile) ||
	!strnone(cond.dtwtfile) || !strnone(cond.dtwotfile)) {
	if (cond.dtworg_flag == XTRUE) {
	    // DTW
	    if ((dtwomat = xget_dtw_mat(orgmat, twf)) == NODATA) {
		fprintf(stderr, "DTW failed matrix (getting CD)\n");
		exit(1);
	    } else if (cond.nmsg_flag == XFALSE)
		fprintf(stderr, "DTWed original matrix [%ld][%ld]\n",
			dtwomat->row, dtwomat->col);
	    // memory free
	    xdmfree(orgmat);
	    // copy
	    dtwtmat = xdmclone(tarmat);
	    // memory free
	    xdmfree(tarmat);
	} else {
	    // DTW
	    if ((dtwomat = xget_dtw_orgmat_dbl(orgmat, twf)) == NODATA) {
		fprintf(stderr, "DTW failed matrix (getting CD)\n");
		exit(1);
	    } else if (cond.nmsg_flag == XFALSE)
		fprintf(stderr, "DTWed original matrix [%ld][%ld]\n",
			dtwomat->row, dtwomat->col);
	    // memory free
	    xdmfree(orgmat);
	    // DTW
	    if ((dtwtmat = xget_dtw_tarmat_dbl(tarmat, twf)) == NODATA) {
		fprintf(stderr, "DTW failed matrix (getting CD)\n");
		exit(1);
	    } else if (cond.nmsg_flag == XFALSE)
		fprintf(stderr, "DTWed target matrix [%ld][%ld]\n",
			dtwtmat->row, dtwtmat->col);
	    // memory free
	    xdmfree(tarmat);
	}

	if (cond.rmuv_flag == XTRUE) {
	    for (k = 0, t = 0; k < dtwtmat->row; k++) {
		if (dtwtmat->data[k][0] > 0.0) {
		    for (l = 0; l < dtwomat->col; l++)
			dtwomat->data[t][l] = dtwomat->data[k][l];
		    for (l = 0; l < dtwtmat->col; l++)
			dtwtmat->data[t][l] = dtwtmat->data[k][l];
		    t++;
		}
	    }
	    tmpomat = xdmzeros(t, dtwomat->col);
	    tmptmat = xdmzeros(t, dtwtmat->col);
	    for (k = 0; k < tmpomat->row; k++) {
		for (l = 0; l < tmpomat->col; l++)
		    tmpomat->data[k][l] = dtwomat->data[k][l];
		for (l = 0; l < tmptmat->col; l++)
		    tmptmat->data[k][l] = dtwtmat->data[k][l];
	    }
	    if (cond.nmsg_flag == XFALSE)
		fprintf(stderr, "Remove UV frames [%ld] -> [%ld]\n",
			dtwtmat->row, t);
	    xdmfree(dtwomat);	xdmfree(dtwtmat);
	    dtwomat = xdmclone(tmpomat);	dtwtmat = xdmclone(tmptmat);
	    xdmfree(tmpomat);	xdmfree(tmptmat);
	}

	// write the DTWed file
	if (!strnone(cond.dtwofile)) {
	    if (cond.nmsg_flag == XFALSE)
		fprintf(stderr, "write DTWed original file\n");
	    writedmatrix(cond.dtwofile, dtwomat, 0);
	}
	if (!strnone(cond.dtwtfile)) {
	    if (cond.nmsg_flag == XFALSE)
		fprintf(stderr, "write DTWed target file\n");
	    writedmatrix(cond.dtwtfile, dtwtmat, 0);
	}
	if (!strnone(cond.dtwotfile)) {
	    if (cond.nmsg_flag == XFALSE)
		fprintf(stderr, "write DTWed joint file\n");
	    dtwotmat = xjoint_matrow(dtwomat, dtwtmat);
	    writedmatrix(cond.dtwotfile, dtwotmat, 0);
	    // memory free
	    xdmfree(dtwotmat);
	}
	// memory free
	xdmfree(dtwomat);
	xdmfree(dtwtmat);
    }

    // memory free
    xlmfree(twf);

    if (cond.nmsg_flag == XFALSE)
	fprintf(stderr, "done\n");

    return 0;
}
