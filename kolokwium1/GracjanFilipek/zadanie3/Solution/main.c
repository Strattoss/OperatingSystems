#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

#define BUFFER_SIZE 128

int main(int argc, char* argv[])
{

 if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    exit(1);
 }

 int toChildFD[2];
 int toParentFD[2];

 pipe(toChildFD);
 pipe(toParentFD);

 int val1,val2,val3 = 0;

 pid_t pid;

    if((pid = fork()) == 0) {
        close(toChildFD[1]);
        close(toParentFD[0]);

    //odczytaj z potoku nienazwanego wartosc przekazana przez proces macierzysty i zapisz w zmiennej val2
    char buffer[BUFFER_SIZE];
    read(toChildFD[0], buffer, BUFFER_SIZE);
    val2 = atoi(buffer);

     val2 = val2 * val2;

    //wyslij potokiem nienazwanym val2 do procesu macierzysego
    int read_chars = sprintf(buffer, "%d", val2);
    write(toParentFD[1], buffer, read_chars);

 } else {

     val1 = atoi(argv[1]);

     close(toChildFD[0]);
     close(toParentFD[1]);

    //wyslij val1 potokiem nienazwanym do procesu potomnego
    char buffer[BUFFER_SIZE];
    int written_chars = sprintf(buffer, "%d", val1);
    write(toChildFD[1], buffer, written_chars);

    sleep(1);

    //odczytaj z potoku nienazwanego wartosc przekazana przez proces potomny i zapisz w zmiennej val3
    read(toParentFD[0], buffer, BUFFER_SIZE);
    val3 = atoi(buffer);

     printf("%d square is: %d\n",val1, val3);
 }
 return 0;
}
