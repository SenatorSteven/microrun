/* eventLoop.c

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
#include <stdlib.h>
#include <X11/Xlib.h>
#include "headers/defines.h"
#include "headers/readConfig.h"

extern const char *programName;
extern uint8_t mode;
extern Display *display;
extern unsigned int maxCommandLength;
extern unsigned int shortcutAmount;

static void grabKeys(Shortcut *const shortcut);
static bool isCommand(const char *const command, const char *const vector);
static void ungrabKeys(Shortcut *const shortcut);

void eventLoop(void){
	Shortcut shortcut[shortcutAmount];
	char _command[shortcutAmount][maxCommandLength + 1];
	char *command[shortcutAmount];
	for(unsigned int currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
		command[currentShortcut] = _command[currentShortcut];
	}
	readConfigKeysCommands(shortcut, command);
	grabKeys(shortcut);
	XSelectInput(display, XDefaultRootWindow(display), KeyPressMask);
	{
		XEvent event;
		unsigned int currentShortcut;
		for(;;){
			XNextEvent(display, &event);
			if(event.type == KeyPress){
				for(currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
					if(event.xkey.keycode == shortcut[currentShortcut].keycode && event.xkey.state == shortcut[currentShortcut].masks){
						if(isCommand("restart", command[currentShortcut])){
							mode = RestartMode;
						}else if(isCommand("exit", command[currentShortcut])){
							mode = ExitMode;
						}else{
							system(command[currentShortcut]);
						}
						break;
					}
				}
				if(mode != ContinueMode){
					break;
				}
			}
		}
	}
	ungrabKeys(shortcut);
	return;
}
static bool isCommand(const char *const command, const char *const vector){
	bool value = 0;
	unsigned int element = 0;
	while(command[element] || vector[element]){
		if(vector[element] >= 'A' && vector[element] <= 'Z'){
			if(!(vector[element] == command[element] || vector[element] == command[element] + 32)){
				element = 0;
				break;
			}
		}else if(vector[element] >= 'a' && vector[element] <= 'z'){
			if(!(vector[element] == command[element] || vector[element] == command[element] - 32)){
				element = 0;
				break;
			}
		}else{
			if(!(vector[element] == command[element])){
				element = 0;
				break;
			}
		}
		++element;
	}
	if(element != 0){
		value = 1;
	}
	return value;
}
static void grabKeys(Shortcut *const shortcut){
	for(unsigned int currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
		if(shortcut[currentShortcut].keycode != AnyKey){
			XGrabKey(display, shortcut[currentShortcut].keycode, shortcut[currentShortcut].masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
		}
	}
	XSync(display, False);
}
static void ungrabKeys(Shortcut *const shortcut){
	for(unsigned int currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
		if(shortcut[currentShortcut].keycode != AnyKey){
			XUngrabKey(display, shortcut[currentShortcut].keycode, shortcut[currentShortcut].masks, XDefaultRootWindow(display));
		}
	}
}
