#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

void removeSpaces(string &str);

bool containsOnlyValidCharacters(string const &str);

bool findSymbolOnTable(string, string [], int);

bool findDefinedSymbolOnTable(string, string [], bool [], int);

string getOpcode(string, string [], string []);

bool checkIsExtern(string, string [], bool []);

string instructionsTable[] = {"ADD", "SUB", "MUL", "DIV", "JMP", "JMPN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};

string directivesTable[] = {"SPACE", "CONST", "BEGIN", "END", "EXTERN", "PUBLIC"};

string opcodes[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14"};

int main(int argc, char **argv) {
    for (int arg = 0; arg < argc - 1; arg++) {
        ifstream file;
        ofstream preProcessedFile("pre_processed_file_" + string(argv[arg + 1]) + ".asm"), objFile(string(argv[arg + 1]) + ".obj");
        string line, originalLine, label, operation, operator1, operator2, number;
        string dataSection[100];
        string symbolsTable[100];
        string objInformation[100];
        string usageTable[100];
        int usageTableInfo[100] = { 0 };
        int symbolsPositionTable[100] = { 0 };
        int dependencyTable[100][100];
        int relatives[100];
        bool isDefined[100] = { false }, isExtern[100] = { false }, isPublic[100] = { false };
        bool isSectionData = false, labelLineBreak = false, sectionTextPresent = false, hasBegin = false, hasEnd = false;
        int dataSectionIndex = 0, lineIndex = 0, positionCounter = 0, symbolsTableIndex = 0, objIndex = 0, usageTableIndex = 0, relativesIndex = 0;
        size_t labelAux, operationAux, operatorsAux;

        fill(*dependencyTable, *dependencyTable + 100*100, -1);

        file.open(string(argv[arg + 1]) + ".asm", ifstream::binary);

        if (file.is_open() && preProcessedFile.is_open()) {
            while (file) {
                lineIndex++;
                getline(file, line);
                originalLine = line;

                if (line.empty()) continue;

                line = originalLine.substr(0, originalLine.find(";", 0));
                line += " ";
                removeSpaces(line);
                transform(line.begin(), line.end(), line.begin(), ::toupper);
                originalLine = line;

                if (line == "SECTION DATA") {
                    isSectionData = true;
                    dataSection[dataSectionIndex] = line;
                    dataSectionIndex++;
                    continue;
                }

                if (line == "SECTION TEXT") {
                    isSectionData = false;
                    sectionTextPresent = true;
                    preProcessedFile << line << " " << endl;
                    continue;
                }

                line += " ";

                if (labelLineBreak) {
                    labelLineBreak = false;

                    if (line.find_first_of(':') != string::npos) {
                        cout << "SYNTAX ERROR: TWO LABELS DEFINED IN LINE " << lineIndex << "! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                        return -1;
                    }

                    if (isSectionData) {
                        dataSection[dataSectionIndex] = label + ": " + line;
                        dataSectionIndex++;
                        continue;
                    } else {
                        preProcessedFile << label << ": " << line << endl;
                        continue;
                    }
                }
                
                labelAux = line.find_first_of(':');
                if (labelAux != string::npos) {
                    label = line.substr(0, labelAux);

                    if (!containsOnlyValidCharacters(label)) {
                        cout << "LEXICAL ERROR: " << label << " IN LINE " << lineIndex << " IS NOT VALID! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                        return -1;
                    }

                    if (line.length() - 1 == label.length() + 1) {
                        labelLineBreak = true;
                        continue;
                    }

                    line = line.substr(labelAux + 2);

                    if (line.find_first_of(':') != string::npos) {
                        cout << "SYNTAX ERROR: TWO LABELS DEFINED IN LINE " << lineIndex << "! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                        return -1;
                    }
                }

                if (!labelLineBreak) {
                    if (isSectionData) {
                        dataSection[dataSectionIndex] = originalLine;
                        dataSectionIndex++;
                    } else {
                        preProcessedFile << originalLine << " " << endl;
                    }
                }

                if (file.eof()) break;
            }
        }

        for (int i = 0; i < dataSectionIndex; i++)
            preProcessedFile << dataSection[i] << " " << endl;

        if (!sectionTextPresent) {
            cout << "SEMANTIC ERROR: SECTION TEXT IS NOT PRESENT! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
            return -1;
        }

        file.close();
        preProcessedFile.close();

        file.open("pre_processed_file_" + string(argv[arg + 1]) + ".asm");

        if (file.is_open() && objFile.is_open() ) {
            while (file) {
                getline(file, line);
                originalLine = line;

                if (line == "SECTION TEXT " || line == "SECTION DATA ")
                    continue;

                labelAux = line.find_first_of(':');
                if (labelAux != string::npos) {
                    label = line.substr(0, labelAux);
                    line = line.substr(labelAux + 2);

                    if (label == "EXTERN") {
                        if (!hasBegin) {
                            cout << "SEMANTIC ERROR: EXTERN IS NOT AVAILABLE BEFORE BEGIN IN LINE " << lineIndex << "! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                            return -1;
                        } else if (hasEnd) {
                            cout << "SEMANTIC ERROR: EXTERN IS NOT AVAILABLE AFTER END IN LINE " << lineIndex << "! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                            return -1;
                        }

                        operationAux = line.find_first_of(' ');
                        operation = line.substr(0, operationAux);

                        symbolsTable[symbolsTableIndex] = operation;
                        isExtern[symbolsTableIndex] = true;
                        symbolsTableIndex++;
                        continue;
                    }

                    if (findDefinedSymbolOnTable(label, symbolsTable, isDefined, symbolsTableIndex)) {
                        cout << "SEMANTIC ERROR: LABEL " << label << " ALREADY DEFINED - LINE " << lineIndex <<  "! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                        return -1;
                    } else {
                        if (findSymbolOnTable(label, symbolsTable, symbolsTableIndex)) {
                            for (int i = 0; i < symbolsTableIndex; i++) {
                                if (symbolsTable[i] == label) {
                                    symbolsTable[i] = label;
                                    symbolsPositionTable[i] = positionCounter;
                                    isDefined[i] = true;
                                    break;
                                }
                            }
                        } else {
                            symbolsTable[symbolsTableIndex] = label;
                            symbolsPositionTable[symbolsTableIndex] = positionCounter;
                            isDefined[symbolsTableIndex] = true;
                            symbolsTableIndex++;
                        }
                    }
                }
                
                operationAux = line.find_first_of(' ');
                if (operationAux != string::npos) {
                    operation = line.substr(0, operationAux);
                    line = line.substr(operationAux+1);

                    if (findSymbolOnTable(operation, instructionsTable, 14)) {
                        if (operation == "STOP") {
                            objInformation[objIndex] = "14";
                            objIndex++;
                        }

                        positionCounter++;
                        if (operation != "STOP") {
                            if (operation != "COPY") {
                                operatorsAux = line.find_last_of(' ');
                                operator1 = line.substr(0, operatorsAux);

                                objInformation[objIndex] = getOpcode(operation, instructionsTable, opcodes);
                                objIndex++;

                                if (findDefinedSymbolOnTable(operator1, symbolsTable, isDefined, symbolsTableIndex)) {
                                    for (int i = 0; i < symbolsTableIndex; i++) {
                                        if (symbolsTable[i] == operator1) {
                                            if (operator1.find('+') != string::npos) {
                                                line = operator1;
                                                operatorsAux = line.find_first_of(' ');
                                                operator1 = line.substr(0, operatorsAux);
                                                number = line.substr(operatorsAux+3);
                                                objInformation[objIndex] = to_string(symbolsPositionTable[i] + stoi(number));
                                            } else {
                                                objInformation[objIndex] = to_string(symbolsPositionTable[i]);
                                            }
                                        }
                                    }

                                    relatives[relativesIndex] = positionCounter;
                                    relativesIndex++;
                                } else {
                                    if (operator1.find('+') != string::npos) {
                                        line = operator1;
                                        operatorsAux = line.find_first_of(' ');
                                        operator1 = line.substr(0, operatorsAux);
                                        number = line.substr(operatorsAux+3);
                                        objInformation[objIndex] = number;
                                    } else {
                                        objInformation[objIndex] = "00";
                                    }

                                    if (findSymbolOnTable(operator1, symbolsTable, symbolsTableIndex)) {
                                        for (int i = 0; i < symbolsTableIndex; i++) {
                                            if (symbolsTable[i] == operator1) {
                                                for (int j = 0; j < 100; j++) {
                                                    if (dependencyTable[i][j] == -1) {
                                                        dependencyTable[i][j] = positionCounter;
                                                        break;
                                                    }
                                                }
                                            }
                                        }

                                        if (checkIsExtern(operator1, symbolsTable, isExtern)) {
                                            usageTable[usageTableIndex] = operator1;
                                            usageTableInfo[usageTableIndex] = positionCounter;
                                            usageTableIndex++;
                                        }

                                        relatives[relativesIndex] = positionCounter;
                                        relativesIndex++;
                                    } else {
                                        symbolsTable[symbolsTableIndex] = operator1;
                                        for (int i = 0; i < 100; i++) {
                                            if (dependencyTable[symbolsTableIndex][i] == -1) {
                                                dependencyTable[symbolsTableIndex][i] = positionCounter;
                                                break;
                                            }
                                        }

                                        relatives[relativesIndex] = positionCounter;
                                        relativesIndex++;
                                        symbolsTableIndex++;
                                    }
                                }
                                objIndex++;
                            }
                            
                            positionCounter++;

                            if (operation == "COPY") {
                                operatorsAux = line.find_first_of(',');
                                operator1 = line.substr(0, operatorsAux);
                                line = line.substr(operatorsAux+2);
                                operatorsAux = line.find_last_of(' ');
                                operator2 = line.substr(0, operatorsAux);

                                objInformation[objIndex] = "09";
                                objIndex++;
                                if (findDefinedSymbolOnTable(operator1, symbolsTable, isDefined, symbolsTableIndex)) {
                                    for (int i = 0; i < symbolsTableIndex; i++) {
                                        if (operator1.find('+') != string::npos) {
                                            line = operator1;
                                            operatorsAux = line.find_first_of(' ');
                                            operator1 = line.substr(0, operatorsAux);
                                            number = line.substr(operatorsAux+3);
                                            objInformation[objIndex] = to_string(symbolsPositionTable[i] + stoi(number));
                                        } else {
                                            objInformation[objIndex] = to_string(symbolsPositionTable[i]);
                                        }
                                    }

                                    relatives[relativesIndex] = positionCounter - 1;
                                    relativesIndex++;
                                } else {
                                    if (operator1.find('+') != string::npos) {
                                        line = operator1;
                                        operatorsAux = line.find_first_of(' ');
                                        operator1 = line.substr(0, operatorsAux);
                                        number = line.substr(operatorsAux+3);
                                        objInformation[objIndex] = number;
                                    } else {
                                        objInformation[objIndex] = "00";
                                    }
                                    if (findSymbolOnTable(operator1, symbolsTable, symbolsTableIndex)) {
                                        for (int i = 0; i < symbolsTableIndex; i++) {
                                            if (symbolsTable[i] == operator1) {
                                                for (int j = 0; j < 100; j++) {
                                                    if (dependencyTable[i][j] == -1) {
                                                        dependencyTable[i][j] = positionCounter - 1;
                                                        break;
                                                    }
                                                }
                                            }
                                        }

                                        if (checkIsExtern(operator1, symbolsTable, isExtern)) {
                                            usageTable[usageTableIndex] = operator1;
                                            usageTableInfo[usageTableIndex] = positionCounter - 1;
                                            usageTableIndex++;
                                        }

                                        relatives[relativesIndex] = positionCounter - 1;
                                        relativesIndex++;
                                    } else {
                                        symbolsTable[symbolsTableIndex] = operator1;
                                        for (int i = 0; i < 100; i++) {
                                            if (dependencyTable[symbolsTableIndex][i] == -1) {
                                                dependencyTable[symbolsTableIndex][i] = positionCounter - 1;
                                                break;
                                            }
                                        }
                                        relatives[relativesIndex] = positionCounter - 1;
                                        relativesIndex++;
                                        symbolsTableIndex++;
                                    }
                                }
                                
                                objIndex++;

                                if (findDefinedSymbolOnTable(operator2, symbolsTable, isDefined, symbolsTableIndex)) {
                                    for (int i = 0; i < symbolsTableIndex; i++) {
                                        if (operator2.find('+') != string::npos) {
                                            line = operator2;
                                            operatorsAux = line.find_first_of(' ');
                                            operator2 = line.substr(0, operatorsAux);
                                            number = line.substr(operatorsAux+3);
                                            objInformation[objIndex] = to_string(symbolsPositionTable[i] + stoi(number));
                                        } else {
                                            objInformation[objIndex] = to_string(symbolsPositionTable[i]);
                                        }
                                    }

                                    relatives[relativesIndex] = positionCounter;
                                    relativesIndex++;
                                } else {
                                    if (operator2.find('+') != string::npos) {
                                        line = operator2;
                                        operatorsAux = line.find_first_of(' ');
                                        operator2 = line.substr(0, operatorsAux);
                                        number = line.substr(operatorsAux+3);
                                        objInformation[objIndex] = number;
                                    } else {
                                        objInformation[objIndex] = "00";
                                    }

                                    if (findSymbolOnTable(operator2, symbolsTable, symbolsTableIndex)) {
                                        for (int i = 0; i < symbolsTableIndex; i++) {
                                            if (symbolsTable[i] == operator2) {
                                                for (int j = 0; j < 100; j++) {
                                                    if (dependencyTable[i][j] == -1) {
                                                        dependencyTable[i][j] = positionCounter;
                                                        break;
                                                    }
                                                }
                                            }
                                        }

                                        if (checkIsExtern(operator2, symbolsTable, isExtern)) {
                                            usageTable[usageTableIndex] = operator2;
                                            usageTableInfo[usageTableIndex] = positionCounter;
                                            usageTableIndex++;
                                        }

                                        relatives[relativesIndex] = positionCounter;
                                        relativesIndex++;
                                    } else {
                                        symbolsTable[symbolsTableIndex] = operator2;
                                        for (int i = 0; i < 100; i++) {
                                            if (dependencyTable[symbolsTableIndex][i] == -1) {
                                                dependencyTable[symbolsTableIndex][i] = positionCounter;
                                                break;
                                            }
                                        }

                                        relatives[relativesIndex] = positionCounter;
                                        relativesIndex++;
                                        symbolsTableIndex++;
                                    }
                                }

                                objIndex++;
                                positionCounter++;
                            }
                        }
                    } else if (findSymbolOnTable(operation, directivesTable, 6)) {
                        if (operation == "CONST") {
                            operatorsAux = line.find_first_of(' ');
                            operator1 = line.substr(0, operatorsAux);

                            if (operator1[1] == 'X') {
                                operator1.erase(0, 2);
                                unsigned int i;
                                istringstream iss(operator1);
                                iss >> hex >> i;

                                objInformation[objIndex] = to_string(i);
                            } else {
                                objInformation[objIndex] = operator1;
                            }
                            objIndex++;
                            positionCounter++;
                        } else if (operation == "SPACE") {
                            operatorsAux = line.find_first_of(' ');
                            if (operatorsAux != string::npos) {
                                operator1 = line.substr(0, operatorsAux);
                                int spaces = stoi(operator1);

                                for (int i = 0; i < spaces; i++) {
                                    objInformation[objIndex] = "00";
                                    objIndex++;
                                    positionCounter++;
                                }
                            } else {
                                objInformation[objIndex] = "00";
                                objIndex++;
                                positionCounter++;
                            }
                        } else if (operation == "BEGIN") {
                            hasBegin = true;
                            hasEnd = false;
                        } else if (operation == "END") {
                            hasEnd = true;
                            hasBegin = false;
                        } else if (operation == "PUBLIC") {
                            if (!hasBegin) {
                                cout << "SEMANTIC ERROR: PUBLIC IS NOT AVAILABLE BEFORE BEGIN IN LINE " << lineIndex << "! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                                return -1;
                            } else if (hasEnd) {
                                cout << "SEMANTIC ERROR: PUBLIC IS NOT AVAILABLE AFTER END IN LINE " << lineIndex << "! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                                return -1;
                            }

                            operatorsAux = line.find_first_of(' ');
                            operator1 = line.substr(0, operatorsAux);

                            if (findSymbolOnTable(operator1, symbolsTable, symbolsTableIndex)) {
                                for (int i = 0; i < symbolsTableIndex; i++) {
                                    if (symbolsTable[i] == operator1) 
                                        isPublic[i] = true;
                                }
                            } else {
                                symbolsTable[symbolsTableIndex] = operator1;
                                isPublic[symbolsTableIndex] = true;
                                symbolsTableIndex++;
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < symbolsTableIndex; i++) {
            if (!isDefined[i] && !isExtern[i]) {
                cout << "SEMANTIC ERROR: LABEL " << symbolsTable[i] << " NOT DEFINED! - ON FILE " << string(argv[arg + 1]) + ".asm" << endl;
                return -1;
            }

            for (int j = 0; j < 100; j++) {
                if (dependencyTable[i][j] == -1) break;

                objInformation[dependencyTable[i][j]] = to_string(stoi(objInformation[dependencyTable[i][j]]) + symbolsPositionTable[i]);
            }
        }

        if (!hasBegin && hasEnd) {
            objFile << "USO" << endl;
            for (int i = 0; i < usageTableIndex; i++)
                objFile << usageTable[i] << " " << usageTableInfo[i] << " " << endl;

            objFile << "DEF" << endl;
            for (int i = 0; i < symbolsTableIndex; i++) {
                if (isPublic[i])
                    objFile << symbolsTable[i] << " " << symbolsPositionTable[i] << " " << endl;
            }

            objFile << "RELATIVOS" << endl;
            for (int i = 0; i < relativesIndex; i++)
                objFile << relatives[i] << " ";
            objFile << endl;
            objFile << "CODE" << endl;
        }



        for (int i = 0; i < objIndex; i++)
            objFile << objInformation[i] << " ";
        objFile << endl;

        file.close();
        objFile.close();
    }

    return 0;
}

void removeSpaces(string &str) {
    int n = str.length();
 
    int i = 0, j = -1;
 
    bool spaceFound = false;
 
    while (++j < n && str[j] == ' ');
 
    while (j < n) {
        if (str[j] != ' ') {
            if ((str[j] == '.' || str[j] == ',' ||
                 str[j] == '?') && i - 1 >= 0 &&
                 str[i - 1] == ' ')
                str[i - 1] = str[j++];
            else
                str[i++] = str[j++];
 
            spaceFound = false;
        } else if (str[j++] == ' ') {
            if (!spaceFound) {
                str[i++] = ' ';
                spaceFound = true;
            }
        }
    }
 
    if (i <= 1)
        str.erase(str.begin() + i, str.end());
    else
        str.erase(str.begin() + i - 1, str.end());

    size_t endpos = str.find_last_not_of("\r\n");
    if (endpos != std::string::npos) {
        str.substr(0,endpos+1).swap(str);
    }
}

bool containsOnlyValidCharacters(string const &str) {
    return str.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890") == string::npos && !isdigit(str[0]);
}

bool findSymbolOnTable(string symbol, string table[], int quantityPositions) {
    for (int i = 0; i < quantityPositions; i++) {
        if (table[i] == symbol)
            return true;
    }
    return false;
}

bool findDefinedSymbolOnTable(string symbol, string table[], bool isDefined[], int quantityPositions) {
    for (int i = 0; i < quantityPositions; i++) {
        if (table[i] == symbol && isDefined[i])
            return true;
    }
    return false;
}

string getOpcode(string instruction, string instructionsTable[], string opcodes[]) {
    for(int i = 0; i < 14; i++) {
        if (instructionsTable[i] == instruction)
            return opcodes[i];
    }
    return "";
}

bool checkIsExtern(string operator1, string symbolsTable[], bool isExtern[]) {
    for (int i = 0; i < 100; i++)
        if (operator1 == symbolsTable[i]) return isExtern[i];
    return false;
}