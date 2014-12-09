TED (TED Enhances Digitization)
===============================

I developed TED for my 2008 submitted Magister Artium thesis ["Zur Erweiterungsfähigkeit bestehender OCR Verfahren auf den Bereich extrem früher Drucke"](http://www.hki.uni-koeln.de/sites/all/files/MA_wieners.pdf) in which I facilitated Optical Character Recognition (OCR) on the digital images of incunables from the project ["Verteilte Digitale Inkunabelbibliothek"](http://inkunabeln.ub.uni-koeln.de/).

The character recognition process is based on a Self Organizing Map (SOM / Kohonen-Map) which works with digital images, intensively prepared by the following operations:

* Image conversion
* Binarization (many different algorithms: simple binarization by threshold to Otsu's Method)
* Median and kFill filtering
* Automatically cutting and deskewing of the image
* Edge detection
* Object / glyph isolation and recognition
* Clustering of isolated glyphs with self organizing map
