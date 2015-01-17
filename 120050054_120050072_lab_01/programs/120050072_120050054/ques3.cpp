#include <iostream>
#include <unistd.h>
#include <sys/wait.h>


using namespace std;
 
void child1(void) {
	sleep(40);
}

void child2(void) {
	//...write your code here
	sleep(30);
}

void child3(void) {
	for(int i=0;i<10000;i++){
		for(int j=0;j<2000;j++){
		}
	}
}

int main(void) {
	cout << "parent process: "<< getpid() << "\n";

	if ( fork() == 0 ) {
		cout << "child1: " << getpid() << "\n";
		child1();
		cout << "exiting child1\n";
		_exit(0);
	}
	
	if ( fork() == 0 ) {
		cout << "child2: " << getpid() << "\n";
		child2();
		cout << "exiting child2\n";
		_exit(0);
	}

	if ( fork() == 0 ) {
		cout << "child3: " << getpid() << "\n";
		child3();
		cout << "exiting child3\n";
		_exit(0);
	}

	int count=0;
	while(count!=2){
		waitpid(-1,NULL,0);
		count++;
	}
	while(true){}
	return 0;
}
