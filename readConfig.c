/* readConfig.c

MIT License

Copyright (C) 2020 Stefanos "Steven" Tsakiris

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include "headers/defines.h"

#define NoVariables /*-----------------*/ 0
#define LinesVariable /*---------------*/ (1 << 0)
#define RestartVariable /*-------------*/ (1 << 1)
#define ExitVariable /*----------------*/ (1 << 2)

#define NoMathOperation /*-------------*/ 0
#define AdditionMathOperation /*-------*/ 1
#define SubtractionMathOperation /*----*/ 2
#define MultiplicationMathOperation /*-*/ 3
#define DivisionMathOperation /*-------*/ 4

extern const char *programName;
extern const char *configPath;
extern char line[DefaultCharactersCount + 1];
extern unsigned int maxCommandLength;
extern unsigned int shortcutAmount;

typedef uint8_t VariableList;
typedef uint8_t MathOperation;

static FILE *getConfigFile(void);
static bool getLine(FILE *const file);
static void pushWhitespace(unsigned int *const element);
static bool isVariable(const char *const variable, unsigned int *const element);
static unsigned int getUnsignedInteger(const unsigned int currentLine, unsigned int *const element);
static int getInteger(unsigned int *const element);
static unsigned int getQuotedStringLength(unsigned int *const element);
static unsigned int getQuotedString(char *const string, unsigned int *const element);
static Shortcut getShortcut(unsigned int *const element);
static void printLineError(const unsigned int currentLine);

bool readConfigScan(void){
	bool value = 0;
	maxCommandLength = 0;
	shortcutAmount = 0;
	FILE *const file = getConfigFile();
	if(file){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		unsigned int e;
		unsigned int length;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine(file)){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & LinesVariable)){
					if(isVariable("lines", &element)){
						pushWhitespace(&element);
						if(isVariable("=", &element)){
							pushWhitespace(&element);
							maxLinesCount = getUnsignedInteger(currentLine, &element);
							hasReadVariable |= LinesVariable;
						}
						continue;
					}
				}
				if(isVariable("onStart", &element)){
					pushWhitespace(&element);
					e = element;
					char command[getQuotedStringLength(&element) + 1];
					element = e;
					command[getQuotedString(command, &element)] = '\0';
					system(command);
					continue;
				}
				if(isVariable("keycode", &element)){
					pushWhitespace(&element);
					getShortcut(&element);
					if(isVariable("restart", &element)){
						if(!(hasReadVariable & RestartVariable)){
							length = 7;
							++shortcutAmount;
						}
					}else if(isVariable("exit", &element)){
						if(!(hasReadVariable & ExitVariable)){
							length = 4;
							++shortcutAmount;
						}
					}else{
						length = getQuotedStringLength(&element);
						++shortcutAmount;
					}
					if(length > maxCommandLength){
						maxCommandLength = length;
					}
					continue;
				}
				if(line[element]){
					printLineError(currentLine);
					continue;
				}
			}
		}
		fclose(file);
		value = 1;
	}
	return value;
}
bool readConfigKeysCommands(Shortcut *const s, char *const *const command){
	bool value = 0;
	unsigned int currentShortcut;
	for(currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
		s[currentShortcut].keycode = AnyKey;
		s[currentShortcut].masks = None;
		*command[currentShortcut] = '\0';
	}
	FILE *const file = getConfigFile();
	if(file){
		currentShortcut = 0;
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		VariableList hasReadVariable = NoVariables;
		char *currentCommand;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine(file)){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & LinesVariable)){
					if(isVariable("lines", &element)){
						pushWhitespace(&element);
						if(isVariable("=", &element)){
							pushWhitespace(&element);
							maxLinesCount = getUnsignedInteger(currentLine, &element);
							hasReadVariable |= LinesVariable;
						}
						continue;
					}
				}
				if(isVariable("keycode", &element)){
					pushWhitespace(&element);
					s[currentShortcut] = getShortcut(&element);
					if(isVariable("restart", &element)){
						if(!(hasReadVariable & RestartVariable)){
							currentCommand = command[currentShortcut];
							currentCommand[0] = 'r';
							currentCommand[1] = 'e';
							currentCommand[2] = 's';
							currentCommand[3] = 't';
							currentCommand[4] = 'a';
							currentCommand[5] = 'r';
							currentCommand[6] = 't';
							currentCommand[7] = '\0';
							++currentShortcut;
						}
					}else if(isVariable("exit", &element)){
						if(!(hasReadVariable & ExitVariable)){
							currentCommand = command[currentShortcut];
							currentCommand[0] = 'e';
							currentCommand[1] = 'x';
							currentCommand[2] = 'i';
							currentCommand[3] = 't';
							currentCommand[4] = '\0';
							++currentShortcut;
						}
					}else{
						currentCommand = command[currentShortcut];
						currentCommand[getQuotedString(currentCommand, &element)] = '\0';
						++currentShortcut;
					}
					continue;
				}
			}
		}
		fclose(file);
		value = 1;
	}
	return value;
}
static FILE *getConfigFile(void){
	FILE *config = fopen(configPath, "r");
	if(!config){
		if((config = fopen(configPath, "w"))){
			fprintf(config, "# configuration file for\n\n");
			fprintf(config, "# ╔═══════════════════════════════════════════════════════════════════════════════════════════════╗\n");
			fprintf(config, "# ║                                                                                               ║\n");
			fprintf(config, "# ║   #           #   #     # # #     # # # #       # # #     # # # #     #       #   #       #   ║\n");
			fprintf(config, "# ║   # #       # #   #   #       #   #       #   #       #   #       #   #       #   #       #   ║\n");
			fprintf(config, "# ║   #   #   #   #   #   #           #       #   #       #   #       #   #       #   # #     #   ║\n");
			fprintf(config, "# ║   #     #     #   #   #           # # # #     #       #   # # # #     #       #   #   #   #   ║\n");
			fprintf(config, "# ║   #     #     #   #   #           #   #       #       #   #   #       #       #   #     # #   ║\n");
			fprintf(config, "# ║   #     #     #   #   #       #   #     #     #       #   #     #     #       #   #       #   ║\n");
			fprintf(config, "# ║   #     #     #   #     # # #     #       #     # # #     #       #     # # #     #       #   ║\n");
			fprintf(config, "# ║                                                                                               ║\n");
			fprintf(config, "# ╚═══════════════════════════════════════════════════════════════════════════════════════════════╝\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# rules #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# certain values can be changed through the headers/defines.h of the program\'s source\n");
			fprintf(config, "# this file needs to be user-specified when launched\n");
			fprintf(config, "# max line character length is %u\n", DefaultCharactersCount);
			fprintf(config, "# comments are signified by a \'#\' at the beginning of the line\n");
			fprintf(config, "# one variable per line, followed by \'=\' and its value\n");
			fprintf(config, "# all spaces and tabs are ignored\n");
			fprintf(config, "# all variables are valued 0 or undefined by default unless stated otherwise\n");
			fprintf(config, "# all variables can be written with random capitalization\n");
			fprintf(config, "# text require the same quote character before and after it\n");
			fprintf(config, "# text quotation is variable, the first character is the quote character\n\n\n\n");
			fprintf(config, "# # # # # # #\n");
			fprintf(config, "# variables #\n");
			fprintf(config, "# # # # # # #\n\n");
			fprintf(config, "# lines, onStart, keycode\n\n\n\n");
			fprintf(config, "# # # # # # # # # # # #\n");
			fprintf(config, "# variable definition #\n");
			fprintf(config, "# # # # # # # # # # # #\n\n");
			fprintf(config, "# lines: config lines to be read\n");
			fprintf(config, "# onStart: command to be executed on program launch\n");
			fprintf(config, "# keycode: combination of keycode + modifiers used to execute command\n\n\n\n");
			fprintf(config, "# # # # #\n");
			fprintf(config, "# extra #\n");
			fprintf(config, "# # # # #\n\n");
			fprintf(config, "# lines: default %u\n", DefaultLinesCount);
			fprintf(config, "# onStart: does not take \'=\'\n");
			fprintf(config, "# keycode: does not take \'=\'\n");
			fprintf(config, "# keycode: modifiers: AnyModifier, Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, Mod5\n");
			fprintf(config, "# keycode: program commands: restart, exit\n");
			fprintf(config, "# math operands: +, -, *, /, (, )\n\n\n\n");
			fprintf(config, "# /config start # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fprintf(config, "lines = 67\n");
			fprintf(config, "# keycode 27 + Mod4 \"restart\"\n");
			fprintf(config, "# /config end # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
			fclose(config);
			config = fopen(configPath, "r");
		}else{
			fprintf(stderr, "%s: could not create config file\n", programName);
		}
	}
	return config;
}
static bool getLine(FILE *const file){
	bool value = 0;
	unsigned int element = 0;
	for(;;){
		line[element] = fgetc(file);
		if(line[element] == '\n' || feof(file)){
			line[element] = '\0';
			break;
		}else if(element < DefaultCharactersCount){
			++element;
		}
	}
	if(!feof(file)){
		value = 1;
	}
	return value;
}
static void pushWhitespace(unsigned int *const element){
	unsigned int e = *element;
	char l = line[e];
	while(l && (l == ' ' || l == '\t')){
		l = line[++e];
	}
	*element = e;
	return;
}
static bool isVariable(const char *const variable, unsigned int *const element){
	bool value = 0;
	unsigned int e = *element;
	unsigned int currentCharacter = 0;
	char l = line[e];
	char v = *variable;
	while(l && v){
		if(l >= 'A' && l <= 'Z'){
			if(l != v && l != v - 32){
				currentCharacter = 0;
				break;
			}
		}else if(l >= 'a' && l <= 'z'){
			if(l != v && l != v + 32){
				currentCharacter = 0;
				break;
			}
		}else if(l != v){
			currentCharacter = 0;
			break;
		}
		l = line[++e];
		v = variable[++currentCharacter];
	}
	if(currentCharacter){
		*element = e;
		value = 1;
	}
	return value;
}
static unsigned int getUnsignedInteger(const unsigned int currentLine, unsigned int *const element){
	unsigned int number = getInteger(element);
	if((int)number < 0){
		fprintf(stderr, "%s: line %u: %i is not an unsigned integer\n", programName, currentLine, (int)number);
		number = 0;
	}
	return number;
}
static int getInteger(unsigned int *const element){
	int number = 0;
	unsigned int e = *element;
	int numberRead = 0;
	int numberOperatedOn = 0;
	MathOperation operation = NoMathOperation;
	MathOperation lastOperation = NoMathOperation;
	while(line[e]){
		pushWhitespace(&e);
		if(line[e] >= '0' && line[e] <= '9'){
			numberRead *= 10;
			numberRead += line[e];
			numberRead -= 48;
			++e;
		}else if(line[e] == '('){
			++e;
			numberRead = getInteger(&e);
		}else if(line[e] == ')'){
			++e;
			break;
		}else if(line[e] == '+' || line[e] == '-' || line[e] == '*' || line[e] == '/'){
			if(number == 0 && numberRead == 0){
				if(line[e] == '/'){
					break;
				}
			}
			if(operation == AdditionMathOperation){
				if(line[e] != '*' && line[e] != '/'){
					if(numberOperatedOn == 0){
						number += numberRead;
					}else{
						if(lastOperation == AdditionMathOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionMathOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == SubtractionMathOperation){
				if(line[e] != '*' && line[e] != '/'){
					if(numberOperatedOn == 0){
						number -= numberRead;
					}else{
						if(lastOperation == AdditionMathOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionMathOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == MultiplicationMathOperation){
				if(numberOperatedOn == 0){
					number *= numberRead;
				}else{
					numberOperatedOn *= numberRead;
				}
				if(line[e] == '+' || line[e] == '-'){
					if(lastOperation == AdditionMathOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionMathOperation){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else if(operation == DivisionMathOperation){
				if(numberOperatedOn == 0){
					number /= numberRead;
				}else{
					numberOperatedOn /= numberRead;
				}
				if(line[e] == '+' || line[e] == '-'){
					if(lastOperation == AdditionMathOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionMathOperation){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else{
				if(number == 0){
					number = numberRead;
				}
			}
			if(line[e] == '+'){
				operation = AdditionMathOperation;
			}else if(line[e] == '-'){
				operation = SubtractionMathOperation;
			}else if(line[e] == '*'){
				operation = MultiplicationMathOperation;
			}else if(line[e] == '/'){
				operation = DivisionMathOperation;
			}
			numberRead = 0;
			++e;
		}else{
			break;
		}
	}
	if(operation == AdditionMathOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionMathOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionMathOperation){
				number -= numberOperatedOn;
			}
		}
		number += numberRead;
	}else if(operation == SubtractionMathOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionMathOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionMathOperation){
				number -= numberOperatedOn;
			}
		}
		number -= numberRead;
	}else if(operation == MultiplicationMathOperation){
		if(numberOperatedOn == 0){
			number *= numberRead;
		}else{
			numberOperatedOn *= numberRead;
			if(lastOperation == AdditionMathOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionMathOperation){
				number -= numberOperatedOn;
			}
		}
	}else if(operation == DivisionMathOperation){
		if(number > 0 || numberOperatedOn > 0){
			if(numberOperatedOn == 0){
				number /= numberRead;
			}else{
				numberOperatedOn /= numberRead;
				if(lastOperation == AdditionMathOperation){
					number += numberOperatedOn;
				}else if(lastOperation == SubtractionMathOperation){
					number -= numberOperatedOn;
				}
			}
		}
	}else{
		if(number == 0){
			number = numberRead;
		}
	}
	*element = e;
	return number;
}
static unsigned int getQuotedStringLength(unsigned int *const element){
	unsigned int length = 0;
	unsigned int e = *element;
	const char quotation = line[e];
	++e;
	while(line[e] != quotation && line[e]){
		++length;
		++e;
	}
	*element = e;
	return length;
}
static unsigned int getQuotedString(char *const string, unsigned int *const element){
	unsigned int currentCharacter = 0;
	unsigned int e = *element;
	const char quotation = line[e];
	++e;
	while(line[e] != quotation && line[e]){
		string[currentCharacter] = line[e];
		++currentCharacter;
		++e;
	}
	*element = e;
	return currentCharacter;
}
static Shortcut getShortcut(unsigned int *const element){
	Shortcut shortcut = {
		.keycode = AnyKey,
		.masks = None
	};
	unsigned int e = *element;
	bool lookingForValue = 1;
	while(line[e]){
		pushWhitespace(&e);
		if(lookingForValue){
			if(line[e] >= '0' && line[e] <= '9'){
				do{
					shortcut.keycode *= 10;
					shortcut.keycode += line[e];
					shortcut.keycode -= 48;
					++e;
				}while(line[e] >= '0' && line[e] <= '9');
			}else if(isVariable("AnyModifier", &e)){
				shortcut.masks |= AnyModifier;
			}else if(isVariable("Shift", &e)){
				shortcut.masks |= ShiftMask;
			}else if(isVariable("Lock", &e)){
				shortcut.masks |= LockMask;
			}else if(isVariable("Control", &e)){
				shortcut.masks |= ControlMask;
			}else if(isVariable("Mod1", &e)){
				shortcut.masks |= Mod1Mask;
			}else if(isVariable("Mod2", &e)){
				shortcut.masks |= Mod2Mask;
		 	}else if(isVariable("Mod3", &e)){
				shortcut.masks |= Mod3Mask;
			}else if(isVariable("Mod4", &e)){
				shortcut.masks |= Mod4Mask;
			}else if(isVariable("Mod5", &e)){
				shortcut.masks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(line[e] == '+'){
				++e;
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	if(shortcut.keycode != AnyKey){
		*element = e;
	}
	return shortcut;
}
static void printLineError(const unsigned int currentLine){
	unsigned int element = 0;
	pushWhitespace(&element);
	fprintf(stderr, "%s: line %u: \"%s\" not recognized as an internal variable\n", programName, currentLine, &line[element]);
	return;
}
