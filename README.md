Translogrifier
==============
Overview
---------------
A lightweight program to remove samples from the results of an MCMC analysis. 
Samples may be either parameters or trees. Samples can undergo burnin and/or 
thinning. All values are in terms of number of samples (NOT generations). If 
the number of samples is unknown, the program can be used to count them.

Installation
---------------
The easiest way to compile is to use the Makefile, and typing the following in a unix prompt:

	make

Otherwise, type the following in a unix prompt:

	g++ -Wall Translogrifier.cpp -m64 -O3 -o Translogrifier

Usage
--------------
To run, type:

	./Translogrifier [-t treefile] or [-p parameterfile] -n thinning [-b burnin] [-r num_runs] [-s suffix] [-count]

where

	'treefile' contains trees, 'parameterfile' contains parameter sample log,
	'thinning' is the interval of resampling,
	'burnin' is the number of samples to be excluded at the beginning of the file,
	'num_runs' is, well, the number of runs to combine. Must have format: prefix.runx.[p/t/suffix], and
	 - in this case, provide only the file prefix for treefile or parameterfile
	   e.g. for 'foo.run1.p' provide 'foo'
	 - PLEASE NOTE: if combining multiple tree files, program assumes identical translation tables in each.
	'suffix' is an abnormal file suffix i.e. NOT '.p' (for parameters) or '.t' (for trees).
	'-count' specifies that samples are simply counted (possibly across files).

###NOTE###
All values are in terms of number of SAMPLES (NOT generations).
All line returns are expected to be in unix format. This is not checked.
