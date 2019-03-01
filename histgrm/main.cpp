#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <new.h>

#include "../include/defs.h"
#include "statistic.h"

void histgrm_usage(char *argv[]) {
    fprintf(stderr, "histgrm_usage: %s [options...] Input Output\n", argv[0]);
    fprintf(stderr, "option:\n");
    fprintf(stderr, "\t%-32s: %s\n", "-max", "maximum, 0.0");
    fprintf(stderr, "\t%-32s: %s\n", "-min", "minimum, 0.0");
    fprintf(stderr, "\t%-32s: %s\n", "-divpt", "division point, 500.0");
    fprintf(stderr, "\t%-32s: %s\n", "-h help", "display this message");
    exit(1);
}

int main(int argc, char *argv[])
{
    int i = 0, fc = 0;
    int tmplen = MAX_LINE - 1;
    char line[MAX_LINE] = "";
    long k, idx, dnum, hnum, fnum = 0, dvnum;
    long *hist = NULL;
    double d, scale, av = 0.0, divpt = 500.0, max = 0.0, min = 0.0;
    double dmax, dmin;
    double *dv = NULL;
    char **fcont = NULL;
    char infile[MAX_LINE] = "";
    char outfile[MAX_LINE] = "";
    FILE *fp;
    HISTGRM histgrm = NULL;
    //set_new_handler(heapover);

    for (i = 1; i < argc; i++) {
	if (streq(argv[i], "-h") || streq(argv[i], "-help")) {
	    histgrm_usage(argv);
	} else if (streq(argv[i], "-max")) {
            if (++i >= argc) histgrm_usage(argv);
	    max = (double)atof(argv[i]);
	} else if (streq(argv[i], "-min")) {
            if (++i >= argc) histgrm_usage(argv);
	    min = (double)atof(argv[i]);
	} else if (streq(argv[i], "-divpt")) {
            if (++i >= argc) histgrm_usage(argv);
	    divpt = (double)atof(argv[i]);
	}else {
	    if (*argv[i] == '-') {
		fprintf(stderr, "Unknown option: %s\n", argv[i]);
		histgrm_usage(argv);
	    }
	    if (fc == 0) {
		strcpy(infile, argv[i]);
		fc++;
	    } else if (fc == 1) {
		strcpy(outfile, argv[i]);
		fc++;
	    } else {
		fprintf(stderr, "too many files\n");
		histgrm_usage(argv);
	    }
	}
    }
    if (fc != 2) {
	fprintf(stderr, "not enough files\n");
	histgrm_usage(argv);
    }
    if (max <= min) {
	fprintf(stderr, "Maximum > Minimum\n");
	exit(1);
    }

    fprintf(stderr, "Input File: %s\n", infile);
    fprintf(stderr, "Output File: %s\n", outfile);

    scale = divpt / (max - min);
    hnum = (long)((max - min) * scale + 0.5) + 1;
    hist = new long [hnum];
    for (k = 0; k < hnum; k++) hist[k] = 0;

    // read file
    dnum = 0;	dmin = max;	dmax = min;
    if ((fp = fopen(infile, "rt")) == NULL) {
	fprintf(stderr, "Can't open file: %s\n", infile);
	exit(1);
    }
    while(fgets(line, tmplen, fp) != NULL) {
	if (line[0] != '#') {
	    d = (double)atof(line);
	    dmin = MIN(d, dmin);	dmax = MAX(d, dmax);
	    idx = (long)((d - min) * scale + 0.5);
	    if (0) { // modified on Feb. 23, 2009
		if (idx < 0 || idx >= hnum) {
		    fprintf(stderr, "Error %f, min = %f, max = %f\n", d, min, max);
		    exit(1);
		}
	    } else {
		if (idx < 0) idx = 0;
		if (idx >= hnum) idx = hnum - 1;
	    }
	    hist[idx] += 1;		dnum++;		av += d;
	}
    }
    fprintf(stderr, "   Accumulated number of samples: %ld\n", dnum);
    fprintf(stderr, "      Minimum = %f, Maximum = %f\n", dmin, dmax);
    fclose(fp);
    
    // read file
    if ((fp = fopen(outfile, "wt")) == NULL) {
	fprintf(stderr, "Can't open file: %s\n", outfile);
	exit(1);
    }
    fprintf(fp, "# Number of samples: %ld\n", dnum);
    fprintf(fp, "# Sample: Minimum = %f, Maximum = %f\n", dmin, dmax);
    /*
    fprintf(fp, "# Av. = %.3f, S.d. = %.3f\n", histgrm->av, histgrm->sd);
    fprintf(fp, "# Min = %.3f(%.3f), Lhin = %.3f, Med = %.3f, Uhin = %.3f, Max = %.3f(%.3f)\n", histgrm->min, min, histgrm->lhin, histgrm->med, histgrm->uhin, histgrm->max, max);
    */
    for (k = 0; k < hnum; k++)
	fprintf(fp, "%f	%e\n", (double)k / scale + min,
		hist[k] / (double)dnum);
    fclose(fp);

    // memory free
    delete [] hist;
    /*
    delete histgrm;
    delete [] dv;	dv = NULL;
    for (k = 0; k < fnum; k++) {delete [] fcont[k];	fcont[k] = NULL;}
    delete [] fcont;	fcont = NULL;
    */
    
    fprintf(stderr, "done\n");

    return 0;
}
