#ifndef _TLOG_H_
#define _TLOG_H_

#include <vector>

using namespace std;

void printProgramInfo ();
void printProgramUsage ();

// General functions
bool checkValidInputFile (string charsetFileName);
bool checkValidOutputFile (string & outputFileName);
string parseString (string stringToParse, int stringPosition);
bool checkStringValue (string stringToParse, string stringToMatch, int stringPosition);
bool checkWhiteSpaceOnly (string stringToParse);
bool checkCommentLine (string stringToParse);
string removeStringSuffix (string stringToParse, char suffixToRemove, bool & suffixEncountered);
string removeStringPrefix (string stringToParse, char characterToRemove);
int convertStringtoInt (string stringToConvert);
string convertIntToString (int intToConvert);
string removeStringElement (string stringToParse, int stringPosition);
void countTreeSamples (string const& fileName, int const& nruns, string & suffix);
void countParameterSamples (string const& fileName, int const& nruns, string & suffix);
vector <string> tokenize (string const& input);

// Specific user-influenced functions
void processCommandLineArguments (int argc, char *argv[], string & fileName, int & thinning,
    int & burnin, string & type, int & nruns, string & suffix, bool & count,
    bool & overwrite);
void collectTreesAndThin (string const& fileName, int const& thinning, int const& burnin,
    string & suffix, int const& nruns, bool & overwrite);
void collectParametersAndThin (string const& fileName, int const& thinning, int const& burnin,
    int const& nruns, string & suffix, bool & overwrite);

#endif /* _TLOG_H_ */