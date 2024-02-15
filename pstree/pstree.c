
#include "stdio.h"
#include "stdlib.h"
#include "dirent.h"

int arr[20001] = {0};

void printPid(char* pid){
	if(!arr[atoi(pid)])	return;
	arr[atoi(pid)]=0;
	printf("+-----|\n");
	printf("\t|--(%s)  ", pid);
	char pid_file[256];
	sprintf(pid_file, "/proc/%s/task", pid);


	// 读取名字
	char pid_name[256];
	sprintf(pid_name, "/proc/%s/status", pid);
	FILE* fp = fopen(pid_name, "r");
	// char *fgets( char *buf, int n, FILE *fp );
	char tmp[128];
	fgets(tmp, 7, fp);
	char buf[128];
	fgets(buf, 30, fp);
	printf("  %s  ", buf);

	// printf("%s\n", pid_file);
	
	// // Open Son`s dir
	DIR *dir = opendir(pid_file);
	if(dir==NULL)	printf("Opendir Error...\n");
	struct dirent *entry;

	entry = readdir(dir);
	entry = readdir(dir);
	entry = readdir(dir);
	int flag = 1;
	while((entry = readdir(dir)) != NULL){
		if(flag){
			printf("\t|-------------------------------|\n");
			flag = 0;
		}
		// printf("arr[atoi(entry->d_name)] is %d", arr[atoi(entry->d_name)]);
		// if(arr[atoi(entry->d_name) ]){
		// 	printf("son: ");
		// 	arr[atoi(entry->d_name)] = 0;
		// 	printPid(entry->d_name);

		// }
		// printf("\n");

		printf("\t\t\t\t");
		// 子pid
		printf("\t|--%s  ", entry->d_name);

		// 子pid的名字
		// 读取名字
		sprintf(pid_name, "/proc/%s/status", pid);
		fp = fopen(pid_name, "r");
		// char *fgets( char *buf, int n, FILE *fp );
		fgets(tmp, 7, fp);
		fgets(buf, 100, fp);
		printf("  %s  ", buf);
		

		
		// printf("%s  ", entry->d_name);
	}
	close(fp);
	// printf("\t|-------------------------------|\n");
	// printf("father\n");

}

int Pstree(int pid, int num_sort, int version){
	const char *dir_path = "/proc";
	DIR *dir = opendir(dir_path);

	if(dir == NULL)
		printf("Proc is Error...\n");

	struct dirent *entry;
	while((entry = readdir(dir)) != NULL){
		if(entry->d_name[0]>='0' && entry->d_name[0]<='9')
			arr[atoi(entry->d_name)] = 1;
	}

	for(int i = 1; i < 2000; i++){
		char c_pid[100];
		sprintf(c_pid, "%d", i);
		printPid(c_pid);
	}


	close(dir);
}	

int main(int argc, char* argv[]){
	
	int pid = 0;
	int num_sort = 0;
	int version = 0;

	Pstree(1,1,1);

	

	return 0;
}
