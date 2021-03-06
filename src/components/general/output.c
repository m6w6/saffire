/*
 Copyright (c) 2012-2013, The Saffire Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Saffire Group the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "general/printf.h"
#include "general/output.h"
#include "general/smm.h"


/**
 * Generic output handlers that write to stdout
 */
static int _stdio_output_char_helper(FILE *f, char c) {
    return fwrite(&c, 1, 1, f);
}
static int _stdio_output_string_helper(FILE *f, char *s) {
    return fwrite(s, strlen(s), 1, f);
}

int (*output_char_helper)(FILE *f, char c) = _stdio_output_char_helper;
int (*output_string_helper)(FILE *f, char *s) = _stdio_output_string_helper;



/**
 * Outputs to a specified file
 */
static void _output(FILE *f, const char *format, va_list args) {
    char *buf;

    if (args == NULL) {
        smm_asprintf(&buf, format, NULL);
    } else {
        smm_vasprintf(&buf, format, args);
    }
    output_string_helper(f, buf);
    smm_free(buf);
}



/**
 * Outputs (to stdout)
 */
void output(const char *format, ...) {
    va_list args;

    va_start(args, format);
    _output(stdout, format, args);
    va_end(args);
}

/**
 * Output warning (to stderr)
 */
void output_debug(const char *format, ...) {
    va_list args;

    va_start(args, format);
    _output(stderr, format, args);
    va_end(args);
}


/**
 * Output warning (to stderr)
 */
void warning(const char *format, ...) {
    va_list args;

    _output(stderr, "Warning: ", NULL);
    va_start(args, format);
    _output(stderr, format, args);
    va_end(args);

    fflush(stderr);
}

/**
 * Output error (to stderr)
 */
void error(const char *format, ...) {
    va_list args;

    _output(stderr, "Error: ", NULL);
    va_start(args, format);
    _output(stderr, format, args);
    va_end(args);

    fflush(stderr);
}


/**
 * Ouputs error (to stderr) and exists with code.
 */
void fatal_error(int exitcode, const char *format, ...) {
    va_list args;

    _output(stderr, "Fatal error: ", NULL);
    va_start(args, format);
    _output(stderr, format, args);
    va_end(args);

    fflush(stderr);

    exit(exitcode);
}


/**
 */
void output_printf(const char *format, t_dll *args) {
    arg_printf(stdout, format, args, output_char_helper);
}


/**
 *
 */
static int detect_terminal() {
    return isatty(fileno(stdout));
}

int is_tty = -1;

void output_ansi(char sequence[]) {
    if (is_tty == -1) {
        is_tty = detect_terminal();
    }

    if (! is_tty) return;

    output("\033[%s", sequence);
    fflush(stdout);
}


/**
 * @param char_helper
 * @param string_helper
 */
void output_set_helpers(int (*char_helper)(FILE *f, char c), int (*string_helper)(FILE *f, char *s)) {
    output_char_helper = char_helper;
    output_string_helper = string_helper;
}
