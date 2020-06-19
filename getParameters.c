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

#define NoParameters /*----*/ 0
#define ConfigParameter /*-*/ (1 << 0)
#define HelpParameter /*---*/ (1 << 1)
#define ExitParameter /*---*/ (1 << 2)

extern const char *programName;
extern const char *configPath;

typedef uint8_t ParameterList;

static bool isParameter(const char *const parameter, const char *const vector);

bool getParameters(const unsigned int parameterCount, const char *const *const parameterVector){
	bool value = 0;
	programName = *parameterVector;
	if(parameterCount > 1){
		const char *currentParameterVector;
		ParameterList hasReadParameter = NoParameters;
		DIR *dir;
		FILE *file;
		for(unsigned int currentParameter = 1; currentParameter < parameterCount; ++currentParameter){
			currentParameterVector = parameterVector[currentParameter];
			if(!(hasReadParameter & ConfigParameter)){
				if(isParameter("-c", currentParameterVector) || isParameter("--config", currentParameterVector)){
					hasReadParameter |= ConfigParameter;
					if(++currentParameter < parameterCount){
						currentParameterVector = parameterVector[currentParameter];
						if(isParameter("-h", currentParameterVector) || isParameter("--help", currentParameterVector)){
							fprintf(stdout, "%s: usage: %s --config \"/path/to/file\"\n", programName, programName);
							fprintf(stdout, "%sif the specified file doesn't exist, it will be created\n%sand it will contain the hardcoded default configuration\n", Tab, Tab);
							hasReadParameter |= HelpParameter;
							break;
						}else if(isParameter("-c", currentParameterVector) || isParameter("--config", currentParameterVector)){
							fprintf(stderr, "%s: no config value specified\n", programName);
							hasReadParameter |= ExitParameter;
							break;
						}else{
							configPath = currentParameterVector;
							if((dir = opendir(configPath))){
								closedir(dir);
								fprintf(stderr, "%s: \"%s\" config value is directory\n", programName, configPath);
								hasReadParameter |= ExitParameter;
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
								hasReadParameter |= ExitParameter;
								break;
							}
						}
					}else{
						fprintf(stderr, "%s: no config value specified\n", programName);
						hasReadParameter |= ExitParameter;
						break;
					}
				}
			}
			if(isParameter("-h", currentParameterVector) || isParameter("--help", currentParameterVector)){
				fprintf(stdout, "%s: usage: %s [parameters] or %s [parameter] [--help]\n", programName, programName, programName);
				fprintf(stdout, "%s[-h], [--help]  %sdisplay this message\n", Tab, Tab);
				fprintf(stdout, "%s[-c], [--config]%sspecify path to config, necessary\n", Tab, Tab);
				hasReadParameter |= HelpParameter;
				break;
			}else if(isParameter("-c", currentParameterVector) || isParameter("--config", currentParameterVector)){
				fprintf(stderr, "%s: the config parameter has already been specified\n", programName);
			}else{
				fprintf(stderr, "%s: \"%s\" is not recognized as program parameter, check help? [-h]\n", programName, currentParameterVector);
			}
			hasReadParameter |= ExitParameter;
			break;
		}
		if(!(hasReadParameter & HelpParameter)){
			if(hasReadParameter & ConfigParameter){
				if(!(hasReadParameter & ExitParameter)){
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
static bool isParameter(const char *const parameter, const char *const vector){
	bool value = 0;
	unsigned int element = 0;
	char p = *parameter;
	char v = *vector;
	while(p || v){
		if(v >= 'A' && v <= 'Z'){
			if(v != p && v != p - 32){
				element = 0;
				break;
			}
		}else if(v >= 'a' && v <= 'z'){
			if(v != p && v != p + 32){
				element = 0;
				break;
			}
		}else if(v != p){
			element = 0;
			break;
		}
		++element;
		p = parameter[element];
		v = vector[element];
	}
	if(element){
		value = 1;
	}
	return value;
}
