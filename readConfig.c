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

#define NoPositions /*-------------*/ 0
#define LinesPosition /*-----------*/ (1 << 0)

#define NoOperation /*-------------*/ 0
#define AdditionOperation /*-------*/ 1
#define SubtractionOperation /*----*/ 2
#define MultiplicationOperation /*-*/ 3
#define DivisionOperation /*-------*/ 4

extern const char *programName;
extern const char *configPath;
extern char line[DefaultCharactersCount + 1];
extern unsigned int maxCommandLength;
extern unsigned int shortcutAmount;

static FILE *getConfigFile(void);
static bool getLine(FILE *const file);
static void pushWhitespace(unsigned int *const element);
static bool isVariable(const char *const variable, unsigned int *const element);
static unsigned int getUnsignedIntegerNumber(const unsigned int currentLine, unsigned int *const element);
static int getIntegerNumber(unsigned int *const element);
static unsigned int getQuotedStringLength(unsigned int *const element);
static unsigned int getQuotedString(char *const string, unsigned int *const element);
static bool getKey(unsigned int *const element, unsigned int *const keycode, uint16_t *const masks);
static void printLineError(const unsigned int currentLine);

bool readConfigScan(void){
	bool value = 0;
	FILE *const file = getConfigFile();
	if(file){
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint8_t hasReadVariable = NoPositions;
		unsigned int startingPoint;
		unsigned int keycode;
		uint16_t masks;
		unsigned int length;
		maxCommandLength = 0;
		shortcutAmount = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine(file)){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & LinesPosition)){
					if(isVariable("lines", &element)){
						pushWhitespace(&element);
						if(isVariable("=", &element)){
							pushWhitespace(&element);
							maxLinesCount = getUnsignedIntegerNumber(currentLine, &element);
							hasReadVariable |= LinesPosition;
						}
						continue;
					}
				}
				if(isVariable("onStart", &element)){
					pushWhitespace(&element);
					startingPoint = element;
					char command[getQuotedStringLength(&element) + 1];
					element = startingPoint;
					command[getQuotedString(command, &element)] = '\0';
					system(command);
					continue;
				}
				if(isVariable("keycode", &element)){
					pushWhitespace(&element);
					getKey(&element, &keycode, &masks);
					length = getQuotedStringLength(&element);
					if(length > maxCommandLength){
						maxCommandLength = length;
					}
					++shortcutAmount;
					continue;
				}
				if(line[element]){
					if(!isVariable("lines",   &element) &&
					   !isVariable("onStart", &element) &&
					   !isVariable("keycode", &element)){
						printLineError(currentLine);
						continue;
					}
				}
			}
		}
		fclose(file);
		value = 1;
	}
	return value;
}
bool readConfigKeysCommands(Shortcut *const shortcut, char *const *const command){
	bool value = 0;
	FILE *const file = getConfigFile();
	if(file){
		unsigned int currentShortcut;
		for(currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
			shortcut[currentShortcut].keycode = AnyKey;
			shortcut[currentShortcut].masks = None;
			command[currentShortcut][0] = '\0';
		}
		unsigned int maxLinesCount = DefaultLinesCount;
		unsigned int element;
		uint8_t hasReadVariable = NoPositions;
		currentShortcut = 0;
		for(unsigned int currentLine = 1; currentLine <= maxLinesCount; ++currentLine){
			if(!getLine(file)){
				break;
			}
			element = 0;
			pushWhitespace(&element);
			if(!isVariable("#", &element)){
				if(!(hasReadVariable & LinesPosition)){
					if(isVariable("lines", &element)){
						pushWhitespace(&element);
						if(isVariable("=", &element)){
							pushWhitespace(&element);
							maxLinesCount = getUnsignedIntegerNumber(currentLine, &element);
							hasReadVariable |= LinesPosition;
						}
						continue;
					}
				}
				if(isVariable("keycode", &element)){
					pushWhitespace(&element);
					getKey(&element, &shortcut[currentShortcut].keycode, &shortcut[currentShortcut].masks);
					command[currentShortcut][getQuotedString(command[currentShortcut], &element)] = '\0';
					++currentShortcut;
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
	unsigned int dereferencedElement = *element;
	while(line[dereferencedElement] && (line[dereferencedElement] == ' ' || line[dereferencedElement] == '	')){
		++dereferencedElement;
	}
	if(dereferencedElement > *element){
		*element = dereferencedElement;
	}
	return;
}
static bool isVariable(const char *const variable, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	bool value = 0;
	unsigned int currentCharacter = 0;
	while(variable[currentCharacter] && line[dereferencedElement]){
		if(variable[currentCharacter] >= 'A' && variable[currentCharacter] <= 'Z'){
			if(!(line[dereferencedElement] == variable[currentCharacter] || line[dereferencedElement] == variable[currentCharacter] + 32)){
				currentCharacter = 0;
				break;
			}
		}else if(variable[currentCharacter] >= 'a' && variable[currentCharacter] <= 'z'){
			if(!(line[dereferencedElement] == variable[currentCharacter] || line[dereferencedElement] == variable[currentCharacter] - 32)){
				currentCharacter = 0;
				break;
			}
		}else{
			if(!(line[dereferencedElement] == variable[currentCharacter])){
				currentCharacter = 0;
				break;
			}
		}
		++dereferencedElement;
		++currentCharacter;
	}
	if(currentCharacter != 0){
		*element = dereferencedElement;
		value = 1;
	}
	return value;
}
static unsigned int getUnsignedIntegerNumber(const unsigned int currentLine, unsigned int *const element){
	unsigned int number = getIntegerNumber(element);
	if((int)number < 0){
		fprintf(stderr, "%s: line %u: %i is not an unsigned integer\n", programName, currentLine, (int)number);
		number = 0;
	}
	return number;
}
static int getIntegerNumber(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	int number = 0;
	int numberRead = 0;
	int numberOperatedOn = 0;
	uint8_t operation = NoOperation;
	uint8_t lastOperation = NoOperation;
	while(line[dereferencedElement]){
		pushWhitespace(&dereferencedElement);
		if(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9'){
			numberRead *= 10;
			numberRead += line[dereferencedElement];
			numberRead -= 48;
			++dereferencedElement;
		}else if(line[dereferencedElement] == '('){
			++dereferencedElement;
			numberRead = getIntegerNumber(&dereferencedElement);
		}else if(line[dereferencedElement] == ')'){
			++dereferencedElement;
			break;
		}else if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-' || line[dereferencedElement] == '*' || line[dereferencedElement] == '/'){
			if(number == 0 && numberRead == 0){
				if(line[dereferencedElement] == '/'){
					break;
				}
			}
			if(operation == AdditionOperation){
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
					if(numberOperatedOn == 0){
						number += numberRead;
					}else{
						if(lastOperation == AdditionOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == SubtractionOperation){
				if(line[dereferencedElement] != '*' && line[dereferencedElement] != '/'){
					if(numberOperatedOn == 0){
						number -= numberRead;
					}else{
						if(lastOperation == AdditionOperation){
							number += numberOperatedOn;
						}else if(lastOperation == SubtractionOperation){
							number -= numberOperatedOn;
						}
					}
				}else{
					numberOperatedOn = numberRead;
					lastOperation = operation;
				}
			}else if(operation == MultiplicationOperation){
				if(numberOperatedOn == 0){
					number *= numberRead;
				}else{
					numberOperatedOn *= numberRead;
				}
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
					if(lastOperation == AdditionOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionOperation){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else if(operation == DivisionOperation){
				if(numberOperatedOn == 0){
					number /= numberRead;
				}else{
					numberOperatedOn /= numberRead;
				}
				if(line[dereferencedElement] == '+' || line[dereferencedElement] == '-'){
					if(lastOperation == AdditionOperation){
						number += numberOperatedOn;
					}else if(lastOperation == SubtractionOperation){
						number -= numberOperatedOn;
					}
					numberOperatedOn = 0;
				}
			}else{
				if(number == 0){
					number = numberRead;
				}
			}
			if(line[dereferencedElement] == '+'){
				operation = AdditionOperation;
			}else if(line[dereferencedElement] == '-'){
				operation = SubtractionOperation;
			}else if(line[dereferencedElement] == '*'){
				operation = MultiplicationOperation;
			}else if(line[dereferencedElement] == '/'){
				operation = DivisionOperation;
			}
			numberRead = 0;
			++dereferencedElement;
		}else{
			break;
		}
	}
	if(operation == AdditionOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionOperation){
				number -= numberOperatedOn;
			}
		}
		number += numberRead;
	}else if(operation == SubtractionOperation){
		if(numberOperatedOn > 0){
			if(lastOperation == AdditionOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionOperation){
				number -= numberOperatedOn;
			}
		}
		number -= numberRead;
	}else if(operation == MultiplicationOperation){
		if(numberOperatedOn == 0){
			number *= numberRead;
		}else{
			numberOperatedOn *= numberRead;
			if(lastOperation == AdditionOperation){
				number += numberOperatedOn;
			}else if(lastOperation == SubtractionOperation){
				number -= numberOperatedOn;
			}
		}
	}else if(operation == DivisionOperation){
		if(number > 0 || numberOperatedOn > 0){
			if(numberOperatedOn == 0){
				number /= numberRead;
			}else{
				numberOperatedOn /= numberRead;
				if(lastOperation == AdditionOperation){
					number += numberOperatedOn;
				}else if(lastOperation == SubtractionOperation){
					number -= numberOperatedOn;
				}
			}
		}
	}else{
		if(number == 0){
			number = numberRead;
		}
	}
	*element = dereferencedElement;
	return number;
}
static unsigned int getQuotedStringLength(unsigned int *const element){
	unsigned int dereferencedElement = *element;
	unsigned int length = 0;
	const char quotation = line[dereferencedElement];
	++dereferencedElement;
	while(line[dereferencedElement] != quotation && line[dereferencedElement]){
		++length;
		++dereferencedElement;
	}
	*element = dereferencedElement;
	return length;
}
static unsigned int getQuotedString(char *const string, unsigned int *const element){
	unsigned int dereferencedElement = *element;
	unsigned int currentCharacter = 0;
	const char quotation = line[dereferencedElement];
	++dereferencedElement;
	while(line[dereferencedElement] != quotation && line[dereferencedElement]){
		string[currentCharacter] = line[dereferencedElement];
		++currentCharacter;
		++dereferencedElement;
	}
	*element = dereferencedElement;
	return currentCharacter;
}
static bool getKey(unsigned int *const element, unsigned int *const keycode, uint16_t *const masks){
	unsigned int dereferencedElement = *element;
	unsigned int dereferencedKeycode = AnyKey;
	uint16_t dereferencedMasks = None;
	bool value = 0;
	bool lookingForValue = 1;
	while(line[dereferencedElement]){
		pushWhitespace(&dereferencedElement);
		if(lookingForValue){
			if(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9'){
				do{
					dereferencedKeycode *= 10;
					dereferencedKeycode += line[dereferencedElement];
					dereferencedKeycode -= 48;
					++dereferencedElement;
				}while(line[dereferencedElement] >= '0' && line[dereferencedElement] <= '9');
			}else if(isVariable("AnyModifier", &dereferencedElement)){
				dereferencedMasks |= AnyModifier;
			}else if(isVariable("Shift", &dereferencedElement)){
				dereferencedMasks |= ShiftMask;
			}else if(isVariable("Lock", &dereferencedElement)){
				dereferencedMasks |= LockMask;
			}else if(isVariable("Control", &dereferencedElement)){
				dereferencedMasks |= ControlMask;
			}else if(isVariable("Mod1", &dereferencedElement)){
				dereferencedMasks |= Mod1Mask;
			}else if(isVariable("Mod2", &dereferencedElement)){
				dereferencedMasks |= Mod2Mask;
			}else if(isVariable("Mod3", &dereferencedElement)){
				dereferencedMasks |= Mod3Mask;
			}else if(isVariable("Mod4", &dereferencedElement)){
				dereferencedMasks |= Mod4Mask;
			}else if(isVariable("Mod5", &dereferencedElement)){
				dereferencedMasks |= Mod5Mask;
			}else{
				break;
			}
			lookingForValue = 0;
		}else{
			if(line[dereferencedElement] == '+'){
				++dereferencedElement;
				lookingForValue = 1;
			}else{
				break;
			}
		}
	}
	if(dereferencedKeycode != AnyKey){
		*element = dereferencedElement;
		*keycode = dereferencedKeycode;
		*masks = dereferencedMasks;
		value = 1;
	}
	return value;
}
static void printLineError(const unsigned int currentLine){
	unsigned int element = 0;
	fprintf(stderr, "%s: line %u: \"", programName, currentLine);
	while(line[element]){
		fprintf(stderr, "%c", line[element]);
		++element;
	}
	fprintf(stderr, "\" not recognized as an internal variable\n");
	return;
}
