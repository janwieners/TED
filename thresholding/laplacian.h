/* Thresholding by using the two histogram peaks */
void peaks(QImage *image, int *hist, float **lap, int lval, int *t);
void thr_lap(QImage *image);
void fhist(float **data, int *hist, int nr, int nc);
void hi_pct(int *hist, int NH, long N, float pct, int *val);
void Laplacian(QImage *input, float **output);
float pix_lap(QImage *image, int r, int c);
float ** f2d (int nr, int nc);






