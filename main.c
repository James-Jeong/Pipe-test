#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/syscall.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define MAX_STR_SIZE 10

int main(){
	pid_t pid1, pid2;
	int fd1[2]; // fd1[0] : read, fd1[1] : write
	int fd2[2]; // fd2[0] : read, fd2[1] : write
	int input = 0;
	char buf[10];
	int i = 0;
	ssize_t n = 0;

	if( pipe(fd1) < 0){ // create pipe1
		fprintf( stderr, "Pipe1 Failed");
		return 0;
	}
	if( pipe(fd2) < 0){ // create pipe2
		fprintf( stderr, "Pipe2 Failed");
		return 0;
	}

	if(( pid1 = fork()) > 0){ // 부모 process가 해야될 일
		input = open( "input.txt", 0); // open input text

		close( fd1[ 0]); // pipe1 read is not used, so it should be closed.

		while(( n = read( input, buf, MAX_STR_SIZE + 1)) > 0){ // read data from input text
			printf("%s\n", buf);
			write( fd1[ 1], buf, n);
			memset( buf, 0, sizeof( buf));
		}

		close( input); // close input text
		close( fd1[ 1]); // pipe1 write is finished, so it have to be closed.

		//wait( 0); // wait for a work of child's process

		if(( pid2 = fork()) == 0){ // create other child process for working pipe2
			pid_t ch2_pid = getpid();
			printf("parent: %ld | child2 start: %ld\n", ( long)( getppid()), ( long)( ch2_pid));

			close( fd2[ 1]); // pipe2 write is not used, so it should be closed.

			while(( n = read( fd2[ 0], buf, MAX_STR_SIZE)) > 0){ // read data from pipe2
				for( i = 0; i < n; i++){
					printf("%c", buf[ i]);
				}
				printf("\n");
			}

			close( fd2[ 0]); // pipe1 read is finished, so it have to be closed.

			printf("end of ch2\n");
			exit( ch2_pid);
		}
		else if( pid2 > 0){
			pid_t child_pid = 0;
			pid_t parent_pid = getpid();

			for( i = 0; i < 2; i++){
				int status = 0;
				child_pid = wait( &status);
				printf("parent: %ld | child finish: %ld | exit status: %d\n", ( long)( parent_pid), ( long)( child_pid), WEXITSTATUS( status));
				status = 0;
			}

			printf("end of parent\n");
		}
		else{
			printf("Fail to fork a ch2 process\n");
			exit(-1);
		}
	}
	else if( pid1 == 0){ // 자식 process가 해야될 일
		pid_t ch1_pid = getpid();
		printf("parent: %ld | child1 start: %ld\n", ( long)( getppid()), ( long)( ch1_pid));

		close( fd1[1]); // pipe1 write is not used, so it should be closed.
		close( fd2[0]); // pipe2 read is not used, so it should be closed.

		while(( n = read( fd1[ 0], buf, MAX_STR_SIZE)) > 0){ // read data from pipe1
			for( i = 0; i < 10; i++){
				buf[ i] = toupper( buf[ i]); // data is changed that all texts change to capital letter
			}
			//printf("%s", buf);
			write( fd2[ 1], buf, n); // write data to pipe2
		}

		close( fd1[ 0]); // pipe1 read is finished, so it have to be closed.
		close( fd2[ 1]); // pipe2 write is finished, so it have to be closed.

		printf("end of ch1\n\n");
		exit( ch1_pid); // sign to parent process for inform to be finished this child process
	}
	else{
		printf("Fail to fork a ch1 process\n");
		exit(-1);
	}
}

