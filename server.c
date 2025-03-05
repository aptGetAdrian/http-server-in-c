#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "vec.h"


/*
TODO:
vrednosti v status code arrayi se ne prenesejo. ocitno je pointer issue. server posle 500 internal server error




*/

#define NUM_STATUS_CODES 511

typedef struct  {
    char* key;
	char* value;
} requestElement;

typedef struct {
    char** status_codes;
	int* clientFd;
} handlerArg;

//typedef struct  {
//    char* code;
//	char* response;
//} statusCodes;


//void fillStatusArray(statusCodes status_codes[NUM_STATUS_CODES], FILE* fptr) {
//	char tempString[50];
//	char* code,* response;
//	int counter = 0;
//	while(fgets(tempString, 50, fptr)) {
//		code = strtok(tempString, ":");
//		response = strtok(NULL, "\n");
//		status_codes[counter] = (statusCodes) {strdup(code), strdup(response)};
//		counter++;
//	}
//
//
//	//for (int i = 0; i < NUM_STATUS_CODES; i++) {
//	//	printf("%s : %s\n", status_codes[i].code, status_codes[i].response);
//	//}
//
//}

//function to fill the array status_codes with the contents of the status_codes.txt file
void fillStatusArray(char* status_codes2[NUM_STATUS_CODES], FILE* fptr) {
	char tempString[50];
	char* code,* response;
	while(fgets(tempString, 50, fptr)) {
		code = strtok(tempString, ":");
		response = strtok(NULL, "\n");
		status_codes2[atoi(code)] = strdup(response);
	}
}

char* bulildStatusCodeReply(int code, char* status_codes[NUM_STATUS_CODES]) {
	if (status_codes[code] == NULL) {
			return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
	} 

	char* pchar = malloc(sizeof(char) * 60);
	if (pchar == NULL) {
		printf("Memory allocation failed: %s...\n", strerror(errno));
		exit(1);
	}
	sprintf(pchar,"%s %d %s\r\n","HTTP/1.1", code, status_codes[code]);
	return pchar;
}

void* clientHandling (void *arguments) {
	char buffer[4096] = {0};
	char buffer2[] = "";
	int counter = 0;
	requestElement requestLine[3];
	requestElement* vec = vector_create(); 
	char *saveptr1, *saveptr2;
	char* replyStatus, *reply;

	handlerArg *args = arguments;
	int clientFd = *args->clientFd;
	char** status_codes = args->status_codes;

	//recieve a buffer of 100 bytes
	recv(clientFd, buffer, 4096, 0);


	//copying buffer into buffer 2
	//strcpy(buffer2, buffer);


	//splitting buffer2 by delim = " "
	char *token = strtok_r(buffer, " ", &saveptr1);
	requestLine[0] = (requestElement) {"Method", token};
	token = strtok_r(NULL, " ", &saveptr1);
	requestLine[1] = (requestElement) {"Target", token};
	token = strtok_r(NULL, "\r\n", &saveptr1);
	requestLine[2] = (requestElement) {"Protocol", token};

	
	////for testing purposes
	//printf("Request line:\n");
	//for (size_t i = 0; i < sizeof(requestLine)/sizeof(requestElement); i++) {
	//	printf("%s: %s\n", requestLine[i].key, requestLine[i].value);
	//}
	
	
	
	token = strtok_r(NULL, "\r\n", &saveptr1);
	while( token != NULL ) {
		char* key = "";
		char* value = "";
		char* token2 = token;

		key = strtok_r(token2, " ", &saveptr2);
		value = strtok_r(NULL, " ", &saveptr2);
		
		if (key[strlen(key)-2] == ':') {
			key[strlen(key)-2] = '\0';
		} else {
			key[strlen(key)-1] = '\0';
		}
		
		//printf("%s\n%s\n", key, value);

		if (key && value) {
			requestElement test = {key, value};
		}
		
		
		
		counter++;

		token = strtok_r(NULL, "\r\n", &saveptr1);
	}


	//DONE PARSING THE HTTP REQUEST
	//for now only the request line and the headers. will do the body later

	
	if (strcmp(requestLine[0].value, "GET") == 0 && strcmp(requestLine[1].value, "") != 0) {
		int bytes_sent;
		if (strncmp(requestLine[1].value, "/echo/", 6) == 0) {
			char *outString = requestLine[1].value + 6;

			if (status_codes[200]) {
				replyStatus = bulildStatusCodeReply(200, status_codes);
			} else {
				replyStatus = strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n");
			}


			reply = malloc(sizeof(char) * 1024);
			snprintf(reply, 1024, "%s%s%ld%s%s\n", replyStatus, "Content-Type: text/plain\r\nContent-Length: ", (strlen(outString) + 1), "\r\n\r\n", outString);
			

			bytes_sent = send(clientFd, reply, strlen(reply), 0);
			free(reply);
		} else if (strcmp(requestLine[1].value, "/user-agent") == 0) {
			char *outString;

			for (size_t i = 0; i < vector_size(vec); i++) {
				if (strcmp(vec[i].key, "User-Agent") == 0) {
					outString = vec[i].value;
				}
			}
			
			if (status_codes[200]) {
				replyStatus = bulildStatusCodeReply(200, status_codes);
			} else {
				replyStatus = strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n");
			}


			reply = malloc(sizeof(char) * 1024);
			snprintf(reply, 1024, "%s%s%ld%s%s\n", replyStatus, "Content-Type: text/plain\r\nContent-Length: ", (strlen(outString) + 1), "\r\n\r\n", outString);
			

			bytes_sent = send(clientFd, reply, strlen(reply), 0);
			free(reply);
		} else {

			FILE *targetFile;
			targetFile = fopen(requestLine[1].value, "r");
			
			if (targetFile == NULL) {
				printf("File does not exist. Returning status 404:\n%s\n", strerror(errno));
				replyStatus = bulildStatusCodeReply(404, status_codes);
				bytes_sent = send(clientFd, replyStatus, strlen(replyStatus), 0);
			} else {
				if (status_codes[200]) {
					replyStatus = bulildStatusCodeReply(200, status_codes);
				} else {
					replyStatus = strdup("HTTP/1.1 500 Internal Server Error\r\n\r\n");
				}
				bytes_sent = send(clientFd, replyStatus, strlen(replyStatus), 0);
				fclose(targetFile);
			}
		}
		
		if (bytes_sent == -1) {
			printf("Error sending server response: %s \n", strerror(errno));
			exit(-1);
		}

	}
	
	
	//printf("Stored tokens:\n");
	//for (int i = 0; i < vector_size(vec); i++) {
	//	printf("%s: %s\n", vec[i].key, vec[i].value);
	//}	
	//free(replyStatus);
	//vector_free(vec);

	//close(*args->clientFd);
	close(clientFd);


	return NULL;
}

/*void insertElement(struct requestElement headers[], int counter, char* key, char* value) {


	//declaring **temp each iteration to dynamically alocate memory for the **strs array of strings
	//using realloc to reallocate the **strs value and expand it by the size of the counter * the size of *char into the new **temp variable
	char **temp = realloc(headers, (counter + 1) * sizeof(struct requestElement));

	//error handling for **temp reallocation
	if (temp == NULL){
		fprintf(stderr, "Failed to allocate memory: %s \n", strerror(errno));
		free(headers);
		return 1;
	}

	headers = temp;

	//allocating uninitialized memory at the [counter] position of the **strs pointer to the size of token + 1 
	headers[counter] = malloc(sizeof(struct requestElement));

	//error handing for allocating memory
	if (headers[counter] == NULL) {
		fprintf(stderr, "Failed to allocate memory: %s \n", strerror(errno));
		for (int i = 0; i < counter; i++) {
			free(headers[i]);
		}
		free(headers);
		return 1;
	}

	struct requestElement temp = {key, value};

	//coppying value of token into the [counter] element of the **strs array of strings
	memcpy(headers[counter], temp.key, sizeof temp.key);

}

*/

int main() {
	//filling array with status codes at startup
	FILE *fptr;
	fptr = fopen("status_codes.txt", "r");
	char* status_codes[NUM_STATUS_CODES] = {NULL};
	fillStatusArray(status_codes, fptr);
	fclose(fptr);
	////////////////////////////////////////////////////

	
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	printf("Logs from your program will appear here!\n");
	

	int server_fd, client_addr_len;
	struct sockaddr_in client_addr;
	

	//creating a tcp network socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
	 	printf("Socket creation failed: %s...\n", strerror(errno));
	 	return 1;
	}
	
	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
	 	printf("SO_REUSEADDR failed: %s \n", strerror(errno));
	 	return 1;
	}
	
	//setting the server address properties
	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
	 								 .sin_port = htons(4221),
	 								 .sin_addr = { htonl(INADDR_ANY) },
									};
	
	//binding the socket
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
	 	printf("Bind failed: %s \n", strerror(errno));
	 	return 1;
	}
	

	//listen for connections, max backlog is 5
	int connectionBacklog = 10;
	if (listen(server_fd, connectionBacklog) != 0) {
	 	printf("Listen failed: %s \n", strerror(errno));
	 	return 1;
	}
	
	
	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);

	while (1) {
		
		//accepting connections
		int clientFd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (clientFd == -1) {
			continue;
		}
		printf("Client connected\n");

		//THREAD IS CREATED//

		pthread_t clientThread;
		handlerArg args;
		args.clientFd = &clientFd;
		args.status_codes = status_codes;

		pthread_create(&clientThread, NULL, clientHandling, (void*) &args);

		pthread_join(clientThread, NULL);

	}

	//close the socket
	close(server_fd);

	return 0;
}
