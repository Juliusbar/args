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

/*

gcc -g -c args.c
gcc -g arguments_example.c args.o -o arguments_example
OR
gcc -g arguments_example.c args.c -o arguments_example
*/

#include "args.h"

int arg_parsing(struct arg_results **R,int argc,char *argv[]){
	int ret;

/*
Type:
0 no parameter
1 can have parameter
3 required parameter
*/

	struct arg_params arguments[]={
		{0,'h',"help"},
		{0,'a',""},
		{0,0,"ab"},
		{1,'b',"cde"},
		{1,'c',""},
		{1,0,"fgh"},
		{2,'d',"ij"},
		{2,'e',""},
		{2,0,"klm"}
	};

	int arguments_n=sizeof arguments/sizeof(struct arg_params);

	ret=arguments_init(R,arguments_n,argc);
	if(ret) return ret;

	ret=argument_parse(argc,argv,arguments_n,arguments,*R);
	if(ret) return ret;

	argument_table(argc,argv,arguments_n,arguments,*R);

	ret=argument_error(argc,argv,*R);

	return ret;
}

int main(int argc, char *argv[]){
	int ret,exit_code=EXIT_SUCCESS;

	struct arg_results *R;

	ret=arg_parsing(&R,argc,argv);

	arguments_free(&R);

	return exit_code;
}
