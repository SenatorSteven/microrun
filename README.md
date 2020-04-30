<!-- README.md

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
SOFTWARE. -->

# microrun
Keypress listener and startup command runner for the X Window System



## Features
- Extremely verbose and helpful default configuration
- Ability to execute startup commands e.g. "feh --bg-fill background.png"
- Ability to execute commands on keypress e.g. "firefox"
- Ability to restart after a configuration edit



## Installation
1. Download
2. Unzip
3. Go to the microrun-master directory
4. Allow "compile.sh" to execute as program
5. Execute "compile.sh", preferably via terminal
6. Done



## Configuration

Before compilation, while possible to edit all files, it is important to pay a visit to headers/defines.h. Through it, you can set the tab size, the amount of characters to read per line and the default number of lines to read.
<br>
<br>
The program name is read at runtime. The first argument passed to the program, that being the name used to execute it, will be the name of the program.



## Terminal help menu

#### $ microrun --help
<pre>
microrun: usage: microrun [parameters] or microrun [parameter] [--help]
    [-h], [--help]      display this message
    [-c], [--config]    specify path to config, necessary
</pre>

#### $ microrun --config --help
<pre>
microrun: usage: microrun --config "/path/to/file"
    if the specified file doesn't exist, it will be created
    and it will contain the hardcoded default configuration
</pre>
