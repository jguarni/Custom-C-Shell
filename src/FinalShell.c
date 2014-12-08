//CISC361 Project #1, UNIX Shell, By: Joe Guarni
//This project implements a UNIX shell with Pipes, I/O Redirection and more.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>


#define lineBufferLength 1000
#define COMMAND_OPTION_LEN 100
#define maxCMD 10
pid_t pid;

void sayHello(){
	printf(
				"****************************************************************** \n");
		printf("Unix Shell: Written and Designed By Joe Guarni \n");
		printf("- Type exit to exit the prompt \n");
		printf(
				"****************************************************************** \n");
		printf("");
}

char split(char *line, char **cmd, char *array)
{
	int l_count = 0, i = 0, offset = 0, ret_value = 0;
	char c, a = 0, b = 0;
	while((c = line[l_count++]) != '\n') {
		if((c == '>') || (c == '<') || (c == '|') || (c == ' ' )) {
			cmd[i][offset] = 0;
			offset = 0;
			if(c != ' ') {
				array[1] = cmd[(array[0] = (i = (i + 1 - a)))][0] = c;
				cmd[i++][1] = 0;
				a = 0;
				b = 1;
			} else {
				i = i + 1 - b;
				a = 1;
				b= 0;
			}
		} else {
			a = 0;
			b = 0;
			cmd[i][offset++] = c;
		}
	}
	cmd[i][offset] = 0;
	return array[1];
}


void flushit (char **cmd, int start, int end)
{
	do {
		end--;
		cmd[end][0] = 0;
	} while(end != start);
}

void exec(char **cmd, int start, int stop)
{
	int j = 0;
	char* CMD[maxCMD], i = 0;
	for(i = 0; i < maxCMD; i++)
		CMD[i] = 0;
	for(i = start; cmd[i][0] != 0 && i <= stop; i++,j++)
		CMD[j] = cmd[i];
	execlp(CMD[0],CMD[0],CMD[1],CMD[2],CMD[3],CMD[4],CMD[5],CMD[6],CMD[7],CMD[8],CMD[9], NULL);
}

void flush_array(char *array, int size)
{
	int i = 0;
	for(;i<size; i++)
		array[i] = 0;
}

void redirectIn(char **cmd, char c)
{
	int fd = 0;
	fd = open(cmd[c+1],O_RDONLY);
	close(0);
	dup(fd);
	exec(cmd, 0, c-1);

}

void redirectOut(char **cmd, char c)
{
	int fd = 0;
	fd = open(cmd[c+1],O_WRONLY|O_CREAT, 0644);
	close(1);
	dup(fd);
	exec(cmd, 0, c-1);
}

void execPipe(char **cmd, char c)
{
	int FD[2];
	pipe(FD);
	int pid1;
	if((pid1 = fork()) == 0) {
		close(1);
		dup(FD[1]);
		exec(cmd, 0, c-1);
		exit(0);
	} else {
		waitpid(pid1, NULL,0);
		close(FD[1]);
		close(0);
		dup(FD[0]);
		exec(cmd,c+1,10);
	}
}

main()
{
	char line[lineBufferLength];
	char *cmd[maxCMD], array[10];
	int i = 0, pid = 0;
	sayHello();

	for(;i < 10; i++)
		cmd[i] = (char*)malloc(COMMAND_OPTION_LEN);
	while(1) {
		printf("CISC361Shell$> ");
		flushit(cmd, 0,sizeof(*cmd));
		flush_array(array, sizeof(array));
		fgets(line,sizeof(line),stdin);


		if(!strcmp(line,"exit\n"))
			break;
		if (split(line, cmd, array) == 0) {
			if((pid = fork()) == 0)
				exec(cmd, 0, 10);
		} else if(array[1] == '<') {
			if((pid = fork()) == 0)
				redirectIn(cmd, array[0]);
		} else if(array[1] == '>') {
			if((pid = fork()) == 0)
				redirectOut(cmd, array[0]);
		} else if(array[1] == '|') {
			if((pid = fork()) == 0)
				execPipe(cmd, array[0]);
		}
		waitpid(pid,NULL,0);
	}
}
