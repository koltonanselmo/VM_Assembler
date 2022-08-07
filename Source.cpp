#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <iterator>
#include <map>
#include <cstring>
#include <algorithm>
using namespace std;

//===================================================================================GLOBAL==================================================================================================================================================================

#define MEMSIZE 1024 * 128
#define PC 8
#define FP 9
#define SP 10
#define SL 11
#define DB 12

char MainMem[MEMSIZE];
//                  0 1 2 3 4 5 6 7 PC8       SL9          SP10     FP11      SB12
int mainReg[13] = { 0,0,0,0,0,0,0,0,0,(MEMSIZE - 32768),(MEMSIZE - 8),(0),(MEMSIZE - 8)};
//instruction registers
int opCode, operand1, operand2 = 0;

map<string, int> symbolTable;
//map<string, int> instructionSet;

//===================================================================================GENERAL==================================================================================================================================================================

void writeInt(int data, int address)
{
	int max_addr = MEMSIZE - 4;
	if (address >= max_addr || address < 0)
		// Out of the range, should be 0-MEMSIZE/4-1
		cout << "Unaddressable address.";  // Unaddressable
	char* ptr = MainMem;
	ptr += address;
	(*(int*)ptr) = data;
}

void writeChar(char data, int address)
{
	int max_addr = MEMSIZE;
	if (address >= max_addr || address < 0)
		// Out of the range, should be 0-MEMSIZE/4-1
		cout << "Unaddressable address.";  // Unaddressable
	MainMem[address] = data;
}

int readInt(int address)
{
	int max_addr = MEMSIZE;
	if (address >= max_addr || address < 0)
		// Out of the range, should be 0-MEMSIZE/4-1
		cout << "Unaddressable address.";  // Unaddressable
	char* ptr = MainMem;
	ptr += address;
	return (*(int*)ptr);
}

char readChar(int address)
{
	int max_addr = MEMSIZE;
	if (address >= max_addr || address < 0)
		// Out of the range, should be 0-MEMSIZE/4-1
		cout << "Unaddressable address.";  // Unaddressable
	return MainMem[address];
}

int stringToInt(string _valueToBeConverted) {
	string valueToBeConverted = _valueToBeConverted;
	int convertedValue = 0;

	stringstream converter(valueToBeConverted);

	converter >> convertedValue;

	return convertedValue;
}

//===================================================================================ASSEMBLER==================================================================================================================================================================

void assembler(string _fileName, int _pass) {
	string fileName = _fileName; //passed in file name
	int pass = _pass;
	int byteCounter = 0; //intilizing byte counter
	string line = ""; //line read in from file one at a time
	vector<string> tokens; //vector to store tokens

	ifstream asmFile(fileName);
	if (pass == 1) { //SYMBOL TABLE PASS
		while (getline(asmFile, line)) { //getting one line from file at a time
			stringstream check1(line);
			string intermediate = ""; //creating a way to tokenize

			while (getline(check1, intermediate, ' ')) { //breaking by white space
				tokens.push_back(intermediate);
			}
			
			if (tokens[0] == ";") {
				//comment do nothing
			}


			else { //not a comment

				if (tokens.size() == 4) {

					symbolTable.insert(pair<string, int>(tokens[0], byteCounter));

					byteCounter = byteCounter + 12;
				}

				//symbol table entry
				else if (tokens[0] == "ADD" || tokens[0] == "ADI" || tokens[0] == "SUB" || tokens[0] == "MUL" || tokens[0] == "DIV" || tokens[0] == "LDR" || tokens[0] == "LDA" || tokens[0] == "MOV" || tokens[0] == "LDB" || tokens[0] == "STR" || tokens[0] == "STB" || tokens[0] == "JMR" || tokens[0] == "JMP" || tokens[0] == "BRZ" || tokens[0] == "BNZ" || tokens[0] == "BLT" || tokens[0] == "BGT" || tokens[0] == "CMP" || tokens[0] == "TRP") {

						symbolTable.insert(pair<string, int>(tokens[0], byteCounter));

						byteCounter = byteCounter + 12;
				}

				else if (tokens[0] == ".INT" || tokens[0] == ".BYT") {
						if (tokens[0] == ".INT") {
							byteCounter += 4;
						}
						else {
							byteCounter += 1;
						}
				}
					
				else { //label
					if (tokens[1] == ".INT") {

						symbolTable.insert(pair<string, int>(tokens[0], byteCounter)); //putting label in symbol table this is a .INT

						byteCounter = byteCounter + 4;
					}
					else if (tokens[1] == ".BYT") {

						symbolTable.insert(pair<string, int>(tokens[0], byteCounter)); // putting label in symbol table this is a .BYT

						byteCounter = byteCounter + 1;
					}
					else {
						cout << "syntax error first pass";
						exit(0);
					}
				}

			}

			tokens.clear(); //clearing tokens vector for next line
		}
		asmFile.close(); //closing file to reopen for pass two
	}


	else if (pass == 2) { //GENERATE BYTE CODE PASS==================================================================PASS TWO==================================================================================================================================

		bool firstInstruction = true; // to set pc to the correct start value after directives

		while (getline(asmFile, line)) { //getting one line from file at a time
			stringstream check1(line);
			string intermediate = ""; //creating a way to tokenize

			while (getline(check1, intermediate, ' ')) { //breaking by white space
				tokens.push_back(intermediate);
			}

			if (tokens[0] == ";") {
				//comment do nothing
			}
			else { //not a comment

				if (tokens.size() == 4) {

					tokens.erase(tokens.begin());

				}

			//intstructions pass two
				if (tokens[0] == "ADD" || tokens[0] == "ADI" || tokens[0] == "SUB" || tokens[0] == "MUL" || tokens[0] == "DIV" || tokens[0] == "LDR" || tokens[0] == "LDA" || tokens[0] == "MOV" || tokens[0] == "LDB" || tokens[0] == "STR" || tokens[0] == "STB" || tokens[0] == "JMR" || tokens[0] == "JMP" || tokens[0] == "BRZ" || tokens[0] == "BNZ" || tokens[0] == "BLT" || tokens[0] == "BGT" || tokens[0] == "CMP" || tokens[0] == "TRP") { //instruction branch


					if (firstInstruction == true) { //
						mainReg[PC] = byteCounter;  //these three lines sole purpose is to set the program counter for VM
						firstInstruction = false;   // 
					}

					
					
					if (tokens[0] == "STR" || tokens[0] == "LDR" || tokens[0] == "STB" || tokens[0] == "LDB" || tokens[0] == "LDA") { //seperated possible indirect instructions for different byte code generation

						//MOVE INSTRUCTIONS
						if (symbolTable.count(tokens[2])) { //CHECKING TO SEE IF IT IS THE INDIRECT OR DIRECT FORM OF INSTUCTION SEPERATED FROM OTHER INSTRUCTIONS FOR THIS PURPOSE

							if (tokens[0] == "STR") {

								//getting address of label
								int tempAddress = symbolTable[tokens[2]];

								//converting string to int
								int operand1Int = 0;
								operand1Int = stringToInt(tokens[1]);

								//generating byte code for str instruction
								writeInt(9, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(tempAddress, byteCounter + 8);

							}
							else if (tokens[0] == "LDR") {

								//getting address of label
								int tempAddress = symbolTable[tokens[2]];

								//converting string to int
								int operand1Int = 0;
								operand1Int = stringToInt(tokens[1]);

								//generating byte code for ldr instruction
								writeInt(10, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(tempAddress, byteCounter + 8);

							}
							else if (tokens[0] == "STB") {

								//getting address of label
								int tempAddress = symbolTable[tokens[2]];

								//converting string to int
								int operand1Int = 0;
								operand1Int = stringToInt(tokens[1]);

								//generating byte code for stb instruction
								writeInt(11, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(tempAddress, byteCounter + 8);

							}
							else if (tokens[0] == "LDB") {

								//getting address of label
								int tempAddress = symbolTable[tokens[2]];

								//converting string to int
								int operand1Int = 0;
								operand1Int = stringToInt(tokens[1]);

								//generating byte code for ldr instruction
								writeInt(10, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(tempAddress, byteCounter + 8);

							}
							else if (tokens[0] == "LDA") {

								//setting what register to put the label address in
								int operand1Int = 0;
								operand1Int = stringToInt(tokens[1]);

								//getting address of label to be loaded in register
								int tempAddress = symbolTable[tokens[2]];

								writeInt(8, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(tempAddress, byteCounter + 8);
							}

						}
						else { //INDIRECT FORM

							if (tokens[0] == "LDR") {

								int operand1Int = stringToInt(tokens[1]);
								int operand2Int = stringToInt(tokens[2]);

								writeInt(23, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(operand2Int, byteCounter + 8);
							}
							else if (tokens[0] == "STR") {

								int operand1Int = stringToInt(tokens[1]);
								int operand2Int = stringToInt(tokens[2]);

								writeInt(22, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(operand2Int, byteCounter + 8);
							}
							else if (tokens[0] == "LDB") {

								int operand1Int = stringToInt(tokens[1]);
								int operand2Int = stringToInt(tokens[2]);

								writeInt(25, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(operand2Int, byteCounter + 8);
							}
							else if (tokens[0] == "STB") {

								int operand1Int = stringToInt(tokens[1]);
								int operand2Int = stringToInt(tokens[2]);

								writeInt(24, byteCounter);
								writeInt(operand1Int, byteCounter + 4);
								writeInt(operand2Int, byteCounter + 8);
							}

						}

					}
					//compare instrcution byte code generation
					else if (tokens[0] == "CMP") {

						int operand1Int = stringToInt(tokens[1]);
						int operand2Int = stringToInt(tokens[2]);

						writeInt(20, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
						writeInt(operand2Int, byteCounter + 8);
					}
					//JUMP INSTRUCTIONs
					else if (tokens[0] == "JMP") {

						int tempAddress = symbolTable[tokens[1]];
						int operand2Int = 0; //not used

						writeInt(1, byteCounter);
						writeInt(tempAddress, byteCounter + 4);
						writeInt(operand2Int, byteCounter + 8);

					}
					else if (tokens[0] == "JMR") { //jump immediate
						
						int operand1Int = stringToInt(tokens[1]);
						int operand2Int = 0; //not used

						writeInt(2, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
						writeInt(operand2Int, byteCounter + 8);
					}

					else if (tokens[0] == "MOV") {

						//converting string to int
						int operand1Int, operand2Int;
						operand1Int = stringToInt(tokens[1]);
						operand2Int = stringToInt(tokens[2]);

						//generating byte code for MOV
						writeInt(7, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
						writeInt(operand2Int, byteCounter + 8);
					}

					else if (tokens[0] == "BGT") {

						int operand1Int = stringToInt(tokens[1]);

						int tempAddress = symbolTable[tokens[2]];

						writeInt(4, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
						writeInt(tempAddress, byteCounter + 8);

					}

					else if (tokens[0] == "BLT") {

						int operand1Int = stringToInt(tokens[1]);

						int tempAddress = symbolTable[tokens[2]];

						writeInt(5, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
						writeInt(tempAddress, byteCounter + 8);

					}

					else if (tokens[0] == "BNZ") {
						
						int operand1Int = stringToInt(tokens[1]);

						int tempAddress = symbolTable[tokens[2]];

						writeInt(3, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
						writeInt(tempAddress, byteCounter + 8);

					}

					else if (tokens[0] == "BRZ") {

						int operand1Int = stringToInt(tokens[1]);

						int tempAddress = symbolTable[tokens[2]];

						writeInt(6, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
						writeInt(tempAddress, byteCounter + 8);

					}

					//ARITHMATIC INSTRUCTIONS

					else if (tokens[0] == "ADD") {
						int operand1Int = stringToInt(tokens[1]); //stringToInt helper Fucntion
						int operand2Int = stringToInt(tokens[2]);

						writeInt(13, byteCounter); //sending opcode to memory
						writeInt(operand1Int, byteCounter + 4); //sending operand one to memory plus offset
						writeInt(operand2Int, byteCounter + 8); //sending operand two to memory plus offset

					}
					else if (tokens[0] == "ADI") {
						int operand1Int = stringToInt(tokens[1]); //stringToInt helper Function
						int operand2Int = stringToInt(tokens[2]); //stringToInt helper Function

						writeInt(14, byteCounter); //sending opcode to memory
						writeInt(operand1Int, byteCounter + 4); //sending operand one to memory plus offset
						writeInt(operand2Int, byteCounter + 8); //sending operand two to memory plus offset

					}
					else if (tokens[0] == "SUB") {
						int operand1Int = stringToInt(tokens[1]); //stringToInt helper Fucntion
						int operand2Int = stringToInt(tokens[2]);

						writeInt(15, byteCounter); //sending opcode to memory
						writeInt(operand1Int, byteCounter + 4); //sending operand one to memory plus offset
						writeInt(operand2Int, byteCounter + 8); //sending operand two to memory plus offset
					}
					else if (tokens[0] == "MUL") {
						int operand1Int = stringToInt(tokens[1]); //stringToInt helper Fucntion
						int operand2Int = stringToInt(tokens[2]);

						writeInt(16, byteCounter); //sending opcode to memory
						writeInt(operand1Int, byteCounter + 4); //sending operand one to memory plus offset
						writeInt(operand2Int, byteCounter + 8); //sending operand two to memory plus offset
					}
					else if (tokens[0] == "DIV") {
						int operand1Int = stringToInt(tokens[1]); //stringToInt helper Fucntion
						int operand2Int = stringToInt(tokens[2]);

						writeInt(17, byteCounter); //sending opcode to memory
						writeInt(operand1Int, byteCounter + 4); //sending operand one to memory plus offset
						writeInt(operand2Int, byteCounter + 8); //sending operand two to memory plus offset
					}


					//trap instuctions

					else if (tokens[0] == "TRP") {
						//converting string to int
						int operand1Int = 0;
						operand1Int = stringToInt(tokens[1]);

						//generating byte code for trap notice only one operand
						writeInt(21, byteCounter);
						writeInt(operand1Int, byteCounter + 4);
					}
					else {
						cout << endl << "syntax error byte code";
					}
					byteCounter = byteCounter + 12;
				}

				//ARRAY CONTINOUS MEMORY=======================================================================================================================================================

				else if (tokens[0] == ".INT" || tokens[0] == ".BYT") {
					if (tokens[0] == ".INT") {

						int tempValue = stringToInt(tokens[1]);
						writeInt(tempValue, byteCounter);
						byteCounter += 4;

					}
					else {
						int tempValue = stringToInt(tokens[1]);
						writeChar(tempValue, byteCounter);
						byteCounter += 1;

					}
				}
				//LABELS=======================================================================================================================================================================

				else { 
					if (tokens[1] == ".INT") {

						//converted string to int for value of directive
						int value = 0;
						value = stringToInt(tokens[2]);

						writeInt(value, byteCounter);;
						byteCounter = byteCounter + 4;
					}
					else if (tokens[1] == ".BYT") {

						//below is converting string to char keep this around incase you need it later
						//char* char_arr;
						//string str_obj(tokens[2]);
						//char_arr = &str_obj[0];

						//writeChar(*char_arr, byteCounter);

						int value = stringToInt(tokens[2]);
						writeChar(value, byteCounter);

						byteCounter = byteCounter + 1;

					}
					else {
						cout << "syntax error second pass lable";
						exit(0);
					}
				}

			}

			tokens.clear(); //clearing tokens vector for the next line
		}
		asmFile.close(); //closing file
	}

}

//===================================================================================VM==================================================================================================================================================================

void virtualMachine() {
	bool run = true;
	while (run) {

		opCode = readInt(mainReg[PC]);
		operand1 = readInt(mainReg[PC] + 4);
		operand2 = readInt(mainReg[PC] + 8);

		switch (opCode) {

		case 21://trap

			if (operand1 == 0) {
				run = false;
			}
			else if (operand1 == 1) {
				cout << mainReg[3];
			}
			else if (operand1 == 2) {
				int tempInput = 0;
				cin >> tempInput;
				mainReg[3] = tempInput;
			}
			else if (operand1 == 3) {
				if ((char)mainReg[3] == '_') {
					cout << " ";
				}
				else if ((char)mainReg[3] == '|') {
					cout << endl;
				}
				else {
					//cout << readChar(mainReg[PC] - 4);
					cout << (char)mainReg[3];
				}
			}
			else if (operand1 == 4) {
				long int tempInput;
				tempInput = getchar();
				mainReg[3] = tempInput;
			}

			mainReg[PC] += 12;

			break;

		case 1://JMP

			mainReg[PC] = operand1;


			break;

		case 2://JMR

			mainReg[PC] = mainReg[operand1];

			mainReg[PC] += 12;

			break;

		case 3://BNZ

			if (mainReg[operand1] != 0) {

				mainReg[PC] = operand2;

			}
			else {
				mainReg[PC] += 12;
			}

			break;

		case 4://BGT

			if (mainReg[operand1] > 0) {

				mainReg[PC] = operand2;

			}
			else {
				mainReg[PC] += 12;
			}

			break;

		case 5://BLT

			if (mainReg[operand1] < 0) {

				mainReg[PC] = operand2;

			}
			else {
				mainReg[PC] += 12;
			}

			break;

		case 6://BRZ

			if (mainReg[operand1] == 0) {

				mainReg[PC] = operand2;

			}
			else {
				mainReg[PC] += 12;
			}

			break;

		case 7://MOV

			mainReg[operand1] = mainReg[operand2];

			mainReg[PC] += 12;

			break;
		case 8://LDA

			mainReg[operand1] = operand2;

			mainReg[PC] += 12;

			break;

		case 9://STR

			writeInt(mainReg[operand1], operand2);

			mainReg[PC] += 12;

			break;

		case 10://LDR

			mainReg[operand1] = readInt(operand2);

			mainReg[PC] += 12;

			break;

		case 11://STB

			writeChar(mainReg[operand1], operand2);

			mainReg[PC] += 12;

			break;

		case 12: //LDB

			mainReg[operand1] = operand2;

			mainReg[PC] += 12;

			break;

		case 13://ADD

			mainReg[operand1] += mainReg[operand2];

			mainReg[PC] += 12;

			break;

		case 14://ADI IMMEDIATE ADD

			mainReg[operand1] += operand2;

			mainReg[PC] += 12;

			break;

		case 15: //SUB

			mainReg[operand1] -= mainReg[operand2];

			mainReg[PC] += 12;

			break;

		case 16: //MUL

			mainReg[operand1] *= mainReg[operand2];

			mainReg[PC] += 12;

			break;

		case 17: //DIV

			mainReg[operand1] /= mainReg[operand2];

			mainReg[PC] += 12;

			break;

		case 20: //CMP

			if (mainReg[operand1] == mainReg[operand2]) {
				mainReg[operand1] = 0;
			}
			else if (mainReg[operand1] > mainReg[operand2]) {
				mainReg[operand1] = 1;
			}
			else if (mainReg[operand1] < mainReg[operand2]) {
				mainReg[operand1] = -1;
			}

			mainReg[PC] += 12;

			break;

		case 22: // STR indirect

			writeInt(mainReg[operand1], mainReg[operand2]);

			mainReg[PC] += 12;

			break;

		case 23: // LDR indirect

			mainReg[operand1] = readInt(mainReg[operand2]);

			mainReg[PC] += 12;

			break;

		case 24: // STB indirect

			writeChar(mainReg[operand1], mainReg[operand2]);


			mainReg[PC] += 12;

			break;

		case 25: // LDB indirect

			mainReg[operand1] = readChar(mainReg[operand2]);


			mainReg[PC] += 12;

			break;

		}
	}
}

//================================================================================================================================MAIN==================================================================================================================================================================

int main(int argc, char* argv[]) {

	if (argc != 2) {
		cout << "Please enter a .asm file";

		return 1;
	}

	string inputFile = argv[1]; // hardcodded remove in final build "proj2.asm" for argv[1]
	int pass = 1;
	assembler(inputFile, pass);
	pass++;
	assembler(inputFile, pass);

	virtualMachine();

	return 0;
}