// Travis Lane
// Heartbleed Extra Credit Server


#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "net.h"
#include "buffer_over_read_message.h"

int main(void) 
{
	int rv;
	int listen_socket = -1;
	int client_socket = -1;

	// Initialize the networking code
	rv = net_init();
	if(rv != 0) {
		fprintf(stderr, "Failed to initialize networking.\n");
		goto out;
	}

	// Start to listen on the socket
	rv = socket_listen("localhost", "21012", &listen_socket);
	if(rv != 0) {
		fprintf(stderr, "Failed to start listening.\n");
		goto out;
	}

	printf("Listening...\n");
	for(;;) // forever
	{
		// THe order here matters!
		char password[256] = "#Hashtag";
		char buff[256] = "";
		int read = 0, sent = 0;

		memset(buff, 0, sizeof(buff));

		// Accept a new connection
		rv = socket_accept(listen_socket, &client_socket);
		if (rv != 0) {
			fprintf(stderr, "Failed to accept connection.\n");
			continue;
		}

		printf("Accepted connection!\n");

		// While the connection is alive and there is data to read
		while(socket_recv(client_socket, buff, sizeof(buff)) > 0) 
		{
			// This looks sketchy
			HeartbleedMessage *hm = (HeartbleedMessage*) buff;

			// HEARTBLEED BUG OCCURS HERE
			sent = socket_send(client_socket, 
				(const char *) hm, 
				sizeof(HeartbleedMessage) + hm->hm_len);

			if(sent <= 0) {
				fprintf(stderr, 
					"Error sending! Error: %d\n", 
					sent);
				break;
			}
		}

		printf("Disconnected.\n");
		socket_close(&client_socket);
	}

out:
	// Done or error, cleanup.
	socket_close(&listen_socket);
	net_cleanup();
	return 0;
}
