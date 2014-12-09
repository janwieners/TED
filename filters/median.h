struct windowintensityvals { int min, median, max; };
bool filtermedian3x3(QImage *image);
bool adaptivemedian(QImage *image, int windowsizemax);
void adaptivemedianwindow(QImage *image, int startx, int starty, int windowsize, windowintensityvals *intensityvals);




