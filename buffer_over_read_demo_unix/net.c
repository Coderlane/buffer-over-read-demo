// Travis Lane
// Unix Socket Wrapper Source

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "net.h"

/// <summary>
/// Does nothing on unix.
/// </summary>
/// <returns>A status code, 0 on success !0 on failure.</returns>
int net_init()
{
	return 0;
}

/// <summary>
/// Does nothing on unix.
/// </summary>
/// <returns>A status code, 0 on success !0 on failure.</returns>
int net_cleanup()
{
	return 0;
}

/// <summary>
/// Connects a socket to the specified hostname/port.
/// </summary>
/// <param name="hostname">The hostname.</param>
/// <param name="port">The port.</param>
/// <param name="out_sock">The output socket.</param>
/// <returns>A status code, 0 on success !0 on failure.</returns>
int socket_connect(const char *hostname, const char *port, int *out_sock)
{
	int rv;
	struct addrinfo *addrs = NULL, *addr = NULL, hints;
	int local_sock = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4 only
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_protocol = IPPROTO_TCP; // TCP

	// Get addresses that are probably our host/port
	rv = getaddrinfo(hostname, port, &hints, &addrs);
	if(rv != 0) {
		fprintf(stderr, 
			"Failed to get address info for %s:%s. Error: %d\n", 
			hostname, port, rv);
		goto out;
	}

	// Iterate over those addresses
	for(addr = addrs; addr != NULL; addr = addr->ai_next) {
		local_sock = socket(addr->ai_family,
			addr->ai_socktype, addr->ai_protocol);
		if(local_sock == -1) {
			// This one didn't work, try the next.
			continue;
		}

		rv = connect(local_sock, addr->ai_addr, addr->ai_addrlen);
		if(rv < 0) {
			close(local_sock);
			local_sock = -1;
			continue;
		}
		// Found our socket, get out of the loop.
		break;
	}

	if(local_sock == -1) {
		fprintf(stderr, "Failed to connect to any address.\n");
		rv = -1;
		goto out;
	}
out:
	if(addrs != NULL) {
		freeaddrinfo(addrs);
	}

	if(rv != 0) {
		if(local_sock != -1) {
			close(local_sock);
		}
		*out_sock = -1;
	} else {
		// Set the output socket to our local socket (copy).
		*out_sock = local_sock;
	}

	return rv;
}

/// <summary>
/// Start a new socket to listen.
/// </summary>
/// <param name="hostname">The hostname.</param>
/// <param name="port">The port.</param>
/// <param name="out_sock">The output socket.</param>
/// <returns>A status code, 0 on success !0 on failure.</returns>
int socket_listen(const char *hostname, const char *port, int *out_sock)
{
	int rv;
	struct addrinfo *addrs = NULL, *addr = NULL, hints;
	int local_sock = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4 only
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_protocol = IPPROTO_TCP; // TCP
	hints.ai_flags = AI_PASSIVE;

	// Get addresses that are probably our host/port
	rv = getaddrinfo(hostname, port, &hints, &addrs);
	if(rv != 0) {
		fprintf(stderr, 
			"Failed to get address info for %s:%s. Error: %d\n", 
			hostname, port, rv);
		goto out;
	}

	if(addrs == NULL) {
		fprintf(stderr, 
			"Failed to get any addreses for %s:%s\n", 
			hostname, port);
		rv = -1;
		goto out;
	}

	// Iterate over those addresses
	for(addr = addrs; addr != NULL; addr = addr->ai_next) {
		local_sock = socket(addr->ai_family,
				addr->ai_socktype, addr->ai_protocol);
		if(local_sock == -1) {
			fprintf(stderr, "Failed to create socket.\n");
			// This one didn't work, try the next.
			continue;
		}

		// Bind to the socket.
		rv = bind(local_sock, addr->ai_addr, (int) addr->ai_addrlen);
		if(rv < 0) {
			fprintf(stderr, "Failed to bind. Error: %d\n", rv);
			close(local_sock);
			local_sock = -1;
			continue;
		}

		// Start listening.
		rv = listen(local_sock, SOMAXCONN);
		if (rv < 0) {
			fprintf(stderr, "Failed to listen.\n");
			close(local_sock);
			local_sock = -1;
			continue;
		}

		// Found our socket, get out of the loop.
		break;
	}

	if(local_sock == -1) {
		fprintf(stderr, "Failed to listen to any address.\n");
		rv = -1;
		goto out;
	}
out:
	if(addrs != NULL) {
		freeaddrinfo(addrs);
	}

	if(rv != 0) {
		if(local_sock != -1) {
			close(local_sock);
		}
		*out_sock = -1;
	} else {
		// Set the output socket to our local socket (copy).
		*out_sock = local_sock;
	}

	return rv;

}

/// <summary>
/// Accepts a new connection from a socket.
/// </summary>
/// <param name="listen_sock">The socket to listen on.</param>
/// <param name="out_sock">The connection that was accepted.</param>
/// <returns>A status code, 0 on success !0 on failure.</returns>
int socket_accept(int listen_sock, int *out_sock)
{
	int rv = 0;
	int local_sock = -1;

	local_sock = accept(listen_sock, NULL, NULL);

	*out_sock = local_sock;

	return local_sock != -1 ? 0 : -1;
}

/// <summary>
/// Close the specified socket.
/// </summary>
/// <param name="sock">The socket.</param>
/// <returns>A status code, 0 on success !0 on failure.</returns>
int socket_close(int *sock)
{
	int rv;
	if(*sock = -1) {
		return 0;
	}

	rv =  close(*sock);
	if(rv == 0) {
		*sock = -1;
	}

	return rv;
}

/// <summary>
/// Send some data across a socket.
/// </summary>
/// <param name="sock">The sock.</param>
/// <param name="buff">The buff.</param>
/// <param name="buff_len">The buff_len.</param>
/// <returns>=0 on error, > 0 bytes sent</returns>
int socket_send(int sock, const char *buff, int buff_len)
{
	int rv = send(sock, buff, buff_len, 0);

	return rv;
}
/// <summary>
/// Recieves some data from a socket.
/// </summary>
/// <param name="sock">The sock.</param>
/// <param name="buff">The buffer.</param>
/// <param name="len">The length of the buffer.</param>
/// <returns><=0 on error, > 0 bytes receved</returns>
int socket_recv(int sock, char *buff, int buff_len)
{
	int rv = recv(sock, buff, buff_len, 0);

	return rv;
}
