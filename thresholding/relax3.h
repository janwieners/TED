void thr_relax (QImage *image);
void minmax (QImage *image, float *gmin, float *gmax);
float update (QImage *image, float **p, float **q);
float R(QImage *image, int r, int c, int rj, int cj, int l1, int l2);
void assign_class (QImage *image, float **, float **q);
float mean (QImage *image);
void meanminmax (QImage *image, int r, int c, 
		 float *mean, float *xmin, float *xmax);
void thresh (QImage *image, float **p, float **q);
float Q(QImage *image, float **p, float **q, int r, int c, int cla);
float ** f2d (int nr, int nc);
int range (QImage *image, int i, int j);




