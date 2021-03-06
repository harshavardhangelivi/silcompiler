struct node * mnode(int TYPE, int NODETYPE, int VALUE, char* NAME, struct node * arglist, struct node * ptr1, struct node *  ptr2, struct node *  ptr3){
	/* return statement check */
	if(NODETYPE==RETURN){
		if(ptr1->TYPE != Glookup(NAME)->TYPE){
			printf("return type mismatch - %s\n", NAME);
			exit(1);
		}
	}
	/* check for boolean read and write */
	if(NODETYPE==READ || NODETYPE==WRITE){
		if(ptr1->TYPE!=INT){
			printf("Integer identifiers are expected for read and write\n");
			exit(1);
		}	
	}
	/* type check for AND n OR */
	if(NODETYPE==AND || NODETYPE==OR){
		if(ptr1->TYPE != BOOL || ptr2->TYPE != BOOL){
			yyerror("boolean operands expected for logical operator");
			exit(1);
		}
	}
	/* type check for NOT */
	if(NODETYPE==NOT && ptr1->TYPE!=BOOL){
			yyerror("boolean operands expected for logical operator");
			exit(1);
	}
	/* type check for % operator */
	if(NODETYPE==MOD){
		if(ptr1->TYPE!=INT || ptr2->TYPE != INT ){
			yyerror("modulo operator expects integers as operands");
			exit(1);
		}
	}
	/* setting type for function and argument type check  */
	if(NODETYPE==FUNC){
		struct Gsymbol * t;
		struct arglist *func_arg;
		struct node * passed_arg;
		t=Glookup(NAME);
		if(t!=NULL){
			func_arg=t->arglist;
			passed_arg=arglist;
/*			
			while(func_arg!=NULL){
				printf("%d  %s\n", func_arg->TYPE, func_arg->NAME);
				func_arg=func_arg->NEXT;
			}
			printf("sdfsdfsdsdds\n");
			while(passed_arg!=NULL){
				printf("%d  %s\n", passed_arg->TYPE, passed_arg->NAME);
				passed_arg=passed_arg->next_arg;
			}
			exit(1);
*/			
			while((func_arg!=NULL) && (passed_arg!=NULL)){
				if(func_arg->reference_flag==1){ 
					if(passed_arg->NODETYPE==REFR){
						if(func_arg->TYPE == passed_arg->ptr1->TYPE){
							passed_arg->TYPE=func_arg->TYPE;
							func_arg=func_arg->NEXT;
							passed_arg=passed_arg->next_arg;					
						}
					}else{
						break;
					}	
				}else{
					if(func_arg->TYPE == passed_arg->TYPE){
						func_arg=func_arg->NEXT;
						passed_arg=passed_arg->next_arg;
					}
				}	
			}
			if(func_arg !=NULL ||  passed_arg!=NULL){
				printf("Invalid arguments passed - %s\n", NAME)	;
				exit(1);
			}
			TYPE=t->TYPE;
		}else{
			printf("Undefined function - %s\n", NAME)	;
			exit(1);
		}
	}
	/* to check whether E in if statemnt has a bool return type */
	if((NODETYPE==IF || NODETYPE==WHILE || NODETYPE==NOT)&& ptr1->TYPE!=BOOL){
		yyerror("boolean expected!\n");
		exit(1);
	}
	/* type check for OPERATORS */
	if(NODETYPE==GT || NODETYPE==LT || NODETYPE==EQ || NODETYPE==NEQ || NODETYPE==LE || NODETYPE==GE || NODETYPE==AND || NODETYPE==OR )
		if(ptr1->TYPE!=INT || ptr2->TYPE!=INT){
			printf("integer expected for relop!\n");
			exit(1);
		}
	if(NODETYPE==PLUS || NODETYPE==MINUS || NODETYPE==PDT || NODETYPE==DIV || NODETYPE==GT || NODETYPE==LT ||   NODETYPE==LE || NODETYPE==GE)	
		if(ptr1->TYPE!=INT || ptr2->TYPE!=INT){
			printf("Integer expected!\n");
			exit(1);
		}
	/* checks if modulus operand is of type INT */		
	if(NODETYPE==MOD && (ptr1->TYPE != INT) ){
		printf("Integer expected with modulus\n");
		exit(1);
	}	
		
	/* checks if assignment operands are of same type */		
	if(NODETYPE==ASGN && (ptr1->TYPE != ptr2->TYPE) ){
		printf("operands to assign-mismatch in type\n");
		exit(1);
	}	
	
	struct node * t;
	t=(struct node *)malloc(sizeof(struct node));
	t->TYPE=TYPE;
	/* to check whether ideantifier is declared.... */	

	if(NODETYPE==ID){
		struct Gsymbol * gsy, *func_pointer;
		struct arglist * arg_pointer;
		//printf("%s in %s\n",NAME,func_name);
		if(func_name!=NULL){
			func_pointer=Glookup(func_name);
			func_pointer=func_pointer->local_decls;
			while(func_pointer!=NULL && strcmp(func_pointer->NAME,NAME)!=0){
				func_pointer=func_pointer->NEXT;
			}

			if(func_pointer==NULL){			
				arg_pointer=Glookup(func_name)->arglist;
				while(arg_pointer!=NULL && strcmp(arg_pointer->NAME,NAME)!=0){
					arg_pointer=arg_pointer->NEXT;
				}
				if(arg_pointer==NULL){
					gsy=Glookup(NAME);
					if(gsy==NULL){
						printf("undefined variable: %s - %s \n",NAME,func_name);
						exit(1);
					}else{
						t->TYPE=gsy->TYPE;
						t->Gentry=gsy;
						t->Gentry->scope_flag='g';
					}
				}else{
					t->TYPE=arg_pointer->TYPE;
					t->Argentry=arg_pointer;
				
				}
			}else{
				t->TYPE=func_pointer->TYPE;
				t->Gentry=func_pointer;
				t->Gentry->scope_flag='l';
			}
		}else{
			if(ptr1->TYPE!=INT){
				printf("Invalid array index\n");
				exit(1);
			}
		
		}
		if(ptr1==NULL){
			return NULL;
		}
	}	
	/*  to ckeck for valid tree formation */
	t->NODETYPE=NODETYPE;
	t->VALUE=VALUE;
	if(NAME!=NULL){
		t->NAME=(char *)malloc(sizeof(char)*50);
		t->NAME=NAME;
	}
	t->next_arg=NULL;
	if(arglist!=NULL){
		t->arglist=(struct node *)malloc(sizeof(struct node));
		t->arglist=arglist;
	}
	if(ptr1!=NULL){
		t->ptr1=(struct node *)malloc(sizeof(struct node));
		t->ptr1=ptr1;
	}
	if(ptr2!=NULL){
		t->ptr2=(struct node *)malloc(sizeof(struct node));
		t->ptr2=ptr2;
	}
	if(ptr3!=NULL){
		t->ptr3=(struct node *)malloc(sizeof(struct node));
		t->ptr3=ptr3;
	}
	return t;
}

struct Gsymbol * make_Gentry(char * NAME,int TYPE,int SIZE,struct arglist * ARGLIST){
	if(func_name!=NULL){
		if(SIZE>1){
			printf("arrays cant be declared locally\n");
			exit(1);
		}
		if(ARGLIST!=NULL){
			printf("functions cant be declared locally\n");
			exit(1);
		}
		struct arglist * targ=Glookup(func_name)->arglist;
		while(targ!=NULL){
			if(!strcmp(NAME,targ->NAME)){
				printf("local name is redeclared as an argument - %s\n",NAME);
				exit(1);
			}
			targ=targ->NEXT;
		}
	}
	check_if_exists(NAME, head);
	struct Gsymbol * t;
	t=(struct Gsymbol *)malloc(sizeof(struct Gsymbol));
	t->BINDING=(int *)malloc(sizeof(int)*SIZE);
	t->NAME=(char *)malloc(sizeof(char)*50);
	t->NAME=NAME;
	t->TYPE=TYPE;
	t->SIZE=SIZE;
	t->NEXT=NULL;
	t->LOC=location;
	location+=SIZE;
	t->fun_def_count=0;
	t->arglist=ARGLIST;
	if(head==NULL)
		head=t;
	else
		tail->NEXT=t;
	tail=t;	
	if(global_flag){
		global_head=t;
		global_done();
	}	
	return t;
}
struct Gsymbol * put_type(struct Gsymbol * t,int type){
	struct Gsymbol * x;
	x=t;
	while(t!=NULL){
		t->TYPE=type;
		t=t->NEXT;
	}
	return x;
}
struct Gsymbol * Glookup(char * name){
	struct Gsymbol * t;
	t=global_head;
	while( t!=NULL && strcmp(t->NAME,name) )
		t=t->NEXT;
	return t;	
}



void check_if_exists(char * NAME, struct Gsymbol * head){
	while(head!=NULL){
		if(!strcmp(head->NAME,NAME)){
			printf("variable redeclared - %s\n",NAME);
			exit(1);
		}
		head=head->NEXT;
	}
}

void fun_check_if_exists(char * NAME){
	struct arglist * head=fun_par_head;
	while(head!=NULL){
		if(!strcmp(head->NAME,NAME)){
			printf("repeated argument - %s\n",NAME);
			exit(1);
		}
		head=head->NEXT;
	}
}


struct arglist * make_argentry(char * name, int type, int flag){
	fun_check_if_exists(name);
	struct arglist * t;
	t=(struct arglist * )malloc(sizeof(struct arglist));
	t->NAME=name;
	t->TYPE=type;
	t->reference_flag=flag;
	t->NEXT = NULL;
	t->LOC=-1*(arg_location)-2;
	arg_location++;
	if(fun_par_head==NULL)
		fun_par_head=t;
	else
		fun_par_tail->NEXT=t;
	fun_par_tail=t;		
	return t;
}

struct arglist * fun_put_type(struct arglist * x, int type){
	struct arglist *t=x;
	while(x!=NULL){
		x->TYPE=type;
		x=x->NEXT;
	}
	return t;
}
void global_done(){
	global_flag=0;
}
struct Gsymbol * set_head(){
	struct Gsymbol * t = head;
	head=NULL;
	location=1;
	return t;
}
struct arglist * set_fun_head(){
	struct arglist * t = fun_par_head;
	fun_par_head=NULL;
	arg_location=1;
	return t;
}
void fun_type_check(char *NAME, int RET_TYPE, struct arglist * args){
	struct Gsymbol * func_pointer;
	struct arglist * t;
	func_pointer = Glookup(NAME);
	if(func_pointer->fun_def_count!=0){
		printf("multiple definitions found for the function - %s\n",NAME);
		exit(1);
	}
	func_pointer->fun_def_count=1;
	if(func_pointer==NULL){
		printf("Function undeclared - %s\n", NAME);
		exit(1);
	}
	if(func_pointer->TYPE != RET_TYPE){
		printf("Function return type mismatch - %s\n",NAME);
		exit(1);
	}
	t=func_pointer->arglist;
	while(t!=NULL && args!=NULL){
		if(t->TYPE!=args->TYPE || strcmp(t->NAME,args->NAME) || t->reference_flag!=args->reference_flag){
printf("Function arguments %d,  %d,  %s,  %s mismatch - %s\n",t->TYPE,args->TYPE,t->NAME,"hgg",NAME);
			exit(1);		
		}
		t=t->NEXT;
		args=args->NEXT;
	}
	if(t!=NULL || args!=NULL){
		printf("Number of arguments mismatch - %s\n",NAME);
		exit(1);			
	}
}
void put_func_decls(char * func_name, struct Gsymbol * func_decls_head){
	struct Gsymbol * t;
	t=Glookup(func_name);
	if(t != NULL)	
		t->local_decls=func_decls_head;
	else{
		printf("function not found - %s",func_name);
		exit(1);
	}
}
void insert_main_to_global(){
	struct Gsymbol * main, * t;
	main=(struct Gsymbol *)malloc(sizeof(struct Gsymbol));
	main->NAME="main";
	main->TYPE=INT;
	main->SIZE=0;
	main->BINDING=(int *)malloc(sizeof(int)*main->SIZE);
	main->NEXT=NULL;
	main->LOC=location;
	location+=main->SIZE;
	main->arglist=NULL;
	t=global_head;
	if(t==NULL) 
		global_head=main;
	else{
		while(t->NEXT!=NULL)
			t=t->NEXT;
		t->NEXT=main;
	}
}

void append_to_rargs(struct node * t){
	if(rargs_head == NULL){
		rargs_head=t;
		rargs_tail=t;
	}
	else{
		rargs_tail->next_arg=t;
		rargs_tail=t;		
		
	}

}

struct node * set_rargs_head(){
	struct node * x=rargs_head;
	rargs_head=NULL;
	return x;
		printf("chk\n");
}
void check_absent_fundef(){
	struct Gsymbol * t;
	t=global_head;
	while(t!=NULL){
		if(strcmp(t->NAME, "main")){
			if(t->SIZE==0){
				if(t->fun_def_count==0){
					printf("Function not defined - %s\n", t->NAME);
					exit(1);
				}
			}
		}	
		t=t->NEXT;
	}
}
