/* Lydrige - A simple interpreted programming language inspired by Lisp.
 * Copyright (c) 2016, Christian Seibold
 * Under MIT License
 *
 * You can find latest source code at:
 * https://github.com/krixano/Lydrige
 *
 * -----------------------------------------------------------------------
 * MIT License
 *
 * Copyright (c) 2016 Christian Seibold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * -----------------------------------------------------------------------
 */

#ifndef COLORS_H_INCLUDE_GUARD
#define COLORS_H_INCLUDE_GUARD

#ifdef _WIN32
#include <windows.h>

#define FOREGROUND_YELLOW FOREGROUND_RED|FOREGROUND_GREEN
#define FOREGROUND_CYAN FOREGROUND_GREEN|FOREGROUND_BLUE
#define FOREGROUND_MAGENTA FOREGROUND_RED|FOREGROUND_BLUE
#define FOREGROUND_WHITE FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE

#define BACKGROUND_YELLOW BACKGROUND_RED|BACKGROUND_GREEN
#define BACKGROUND_CYAN BACKGROUND_GREEN|BACKGROUND_BLUE
#define BACKGROUND_MAGENTA BACKGROUND_RED|BACKGROUND_BLUE
#define BACKGROUND_WHITE BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE

#endif

#define COL_RED "\x1b[31m" // Error
#define COL_GREEN "\x1b[32m" // Prompt, Success?
#define COL_YELLOW "\x1b[33m" // Function and Variable Names?
#define COL_BLUE "\x1b[34m"
#define COL_MAGENTA "\x1b[35m" // Function args
#define COL_CYAN "\x1b[36m" // Information
#define COL_RESET "\x1b[0m" // Statements, etc.

typedef enum COLOR
{
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_WHITE,
    COLOR_BLACK
} COLOR;

#ifdef _WIN32
HANDLE hConsole; // Used for coloring output on Windows
#endif

void setColor(COLOR foreground);
void resetColor(void);
void colors_printf(COLOR foreground, const char *fmt, ...);
void colors_puts(COLOR foreground, const char *fmt, ...);

#endif