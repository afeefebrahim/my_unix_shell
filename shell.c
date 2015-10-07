#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
void cat_redirect(char *arg[],int j,char *path);
void parse_argu(char *arg[],char *l);
void pipeing(char *l);
void arg_line_conv(char *arg[],char *);
void check_argu(char *arg[],char *duparg[],int *c,int *flag);
main()
{
	int pid,i,c,j=0,flag;
 	int done = 0;
	char deli;
	char pipe_l[100] ;
	char path[100];	
	char line[100];
	char *l,*arg[10],*duparg[10];
	while(!done){
		strcpy(pipe_l," ");		
		flag = 0;
		printf("my_shell/$");
		strcpy(path,"/bin/");
		fgets(line,sizeof(line),stdin);
		l = strtok(line,"\n");
		parse_argu(arg,l);
		strcat(path,arg[0]);
		check_argu(arg,duparg,&c,&flag);
			pid = fork();
			switch(pid ){
				case -1:
					printf("error occured");
					break;
				case 0:
					if(flag == 1){
						cat_redirect(arg,c,path);
					}
				        else if (flag == 2){
						arg_line_conv(arg,pipe_l);	
						pipeing(pipe_l);
					}
					else
				 		execv(path,duparg);
					break;
	       			default :
					wait(&pid);
					break;
			}
		}
		
		printf("\n");
	
}


void parse_argu(char *arg[],char *l)
{
	int i=0;
	char *temp;
	temp = strtok(l," ");
	while(temp !=NULL){
		arg[i++] = temp;
		temp = strtok(NULL," ");
	}
	arg[i] = NULL;

}

void check_argu(char *arg[],char *duparg[],int *c,int *flag)
{
	int i,j = 0;
	for(i = 0; arg[i] != NULL;i++){
		if(strcmp(arg[i],">") != 0 && strcmp(arg[i],"<") != 0 ){
				
			 	duparg[j++] = arg[i];
		
			}
			if( strcmp(arg[i],">") == 0 || strcmp(arg[i],">>") == 0) {
				*flag = 1;
				*c = i;
			}
			if(strcmp(arg[i],"|") == 0){
				*flag = 2;
				break;
			}
			
		}
		duparg[j] = NULL;
}

void arg_line_conv(char *arg[],char *line)
{
	int i;
	for(i = 0; arg[i] != NULL;i++){
		strcat(line,arg[i]);
		strcat(line, " ");
	}
	strcat(line,"\0");
}
 
void cat_redirect(char *arg[],int j,char *path)
{
	int fd1,i,k=0;
	char *l[10];
	for(i = 0 ;arg[i] != NULL; i++)
		if (i != j && i != j+1)
			l[k++] = arg[i] ;
	l[k++] = NULL;
	if (strcmp(arg[j],">" ) == 0)
		fd1 = open(arg[j+1],O_WRONLY|O_CREAT|O_TRUNC,0644);
	else if(strcmp(arg[j],">>") == 0)
		fd1 = open(arg[j+1],O_WRONLY|O_APPEND,0644);
	close(1);
	dup(fd1);
	execv(path,l);
}


void pipeing(char *arg)
{
	
	int i = 0,fd1[2],fd2[2],max,pid,j;
	char *temp;
	char * command[25];
	char *p_argu[25];
	char path[25];
	temp = strtok(arg,"|");
	while(temp != NULL){
		command[i++] = temp;
		temp = strtok(NULL,"|");
	}
	command[i] = NULL;
	max = i;
	for (i =0 ;command[i] != NULL;i++){
		if(i%2 != 0)
			pipe(fd1);
		else
			pipe(fd2);
		pid = fork();
		switch(pid){
			case -1:
				if(i != max){
				if(i%2 != 0 )
					close(fd1[1]);
				else
					close(fd2[1]); 
				}
				printf("error occured");
				return;
			case 0:
				if(i == 0){
					dup2(fd2[1],1);
				}
				else if(i == max-1){
					if((max ) % 2 != 0)
						dup2(fd1[0],0);
					else
						dup2(fd2[0],0);
	
				}
				else{
					if(i%2 != 0){
						dup2(fd2[0],0);
						dup2(fd1[1],1);
					}else{
						dup2(fd1[0],0);
						dup2(fd2[1],1);
					}
				}
				parse_argu(p_argu,command[i]);
				if(execvp(p_argu[0],p_argu) == -1)
					printf("execution failed\n");
			}
				if(i == 0)
					close(fd2[1]);
				else if(i == max-1){
					if((max)%2 != 0)
						close(fd1[0]);
					else
						close(fd2[0]);
					}
				else{	
					if(i%2 != 0){
						close(fd2[0]);
						close(fd1[1]);
					}else{
						close(fd1[0]);
						close(fd2[1]);
					}
				}
				wait(&pid);


	}
}
