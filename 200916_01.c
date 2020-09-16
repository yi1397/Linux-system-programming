#include <unistd.h>
#include <stdio.h>
int main()
{
	pid_t pid;
	pid = fork();
	if(pid == 0) return 0;
	if(pid == -1) printf("fork() failed");
	printf("my pid : %d\n child pid : %d\n", getpid(), pid);
	return 0;
}
