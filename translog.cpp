
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

#include "translog.h"

// version information
double version = 0.41;
string month = "January";
int year = 2016;

// *** LAS FUNCIONES *** //

void printProgramInfo() {
    cout << endl << 
    "************************************************" << endl <<
    "          Translogrifier version " << version    << endl <<
    "                Joseph W. Brown"                  << endl <<
    "             University of Michigan"              << endl <<
    "         Complaints: josephwb@umich.edu"          << endl <<
    "                 " << month <<", " << year <<        endl << 
    "************************************************" << endl << endl;
}

void processCommandLineArguments(int argc, char *argv[], string & fileName, int & thinning,
    int & burnin, string & type, int & nruns, string & suffix, bool & count,
    bool & overwrite)
{
    if (argc == 1) {
        cout << "Enter the name of the log file to be thinned: ";
        cin >> fileName;
        cout << "Enter the thinning value 'n' (where every nth sample will be retained): ";
        cin >> thinning;
    } else {
        for (int i = 1; i < argc; i++) {
            string temp = argv[i];
            
            if (temp == "-h" || temp == "-help") {
                cout
                << "Program description: Thins sample (tree or parameter) log files." << endl
                << endl
                << "To compile, type the following in a unix prompt:" << endl << endl
                << "make" << endl
                << endl
                << "To run, type:" << endl;
                printProgramUsage ();
                exit(0);  
            } else if (temp == "-t") {
                i++;
                fileName = argv[i];
                type = "tree";
                continue;
            } else if (temp == "-p") {
                i++;
                fileName = argv[i];
                type = "parameter";
                continue;
            } else if (temp == "-n") {
                i++;
                thinning = convertStringtoInt(argv[i]);
                continue;
            } else if (temp == "-b") {
                i++;
                burnin = convertStringtoInt(argv[i]);
                continue;
            } else if (temp == "-r") {
                i++;
                nruns = convertStringtoInt(argv[i]);
                continue;
            } else if (temp == "-s") {
                i++;
                suffix = argv[i];
                continue;
            } else if (temp == "-count") {
                count = true;
                continue;
            } else if (temp == "-overwrite") {
                overwrite = true;
                continue;
            } else {
                cout
                << "Unknown command-line argument '" << argv[i] << "' encountered." << endl
                << endl
                << "Usage:" << endl;
                printProgramUsage ();
                exit(0);
            }
            cout << endl;
        }
    }
}

void printProgramUsage () {
    cout << "./Translogrifier [-t treefile] or [-p parameterfile] [-n thinning] [-b burnin] [-r num_runs] [-s suffix] [-count] [-overwrite] [-h]" << endl
    << endl
    << "where" << endl
    << endl
    << "'treefile' contains trees, 'parameterfile' contains parameter sample log," << endl
    << "'thinning' is the interval of resampling," << endl
    << "'burnin' is the number of samples to be excluded at the beginning of the file," << endl
    << "'num_runs' is, well, the number of runs to combine. Must have format: prefix.runx.[p/t/suffix], and" << endl
    << " - in this case, provide only the file prefix for treefile or parameterfile" << endl
    << "   e.g. for 'foo.run1.p' provide 'foo'" << endl
    << " - PLEASE NOTE: if combining multiple tree files, program assumes identical translation tables in each." << endl
    << "'suffix' is an abnormal file suffix i.e. NOT '.p' (for parameters) or '.t' (for trees)." << endl
    << "'-count' specifies that samples are simply counted (possibly across files)." << endl
    << "'-overwrite' will overwrite files without a warning message." << endl
    << "'-h' prints this help" << endl
    << endl
    << "*** NOTE *** All values are in terms of number of SAMPLES (NOT generations)." <<endl << endl
    << "*** NOTE *** All line returns are expected to be in unix format. This is not checked." << endl << endl;
}

void countTreeSamples (string const& fileName, int const& nruns, string & suffix) {
    int totalTrees = 0;
    bool commentLine = false;
    bool whiteSpaceOnly = false;
    
    if (suffix.empty()) {
        suffix = "t";
    }
    
    cout << "READING IN AND COUNTING TREE SAMPLES..." << endl << endl;
    
    for (int i = 0; i < nruns; i++) {
        ifstream treeInput;
        string currentFile = fileName;
        if (nruns > 1) {
// use MrBayes naming convention
            string runNumber = convertIntToString((i +1));
            currentFile = currentFile + ".run" + runNumber + "." + suffix;
        }
        
        checkValidInputFile(currentFile);
        treeInput.open(currentFile.c_str());
        int treeCounter = 0;        // Total samples in current file
        string line;
        
    // Read in every non-empty (or non-whitespace), non-commented-out line
        while (getline(treeInput,line)) {
            int stringPosition = 0;
            string temp;
            commentLine = checkCommentLine(line);
            whiteSpaceOnly = checkWhiteSpaceOnly(line);
            if (line.empty() || commentLine || whiteSpaceOnly) { // header
                continue;
            } else {
                if (checkStringValue(line, "tree", stringPosition)) {
                    treeCounter++;
                }
            }
        }
        treeInput.close();
        if (nruns > 1) {
            cout << "Read " << treeCounter << " samples from file " << i+1 << " of " << nruns << "." << endl << endl;
        }
        totalTrees += treeCounter;
    }
    
    cout << endl << "Read a total of " << totalTrees << " tree samples." << endl;
}

void countParameterSamples (string const& fileName, int const& nruns, string & suffix) {
    int numSamples = 0;
    int numPars = 0;
    bool commentLine = false;
    bool whiteSpaceOnly = false;
    vector <string> colnames;
    
    if (suffix.empty()) {
        suffix = "p";
    }
    
    cout << "READING IN AND COUNTING PARAMETER SAMPLES..." << endl << endl;
    
    for (int i = 0; i < nruns; i++) {
        ifstream parameterInput;
        string currentFile = fileName;
        if (nruns > 1) {
// use MrBayes naming convention
            string runNumber = convertIntToString((i +1));
            currentFile = currentFile + ".run" + runNumber + "." + suffix;
        }
        
        checkValidInputFile(currentFile);
        parameterInput.open(currentFile.c_str());
        
        int parameterCounter = 0;
        string line;
        bool firstLine = true;
        
    // Read in every non-empty (or non-whitespace), non-commented-out line
        while (getline(parameterInput, line)) {
            //int stringPosition = 0;
            //string temp;
            
            commentLine = checkCommentLine(line);
            whiteSpaceOnly = checkWhiteSpaceOnly(line);
            if (line.empty() || commentLine || whiteSpaceOnly) {
                continue;
            //} else if (checkStringValue(line, "Gen", stringPosition) || checkStringValue(line, "state", stringPosition)) { // MrBayes or BEAST
            } else if (firstLine) {
                vector <string> header = tokenize(line);
                int curpars = header.size();
                if (i == 0) {
                    numPars = curpars;
                    colnames = header;
                } else {
                    // check that we've still got the same number of parameters i.e. files match
                    if (curpars != numPars) {
                        cout << "Error: number of parameters in file " << (i + 1)
                            << "(" << curpars << ") does not match that from file 1 ("
                            << numPars << "). Exiting." << endl;
                        exit(0);
                    } else if (header != colnames) {
                        // check that the headers match (not just in length)
                        cout << "Error: header for file " << (i + 1)
                            << "does not match that from file 1. Exiting." << endl;
                        exit(0);
                    }  
                }
                firstLine = false;
                continue;
            } else {
                parameterCounter++;
                continue;
            }
        }
        parameterInput.close();
        if (nruns > 1) {
            cout << "Read " << parameterCounter << " samples (with " << numPars 
                << " columns) from file " << (i + 1) << " of " << nruns << "." << endl << endl;
        }
        numSamples += parameterCounter;
    }
    
    cout << "Read a total of " << numSamples << " parameter samples (with " << numPars 
            << " columns)." << endl;
}

// assumes delimiter is some form of whitespace
vector <string> tokenize (string const& input) {
    vector <string> tokens;
    string temp;
    istringstream str(input);
    while (str >> temp) {
        tokens.push_back(temp);
    }
    return tokens;
}

bool checkValidInputFile (string fileName) {
    bool validInput = false;
    ifstream tempStream;
    
    tempStream.open(fileName.c_str());
    if (tempStream.fail()) {
        ofstream errorReport("Error.Translogrifier.txt");
        errorReport << "Translogrifier analysis failed." << endl << "Error: unable to open file '";
        errorReport << fileName << "'" << endl;
        errorReport.close();
        
        cerr << endl << "Translogrifier analysis failed. " << endl << "Error: unable to open file '";
        cerr << fileName << "'" <<  endl;
        exit(1);
    } else {
        cout << "Successfully opened file '" << fileName << "'." <<  endl << endl;
        validInput = true;
        tempStream.close();
        tempStream.clear();
    }
    return validInput;
}

bool checkStringValue (string stringToParse, string stringToMatch, int stringPosition) {
// Performs case-insenstive string match test
    string testString = parseString(stringToParse, stringPosition);
    if (testString.size() != stringToMatch.size()) {
        return false;
    }
    if (testString == stringToMatch) {
        return true;
    }
    
    for (size_t i = 0; i < testString.size(); ++i) {
        if (toupper(testString[i]) == toupper(stringToMatch[i])) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

bool checkCommentLine (string stringToParse) {
    bool commentLine = false;
    char firstCharacter = stringToParse[0];
    if (firstCharacter == '[') { // Traditional NEXUS-style comment
//        cout << "Dude, you've got yourself a comment there. Ignoring entire line, assuming comment does not extend across multiple lines." << endl;
        commentLine = true;
    }
    if (firstCharacter == '#') { // BEAST output sometimes uses this
//        cout << "Dude, you've got yourself a comment there. Ignoring entire line, assuming comment does not extend across multiple lines." << endl;
        commentLine = true;
    }
    return commentLine;
}

bool checkWhiteSpaceOnly (string stringToParse) {
    bool whiteSpaceOnly = true;
    vector<string> tempVector;
    istringstream tempStream(stringToParse);
    string tempString;
    while (tempStream >> tempString) {
        if (tempString != "    " && tempString != " ") {
            whiteSpaceOnly = false;
        }
    }
    return whiteSpaceOnly;
}

string parseString (string stringToParse, int stringPosition) {
    vector<string> tempVector;
    istringstream tempStream(stringToParse);
    string tempString;
    int counter = 0;
    while (tempStream >> tempString && counter <= stringPosition) {
        tempVector.push_back(tempString);
        counter++;
    }
    
    return tempVector[stringPosition];
}

string removeStringElement (string stringToParse, int stringPosition) {
    vector<string> tempVector;
    istringstream tempStream(stringToParse);
    string tempString;
    string returnString;
    int i = 0;
    while (tempStream >> tempString) {
        if (i != stringPosition) {
            tempVector.push_back(tempString);
        }
        i++;
    }
    
    for (int i = 0; i < (int)tempVector.size(); i++) {
        returnString  = returnString + "\t" + tempVector[i];
    }
    
    return returnString;
}

int convertStringtoInt (string stringToConvert) {
    int tempInt = 0;
    istringstream tempStream(stringToConvert);
    tempStream >> tempInt;
    
    return tempInt;
}

string convertIntToString (int intToConvert) {
    string tempString;
    stringstream tempStream;
    tempStream << intToConvert;
    tempString = tempStream.str();
    
    return tempString;
}

string removeStringSuffix (string stringToParse, char suffixToRemove, bool & suffixEncountered) {
    string temp;
    vector<char> tempVector;
    int charCounter = 0;
    int suffixStart = 0;
    suffixEncountered = false;
    
    for (string::iterator charIter = stringToParse.begin(); charIter < stringToParse.end(); charIter++ ) {
        tempVector.push_back(*charIter);
        if (*charIter == suffixToRemove) {
            suffixStart = charCounter;
            suffixEncountered = true;
        }
        charCounter++;
    }
    if (suffixEncountered) {
        for (int charIter = 0; charIter < suffixStart; charIter++) {
            temp += tempVector[charIter];
        }
    }
    if (!suffixEncountered) {
        temp = stringToParse;
    }
    return temp;
}

string removeStringPrefix(string stringToParse, char characterToRemove) {
    string tempString;
    vector <char> tempVector;
    int charCounter = 0;
    int characterPosition = 0;
    bool characterEncountered = false;
    
    for (string::iterator charIter = stringToParse.begin(); charIter < stringToParse.end(); charIter++ ) {
        tempVector.push_back(*charIter);
        if (*charIter == characterToRemove) {
            characterPosition = charCounter;
            characterEncountered = true;
        }
        charCounter++;
    }
    if (characterEncountered) {
        for (int charIter = characterPosition + 1; charIter < charCounter; charIter++) {
            tempString += tempVector[charIter];
        }
    }
    if (!characterEncountered)
    {
        tempString = stringToParse;
    }
    return tempString;
}

bool checkValidOutputFile (string & outputFileName) {
    bool testOutBool = true;
    bool fileNameAcceptable = false;
    bool keepFileName = false;
    
// First, check if file already exists, so overwriting can be prevented
    fstream testIn;
    while (!fileNameAcceptable) {
        testIn.open(outputFileName.c_str());
        if (!testIn) {
            testIn.close();
            fileNameAcceptable = true;
        } else {
            testIn.close();
            cout << endl << "Default output file '" << outputFileName << "' exists!  Change name (0) or overwrite (1)? ";
            cin >> keepFileName;
            if (!keepFileName) {
                cout << "Enter new output file name: ";
                cin >> outputFileName;
            } else {
                cout << "Overwriting existing file '" << outputFileName << "'." << endl;
                fileNameAcceptable = true;
            }
        }
    }
    
    ofstream outFile;
    outFile.open(outputFileName.c_str());
    
    if (outFile.fail()) {
        ofstream errorReport("Error.Translogrifier.txt");
        errorReport << "Translogrifier analysis failed." << endl << "Error: unable to open file '";
        errorReport << outputFileName << "'" << endl;
        errorReport.close();

        cerr << endl << "Translogrifier analysis failed." << endl << "Error: unable to open file '";
        cerr << outputFileName << "'" <<  endl;
        testOutBool = false;
        exit(1);
    } else {
        outFile.close();
        outFile.clear();
    }
    return testOutBool;
}

void collectTreesAndThin (string const& fileName, int const& thinning, int const& burnin,
    string & suffix, int const& nruns, bool & overwrite)
{
    ofstream thinnedTrees;
    bool validFileName = false;
    string tempFileName;
    
    int totalTrees = 0;
    int totalSamples = 0;
    
    if (nruns == 1) {
        tempFileName = removeStringSuffix(fileName, '.', validFileName);
    } else {
// e.g. 'Archaeopteryx_gamma-rates_unequal-frequencies_temp-0.05.run1.p'
        tempFileName = fileName; // only prefix passed in
    }
    
    if (suffix.empty()) {
        suffix = "t";
    }
    
    tempFileName = tempFileName + "_thinned-" + convertIntToString(thinning) + "_burnin-" + convertIntToString(burnin) + ".trees";
    
    if (!overwrite) {
        // Check if file exists/is writable
        validFileName = false;
        while (!validFileName) {
            validFileName = checkValidOutputFile(tempFileName);
        }
    }
    
    thinnedTrees.open(tempFileName.c_str());
    bool commentLine = false;
    bool whiteSpaceOnly = false;

    cout << endl
    << "READING IN AND THINNING TREES..." << endl << endl;
    
    for (int i = 0; i < nruns; i++) {
        ifstream treeInput;
        string currentFile = fileName;
        if (nruns > 1) {
// use MrBayes naming convention
            string runNumber = convertIntToString((i +1));
            currentFile = currentFile + ".run" + runNumber + "." + suffix;
        }
        
        checkValidInputFile(currentFile);
        treeInput.open(currentFile.c_str());
        
        cout << "Extracting samples from file '" << currentFile << "'." << endl;
        
        if (burnin != 0) {
            cout << "Ignoring first (" << burnin << ") trees..." << endl;
        } else {
            cout << "No burnin declared." << endl;
        }
        cout << "Retaining every (" << thinning << ") trees..." << endl;
        
        int treeCounter = 0;        // Total samples
        int sampleCounter = 0;        // Samples retained
        string line;
        bool treesEncountered = false;
        
    // Read in every non-empty (or non-whitespace), non-commented-out line
        while (getline(treeInput,line)) {
            int stringPosition = 0;
            string temp;
            commentLine = checkCommentLine(line);
            whiteSpaceOnly = checkWhiteSpaceOnly(line);
            if (line.empty() || commentLine || whiteSpaceOnly) {
                if (i == 0) {
                    thinnedTrees << line << endl;
                }
                continue;
            } else {
                if (checkStringValue(line, "tree", stringPosition)) { // tree line
                    if (treeCounter == 0) treesEncountered = true;
                    if ((treeCounter-burnin) > 0 && (treeCounter-burnin) < thinning) {
                        treeCounter++;
                        totalTrees++;
                        continue;
                    } else if ((treeCounter-burnin) == 0) {
//    tree rep.1 = [something_maybe] ((((((((((((((4:0.3223,
                        temp = removeStringElement(line, 0);
                        temp = removeStringElement(temp, 0); // will this work, or delete just the tab?
                        thinnedTrees << "tree STATE_" << totalSamples << temp << endl;
                        treeCounter++;
                        totalTrees++;
                        sampleCounter++;
                        totalSamples++;
                        continue;
                    } else if ((treeCounter-burnin) > 0 && (treeCounter-burnin) % thinning == 0) {
                        temp = removeStringElement(line, 0);
                        temp = removeStringElement(temp, 0); // will this work, or delete just the tab?
                        thinnedTrees << "tree STATE_" << totalSamples << temp << endl;
                        treeCounter++;
                        totalTrees++;
                        sampleCounter++;
                        totalSamples++;
                        continue;
                    } else { // shouldn't get here
                        treeCounter++;
                        totalTrees++;
                        continue;
                    }
                } else { // not a tree line. only care about first file here. don't want anything below trees
                    if (i == 0 && !treesEncountered) { // keep header from first file
                        thinnedTrees << line << endl;
                    }
                }
            }
        }
        treeInput.close();
        cout << "Retained " << sampleCounter << " samples." << endl << endl;
    }
    thinnedTrees << "End;" << endl;
    thinnedTrees.close();
    
    cout << endl << "Successfully created file '" << tempFileName << "', populated with " << totalSamples << " trees (from original " <<
    totalTrees << " samples)." << endl;
}

void collectParametersAndThin (string const& fileName, int const& thinning, int const& burnin,
    int const& nruns, string & suffix, bool & overwrite)
{
    ofstream thinnedParameters;
    bool validFileName = false;
    string tempFileName;
    
    int totalParameters = 0;
    int totalSamples = 0;
    
    if (nruns == 1) {
        tempFileName = removeStringSuffix(fileName, '.', validFileName);
    } else {
// e.g. 'Archaeopteryx_gamma-rates_unequal-frequencies_temp-0.05.run1.p'
        tempFileName = fileName; // only prefix passed in
    }
    
    if (suffix.empty()) {
        suffix = "p";
    }
    
    tempFileName = tempFileName + "_thinned-" + convertIntToString(thinning) + "_burnin-" + convertIntToString(burnin) + "." + suffix;
        
    if (!overwrite) {
        // Check if file exists/is writable
        validFileName = false;
        while (!validFileName) {
            validFileName = checkValidOutputFile(tempFileName);
        }
    }
    
    thinnedParameters.open(tempFileName.c_str());
    bool commentLine = false;
    bool whiteSpaceOnly = false;
    
    cout << endl
    << "READING IN AND THINNING PARAMETERS..." << endl << endl;
    
    for (int i = 0; i < nruns; i++) {
        ifstream parameterInput;
        string currentFile = fileName;
        if (nruns > 1) {
// use MrBayes naming convention
            string runNumber = convertIntToString((i +1));
            currentFile = currentFile + ".run" + runNumber + "." + suffix;
        }
        
        checkValidInputFile(currentFile);
        parameterInput.open(currentFile.c_str());
        
        cout << "Extracting samples from file '" << currentFile << "'." << endl;
        
        if (burnin != 0) {
            cout << "Ignoring first (" << burnin << ") samples..." << endl;
        } else {
            cout << "No burnin declared." << endl;
        }
        cout << "Retaining every (" << thinning << ") samples..." << endl;
        
        int parameterCounter = 0;
        int sampleCounter = 0;
        string line;
        //bool headerEncountered = false;
        
    // Read in every non-empty (or non-whitespace), non-commented-out line
        while (getline(parameterInput, line)) {
            int stringPosition = 0;
            string temp;
            
            commentLine = checkCommentLine(line);
            whiteSpaceOnly = checkWhiteSpaceOnly(line);
            if (line.empty() || commentLine || whiteSpaceOnly) {
                // keep any comments from top of first file. don't really need this...
                if (i == 0) {
                    thinnedParameters << line << endl;
                }
                continue;
            } else if (checkStringValue(line, "Gen", stringPosition) || checkStringValue(line, "state", stringPosition)) { // MrBayes or BEAST
                // keep header from first file
                if (i == 0) {
                    thinnedParameters << line << endl;
                }
                continue;
            } else {
                if ((parameterCounter - burnin) > 0 && (parameterCounter - burnin) < thinning) {
                    parameterCounter++;
                    totalParameters++;
                    continue;
                } else if ((parameterCounter - burnin) == 0) { // Keep first sample
                    temp = removeStringElement(line, 0);
                    thinnedParameters << totalSamples << temp << endl;
                    parameterCounter++;
                    totalParameters++;
                    sampleCounter++;
                    totalSamples++;
                    continue;
                } else if ((parameterCounter - burnin) > 0 && (parameterCounter - burnin) % thinning == 0) {
                    temp = removeStringElement(line, 0);
                    thinnedParameters << totalSamples << temp << endl;
                    parameterCounter++;
                    totalParameters++;
                    sampleCounter++;
                    totalSamples++;
                    continue;
                } else {
                    parameterCounter++;
                    totalParameters++;
                    continue;
                }
            }
        }
        parameterInput.close();
        cout << "Retained " << sampleCounter << " samples." << endl;
    }
    thinnedParameters.close();
    
    cout << endl << "Successfully created file '" << tempFileName << "', populated with "
        << totalSamples << " samples (from original " << totalParameters << " samples)." << endl;
}
