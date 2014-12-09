// Kfill Implementation from
// http://cis.k.hosei.ac.jp/~wakahara/kfill.c
void k_fill(QImage *image, int windowsize);

// O'Gorman Implementation
int kfill(QImage *inputimage);
int filltest (register long pack, int k, long fill01);
int fill0(QImage *image, unsigned char **f0Table, unsigned char **f1Table,
	  long fillFlag, long kMax, long *change, long *nONs, long *nFill);
int fill (QImage *image, unsigned char **f0Table, unsigned char **f1Table,
	  long fillFlag, long kMax, long *change, long *nFill);
long ksize (QImage *image, long x, long y, long kMax, long fillFlag);
int getring (QImage *image, int x, int y, long k, long fillFlag, long *ring);
int fillsqr (QImage *image, int x, int y, long k, long fillFlag, long *nFill);





