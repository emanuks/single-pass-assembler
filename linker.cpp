#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <limits>

using namespace std;

int main(int argc, char **argv) {
    string usageTable[100], defTable[100];
    int usageTablePosition[100], defTablePosition[100], relativePositions[100], instructions[100];
    int usageTableIndex = 0, defTableIndex = 0, relativePositionsIndex = 0, instructionsIndex = 0;
    int instructionCount = 0;
    int correctionFactors[5] = { 0 };
    bool markedRelativePositions[100] = { false };
    ofstream execFile(string(argv[1]) + ".exe");

    for (int arg = 0; arg < argc - 1; arg++) {
        ifstream file;
        string line, reader;
        correctionFactors[arg] = instructionCount;
        bool isUse = false, isDef = false, isRelatives = false, isCode = false;
        size_t readerAux;

        file.open(string(argv[arg + 1]) + ".obj", ifstream::binary);

        if (file.is_open() && execFile.is_open()) {
            while(file) {
                getline(file, line);

                if (line.empty()) continue;

                if (argc == 2) {
                    execFile << line << endl;
                    continue;
                }

                if (line == "USO") {
                    isUse = true;
                    continue;
                } else if (line == "DEF") {
                    isUse = false;
                    isDef = true;
                    continue;
                } else if (line == "RELATIVOS") {
                    isDef = false;
                    isRelatives = true;
                    continue;
                } else if (line == "CODE") {
                    isRelatives = false;
                    isCode = true;
                    continue;
                }

                if (isUse) {
                    readerAux = line.find_first_of(" ");
                    reader = line.substr(0, readerAux);
                    line = line.substr(readerAux + 1);

                    usageTable[usageTableIndex] = reader;

                    readerAux = line.find_first_of(" ");
                    reader = line.substr(0, readerAux);

                    usageTablePosition[usageTableIndex] = stoi(reader) + correctionFactors[arg];
                    usageTableIndex++;
                } else if (isDef) {
                    readerAux = line.find_first_of(" ");
                    reader = line.substr(0, readerAux);
                    line = line.substr(readerAux + 1);

                    defTable[defTableIndex] = reader;

                    readerAux = line.find_first_of(" ");
                    reader = line.substr(0, readerAux);

                    defTablePosition[defTableIndex] = stoi(reader) + correctionFactors[arg];
                    defTableIndex++;
                } else if (isRelatives) {
                    while(line.size() > 0) {
                        readerAux = line.find_first_of(" ");
                        reader = line.substr(0, readerAux);
                        line = line.substr(readerAux + 1);

                        relativePositions[relativePositionsIndex] = stoi(reader) + correctionFactors[arg];
                        relativePositionsIndex++;
                    }
                } else {
                    while(line.size() > 0) {
                        readerAux = line.find_first_of(" ");
                        reader = line.substr(0, readerAux);
                        line = line.substr(readerAux + 1);

                        instructions[instructionsIndex] = stoi(reader);

                        for (int i = 0; i < relativePositionsIndex; i++) {
                            if (relativePositions[i] == instructionsIndex && !markedRelativePositions[instructionsIndex]) {
                                instructions[instructionsIndex] += correctionFactors[arg];
                                markedRelativePositions[instructionsIndex] = true;
                            }
                        }

                        instructionsIndex++;
                        instructionCount++;
                    }
                }

                if (file.eof()) break;
            }
        }

        file.close();
    }

    int currentCorrectionFactor = 0;

    for (int i = 1; i < 5; i++) {
        if (correctionFactors[i] == 0)
            correctionFactors[i] = instructionCount;
    }

    for (int i = 0; i < usageTableIndex; i++) {
        for (int j = 0; j < defTableIndex; j++) {
            if (currentCorrectionFactor < 3 && (usageTablePosition[i] >= correctionFactors[currentCorrectionFactor + 1]))
                currentCorrectionFactor++;

            if (usageTable[i] == defTable[j]) {
                instructions[usageTablePosition[i]] += defTablePosition[j] - correctionFactors[currentCorrectionFactor];
                markedRelativePositions[relativePositions[i]] = true;
            }
        }
    }

    currentCorrectionFactor = 0;

    for (int i = 0; i < relativePositionsIndex; i++) {
        if (currentCorrectionFactor < 3 && (relativePositions[i] >= correctionFactors[currentCorrectionFactor + 1]))
            currentCorrectionFactor++;

        if (!markedRelativePositions[relativePositions[i]]) {
            instructions[relativePositions[i]] += correctionFactors[currentCorrectionFactor];
            markedRelativePositions[relativePositions[i]] = true;
        }
    }

    for (int i = 0; i < instructionCount; i++) 
        execFile << instructions[i] << " ";
    execFile << endl;

    execFile.close();

    return 0;
}