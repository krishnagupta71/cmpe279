# CMPE 279 - Assignments

### Submitted By: Krishna Gupta - 015721199

## How to run assignment 1:

For Linux based system. 

Step 1: Install GCC compiler uusing the following commands

  ```sudo apt update```
  ```sudo apt install build-essential```
  ```sudo apt-get install manpages-dev```
  validate installation using command: ```gcc --version```
  
Step 2: Clone github repo

  ```git clone <url>```
 
 Step 3: Compile server.c and client.c 
 
  ```gcc -o server server.c```
  ```gcc -o client client.c```
 
 Step 4: Run server and then client 
 
  ```sudo ./server```
  ```./client```
