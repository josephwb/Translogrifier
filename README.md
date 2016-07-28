Translogrifier
==============
Overview
---------------
A lightweight program to remove samples from the (potentially replicated) results of an MCMC analysis. 
Samples may be either parameters or trees. Samples can undergo burnin and/or 
thinning. All values are in terms of number of samples (NOT generations). If 
the number of samples is unknown, the program can be used to count them. At the 
moment, minimal run indication is printed to screen (just which files are currently 
being worked upon). If you have a single file (or large files), you may think 
things have gone badly; but they haven't! As a benchmark, thinning a 9+ GB tree 
log file takes ~2 minutes.

Installation
---------------
Compile is as easy as typing the following in a unix prompt:

	make

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
Assumes that translation tables (if present) are identical across files. This is not yet checked.

###UPDATE###
I am moving all of this over to [phyx](https://github.com/FePhyFoFum/phyx).
