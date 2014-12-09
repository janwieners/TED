/* 
 * kfill.c
 * 
 * Practical Algorithms for Image Analysis
 * 
 * Copyright (c) 1997, 1998, 1999 MLMSoftwareGroup, LLC
 */

/* KFILL:       program performs kxk salt-and-pepper noise reduction
 *            on binary image using table look-up and run-length coding
 *              for speed.
 *              usage: kfill inimg outimg [-f FDIFF] [-c] [-n NITER]
 *                                        [-r R%][-k K] [-l] [-d] [-I] [-L]
 *              For options, type "kfill".
 *              NOTE: Because this program uses modified run-length code
 *                    on only the ON pixels, the OFF and ON lengths are
 *                    not treated equivalently. Specifically, while the
 *                    calculations for the ON pixels are correct as 
 *                    expected, those for the OFF pixels may not be. 
 *                    For example, if an OFF run-length is longer than
 *                    MIN0RUN, the pixels within that run are not inspected.
 *                    If a scan line is omitted, then pixels on that line
 *                    are never inspected, and they can never become ON.
 *                    Oh, there is an exception to this on the first 
 *                    iteration, since KFILL inspects all pixels to
 *                    perform runlength coding on this iteration.
 */
#include <cmath>
#include <QImage>
#include <QProgressDialog>
#include "kfill.h"

// O'Gorman Stuff
#define DFLTCFLAG 1		/* flag =1 if connectivity matters; or 0 */
#define DFLTEFLAG 1		/* flag =1 if endpoint matters; or 0 */
#define DFLTKMAX 5		/* dflt maximum thinning kernel size */
#define MAXKMAX 21		/* max of the maximum thinning kernel size */
#define DFLTITER 20		/* default number of iterations */
#define DFLTDIRTY 0		/* default pct of max noise left on iter */

#define MIN0RUN 5		/* minimum run of zeros to run-length code */
#define FILLINITIAL 1		/* initial fill type */

long OFF, ON;			/* initial/final values of pixels */
unsigned char FILL0, FILL1;	/* values of OFF fill, ON fill */
long fDiff,			/* fNum difference from default */
  cFlag,			/* if =1, retain connectivity; if 0, don't */
  eFlag;			/* if =1, retain endpoint; if 0, don't */

struct point
{
	int x;
	int y;
};

struct point imgSize;		/* image size */
long ySizeM1, xSizeM1;		/* y,x lengths minus 1 */
long **xRun;			/* no., then x locns of 1/0 runs for each y */

// - O'Gorman Stuff

// Kfill Implementation from
// http://cis.k.hosei.ac.jp/~wakahara/kfill.c

const int BLACK=0;
const int WHITE=255;

void k_fill(QImage *image, int windowsize)
{
  int x, y;             /* X/Y coordinates of boundary point */
  int k;                /* k-filling parameter */
  int xs, xe, ys, ye;   /* window edge points */
  int npc, npp;         /* number of core and perimeter points */
  int i, j;
  int * data;           /* on/off values of surrounding points */
  int count;            /* number of changes in a cycle */
  int sum, cross, corner;

  int height=image->height();
  int width=image->width();
  QRgb *RGB;
  
  /* outermost lines are cleared */
  for(x=0; x<width; x++)
  {
	  image->setPixel(x, 0, qRgb(WHITE, WHITE, WHITE));
	  image->setPixel(x, height - 1, qRgb(WHITE, WHITE, WHITE));
  }

  for (y=0; y<height; y++)
  {
	  image->setPixel(0, y, qRgb(WHITE, WHITE, WHITE));
	  image->setPixel(width-1, y, qRgb(WHITE, WHITE, WHITE));
  }
  
  k=windowsize;
  
  npc = (k - 2) * (k - 2);
  npp = 4 * (k - 1);
  data = (int *)malloc(npp * sizeof(int));
  xs = ys = k%2 == 0 ? k/2 - 1 : k/2;
  xe = ye = k/2;
  
  do {
    count = 0;
   
    /* White filling */
    for (y = ys; y < height - ye; y++) {
      for (x = xs; x < width - xe; x++) {
	sum = 0;
	for (j = -ys + 1; j <= ye - 1; j++) {
	  for (i = -xs + 1; i <= xe - 1; i++) {
		  RGB=(QRgb *)image->scanLine(y + j) + x + i;
		  if (qRed(*RGB)==WHITE) sum++;
	  }
	}
	if (sum != npc) continue;
	i = 0;
	for (j = 0; j >= - ys; j--) {
		RGB=(QRgb *)image->scanLine(y + j) + x + xe;
	  data[i++] = qRed(*RGB) == BLACK ? 1 : 0;
	}
	for (j = xe - 1; j >= - xs; j--) {
		RGB=(QRgb *)image->scanLine(y - ys) + x + j;
	  data[i++] = qRed(*RGB) == BLACK ? 1 : 0;
	}
	for (j = - ys + 1; j <= ye; j++) {
		RGB=(QRgb *)image->scanLine(y + j) + x - xs;
	  data[i++] = qRed(*RGB) == BLACK ? 1 : 0;
	}
	for (j = - xs + 1; j <= xe; j++) {
		RGB=(QRgb *)image->scanLine(y + ye) + x + j;
	  data[i++] = qRed(*RGB) == BLACK ? 1 : 0;
	}
	for (j = ye - 1; j > 0; j--) {
		RGB=(QRgb *)image->scanLine(y + j) + x + xe;
	  data[i++] = qRed(*RGB) == BLACK ? 1 : 0;
	}
	sum = 0;
	cross = 0;
	for (i = 0; i < npp; i++) {
	  sum += data[i];
	  cross += abs(data[(i + 1)%npp] - data[i]);
	}
	cross /= 2;
	corner = data[ys] + data[ys + k - 1]
	  + data[ys + 2*(k - 1)] + data[ys + 3*(k - 1)];
	if ((cross <= 1) && (sum > (3*k - 4) || 
			     (sum == 3*k - 4 && corner == 2))) {
	  for (j = -ys + 1; j <= ye - 1; j++)
	  {
	    for (i = -xs + 1; i <= xe - 1; i++)
		{
			RGB=(QRgb *)image->scanLine(y + j) + x + i;
			*RGB=qRgb(BLACK, BLACK, BLACK);
	    }
	  }
	  count++;
	}
      }
    }
    
    /* Black filling */
    for (y = ys; y < height - ye; y++) {
      for (x = xs; x < width - xe; x++) {
	sum = 0;
	for (j = -ys + 1; j <= ye - 1; j++) {
	  for (i = -xs + 1; i <= xe - 1; i++) {
		  RGB=(QRgb *)image->scanLine(y + j) + x + i;
	    if (qRed(*RGB)==BLACK) sum++;
	  }
	}
	if (sum != npc) continue;
	i = 0;
	for (j = 0; j <= ys; j++) {
		RGB=(QRgb *)image->scanLine(y - j) + x + xe;
	  data[i++] = qRed(*RGB) == WHITE ? 1 : 0;
	}
	for (j = - xe + 1; j <= xs; j++) {
		RGB=(QRgb *)image->scanLine(y - ys) + x -j;
	  data[i++] = qRed(*RGB) == WHITE ? 1 : 0;
	}
	for (j = - ys + 1; j <= ye; j++) {
		RGB=(QRgb *)image->scanLine(y + j) + x - xs;
		data[i++] = qRed(*RGB) == WHITE ? 1 : 0;
	}
	for (j = - xs + 1; j <= xe; j++) {
		RGB=(QRgb *)image->scanLine(y + ye) + x + j;
	  data[i++] = qRed(*RGB) == WHITE ? 1 : 0;
	}
	for (j = ye - 1; j > 0; j--) {
		RGB=(QRgb *)image->scanLine(y + j) + x + xe;
	  data[i++] = qRed(*RGB) == WHITE ? 1 : 0;
	}
	sum = 0;
	cross = 0;
	for (i = 0; i < npp; i++) {
	  sum += data[i];
	  cross += abs(data[i] - data[(i + 1)%npp]);
	}
	cross /= 2;
	corner = data[ys] + data[ys + k - 1]
	  + data[ys + 2*(k - 1)] + data[ys + 3*(k - 1)];
	if ((cross <= 1) && (sum > (3*k - 4) || 
			     (sum == 3*k - 4 && corner == 2))) {
	  for (j = -ys + 1; j <= ye - 1; j++) {
	    for (i = -xs + 1; i <= xe - 1; i++)
		{
			RGB=(QRgb *)image->scanLine(y + j) + x + i;
			*RGB=qRgb(WHITE, WHITE, WHITE);
	    }
	  }
	  count++;
	}
      }
    } 
  } while (count);
  free(data);
}




// O'Gorman Implementation

int kfill(QImage *image)
{
  register long x, y,		/* image coordinates */
    i, k;			/* sidelength of current thinning kernel */
  unsigned char **f0Table,	/* table of FILL0 values for each k */
  **f1Table;			/* table of FILL1 values for each k */
  long kMax,			/* max. sidelength of thinning kernel */
    maxIter,			/* maximum no. of iterations */
    change[MAXKMAX],		/* no. erasures for each mask size */
    nTable,			/* size of fTable for each k */
    nIter,			/* no. iterations */
    pctDirty,			/* pct. of max noise left on an iteration */
    nChange,			/* no. thinning operations on iteration */
    nChangeB4,			/* no. changes on iteration before */
    nChangeMax,			/* no. max. changes in one iteration */
    nChangeThresh,		/* change threshold of max pct removed */
    invertFlag,			/* invert input image before processing */
    nONs,			/* total ONs in original image */
    fillFlag,			/* ON fill (if =1), or OFF erase (if =0) */
    nFill;			/* cumulative no. filled in image */
  double pow();

  imgSize.x=image->width();
  imgSize.y=image->height();
  ySizeM1=imgSize.y-1;
  xSizeM1=imgSize.x-1;

  fDiff = 0;
  cFlag = DFLTCFLAG;
  eFlag = DFLTEFLAG;
  maxIter = DFLTITER;
  pctDirty = DFLTDIRTY;
  kMax = DFLTKMAX;
  fillFlag = FILLINITIAL;
  invertFlag = 0;

    QRgb *RGB;

  xRun = (long **) calloc (imgSize.y, sizeof (long));

  OFF=255;
  FILL0=(unsigned char)OFF + 1;
  ON=0;
  FILL1=(unsigned char)(ON - 1);

/* make table of fill-values for FILL0 and FILL1 */
  f0Table = (unsigned char **) calloc (kMax - 2, sizeof (unsigned char *));
  f1Table = (unsigned char **) calloc (kMax - 2, sizeof (unsigned char *));
  for (k = 3; k <= kMax; k++)
  {
    nTable=(float) std::pow((float)2.0, (float)(4.0*(k-1.0)) );

    f0Table[k - 3] =
      (unsigned char *) calloc (nTable, sizeof (unsigned char));
    f1Table[k - 3] =
      (unsigned char *) calloc (nTable, sizeof (unsigned char));
    for (i = 0; i < nTable; i++)
    {
      f0Table[k - 3][i] = filltest(i, k, (long) FILL0);
      f1Table[k - 3][i] = filltest(i, k, (long) FILL1);
    }
  }

/* zero image borders */
  for(x=0; x<imgSize.x; x++)
  {
	  image->setPixel(x, 0, qRgb(OFF, OFF, OFF));
	  image->setPixel(x, image->height() - 1, qRgb(OFF, OFF, OFF));
  }

  for (y=0; y<imgSize.y; y++)
  {
	  image->setPixel(0, y, qRgb(OFF, OFF, OFF));
	  image->setPixel(image->width() -1, y, qRgb(OFF, OFF, OFF));
  }

  for (k = 0; k <= kMax; k++)
    change[k] = 0;

/* on first iteration, perform filling and accumulate x-run info */
  nChange = fill0 (image, f0Table, f1Table, fillFlag, kMax, change, &nONs, &nFill);
  nChangeMax = nChange;
  nChangeThresh = 0;
  nChangeB4 = nChangeThresh + 1;
  for (i = 3; i <= kMax; i++)
  {
    change[i] = 0;
  }

  if (fillFlag == 1)
  {
    fillFlag = 0;
    FILL0++;
  }
  else
  {
    fillFlag = 1;
    --FILL1;
  }

/* on subsequent iterations, perform filling */
  for (nIter = 1; nIter < maxIter &&
       (nChange > nChangeThresh || nChangeB4 > nChangeThresh); nIter++)
  {
    nChangeB4 = nChange;
    nChange = fill (image, f0Table, f1Table, fillFlag, kMax, change, &nFill);
    for (i = 3; i <= kMax; i++)
    {
      change[i] = 0;
    }

    if (fillFlag == 1)
    {
      FILL0++;
      if (nChangeB4 != 0)
		  fillFlag = 0;
    }
    else
    {
      --FILL1;
      if (nChangeB4 != 0)
		  fillFlag = 1;
    }
    if (nChange > nChangeMax)
      nChangeMax = nChange;
    nChangeThresh = (pctDirty * nChangeMax) / 100;
  }

  int checkval;

  for (y = 1; y < ySizeM1; y++)
  {
    for (x = 1; x < xSizeM1-1; x++)
    {
		RGB=(QRgb *)image->scanLine(y)+x;
		checkval=qRed(*RGB);
      if(checkval <= FILL0)
		  image->setPixel(x, y, qRgb(ON, ON, ON));
      else if(checkval >= FILL1)
		  image->setPixel(x, y, qRgb(OFF, OFF, OFF));
		//if(checkval <= FILL0)
		//	image->setPixel(x, y, qRgb(OFF, OFF, OFF));
		//else if(checkval >= FILL1)
		//	image->setPixel(x, y, qRgb(ON, ON, ON));
    }
  }

  return (0);
}



/* FILLTEST:    function makes decision to fill or not based on CNUM
 *            and FNUM in perimeter ring
 *                      usage: fillFlag = filltest (pack, k, fill01)
 *              fillFlag = 0 if no fill, 1 if OFF fill, 2 if ON fill,
 *                              3 if ON and OFF fill
 */

int filltest (register long pack, int k, long fill01)
/* packed ring of perimeter pixelsr */
/* square sidelength of ring */
/* fill value: FILL0 or FILL1 */
{
  int nRing,		/* no. pixels in ring */
    n, i;
  unsigned char *ring;		/* neighborhood pixels array */
  long fNum,			/* no. 1s on ring */
    cNum,			/* connectivity number */
    m;
  long lower, upper;		/* adjacent ring elements for cNum calc */
  long nCornerOn,		/* no. corners ON */
    fNumThresh;			/* threshold of fNum */

  ring = (unsigned char *) malloc (4 * (k - 1));

/* unpack ring from word to array */
  nRing = 4 * k - 4;
  for (i = 0; i < nRing; i++)
    ring[i] = (pack >> i) & 01;

/* calculate CNUM, first skipping corners */
  for (i = 2, cNum = 0; i < nRing; i++)
  {
    lower = (long) ring[i - 1];
    if ((i % (k - 1)) == 0)
      i++;			/* skip the corner pixels */
    upper = (long) ring[i];
    if (upper != 0 && lower == 0)
      cNum++;
  }
  if (ring[1] != 0 && ring[nRing - 1] == 0)
    cNum++;

/* CNUM at corners */
  for (n = 1, nCornerOn = 0; n < 4; n++)
  {
    m = n * (k - 1);
    if (ring[m] != 0)
    {
      if (ring[m - 1] == 0 && ring[m + 1] == 0)
	cNum++;
      nCornerOn++;
    }
  }
  if (ring[0] != 0)
  {
    if (ring[1] == 0 && ring[nRing - 1] == 0)
      cNum++;
    nCornerOn++;
  }

/* calculate FNUM */
  if (fill01 == (long) FILL1)
  {
    for (i = 0, fNum = 0; i < nRing; i++)
      if (ring[i] != 0)
	fNum++;
  }
  else
  {
    for (i = 0, fNum = 0; i < nRing; i++)
      if (ring[i] == 0)
	fNum++;
  }

/* to fill or not to fill */
  if (cFlag == 0 || (cFlag != 0 && cNum <= 1))
  {
    fNumThresh = 3 * (k - 1) - 1 + fDiff;
    if (fill01 == (long) FILL1 || eFlag == 0)
    {
      if (fNum > fNumThresh)
	return (1);
      if (fNum == fNumThresh && nCornerOn == 2)
	return (1);
    }
    else
    {
      if (fNum == nRing)
	return (1);
      if (fNum == fNumThresh && nCornerOn == 2)
	return (1);
    }
  }
  return (0);
}



/* FILL0:       function performs first iteration of thinning, as well
 *            as compiling run-lengths
 *                  usage: nChange = fill0 (f0Table, f1Table, fillFlag, kMax,
 *                                              change, &nONs, &nFill)
 */

int
fill0(QImage *image, unsigned char **f0Table, unsigned char **f1Table,
	  long fillFlag, long kMax, long *change, long *nONs, long *nFill)
/* table of FILL0 values for each k */
/* table of FILL1 values for each k */
/* ON fill (if =1), or OFF erase (if =0) */
/* max sidelength of thinning kernel */
/* no. erasures for each mask size */
/* no. original ON pixels in image */
/* cumulative no. filled this iteration */
{
  register long x, y,		/* image coordinates */
    iXRun,			/* index of runs in x */
    k,				/* sidelength of current thinning kernel */
    kM1;			/* k minus 1 */
  long ring,			/* word containing neighborhood pixels */
    nChange,			/* no. thinning operations on iteration */
    fillValue,			/* fillValue = 1 to fill or 0 */
    onRun;			/* flag = 1 for run of 1s; 0 for 0s */

  QRgb *RGB;
  int checkval=0;

  *nONs = nChange = 0;
  for (y = 1; y < ySizeM1; y++)
  {
    xRun[y] = (long *) calloc (imgSize.x + 1, sizeof (long));
    xRun[y][0] = -MIN0RUN;

    for (x = 1, iXRun = 1, onRun = 0; x < xSizeM1; x++)
    {
		RGB=(QRgb *)image->scanLine(y)+x;
		checkval=qRed(*RGB);
      if(checkval<=FILL0)
      {
	if (onRun == 1)
	{
	  onRun = 0;
	  xRun[y][iXRun++] = (x - 1 >= imgSize.x) ? xSizeM1 : x - 1;
	}
      }
      else
      {
	if (onRun == 0)
	{
	  onRun = 1;
	  if ((x - xRun[y][iXRun - 1]) < MIN0RUN)
	    --iXRun;
	  else
	    xRun[y][iXRun++] = (x < 0) ? 1 : x;
	}
	(*nONs)++;
      }
      k = ksize (image, x, y, kMax, fillFlag);
      kM1 = (k > 3) ? k - 1 : 3;
      while (k >= kM1)
      {
	getring (image, x, y, k, fillFlag, &ring);
	fillValue = (fillFlag == 0) ?
	  f0Table[k - 3][ring] : f1Table[k - 3][ring];
	if (fillValue == 1)
	{
	  nChange++;
	  change++;
	  fillsqr(image, x, y, k, fillFlag, nFill);
	  break;
	}
	--k;
      }
    }
    --iXRun;
    if (iXRun % 2 != 0)
      xRun[y][++iXRun] = x;
    xRun[y][0] = iXRun;
    xRun[y] = (long *) realloc (xRun[y], (sizeof (long)) * (iXRun + 1));
  }
  return (nChange);
}



/* FILL:        function performs an iteration of thinning
 *                usage: nChange = fill (f0Table, f1Table, fillFlag, kMax,
 *                                                      change, &nFill)
 */

int fill (QImage *image, unsigned char **f0Table, unsigned char **f1Table,
	  long fillFlag, long kMax, long *change, long *nFill)
/* table of FILL0 values for each k */
/* table of FILL1 values for each k */
/* ON fill (if =1), or OFF erase (if =0) */
/* max sidelength of thinning kernel */
/* no. erasures for each mask size */
/* cumulative no. filled this iteration */
{
  register long x, y,		/* image coordinates */
    xStart, xEnd,		/* start and end of x-run */
    iXRun,			/* index of runs in x */
    k,				/* sidelength of current thinning kernel */
    kM1;			/* k minus 1 */
  long ring,			/* word containing nbrhood pixels */
    nChange,			/* no. thinning operations on iteration */
    fillValue;			/* fillValue = 1 to fill or 0 */

  nChange = 0;
  for (y = 1; y < ySizeM1; y++)
  {
    for (iXRun = 1, x = 1; iXRun <= xRun[y][0]; iXRun += 2)
    {
      xStart = xRun[y][iXRun] - kMax + 2;
      xStart = (xStart > x) ? xStart : x;
      xEnd = xRun[y][iXRun + 1] + kMax - 2;
      if (xEnd > xSizeM1)
	xEnd = xSizeM1;
      for (x = xStart; x <= xEnd; x++)
      {
	k = ksize (image, x, y, kMax, fillFlag);
	kM1 = (k > 3) ? k - 1 : 3;
	while (k >= kM1)
	{
	  getring(image, x, y, k, fillFlag, &ring);
	  fillValue = (fillFlag == 0) ?
	    f0Table[k - 3][ring] : f1Table[k - 3][ring];
	  if (fillValue == 1)
	  {
	    nChange++;
	    change++;
	    fillsqr (image, x, y, k, fillFlag, nFill);
	    break;
	  }
	  --k;
	}
      }
    }
  }

  return (nChange);
}



/* KSIZE:       function determines k, where kxk is largest square
 *            around (x,y) which contains all OFF if fillFlag=0,
 *              or all ON for fillFlag=1
 *                      usage: k = ksize (x, y, kMax, fillFlag)
 */

long ksize (QImage *image, long x, long y, long kMax, long fillFlag)
/* image coordinates */
/* maximum k value */
/* ON fill (if =1), or OFF erase (if =0) */
{
  register long xMask, yMask,	/* x,y mask coordinates */
    xEnd, yEnd,			/* end coord.s of square */
    k;				/* mask size */
  long upHalf, downHalf,	/* half of mask below and above center */
    xStart,			/* x- start and end of square */
    yStart;			/* y- start and end of square */

  QRgb *RGB;
  int checkval=0;

  RGB=(QRgb *)image->scanLine(y)+x;
  checkval=qRed(*RGB);
  if (fillFlag == 0)
  {
    if (checkval <= FILL0)
      return (0);
    else if (kMax == 3)
      return (3);
  }
  else
  {
    if (checkval >= FILL1)
      return (0);
    else if (kMax == 3)
      return (3);
  }

  for (k = 4; k <= kMax; k++)
  {
    if (k % 2 == 1)
      downHalf = upHalf = (k - 3) / 2;
    else
    {
      upHalf = (k - 2) / 2;
      downHalf = (k - 4) / 2;
    }
    xStart = x - downHalf;
    xEnd = x + upHalf;
    yStart = y - downHalf;
    yEnd = y + upHalf;
    if (xStart <= 0 || yStart <= 0
	|| xEnd >= (imgSize.x - 1) || yEnd >= ySizeM1)
      return (k - 1);
    for (yMask = yStart; yMask <= yEnd; yMask++)
      for (xMask = xStart; xMask <= xEnd; xMask++)
      {
		  RGB=(QRgb *)image->scanLine(yMask)+xMask;
		  checkval=qRed(*RGB);
	if (fillFlag == 0)
	{
		
	  if (checkval < FILL0)
	    return (k - 1);
	}
	else if (checkval > FILL1)
	  return (k - 1);
      }
  }
  return (kMax);
}



/* GETRING:     function gets ring of pixels on perimeter of k-size square
 *                usage:  getring (x, y, k, fillFlag, *ring)
 */

int getring (QImage *image, int x, int y, long k, long fillFlag, long *ring)
/* image coordinages */
/* square sidelength of ring */
/* ON fill (if =1), or OFF erase (if =0) */
/* ring of pixels on perimeter of kxk sqr */
{
  int xEnd, yEnd,		/* x,y ends of square */
    i, xStart, yStart;		/* start and end of square */
  long upHalf, downHalf;	/* half of mask below and above center */

  int checkval;
  QRgb *RGB;

  if (k % 2 == 1)
    downHalf = upHalf = (k - 1) / 2;
  else
  {
    upHalf = k / 2;
    downHalf = (k - 2) / 2;
  }
  xStart = x - downHalf;
  xEnd = x + upHalf;
  yStart = y - downHalf;
  yEnd = y + upHalf;

  i = 0;
  *ring = 0;

  if (fillFlag == 0)
  {
	  RGB=(QRgb *)image->scanLine(y)+x;
	  checkval=qRed(*RGB);
    for (x = xStart, y = yStart; x <= xEnd; x++, i++)
		if (checkval >= FILL0)
			*ring = *ring | (01 << i);
    for (y = yStart + 1, x = xEnd; y <= yEnd; y++, i++)
      if (checkval >= FILL0)
	*ring = *ring | (01 << i);
    for (x = xEnd - 1, y = yEnd; x >= xStart; --x, i++)
      if (checkval >= FILL0)
	*ring = *ring | (01 << i);
    for (y = yEnd - 1, x = xStart; y > yStart; --y, i++)
      if (checkval >= FILL0)
	*ring = *ring | (01 << i);
  }
  else
  {
	  RGB=(QRgb *)image->scanLine(y)+x;
	  checkval=qRed(*RGB);
    for (x = xStart, y = yStart; x <= xEnd; x++, i++)
      if (checkval > FILL1)
	*ring = *ring | (01 << i);
    for (y = yStart + 1, x = xEnd; y <= yEnd; y++, i++)
      if (checkval > FILL1)
	*ring = *ring | (01 << i);
    for (x = xEnd - 1, y = yEnd; x >= xStart; --x, i++)
      if (checkval > FILL1)
	*ring = *ring | (01 << i);
    for (y = yEnd - 1, x = xStart; y > yStart; --y, i++)
      if (checkval > FILL1)
	*ring = *ring | (01 << i);
  }
  return (0);
}



/* FILLSQR:     function fills square with OFFs or ONs
 *                    usage: fillsqr (x, y, k, fillFlag, &nFill)
 */

int fillsqr (QImage *image, int x, int y, long k, long fillFlag, long *nFill)
/* image coordinages */
/* square sidelength of ring */
/* ON fill (if =1), or OFF erase (if =0) */
/* no. of filled */
{
  register long xEnd, yEnd;	/* upper bounds of center fill area */
  long upHalf, downHalf,	/* half of mask below and above center */
    yStart,			/* bounds of center fill area */
    xStart;
  int checkval;

  QRgb *RGB;

/* fill for 3x3 */
  if (k == 3)
  {
	  RGB=(QRgb *)image->scanLine(y)+x;
	  checkval=qRed(*RGB);
    if (fillFlag == 0)
    {
      if (checkval > FILL0)
      {
		  (*nFill)++;
		  image->setPixel(x, y, qRgb(FILL0, FILL0, FILL0));
      }
    }
    else
    {
      if (checkval < FILL1)
      {
		  (*nFill)++;
		  image->setPixel(x, y, qRgb(FILL1, FILL1, FILL1));
      }
    }
  }

/* fill for kxk > 3x3 */
  else
  {
    if (k % 2 == 1)
      downHalf = upHalf = (k - 3) / 2;
    else
    {
      upHalf = (k - 2) / 2;
      downHalf = (k - 4) / 2;
    }
    xStart = x - downHalf;
    xEnd = x + upHalf;
    yStart = y - downHalf;
    yEnd = y + upHalf;
	RGB=(QRgb *)image->scanLine(y)+x;
	checkval=qRed(*RGB);
    for (y = yStart; y <= yEnd; y++)
    {
      for (x = xStart; x <= xEnd; x++)
      {
	if (fillFlag == 0)
	{
	  if (checkval > FILL0)
	  {
	    (*nFill)++;
		image->setPixel(x, y, qRgb(FILL0, FILL0, FILL0));
	  }
	}
	else if(checkval < FILL1)
	{
	  (*nFill)++;
	  image->setPixel(x, y, qRgb(FILL1, FILL1, FILL1));
	}
      }
    }
  }
  return (0);
}

