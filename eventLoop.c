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
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include "headers/defines.h"
#include "headers/readConfig.h"

extern const char *programName;
extern Mode mode;
extern Display *display;
extern unsigned int maxCommandLength;
extern unsigned int shortcutAmount;

static void grabKeys(const Shortcut *const s);
static bool isCommand(const char *const command, const char *const vector);
static void ungrabKeys(const Shortcut *const s);

void eventLoop(void){
	XSelectInput(display, XDefaultRootWindow(display), KeyPressMask);
	Shortcut shortcut[shortcutAmount];
	char _command[shortcutAmount][maxCommandLength + 1];
	char *command[shortcutAmount];
	for(unsigned int currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
		command[currentShortcut] = _command[currentShortcut];
	}
	if(readConfigKeysCommands(shortcut, command)){
		grabKeys(shortcut);
	}else{
		fprintf(stderr, "%s: could not read shortcuts\n", programName);
	}
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
	char v = *vector;
	char c = *command;
	while(v || c){
		if(v >= 'A' && v <= 'Z'){
			if(v != c && v != c - 32){
				element = 0;
				break;
			}
		}else if(v >= 'a' && v <= 'z'){
			if(v != c && v != c + 32){
				element = 0;
				break;
			}
		}else if(v != c){
			element = 0;
			break;
		}
		++element;
		v = vector[element];
		c = command[element];
	}
	if(element){
		value = 1;
	}
	return value;
}
static void grabKeys(const Shortcut *const s){
	for(unsigned int currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
		if(s[currentShortcut].keycode != AnyKey){
			XGrabKey(display, s[currentShortcut].keycode, s[currentShortcut].masks, XDefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
		}
	}
	XSync(display, False);
	return;
}
static void ungrabKeys(const Shortcut *const s){
	for(unsigned int currentShortcut = 0; currentShortcut < shortcutAmount; ++currentShortcut){
		if(s[currentShortcut].keycode != AnyKey){
			XUngrabKey(display, s[currentShortcut].keycode, s[currentShortcut].masks, XDefaultRootWindow(display));
		}
	}
	XSync(display, False);
	return;
}
