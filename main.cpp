/*

************************************************
          Translogrifier version 0.41
                Joseph W. Brown
             University of Michigan
            Museum of Zoology (Birds)
                josephwb@umich.edu
                 January, 2016
************************************************

A lightweight program to remove samples from the results of an MCMC analysis. 
Samples may be either parameters or trees. Samples can undergo burnin and/or 
thinning. All values are in terms of number of samples (NOT generations). If 
the number of samples is unknown, the program can be used to count them.

The easiest way to compile is to use the Makefile, and typing the following in a unix prompt:

make

Otherwise, type the following in a unix prompt:
g++ -Wall Translogrifier.cpp -m64 -O3 -o Translogrifier

To run, type:
./Translogrifier [-t treefile] or [-p parameterfile] [-n thinning] [-b burnin] [-r num_runs] [-s suffix] [-count] [-h]

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
'-h' print this help.

*** NOTE *** All values are in terms of number of SAMPLES (NOT generations).
*** NOTE *** All line returns are expected to be in unix format. This is not checked.

TODO: allow multiple files - DONE!
 - check that parameter log files all contain the same number of parameters
TODO: allow newick trees
TODO: allow arbitrarily named files passed in as a list
TODO: update argument parsing; use get_opt
TODO: make sure memory kept low through streaming
TODO: check translation tables are identical
TODO: when multiple files involved, use multiple threads

TODO: more default suffixes (i.e. BEAST ones: .log, .trees)

TODO: move over to phyx

*/

#include <iostream>
#include <stdlib.h>

using namespace std;

#include "translog.h"

int main(int argc, char *argv[]) {
    string fileName;
    string type;
    string suffix;
    int thinning = 1;
    int burnin = 0;
    int nruns = 1;
    bool count = false;

    printProgramInfo();
    processCommandLineArguments(argc, argv, fileName, thinning, burnin, type, nruns, suffix, count);
    
    if (count) { // simply count number of samples present i.e. file may be too large to read it directly
        if (type == "tree") {
            countTreeSamples(fileName, nruns, suffix);
        } else if (type == "parameter") {
            countParameterSamples(fileName, nruns, suffix);
        }
    } else {
        if (type == "tree") {
            collectTreesAndThin(fileName, thinning, burnin, suffix, nruns);
        } else if (type == "parameter") {
            collectParametersAndThin(fileName, thinning, burnin, nruns, suffix);
        }
    }
    
    cout << endl << "Fin." << endl;
    return 0;
}




