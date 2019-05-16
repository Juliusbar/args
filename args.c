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

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "args.h"

#define ALLOC(p,n) ((p)=malloc(sizeof(*(p))*(n)))

int add_element(struct arg_par **A,int N){
	int i;
	for(i=0;i<N;i++) A[i]=0;
	for(i=0;i<N;i++){
		ALLOC(A[i],1);
		if(!A[i]) return 1;
		A[i]->next=0;
		A[i]->s=0;
	}
	return 0;
}

int add_element_next(struct arg_par **A,char *S,int n){
	ALLOC((*A)->s,n);
	if(!(*A)->s) return 1;
	strncpy((*A)->s,S,n);
	ALLOC((*A)->next,1);
	if(!(*A)->next) return 2;
	*A=(*A)->next;
	(*A)->next=0;
	(*A)->s=0;
	return 0;
}

void free_element(struct arg_par **A,int N){
	struct arg_par *T;
	int i;
	for(i=0;i<N;i++){
		while(A[i]){
			if(A[i]->s) free(A[i]->s);
			T=A[i];
			A[i]=A[i]->next;
			free(T);
		}
	}
}

int add_err(struct arg_err **A,int N){
	int i;
	for(i=0;i<N;i++) A[i]=0;
	for(i=0;i<N;i++){
		ALLOC(A[i],1);
		if(!A[i]) return 1;
		A[i]->next=0;
		A[i]->err=0;
		A[i]->pos=0;
	}
	return 0;
}

int add_err_next(struct arg_err **A,char E,int P){
	if((*A)->err==0){
		(*A)->err=E;
		(*A)->pos=P;
		return 0;
	}else{
		ALLOC((*A)->next,1);
		if(!(*A)->next) return 1;
		*A=(*A)->next;
		(*A)->next=0;
		(*A)->err=E;
		(*A)->pos=P;
		return 0;
	}
}

void free_err(struct arg_err **A,int N){
	struct arg_err *T;
	int i;
	for(i=0;i<N;i++){
		while(A[i]){
			T=A[i];
			A[i]=A[i]->next;
			free(T);
		}
	}
}

int arguments_init(struct arg_results **R,int arg_n,int argc){
	int ret;

	ALLOC(*R,1);
	if(!*R) return 1;

	(*R)->arg_n=arg_n;
	(*R)->argc=argc;

	(*R)->parameters=0;
	(*R)->errors=0;

	ALLOC((*R)->arg_list,arg_n);
	if(!(*R)->arg_list) return 2;

	ALLOC((*R)->parameters,arg_n);
	if(!(*R)->parameters) return 3;
	ret=add_element((*R)->parameters,arg_n);
	if(ret) return 4;

	ALLOC((*R)->errors,argc);
	if(!(*R)->errors) return 5;
	ret=add_err((*R)->errors,argc);
	if(ret) return 6;

	return 0;
}

void arguments_free(struct arg_results **R){
	if(*R){
		if((*R)->parameters){
			free_element((*R)->parameters,(*R)->arg_n);
			free((*R)->parameters);
			(*R)->parameters=0;
		}

		if((*R)->errors){
			free_err((*R)->errors,(*R)->argc);
			free((*R)->errors);
			(*R)->errors=0;
		}

		if((*R)->arg_list){
			free((*R)->arg_list);
			(*R)->arg_list=0;
		}

		free(*R);
		*R=0;

	}
}

int argument_parse(int argc,char **argv,int arguments_n,struct arg_params *arguments,struct arg_results *R){
	int N,i,j,n1,arguments_s_n[arguments_n],parameter=0,ret,exit_code=0;
	char cont,cont1;

	struct arg_par **arguments_par_s;
	struct arg_err **argument_err;

	for(i=0;i<arguments_n;i++) R->arg_list[i]=0;

	ALLOC(arguments_par_s,arguments_n);
	if(!arguments_par_s){
		exit_code=1;
		goto err1;
	}
	ALLOC(argument_err,argc);
	if(!argument_err){
		exit_code=2;
		goto err2;
	}

	for(i=0;i<arguments_n;i++) arguments_par_s[i]=R->parameters[i];
	for(i=0;i<argc;i++) argument_err[i]=R->errors[i];

	for(j=0;(j<arguments_n)&&(arguments[j].arg_s);j++) arguments_s_n[j]=strlen(arguments[j].arg_s);

	for(N=1;N<argc;N++){
		n1=strlen(argv[N]);
		if(parameter){
			ret=add_element_next(arguments_par_s+parameter,argv[N],n1+1);
			if(ret){
				exit_code=3;
				goto err3;
			}
			parameter=0;
		}else{
			if((n1>0)&&(argv[N][0]=='-')){
				if(n1>1){
					if(argv[N][1]=='-'){
						if(n1>2){
							cont=1;
							for(j=0;cont&&(j<arguments_n);j++){
								if(arguments_s_n[j]){
									cont=strncmp(argv[N]+2,arguments[j].arg_s,arguments_s_n[j]);
									if(!cont){
										if(argv[N][2+arguments_s_n[j]]==0){
											if(arguments[j].type==0){
												R->arg_list[j]++;
											}else if(arguments[j].type==1){
												R->arg_list[j]++;
												ret=add_element_next(arguments_par_s+j,"",1);
												if(ret){
													exit_code=4;
													goto err3;
												}
											}else if(arguments[j].type==2){ //argument without required parameter
												ret=add_err_next(argument_err+N,1,1+arguments_s_n[j]);
												if(ret){
													exit_code=5;
													goto err3;
												}
											}else{ //wrong type
												ret=add_err_next(argument_err+N,2,2);
												if(ret){
													exit_code=6;
													goto err3;
												}
											}
										}else if(argv[N][2+arguments_s_n[j]]=='='){
											if(arguments[j].type==1){
												R->arg_list[j]++;
												ret=add_element_next(arguments_par_s+j,argv[N]+3+arguments_s_n[j],n1-arguments_s_n[j]-2);
												if(ret){
													exit_code=7;
													goto err3;
												}
											}else if(arguments[j].type==2){
												if((3+arguments_s_n[j])<n1){
													R->arg_list[j]++;
													ret=add_element_next(arguments_par_s+j,argv[N]+3+arguments_s_n[j],n1-arguments_s_n[j]-2);
													if(ret){
														exit_code=8;
														goto err3;
													}
												}else{ //argument without required parameter
													ret=add_err_next(argument_err+N,3,2+arguments_s_n[j]);
													if(ret){
														exit_code=9;
														goto err3;
													}
												}
											}else if(arguments[j].type==0){ // should not be parameter
												ret=add_err_next(argument_err+N,4,2+arguments_s_n[j]);
												if(ret){
													exit_code=10;
													goto err3;
												}
											}else{ //wrong type
												ret=add_err_next(argument_err+N,5,2);
												if(ret){
													exit_code=11;
													goto err3;
												}
											}
										}else{ //parameter not found
											cont=1;
										}
									}
								}
							}
							if(cont){ //parameter not found
								ret=add_err_next(argument_err+N,6,2);
								if(ret){
									exit_code=12;
									goto err3;
								}
							}
						}else{ //empty parameter list '--'
							ret=add_err_next(argument_err+N,7,0);
							if(ret){
								exit_code=13;
								goto err3;
							}
						}
					}else{
						cont1=1;
						for(i=1;cont1&&(i<n1);i++){
							cont=1;
							for(j=0;cont&&(j<arguments_n);j++){
								if(arguments[j].arg_c){
									cont=argv[N][i]!=arguments[j].arg_c;
									if(!cont){
										if(arguments[j].type==0){
											R->arg_list[j]++;
										}else if((arguments[j].type==1)||(arguments[j].type==2)){
											if((i+1)<n1){
												R->arg_list[j]++;
												ret=add_element_next(arguments_par_s+j,argv[N]+i+1,n1-i);
												if(ret){
													exit_code=14;
													goto err3;
												}
												cont1=0;
											}else{
												if(((N+1)<argc)){
													R->arg_list[j]++;
													parameter=j;
												}else if(arguments[j].type==1){
													R->arg_list[j]++;
													ret=add_element_next(arguments_par_s+j,"",1);
													if(ret){
														exit_code=15;
														goto err3;
													}
												}else{ //not enough parameters
													ret=add_err_next(argument_err+N,8,i);
													if(ret){
														exit_code=16;
														goto err3;
													}
												}
											}
										}else{ //wrong type
											ret=add_err_next(argument_err+N,9,i);
											if(ret){
												exit_code=17;
												goto err3;
											}
										}
									}
								}
							}
							if(cont){ //parameter not found
								ret=add_err_next(argument_err+N,10,i);
								if(ret){
									exit_code=18;
									goto err3;
								}
							}
						}
					}
				}else{ //empty parameter list '-'
					ret=add_err_next(argument_err+N,11,0);
					if(ret){
						exit_code=19;
						goto err3;
					}
				}
			}else{ //not a parameter
				ret=add_err_next(argument_err+N,12,0);
				if(ret){
					exit_code=20;
					goto err3;
				}
			}
		}
	}
err3:
	free(arguments_par_s);
err2:
	free(argument_err);
err1:
	return exit_code;
}

void argument_table(int argc,char **argv,int arguments_n,struct arg_params *arguments,struct arg_results *R){
	int i;
	struct arg_err *argument_errors;
	struct arg_par *arg_parameters;

	printf("i\tType\tArg1\tArg2\tN\tParams\n");
	for(i=0;i<arguments_n;i++){
		printf("%d\t",i);
		printf("%i\t%c\t%s",arguments[i].type,arguments[i].arg_c,arguments[i].arg_s);
		printf("\t");
		printf("%i",R->arg_list[i]);
		arg_parameters=R->parameters[i];
		while(arg_parameters){
			printf("\t");
			if(arg_parameters->s) printf("%s",arg_parameters->s);
			arg_parameters=arg_parameters->next;
		}
		printf("\n");
	}

	printf("\ni\tArg\tERR\tPosition\n");
	for(i=0;i<argc;i++){
		printf("%i\t%s",i,argv[i]);
		argument_errors=R->errors[i];
		while(argument_errors){
			printf("\t%i\t%i",argument_errors->err,argument_errors->pos);
			argument_errors=argument_errors->next;
		}
		printf("\n");
	}
	printf("\n");

	return;
}

void print_arg_error(int i){
	if(i==1) printf("Argument without required parameter");
	else if(i==2) printf("Wrong type");
	else if(i==3) printf("Argument without required parameter");
	else if(i==4) printf("Should not be parameter");
	else if(i==5) printf("Wrong type");
	else if(i==6) printf("Parameter not found");
	else if(i==7) printf("Empty parameter list \'--\'");
	else if(i==8) printf("Not enough parameters");
	else if(i==9) printf("Wrong type");
	else if(i==10) printf("Parameter not found");
	else if(i==11) printf("Empty parameter list \'--\'");
	else if(i==12) printf("Not a parameter");
	else printf("Error");
	printf(" (%i)\n",i);
}

int argument_error(int argc,char **argv,struct arg_results *R){
	int i,j,exit_code=0;
	struct arg_err *argument_errors;

	for(i=0;i<argc;i++){
		argument_errors=R->errors[i];
		while(argument_errors){
			if(argument_errors->err){
				exit_code=1;
				printf("%s\n",argv[i]);
				for(j=0;j<argument_errors->pos;j++) printf("~");
				printf("^\n");
				print_arg_error(argument_errors->err);
			}
			argument_errors=argument_errors->next;
		}
	}

	return exit_code;
}
