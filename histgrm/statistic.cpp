#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <new.h>

#include "../include/defs.h"
#include "../include/matrix.h"
#include "../straight/sub/straight_sub.h"
#include "statistic.h"

#define TVAL 1.96

LRM_CLASS::LRM_CLASS(double *x, double *y, long n)
{
    avx = 0.0;    avy = 0.0;    avxx = 0.0;    avxy = 0.0;
    vx = 0.0;     vy = 0.0;     vxy = 0.0;
    ccoef = 0.0;  rse = 0.0;
    param = NULL;    ey = NULL;

    num = n;
    lrmparam(x, y);
    estlrm(x);
    corcoef();
    rselrm(y);
}

LRM_CLASS::~LRM_CLASS()
{
    delete [] param;  param = NULL;
    delete [] ey;  ey = NULL;
}

void LRM_CLASS::stinfo(double *x, double *y)
{
    long k;
    
    avx = 0.0;     avy = 0.0;    avxx = 0.0;    avxy = 0.0;
    for (k = 0; k < num; k++) {
        avx += x[k];		avy += y[k];
	avxy += x[k] * y[k];	avxx += x[k] * x[k];
    }
    avx /= (double)num;		avy /= (double)num;
    avxy /= (double)num;	avxx /= (double)num;
    
    vx = 0.0;    vy = 0.0;    vxy = 0.0;
    for (k = 0; k < num; k++) {
        vx += SQUARE(x[k] - avx);      vy += SQUARE(y[k] - avy);
	vxy += (x[k] - avx) * (y[k] - avy);
    }
    vx /= (double)num;    vy /= (double)num;    vxy /= (double)num;
    
    return;
}

void LRM_CLASS::lrmparam(double *x, double *y)
{
    stinfo(x, y);
    param = new double [2];
    if (avx == 0.0 && avxx == 0.0 && avxy == 0.0) {
	param[1] = 0.0;
    } else {
	param[1] = (avx * avy - avxy) / (avx * avx - avxx);
    }
    param[0] = avy - param[1] * avx;

    return;
}

void LRM_CLASS::estlrm(double *x)
{
    long k;

    ey = new double [num];
    for (k = 0; k < num; k++) ey[k] =  param[0] + param[1] * x[k];
    
    return;
}

void LRM_CLASS::corcoef()
{
    if (vx * vy == 0.0) {
        fprintf(stderr, "Covariance is 0: cov(x) = %f, cov(y) = %f\n",
		vx, vy);
	ccoef = 0.0;
    } else {
	ccoef = vxy / sqrt(vx * vy);
    }

    return;
}

void LRM_CLASS::rselrm(double *y)
{
    long k;

    rse = 0.0;
    for (k = 0; k < num; k++) rse += SQUARE(y[k] - ey[k]);
    rse = sqrt(rse / (double)(num - 2));

    return;
}

void LRM_CLASS::write_lrmf(char *file, double *x, double *y, XBOOL estflag)
{
    long k;
    FILE *fp;

    if ((fp = fopen(file, "wt")) == NULL) {
        fprintf(stderr, "Can't open file: %s\n", file);
	exit(1);
    }
    fprintf(fp, "# Linear regression model\n");
    fprintf(fp, "# y = %f + %f * x\n", param[0], param[1]);
    fprintf(fp, "# Correlation coefficient = %f\n", ccoef);
    fprintf(fp, "# Residual standard error = %f\n", rse);
    if (estflag == XFALSE) {
        fprintf(fp, "# Raw data\n");
        for (k = 0; k < num; k++) fprintf(fp, "%f	%f\n", x[k], y[k]);
    } else {
        fprintf(fp, "# Feature	Raw data	Estimated data\n");
        for (k = 0; k < num; k++) fprintf(fp, "%f	%f	%f\n", x[k], y[k], ey[k]);
    }
    fclose(fp);

    return;
}


LMRM_CLASS::LMRM_CLASS(double **x, double *y, long n, long p)
{
    avx = NULL;    avy = 0.0;    avey = 0.0;
    vy = 0.0;      vey = 0.0;     vyey = 0.0;
    mcoef = 0.0;   rse = 0.0;
    vcm = NULL;    vyv = NULL;
    param = NULL;  ey = NULL;

    num = n;
    dim = p;
    if (p < dim) {
        fprintf(stderr, "Dimension should be more than 2\n");
	exit(1);
    }
    lmrmparam(x, y);
    estlmrm(x);
    mcorcoef(y);
    rselmrm(y);
}

LMRM_CLASS::~LMRM_CLASS()
{
    long k;

    for (k = 0; k < dim; k++) {delete [] vcm[k]; vcm[k] = NULL;}
    delete [] vcm;  vcm = NULL;
    delete [] avx;  avx = NULL;
    delete [] vyv;  vyv = NULL;
    delete [] param;  param = NULL;
    delete [] ey;  ey = NULL;
}

void LMRM_CLASS::stinfo(double **x, double *y)
{
    long k, l, m;

    avx = new double [dim];
    for (l = 0; l < dim; l++) avx[l] = 0.0;
    avy = 0.0;
    for (k = 0; k < num; k++) {
        for (l = 0; l < dim; l++) avx[l] += x[k][l];
	avy += y[k];
    }
    for (l = 0; l < dim; l++) avx[l] /= (double)num;
    avy /= (double)num;

    vcm = new double * [dim];
    vyv = new double [dim];
    for (l = 0; l < dim; l++) {
        vcm[l] = new double [dim];
	for (m = 0; m < dim; m++) vcm[l][m] = 0.0;
	vyv[l] = 0.0;
    }

    for (k = 0; k < num; k++) {
        for (l = 0; l < dim; l++) {
	    for (m = l; m < dim; m++)
	        vcm[l][m] += (x[k][l] - avx[l]) * (x[k][m] - avx[m]);
	    vyv[l] += (y[k] - avy) * (x[k][l] - avx[l]);
	}
    }
    for (l = 0; l < dim; l++) {
        for (m = l; m < dim; m++) vcm[l][m] /= (double)num;
	for (m = 0; m < l; m++) vcm[l][m] = vcm[m][l];
	vyv[l] /= (double)num;
    }

    return;
}

double** LMRM_CLASS::xinvvcm()
{
    long k, l;
    double **invm = NULL;
    DMATRIX tmpm = NODATA;
    DMATRIX tmpim = NODATA;

    invm = new double * [dim];
    for (k = 0; k < dim; k++) invm[k] = new double [dim];

    tmpm = xdmalloc(dim, dim);
    for (k = 0; k < dim; k++)
        for (l = 0; l < dim; l++) tmpm->data[k][l] = vcm[k][l];
    tmpim = ss_xinvmat_svd(tmpm, 1.0e-12);

    for (k = 0; k < dim; k++)
        for (l = 0; l < dim; l++) invm[k][l] = tmpim->data[k][l];

    xdmfree(tmpm);    xdmfree(tmpim);

    return invm;
}

void LMRM_CLASS::lmrmparam(double **x, double *y)
{
    long k, l;
    double **invm = NULL;

    stinfo(x, y);
    invm = xinvvcm();

    param = new double [dim + 1];
    for (k = 0; k < dim; k++) {
        param[k + 1] = 0.0;
        for (l = 0; l < dim; l++) param[k + 1] += invm[k][l] * vyv[l];
    }
    param[0] = avy;
    for (k = 0; k < dim; k++) param[0] -= param[k + 1] * avx[k];

    for (k = 0; k < dim; k++) {
	delete [] invm[k];      invm[k] = NULL;
    }
    delete [] invm;   invm = NULL;
    
    return;
}

void LMRM_CLASS::estlmrm(double **x)
{
    long k, l;

    ey = new double [num];
    for (k = 0; k < num; k++) {
        ey[k] = param[0];
	for (l = 0; l < dim; l++)
	    ey[k] +=  param[l + 1] * x[k][l];
    }

    return;
}

void LMRM_CLASS::mcorcoef(double *y)
{
    long k;

    avey = 0.0;
    for (k = 0; k < num; k++) avey += ey[k];
    avey /= (double)num;

    vy = 0.0;    vey = 0.0;    vyey = 0.0;
    for (k = 0; k < num; k++) {
        vy += SQUARE(y[k] - avy);
        vey += SQUARE(ey[k] - avey);	vyey += (y[k] - avy) * (ey[k] - avey);
    }
    vy /= (double)num;    vey /= (double)num;    vyey /= (double)num;

    if (vy * vey == 0.0) {
        fprintf(stderr, "Covariance is 0: cov(y) = %f, cov(ey) = %f\n",
		vy, vey);
	exit(1);
    }
    mcoef = vyey / sqrt(vy * vey);

    return;
}

void LMRM_CLASS::rselmrm(double *y)
{
    long k;
    
    rse = 0.0;
    for (k = 0; k < num; k++) rse += SQUARE(y[k] - ey[k]);
    rse = sqrt(rse / (double)(num - (dim + 1)));

    return;
}

void LMRM_CLASS::write_lmrmf(char *file, double **x, double *y, XBOOL estflag,
			     XBOOL erflag)
{
    long k, l;
    double *pcv = NULL;
    FILE *fp;
    LMRMsub lmrmsub;

    if ((fp = fopen(file, "wt")) == NULL) {
        fprintf(stderr, "Can't open file: %s\n", file);
	exit(1);
    }
    fprintf(fp, "# y = %f", param[0]);
    for (l = 0; l < dim; l++) fprintf(fp, " + %f * x%ld", param[l + 1], l + 1);
    fprintf(fp, "\n");
    fprintf(fp, "# Multiple correlation coefficient = %f\n", mcoef);
    fprintf(fp, "# Standard deviation of error = %f\n", rse);
    // partial correlation coefficient
    pcv = lmrmsub.xget_parcoef(x, y, num, dim);
    fprintf(fp, "# Partial correlation coefficient\n");
    fprintf(fp, "# ");
    for (k = 0; k < dim; k++) fprintf(fp, " %f", pcv[k]);
    fprintf(fp, "\n");
    delete [] pcv;    pcv = NULL;

    if (estflag == XFALSE) {
        fprintf(fp, "# Raw data\n");
	for (k = 0; k < num; k++) {
	    fprintf(fp, "%f", x[k][0]);
	    for (l = 1; l < dim; l++) fprintf(fp, " %f", x[k][l]);
	    if (erflag == XFALSE) {
/*	        fprintf(fp, " %f\n", y[k]);*/
	        fprintf(fp, " %f %f\n", y[k], ey[k]);
	    } else {
	      /*fprintf(fp, " %f\n", ABS(y[k] - ey[k]));*/
	        fprintf(fp, " %f %f %f\n", y[k], ey[k], y[k] - ey[k]);
	    }
	}
    } else {
        fprintf(fp, "# Estimated data\n");
	for (k = 0; k < num; k++) {
	    fprintf(fp, "%f", x[k][0]);
	    for (l = 1; l < dim; l++) fprintf(fp, " %f", x[k][l]);
	    if (erflag == XFALSE) {
	        fprintf(fp, " %f\n", ey[k]);
	    } else {
	      /*fprintf(fp, " %f\n", ABS(y[k] - ey[k]));*/
	        fprintf(fp, " %f\n", y[k] - ey[k]);
	    }
	}
    }
    fclose(fp);

    return;
}


double* LMRMsub::xget_parcoef(double **x, double *y, long n, long p)
{
    long k, l, m, mm;
    double suu, suv, svv, avu, avv;
    double **dv = NULL;
    double *t = NULL, *u = NULL, *v = NULL;
    double *pcv = NULL;
    LMRM ylmrm = NULL, tlmrm = NULL;

    // memory allocation
    t = new double [n];    u = new double [n];    v = new double [n];
    dv = new double * [n];
    for (k = 0; k < n; k++) dv[k] = new double [p - 1];
    pcv = new double [p];

    for (k = 0; k < p; k++) {	// target dimension: k
	for (m = 0, mm = 0; m < p; m++) {
	    if (m == k) {	// target
		for (l = 0; l < n; l++) t[l] = x[l][m];
	    } else {		// data
		for (l = 0; l < n; l++) dv[l][mm] = x[l][m];
		mm++;
	    }
	}
	ylmrm = new LMRM_CLASS(dv, y, n, p - 1);
	tlmrm = new LMRM_CLASS(dv, t, n, p - 1);
	// estimated error
	for (l = 0, avu = 0.0, avv = 0.0; l < n; l++) {
	    u[l] = y[l] - ylmrm->ey[l];	avu += u[l];
	    v[l] = t[l] - tlmrm->ey[l];	avv += v[l];
	}
	avu /= (double)n;	avv /= (double)n;
	for (l = 0, suu = 0.0, svv = 0.0, suv = 0.0; l < n; l++) {
	    suu += SQUARE(u[l] - avu);	svv += SQUARE(v[l] - avv);
	    suv += (u[l] - avu) * (v[l] - avv);
	}
	suu /= (double)n;	svv /= (double)n;	suv /= (double)n;
	// partial correlation coefficient
	pcv[k] = suv / sqrt(suu * svv);
	// memory free
	delete ylmrm;	delete tlmrm;
    }
    // memory free
    delete [] t;	t = NULL;
    delete [] u;	u = NULL;
    delete [] v;	v = NULL;
    for (k = 0; k < n; k++) {delete [] dv[k]; dv[k] = NULL;}
    delete [] dv;	dv = NULL;

    return pcv;
}


HISTGRM_CLASS::HISTGRM_CLASS(double *x, long n)
{
    av = 0.0;	sd = 0.0;	ubv = 0.0;
    max = 0.0;	min = 0.0;
    med = 0.0;	uhin = 0.0;	lhin = 0.0;
    scale = 0.0;	hnum = 0;
    data = NULL;	hist = NULL;	idxv = NULL;

    if (n <= 0 || x == NULL) {
	fprintf(stderr, "Error: HISTGRM_CLASS::HISTGRM_CLASS\n");
	exit(1);
    }
    num = n;

    readdata(x);
    getparam1();
    getparam2();
}

HISTGRM_CLASS::HISTGRM_CLASS(float *x, long n)
{
    av = 0.0;	sd = 0.0;	ubv = 0.0;
    max = 0.0;	min = 0.0;
    med = 0.0;	uhin = 0.0;	lhin = 0.0;
    scale = 0.0;	hnum = 0;
    data = NULL;	hist = NULL;	idxv = NULL;

    if (n <= 0 || x == NULL) {
	fprintf(stderr, "Error: HISTGRM_CLASS::HISTGRM_CLASS\n");
	exit(1);
    }
    num = n;

    readfdata(x);
    getparam1();
    getparam2();
}

HISTGRM_CLASS::HISTGRM_CLASS(long *x, long n)
{
    av = 0.0;	sd = 0.0;	ubv = 0.0;
    max = 0.0;	min = 0.0;
    med = 0.0;	uhin = 0.0;	lhin = 0.0;
    scale = 0.0;	hnum = 0;
    data = NULL;	hist = NULL;	idxv = NULL;

    if (n <= 0 || x == NULL) {
	fprintf(stderr, "Error: HISTGRM_CLASS::HISTGRM_CLASS\n");
	exit(1);
    }
    num = n;

    readldata(x);
    getparam1();
    getparam2();
}

HISTGRM_CLASS::HISTGRM_CLASS()
{
    data = NULL;
    idxv = NULL;
    hist = NULL;
}

HISTGRM_CLASS::HISTGRM_CLASS(int *x, long n)
{
    av = 0.0;	sd = 0.0;	ubv = 0.0;
    max = 0.0;	min = 0.0;
    med = 0.0;	uhin = 0.0;	lhin = 0.0;
    scale = 0.0;	hnum = 0;
    data = NULL;	hist = NULL;	idxv = NULL;

    if (n <= 0 || x == NULL) {
	fprintf(stderr, "Error: HISTGRM_CLASS::HISTGRM_CLASS\n");
	exit(1);
    }
    num = n;

    readidata(x);
    getparam1();
    getparam2();
}

HISTGRM_CLASS::~HISTGRM_CLASS()
{
    if (data != NULL) {delete [] data;	data = NULL;}
    if (idxv != NULL) {delete [] idxv;	idxv = NULL;}
    if (hist != NULL) {delete [] hist;	hist = NULL;}
}

void HISTGRM_CLASS::readdata(double *x)
{
    long k;

    data = new double [num];
    for (k = 0; k < num; k++) data[k] = x[k];

    return;
}

void HISTGRM_CLASS::readfdata(float *x)
{
    long k;

    data = new double [num];
    for (k = 0; k < num; k++) data[k] = (double)x[k];

    return;
}

void HISTGRM_CLASS::readldata(long *x)
{
    long k;

    data = new double [num];
    for (k = 0; k < num; k++) data[k] = (double)x[k];

    return;
}

void HISTGRM_CLASS::readidata(int *x)
{
    long k;

    data = new double [num];
    for (k = 0; k < num; k++) data[k] = (double)x[k];

    return;
}

void HISTGRM_CLASS::getparam1()
{
    long k;

    max = data[0];	min = data[0];	av = 0.0;
    for (k = 0; k < num; k++) {
	if (max < data[k]) max = data[k];
	if (min > data[k]) min = data[k];
	av += data[k];
    }
    av /= (double)num;
    sd = 0.0;
    for (k = 0; k < num; k++) sd += SQUARE(data[k] - av);
    ubv = sd / (double)(num - 1);
    sd = sqrt(sd / (double)num);

    return;
}

void HISTGRM_CLASS::getparam2()
{
    long k, mididx;
    double dmididx;
    double *vec = NULL;

    idxv = new long [num];
    vec = new double [num];

    for (k = 0; k < num; k++) {
	idxv[k] = k;	vec[k] = data[k];
    }
    quicksort(vec, 0, num - 1, idxv);
    dmididx = ((double)num + 1.0) / 2.0;
    mididx = (long)dmididx;
    if (dmididx - (double)mididx != 0.0) {
        med = (vec[mididx - 1] + vec[mididx]) / 2.0;
    } else {
        med = vec[mididx - 1];
    }
    dmididx = ((double)mididx + 1.0) / 2.0;
    mididx = (long)dmididx;
    if (dmididx - (double)mididx != 0.0) {
        lhin = (vec[mididx - 1] + vec[mididx]) / 2.0;
	uhin = (vec[num - 1 - (mididx - 1)] + vec[num - 1 - mididx]) / 2.0;
    } else {
        lhin = vec[mididx - 1];
	uhin = vec[num - 1 - (mididx - 1)];
    }

    delete [] vec;	vec = NULL;

    return;
}

double HISTGRM_CLASS::get_climit(double tval)
{
    double climit;

    climit = tval * sqrt(ubv / (double)num);

    return climit;
}

void HISTGRM_CLASS::calhist(double divpt)
{
    long k;

    scale = divpt / (double)(max - min);
    hnum = (long)((max - min) * scale + 0.5) + 1;
    hist = new long [hnum];
    for (k = 0; k < hnum; k++) hist[k] = 0;
    for (k = 0; k < num; k++)
	hist[(long)((data[k] - min) * scale + 0.5)] += 1;

    return;
}

void HISTGRM_CLASS::calhist()
{
    long k;

    scale = 1.0;
    hnum = (long)((max - min) * scale + 0.5) + 1;
    hist = new long [hnum];
    for (k = 0; k < hnum; k++) hist[k] = 0;
    for (k = 0; k < num; k++)
	hist[(long)((data[k] - min) * scale + 0.5)] += 1;

    return;
}

void HISTGRM_CLASS::writehist(FILE *fp)
{
    long k;
    double climit;

    climit = get_climit(TVAL);
    fprintf(fp, "# Av. = %f, S.D. = %f, C.L. = %f\n", av, sd, climit);
    fprintf(fp, "# Min = %f, Lhin = %f, Med = %f, Uhin = %f, Max = %f\n",
	    min, lhin, med, uhin, max);
    for (k = 0; k < hnum; k++)
	fprintf(fp, "%f	%f\n", (double)k / (double)scale + (double)min,
		(double)hist[k] / (double)num);

    return;
}

void HISTGRM_CLASS::writedens(FILE *fp)
{
    long k;
    double climit;

    climit = get_climit(TVAL);
    fprintf(fp, "# Av. = %f, S.D. = %f, C.L. = %f\n", av, sd, climit);
    fprintf(fp, "# Min = %f, Lhin = %f, Med = %f, Uhin = %f, Max = %f\n",
	    min, lhin, med, uhin, max);
    for (k = 0; k < hnum; k++)
	fprintf(fp, "%f	%f\n", (double)k / (double)scale + (double)min,
		(double)hist[k] / (double)num * (double)scale);

    return;
}

void HISTGRM_CLASS::delhist()
{
    scale = 0.0;
    hnum = 0;
    if (hist != NULL) {delete [] hist;    hist = NULL;}

    return;
}

void HISTGRM_CLASS::writedist(FILE *fp)
{
    long k;
    double climit;

    climit = get_climit(TVAL);
    fprintf(fp, "# Av. = %f, S.D. = %f, C.L. = %f\n", av, sd, climit);
    fprintf(fp, "# Min = %f, Lhin = %f, Med = %f, Uhin = %f, Max = %f\n",
	    min, lhin, med, uhin, max);
    for (k = 0; k < num; k++)
	fprintf(fp, "%f	%f\n", data[idxv[k]], (double)(k + 1) / (double)num);
/*    for (k = 0; k < num; k++)
	fprintf(fp, "%f	%f\n", data[idxv[k]], (double)(k + 1) / (double)num * (max - 1) + 1.0);*/
    
    return;
}

double* HISTGRM_CLASS::calunidist()
{
    long k, l, m, dnum;
    double pre, sum = 0.0;
    double *unidata = NULL;

    unidata = new double [num];
    pre = idxv[0];	dnum = 0;
    for (k = 0, l = 0, m = 0; k < num; k++) {
	if (pre == data[idxv[k]]) {
	    sum += (double)(k + 1) / (double)num * (max - 1) + 1.0;
	    dnum++;
	} else {
	    sum /= (double)dnum;
	    for (; l < k; l++, m++) unidata[idxv[m]] = sum;
	    pre = data[idxv[k]];
	    sum = (double)(k + 1) / (double)num * (max - 1) + 1.0;
	    dnum = 1;
	}
    }
    if (m != k) {
	sum /= (double)dnum;
	for (; l < k; l++, m++) unidata[idxv[m]] = sum;
    }

    return unidata;
}

void HISTGRM_CLASS::writeunidistdata(char *file)
{
    long k;
    double *unidata = NULL;
    FILE *fp;

    if ((fp = fopen(file, "wt")) == NULL) {
	fprintf(stderr, "%s\n", file);
	exit(1);
    }
    unidata = calunidist();
    for (k = 0; k < num; k++) fprintf(fp, "%f\n", unidata[k]);
    fclose(fp);
    delete [] unidata;    unidata = NULL;
    
    return;
}

void HISTGRM_CLASS::quicksort(double *array, long lower, long upper, long *idx)
{
    long l, u, ltmp;
    double bound, tmp;

    bound = array[lower];
    l = lower;	u = upper;
    do {
	while (array[l] < bound) l++;
	while (array[u] > bound) u--;
	if (l <= u) {
	    tmp = array[u];
	    array[u] = array[l];	array[l] = tmp;
	    ltmp = idx[u];
	    idx[u] = idx[l];		idx[l] = ltmp;
	    l++;			u--;
	}
    } while (l < u);
    if (lower < u) quicksort(array, lower, u, idx);
    if (l < upper) quicksort(array, l, upper, idx);

    return;
}
