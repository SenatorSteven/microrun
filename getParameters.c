/* getParameters.c

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

#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include "headers/defines.h"

#define NoPositions /*----*/ 0
#define ConfigPosition /*-*/ (1 << 0)
#define HelpPosition /*---*/ (1 << 1)
#define ExitPosition /*---*/ (1 << 2)

extern const char *programName;
extern const char *configPath;

static bool isArgument(const char *const argument, const char *const vector);

bool getParameters(const unsigned int argumentCount, const char *const *const argumentVector){
	bool value = 0;
	programName = argumentVector[0];
	if(argumentCount > 1){
		const char *currentArgumentVector;
		uint8_t hasReadVariable = NoPositions;
		DIR *dir;
		FILE *file;
		for(unsigned int currentArgument = 1; currentArgument < argumentCount; ++currentArgument){
			currentArgumentVector = argumentVector[currentArgument];
			if(!(hasReadVariable & ConfigPosition)){
				if(isArgument("-c", currentArgumentVector) || isArgument("--config", currentArgumentVector)){
					hasReadVariable |= ConfigPosition;
					if(++currentArgument < argumentCount){
						currentArgumentVector = argumentVector[currentArgument];
						if(isArgument("-h", currentArgumentVector) || isArgument("--help", currentArgumentVector)){
							fprintf(stdout, "%s: usage: %s --config \"/path/to/file\"\n", programName, programName);
							fprintf(stdout, "%sif the specified file doesn't exist, it will be created\n%sand it will contain the hardcoded default configuration\n", Tab, Tab);
							hasReadVariable |= HelpPosition;
							break;
						}else if(isArgument("-c", currentArgumentVector) || isArgument("--config", currentArgumentVector)){
							fprintf(stderr, "%s: no config value specified\n", programName);
							hasReadVariable |= ExitPosition;
							break;
						}else{
							configPath = currentArgumentVector;
							if((dir = opendir(configPath))){
								closedir(dir);
								fprintf(stderr, "%s: \"%s\" config value is directory\n", programName, configPath);
								hasReadVariable |= ExitPosition;
								break;
							}else if((file = fopen(configPath, "r"))){
								fclose(file);
								continue;
							}else if((file = fopen(configPath, "w"))){
								fclose(file);
								remove(configPath);
								continue;
							}else{
								fprintf(stderr, "%s: could not create config file\n", programName);
								hasReadVariable |= ExitPosition;
								break;
							}
						}
					}else{
						fprintf(stderr, "%s: no config value specified\n", programName);
						hasReadVariable |= ExitPosition;
						break;
					}
				}
			}
			if(isArgument("-h", currentArgumentVector) || isArgument("--help", currentArgumentVector)){
				fprintf(stdout, "%s: usage: %s [parameters] or %s [parameter] [--help]\n", programName, programName, programName);
				fprintf(stdout, "%s[-h], [--help]  %sdisplay this message\n", Tab, Tab);
				fprintf(stdout, "%s[-c], [--config]%sspecify path to config, necessary\n", Tab, Tab);
				hasReadVariable |= HelpPosition;
				break;
			}else if(isArgument("-c", currentArgumentVector) || isArgument("--config", currentArgumentVector)){
				fprintf(stderr, "%s: the config parameter has already been specified\n", programName);
			}else{
				fprintf(stderr, "%s: \"%s\" is not recognized as program parameter, check help? [-h]\n", programName, currentArgumentVector);
			}
			hasReadVariable |= ExitPosition;
			break;
		}
		if(!(hasReadVariable & HelpPosition)){
			if(hasReadVariable & ConfigPosition){
				if(!(hasReadVariable & ExitPosition)){
					value = 1;
				}
			}else{
				fprintf(stderr, "%s: no config parameter specified\n", programName);
			}
		}
	}else{
		fprintf(stderr, "%s: no config parameter specified\n", programName);
	}
	return value;
}
static bool isArgument(const char *const argument, const char *const vector){
	bool value = 0;
	unsigned int element = 0;
	char a = argument[element];
	char v = vector[element];
	while(a || v){
		if((v >= 'A' && v <= 'Z' && v != a && v != a + 32) || (v >= 'a' && v <= 'z' && v != a && v != a - 32) || v != a){
			element = 0;
			break;
		}
		++element;
		a = argument[element];
		v = vector[element];
	}
	if(element){
		value = 1;
	}
	return value;
}
