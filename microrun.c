/* microrun.c

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
#include <X11/Xlib.h>
#include "headers/defines.h"
#include "headers/getParameters.h"
#include "headers/readConfig.h"
#include "headers/eventLoop.h"

extern const char *programName;
extern uint8_t mode;
extern Display *display;
extern unsigned int shortcutAmount;

int main(const int argumentCount, const char *const *const argumentVector){
	if(getParameters((unsigned int)argumentCount, argumentVector)){
		for(;;){
			mode = ContinueMode;
			if((display = XOpenDisplay(NULL))){
				readConfigScan();
				if(shortcutAmount){
					eventLoop();
				}else{
					fprintf(stderr, "%s: no shortcut specified\n", programName);
					mode = ExitMode;
				}
				XCloseDisplay(display);
			}else{
				fprintf(stderr, "%s: could not connect to server\n", programName);
				mode = ExitMode;
			}
			if(mode == ExitMode){
				break;
			}
		}
	}
	return 0;
}
