#include <stdio.h>
#include <errno.h>

#include "errorhandler.h"
#include "syscall.h"

int handle_socket_err()
{
    switch (errno) {
        case EACCES:
            printf("Could not create socket - permission for this type of socket or socket protocol denied!\n");
            break;
        case EAFNOSUPPORT:
            printf("Could not create socket - the implementation does not support the specified address family!\n");
            break;
        case EINVAL:
            printf("Could not create socket - unknown protocol, protocol family not available or invalid flags in type!\n");
            break;
        case EMFILE:
            printf("Could not create socket - the per-process limit on the number of open fds has been reached!\n");
            break;
        case ENFILE:
            printf("Could not create socket - the system-wide limit on the total number of open fds has been reached!\n");
            break;
        case ENOBUFS:
        case ENOMEM:
            printf("Could not create socket - insufficient memory!\n");
            break;
        case EPROTONOSUPPORT:
            printf("Could not create socket - the protocol or protocol type is not supported within this domain!\n");
            break;
        default:
            printf("Could not create socket - operation failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

int handle_bind_err(const int sockfd)
{
    switch (errno) {
        case EACCES:
            printf("Could not bind socket - address is protected!\n");
            break;
        case EADDRINUSE:
            printf("Could not bind socket - address already in use or "
                    "address was set to 0 and all ephemeral ports were already in use!\n");
            break;
        case EBADF:
            printf("Could not bind socket - %i is not a valid fd!\n", sockfd);
            break;
        case EINVAL:
            printf("Could not bind socket - socket is either already bound to an address or "
                    "addrlen is wrong or addr is not a valid address for this socket's domain!\n");
            break;
        case ENOTSOCK:
            printf("Could not bind socket - fd %i does not refer to a socket!\n", sockfd);
            break;
        default:
            printf("Could not bind socket - operation failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

int handle_listen_err()
{
    switch (errno) {
        case EADDRINUSE:
            printf("Listen syscall failed - address already in use or all ephemeral ports taken!\n");
            break;
        case EBADF:
            printf("Listen syscall failed - sockfd is not a valid file descriptor. "
                    "As a user there is nothing you can do, this is a bug!\n");
            break;
        case ENOTSOCK:
            printf("Listen syscall failed - sockfd does not refer to a socket. "
                    "As a user there is nothing you can do, this is a bug!\n");
            break;
        case EOPNOTSUPP:
            printf("Listen syscall failed - socket is not of a type that supports the listen() operation. "
                    "As a user there is nothing you can do, this is a bug!\n");
            break;
        default:
            printf("Listen syscall failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

int handle_accept_err()
{
    switch (errno) {
        /* The following two are the same - at least in C */
        /* case EWOULDBLOCK: */
        case EAGAIN:
            printf("Could not accept - socket is marked nonblocking and no connections are present to be accepted.\n");
            break;
        case EBADF:
            printf("Could not accept - sockfd is not an open fd.\n");
            break;
        case ECONNABORTED:
            printf("Could not accept - connection has been aborted.\n");
            break;
        case EFAULT:
            printf("Could not accept - addr argument is not in a writeable part of the user address space.\n");
            break;
        case EINTR:
            printf("Could not accept - syscall was interrupted by a signal before a connection arrived.\n");
            break;
        case EINVAL:
            printf("Could not accept - socket is not listening for connections or addrlen is invalid or invalid value in flags.\n");
            break;
        case EMFILE:
            printf("Could not accept - The per-process limit on the number of open fds has been reached.\n");
            break;
        case ENFILE:
            printf("Could not accept - The system-wide limit on the total number of open files has been reached.\n");
            break;
        case ENOBUFS:
        case ENOMEM:
            printf("Could not accept - not enough free memory. This often indicates that the memory allocation is limited by "
                    "the socket buffer limits rather than the system memory.\n");
            break;
        case ENOTSOCK:
            printf("Could not accept - sockfd does not refer to a socket.\n");
            break;
        case EOPNOTSUPP:
            printf("Could not accept - the referenced socket is not of type SOCK_STREAM.\n");
            break;
        case EPROTO:
            printf("Could not accept - protocol error.\n");
            break;
        case EPERM:
            printf("Could not accept - firewall rules forbid connection.\n");
            break;
        default:
            printf("Could not accept - error code %i.\n", errno);
            break;
    }

    return -1;
}

int handle_recv_err()
{
    switch (errno) {
        /* The following two are the same - at least in C */
        /* case EWOULDBLOCK: */
        case EAGAIN:
            printf("Could not recv - socket is marked nonblocking and recv would block or recv timeout expired before data has arrived.\n");
            break;
        case EBADF:
            printf("Could not recv - sockfd is an invalid fd.\n");
            break;
        case ECONNREFUSED:
            printf("Could not recv - remote host refused the network connection (typically because it is not running the requested service).\n");
            break;
        case EFAULT:
            printf("Could not recv - recv buffer pointer points to outside the process's address space.\n");
            break;
        case EINTR:
            printf("Could not recv - recv was sigkilled.\n");
            break;
        case EINVAL:
            printf("Could not recv - invalid argument.\n");
            break;
        case ENOMEM:
            printf("Could not recv - could not allocate sufficient memory.\n");
            break;
        case ENOTCONN:
            printf("Could not recv - socket is associated with a connection-oriented protocol and has not been connected.\n");
            break;
        case ENOTSOCK:
            printf("Could not recv - sockfd does not refer to a socket.\n");
            break;
        default:
            printf("Could not recv - error code %i.\n", errno);
            break;
    }

    return -1;
}

int handle_close_err(const int sockfd)
{
    switch(errno) {
        case EBADF:
            printf("Close syscall failed - %i isn't a valid open fd!\n", sockfd);
            break;
        case EINTR:
            printf("Close syscall was interrupted by a signal!\n");
            if (syscall(CLOSE_SYSCALL_NO, sockfd) != 0) {
                /* TODO (GM): Can this cause an infinite loop? */
                handle_close_err(sockfd);
            }

            break;
        case EIO:
            printf("Close syscall failed - I/O error!\n");
            break;
        default:
            printf("Close syscall failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

int handle_connect_error()
{
    switch (errno) {
        case EACCES:
        case EPERM:
            printf("Could not connect to socket - permission error!\n");
            break;
        case EADDRINUSE:
            printf("Could not connect to socket - local address already in use!\n");
            break;
        case EADDRNOTAVAIL:
            printf("Could not connect to socket - socket has not been bound to address and "
                    "when trying to bind it to an ephemeral port all ephemeral ports were already in use!\n");
            break;
        case EAFNOSUPPORT:
            printf("Could not connect to socket - the passed address did not have the correct address in its sa_family field!\n");
            break;
        case EAGAIN:
            printf("Could not connect to socket - Unsufficient entries in the routing cache!\n");
            break;
        case EALREADY:
            printf("Could not connect to socket - socket is nonblocking and previous connection attempt has not been completed!\n");
            break;
        case EBADF:
            printf("Could not connect to socket - sockfd is not a valid open fd!\n");
            break;
        case ECONNREFUSED:
            printf("Could not connect to socket - Connection refused, no one is listening on the remote address!\n");
            break;
        case EFAULT:
            printf("Could not connect to socket - socket addr is outside the user's address space!\n");
            break;
        case EINPROGRESS:
            printf("Could not connect to socket - Socket is nonblocking and connection cannot be completed immediately.\n");
            break;
        case EINTR:
            printf("Could not connect to socket - syscall was interrupted by a caught signal!\n");
            break;
        case EISCONN:
            printf("Could not connect to socket - socket is already connected.\n");
            return 0;
        case ENETUNREACH:
            printf("Could not connect to socket - network unreachable.\n");
            break;
        case ENOTSOCK:
            printf("Could not connect to socket - sockfd does not refer to a socket!\n");
            break;
        case EPROTOTYPE:
            printf("Could not connect to socket - socket type does not support the requested protocol.\n");
            break;
        case ETIMEDOUT:
            printf("Could not connect to socket - timeout.\n");
            break;
        default:
            printf("Could not connect - error code %i.\n", errno);
            break;
    }

    return -1;
}

int handle_send_error()
{
    switch (errno) {
        case EACCES:
            printf("Could not send - EACCES error!\n");
            break;
        /* The following two are the same - in C at least */
        case EAGAIN:
        /* case EWOULDBLOCK: */
            printf("Could not send - socket has not been bound to an address and no ephemeral ports were available OR "
                    "tried to send a blocking request to a nonblocking socket.\n");
            break;
        case EALREADY:
            printf("Could not send - another fast open is in progress!\n");
            break;
        case EBADF:
            printf("Could not send - sockfd is not a valid open fd!\n");
            break;
        case ECONNRESET:
            printf("Could not send - connection reset by peer!\n");
            break;
        case EDESTADDRREQ:
            printf("Could not send - the socket is not in connection mode and no peer address is set!\n");
            break;
        case EFAULT:
            printf("Could not send - and invalid user space adress was specfied for an argument!\n");
            break;
        case EINTR:
            printf("Could not send - a signal occurred before any data was transmitted!\n");
            break;
        case EINVAL:
            printf("Could not send - invalid argument passed!\n");
            break;
        case EISCONN:
            printf("Could not send - the connection-mode socket was connected already but a recipient was specified.\n");
            break;
        case EMSGSIZE:
            printf("Could not send - the socket type requires that messages be sent atomically, and the size of the message "
                    "to be sent made this impossible.\n");
            break;
        case ENOBUFS:
            printf("Could not send - the output interface for a network interface was full.\n");
            break;
        case ENOMEM:
            printf("Could not send - no memory available.\n");
            break;
        case ENOTCONN:
            printf("Could not send - the socket is not connected, and no target has been given.\n");
            break;
        case ENOTSOCK:
            printf("Could not send - sockfd does not refer to a socket!\n");
            break;
        case EOPNOTSUPP:
            printf("Could not send - some bit in the flags argument is inappropriate for this socket type!\n");
            break;
        case EPIPE:
            printf("Could not send - the local end has been shut down on a connection oriented socket!\n");
            break;
        default:
            printf("Could not send - error code %i.\n", errno);
            break;
    }

    return -1;
}
