#include <conio.h>
#include <sched.h>
#define MAXLINE 1024
int main(){
	int i,j;
	i = Get_Time_Of_Day();
	Get_NewTOD(&j);
	Print("%d,%d\n",i,j);
	return 0;
	}
