#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
    system("find / -name \"*.c\" 2> /dev/null >C_files"); //.c 확장자로 끝나는 파일을 검색하고 C_files 파일에 저장한다.(권한이 없는 폴더는 건너뜀)
    system("grep -c '\n' C_files"); //C_files 파일의 라인수를 출력한다
    system("cat -n C_files"); //C_files의 파일의 내용을 라인넘버를 포함하여 출력한다
}
