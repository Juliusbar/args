/*
 * Copyright (c) 2019 Julius Barzdziukas <julius.barzdziukas@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef ARGS_H
#define ARGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define argument_s_max_size 100

struct arg_par{
	char *s;
	struct arg_par *next;
};

struct arg_err{
	char err;
	int pos;
	struct arg_err *next;
};

struct arg_params{
	char type,arg_c,arg_s[argument_s_max_size];
};

struct arg_results{
	int arg_n;
	int argc;
	char *arg_list;
	struct arg_par **parameters;
	struct arg_err **errors;
};

int add_element(struct arg_par **A,int N);

int add_element_next(struct arg_par **A,char *S,int n);

void free_element(struct arg_par **A,int N);

int add_err(struct arg_err **A,int N);

int add_err_next(struct arg_err **A,char E,int P);

void free_err(struct arg_err **A,int N);

int arguments_init(struct arg_results **R,int arg_n,int argc);

void arguments_free(struct arg_results **R);

int argument_parse(int argc,char **argv,int arguments_n,struct arg_params *arguments,struct arg_results *R);

void argument_table(int argc,char **argv,int arguments_n,struct arg_params *arguments,struct arg_results *R);

void print_arg_error(int i);

int argument_error(int argc,char **argv,struct arg_results *R);

#ifdef __cplusplus
}
#endif 

#endif
