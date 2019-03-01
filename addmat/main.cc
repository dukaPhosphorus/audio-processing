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
/*  Creating Joint Feature Vectors                                   */
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

typedef struct CONDITION_STRUCT {
    long dim;
    double coef;
    double divcoef;
    char *frmsqef;
    XBOOL lf0_flag;
    XBOOL root_flag;
    XBOOL msg_flag;
    XBOOL help_flag;
} CONDITION;

CONDITION cond = {25, 1.0, 1.0, NULL, XFALSE, XFALSE, XTRUE, XFALSE};

#define NUM_ARGFILE 3
ARGFILE argfile_struct[] = {
    {"[mat1file]", NULL},
    {"[mat2file]", NULL},
    {"[outfile]", NULL},
};

#define NUM_OPTION 8
OPTION option_struct[] ={
    {"-dim", NULL, "dimension of vector", "dim",
	 NULL, TYPE_LONG, &cond.dim, XFALSE},
    {"-coef", NULL, "coefficient (mat1 + coef * mat2)", "coef",
	 NULL, TYPE_DOUBLE, &cond.coef, XFALSE},
    {"-divcoef", NULL, "coefficient for division (-frmsqef)", "divcoef",
	 NULL, TYPE_DOUBLE, &cond.divcoef, XFALSE},
    {"-frmsqef", NULL, "frame square error file", "frmsqef",
	 NULL, TYPE_STRING, &cond.frmsqef, XFALSE},
    {"-lf0", NULL, "log F0", NULL,
	 NULL, TYPE_BOOLEAN, &cond.lf0_flag, XFALSE},
    {"-root", NULL, "frame root square error", NULL,
	 NULL, TYPE_BOOLEAN, &cond.root_flag, XFALSE},
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
    int i, fc;
    long k, l;
    long vv = 0, vu = 0, uv = 0, uu = 0;
    XBOOL errflag = XTRUE;
    double sqe = 0.0, sumsqe = 0.0;
    DMATRIX mat1 = NODATA;
    DMATRIX mat2 = NODATA;
    FILE *fp = NULL;

    // get program name
    options_struct.progname = xgetbasicname(argv[0]);

    // get option
    for (i = 1, fc = 0; i < argc; i++) {
	if (getoption(argc, argv, &i, &options_struct) == UNKNOWN) {
	    getargfile(argv[i], &fc, &options_struct);
	}
    }

    // display message
    if (cond.help_flag == XTRUE)
	printhelp(options_struct, "Creating Joint Feature Vectors");
    if (fc != options_struct.num_file)
	printerr(options_struct, "not enough files");

    if (cond.msg_flag == XTRUE) {
	fprintf(stderr, "Input 1 File: %s\n", options_struct.file[0].name);
	fprintf(stderr, "Input 2 File: %s\n", options_struct.file[1].name);
	fprintf(stderr, "Output File: %s\n", options_struct.file[2].name);
    }

    mat1 = xreaddmatrix(options_struct.file[0].name, cond.dim, 0);
    mat2 = xreaddmatrix(options_struct.file[1].name, cond.dim, 0);

    if (mat1->row != mat2->row) {
	fprintf(stderr, "Error: different size ([%ld] [%ld])\n",
		mat1->row, mat2->row);
	exit(1);
    }

    if (!strnone(cond.frmsqef)) {
	check_dir(cond.frmsqef);
	if ((fp = fopen(cond.frmsqef, "wt")) == NULL) {
	    fprintf(stderr, "Can't open file: %s\n", cond.frmsqef);
	    exit(1);
	}
    }

    for (k = 0, sumsqe = 0.0; k < mat1->row; k++) {
	if (cond.lf0_flag == XTRUE) {
	    errflag = XFALSE;
	    if (mat1->data[k][0] > 0.0 && mat2->data[k][0] > 0.0) {
		vv++;
		errflag = XTRUE;
	    } else if (mat1->data[k][0] > 0.0) vu++;
	    else if (mat2->data[k][0] > 0.0) uv++;
	    else uu++;
	} else vv++;
	for (l = 0, sqe = 0.0; l < mat1->col; l++) {
	    mat1->data[k][l] += cond.coef * mat2->data[k][l];
	    if (fp != NULL)
		if (errflag == XTRUE)
		    sqe += mat1->data[k][l] * mat1->data[k][l];
	}
	if (fp != NULL) {
	    if (errflag == XTRUE) {
		sqe /= cond.divcoef;
		if (cond.root_flag) sqe = sqrt(sqe);
		fprintf(fp, "%f\n", sqe);
		sumsqe += sqe;
	    }
	}
    }
    if (fp != NULL) {
	fclose(fp);
	if (cond.msg_flag == XTRUE)
	    fprintf(stderr, "Frame-based sum of square errors: %s\n",
		    cond.frmsqef);
	if (cond.lf0_flag == XTRUE) {
	    printf("V1V2: %ld %.2f\n", vv,
		   (double)vv / (double)mat1->row * 100.0);
	    printf("V1U2: %ld %.2f\n", vu,
		   (double)vu / (double)mat1->row * 100.0);
	    printf("U1V2: %ld %.2f\n", uv,
		   (double)uv / (double)mat1->row * 100.0);
	    printf("U1U2: %ld %.2f\n", uu,
		   (double)uu / (double)mat1->row * 100.0);
	}
	fprintf(stderr, "#RMSE\n");
	if (cond.root_flag == XTRUE)
	    fprintf(stderr, "%f\n", sumsqe / (double)vv);
	else
	    fprintf(stderr, "%f\n", sqrt(sumsqe / (double)vv));
    }

    if (cond.msg_flag == XTRUE) {
	fprintf(stderr, "mat1 + %f * mat2\n", cond.coef);
	fprintf(stderr, "got matrix [%ld][%ld]\n", mat1->row, mat1->col);
    }

    // write output file
    writedmatrix(options_struct.file[2].name, mat1, 0);

    // memory free
    xdmfree(mat1);
    xdmfree(mat2);

    if (cond.msg_flag == XTRUE) fprintf(stderr, "done\n");

    return 0;
}
