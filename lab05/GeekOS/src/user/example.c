#include <conio.h>
#include <sched.h>
#define MAXLINE 1024
int main(){
	/*char input[MAXLINE];
	int i = 0;
	while(true){
		Read_Line(input,MAXLINE);
		for(i=0;i<MAXLINE && input[i] != '\0';++i){
			if( input[i] == '@'){
				Print("\n");
				break;
				}
			Print("%c",input[i]);
		}
		if(i != MAXLINE ){
			if( input[i] != '\0' ) break;
			}
		Print("\n");
	}*/
	//Print("hello world!!!");
	char input[MAXLINE];
	int i= 0;
	char x;
	Keycode k;
	k= (Keycode) Get_Key();
	while(true){
		k= (Keycode) Get_Key();
		x = k;
		k= (Keycode) Get_Key();
		if(x == '@')break;
		input[i]=x;
		i++;
		}
	Print("%s\n",input);
	return 0;
}
