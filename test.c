#include <stdio.h>
#include<unistd.h>
#include<signal.h>

int main(int argc, char *argv[]) 
{
	int i, n;
	
	printf("argc is %d\n", argc);
	for (i=0; i < argc; i++) {
		printf("argc%d is %s\n", i, argv[i]);
	}
	
	return(0);
}
