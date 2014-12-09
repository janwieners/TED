/* Minimizing fuzziness */
void thr_fuzz_entropy(QImage *image);
void thr_fuzz_yager(QImage *image);
void histogram (QImage *image, double *hist);
double Ux (int g, int u0, int u1, int t);
double fuzzy_entropy(double *hist, int u0, int u11, int t);
double Shannon (double x);
double fuzzy_yager(int u0, int u1, int t);
double Yager (int u0, int u1, int t);





