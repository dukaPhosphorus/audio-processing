#ifndef __STATISTIC_H
#define __STATISTIC_H

typedef class LRM_CLASS {
public:
    long num;
    double avx;
    double avy;
    double avxx;
    double avxy;
    double vx;
    double vy;
    double vxy;
    double ccoef;
    double rse;
    double *param;
    double *ey;

    LRM_CLASS(double *x, double *y, long n);
    ~LRM_CLASS();

    void stinfo(double *x, double *y);
    void lrmparam(double *x, double *y);
    void estlrm(double *x);
    void corcoef();
    void rselrm(double *y);
    void write_lrmf(char *file, double *x, double *y, XBOOL estflag);
} *LRM;

typedef class LMRM_CLASS {
public:
    long num;
    long dim;
    double *avx;
    double avy;
    double avey;
    double vy;
    double vey;
    double vyey;
    double mcoef;
    double rse;
    double **vcm;
    double *vyv;
    double *param;
    double *ey;

    LMRM_CLASS(double **x, double *y, long n, long p);
    ~LMRM_CLASS();
    
    void stinfo(double **x, double *y);
    double** xinvvcm();
    void lmrmparam(double **x, double *y);
    void estlmrm(double **x);
    void mcorcoef(double *y);
    void rselmrm(double *y);
    void write_lmrmf(char *file, double **x, double *y, XBOOL estflag,
		     XBOOL erflag);
} *LMRM;

class LMRMsub {
public:
    double* xget_parcoef(double **x, double *y, long n, long p);
};
  
     
typedef class HISTGRM_CLASS {
public:
    long num;
    long hnum;
    long *idxv;
    long *hist;
    double av;
    double sd;
    double ubv;
    double max;
    double min;
    double med;
    double uhin;
    double lhin;
    double scale;
    double *data;

    HISTGRM_CLASS();
    HISTGRM_CLASS(double *x, long n);
    HISTGRM_CLASS(float *x, long n);
    HISTGRM_CLASS(long *x, long n);
    HISTGRM_CLASS(int *x, long n);
    ~HISTGRM_CLASS();

    void readdata(double *x);
    void readfdata(float *x);
    void readldata(long *x);
    void readidata(int *x);
    void getparam1();
    void getparam2();
    double get_climit(double tval);
    void calhist(double divpt);
    void calhist();
    void writehist(FILE *fp);
    void writedens(FILE *fp);
    void delhist();
    void writedist(FILE *fp);
    double *calunidist();
    void writeunidistdata(char *file);
    void quicksort(double *array, long lower, long upper, long *idx);
} *HISTGRM;


#endif /* __STATISTIC_H */
