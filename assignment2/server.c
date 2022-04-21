// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 8080



void reexec_from_child(int new_socket){

    printf("In Re-exec from child - UID is %d \n", getuid());
    pid_t c_pid = fork();
    int c_status;

    //fork call return negative value if a error has occured
    if(c_pid<0){
        perror("child failed");
        exit(EXIT_FAILURE);
    }

    //fork call return zero value if it's the child process
    else if(c_pid == 0) {
        char socket_add[12];
        sprintf(socket_add, "%d", new_socket);
        char * argument_list[] = {"./server", socket_add, NULL};
        /**
         * Ref: https://www.journaldev.com/40793/execvp-function-c-plus-plus
         * execvp() takes in the name of the UNIX command to run, as the first argument 
         * followed by the array of command line arguments we want to pass to the UNIX command
         * */
        if(execvp(argument_list[0], argument_list) < 0){
            perror("exec");
            exit(EXIT_FAILURE);
        }
    }

    //fork call return positive value if it's the parent process
    else if (c_pid > 0)
    {
        printf("Parent process waiting after fork\n");
        waitpid(c_pid, &c_status, 0);
        printf("Parent process exiting after child finished\n");
        // exit(1);
    }
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    //True if no command line arguments present. This will run first time when we run from terminal
    if(argc == 1){

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Attaching socket to port 80
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                    &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Forcefully attaching socket to the port 80
        if (bind(server_fd, (struct sockaddr *)&address,
                sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        /**
         * Ref: https://man7.org/linux/man-pages/man2/accept.2.html
         * The accept() system call extracts the first connection request on the 
         * queue of pending connections and returns a new file descriptor referring to that socket
         **/
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        //This funtion re-executes server code from child process
        reexec_from_child(new_socket);
    }

    // This will run from forked child process when argc is 2 since socket adress will be passed as argument
    else{

        //this is the right point from assignment 1 to drop privileges 
        struct passwd *pw;
        pw = getpwnam("nobody"); // getting nobody user

        if( pw == NULL ){
            printf("Unable to get user nobody.\n");
            exit(1);
        }

        printf("Nobody UID %ld \n", (long)pw->pw_uid);


        long cp_uid = setuid(pw->pw_uid);

        if(setuid(pw->pw_uid) == -1)
        {
            perror("setuid failed");
            exit(EXIT_FAILURE);
        }
        //reading the new socket passed from argument when child process runs ./server
        new_socket = atoi(argv[1]);
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        return 0;
    }



    

    
    
}
