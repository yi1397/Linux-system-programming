#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
    int a;
    int i;
    scanf("%d",&a);
    for(i = a; i<=100; i+=a)
    {
        printf("%d\n",i);
    }
}
