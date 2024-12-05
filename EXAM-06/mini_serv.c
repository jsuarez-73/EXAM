#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*This program don't care about free memory, but just for implementors
  when the extract_message is called every time, the memory must be 
  released when broadcast it.
*/
typedef struct	s_client
{
	int		id;
	int		fd;
	char	*buf;
}	t_client;

static char	*E_WARG = "Wrong number of arguments\n";
static char	*E_FATAL = "Fatal error\n";
static char	*CMSG = "client %d: ";
static char	*CARRV = "server: client %d just arrived\n";
static char	*CLEFT = "server: client %d just left\n";
static int	g_id;//Id for clients.
static int	g_hfd;//Highest file descriptor to be poll.
static int	g_sockfd;//passive socket.
static fd_set	g_afds, g_wfds, g_rfds;//Sets for inspection I/O operations.
static t_client	g_lclient[1024];//Slot for clients to be registered.

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void	ft_error(char *err)
{
	//If passive socket have been assigned so close it.
	if (g_sockfd)
		close(g_sockfd);
	//Send the error message to stderr.
	write(2, err, strlen(err));
	//Bad ByeÂ!
	exit(1);
}

void	ft_broadcast_message(t_client *client, char *msg)
{
	if (!client || !msg)
		return ;
	for (t_client *iclient = g_lclient; iclient < g_lclient + 1024; iclient++)
	{
		//For every client that is not the sender, send the message without
		//block.
		if (FD_ISSET(iclient->fd, &g_wfds) && client->fd != iclient->fd)
			send(iclient->fd, msg, strlen(msg), MSG_DONTWAIT);
	}
}

void	ft_close_connection(t_client *client)
{
	char	tmpmsg[512];

	if (!client)
		return ;
	//Clean the buffer.
	bzero(tmpmsg, sizeof(tmpmsg));
	//close the connection with that socket.
	close(client->fd);
	//Remove that socket from the "all fd for inspection" set.
	FD_CLR(client->fd, &g_afds);
	//Format the message to broadcast.
	sprintf(tmpmsg, CLEFT, client->id);
	//broadcast it.
	ft_broadcast_message(client, tmpmsg);
	//Clean the slot occupied by the client.
	bzero(client, sizeof(t_client));
}

void	ft_send_messages(void)
{
	char	tmpmsg[512];
	char	*msg;

	for (t_client *client = g_lclient; client < g_lclient + 1024; client++)
	{
		//For every client while there are lines in buffer extract them
		//and broadcast them, but before format the client leading header
		//and them broadcast it with the message extracted.
		while (extract_message(&client->buf, &msg))
		{
			//Each time the buffer needs to be cleaned.
			bzero(tmpmsg, sizeof(tmpmsg));
			sprintf(tmpmsg, CMSG, client->id);
			ft_broadcast_message(client, tmpmsg);
			ft_broadcast_message(client, msg);
		}
	}
}

void	ft_read_messages(void)
{
	char	tmpmsg[512];
	int		nbytes;

	bzero(tmpmsg, sizeof(tmpmsg));
	for (t_client *client = g_lclient; client < g_lclient + 1024; client++)
	{
		//If any client is ready to be read so do it.
		if (FD_ISSET(client->fd, &g_rfds))
		{
			//Read the bytes available in the read pipe socket but dont wait
			//to avoid blocking operations.
			nbytes = recv(client->fd, tmpmsg, sizeof(tmpmsg) - 1, MSG_DONTWAIT);
			//If the read pipe send a 0 means EOF so close connection with the
			//client.
			if (nbytes == 0)
				ft_close_connection(client);
			//Otherwise just concat the info in the client buffer.
			else if (nbytes > 0)
				client->buf = str_join(client->buf, tmpmsg);
		}
	}
}

void	ft_new_connection(void)
{
	int connfd, len;
	struct sockaddr_in cli;
	char	tmpmsg[512];

	len = sizeof(cli);
	bzero(tmpmsg, sizeof(tmpmsg));
	//Accept the incoming client.
	connfd = accept(g_sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
	//If fail just return.
	if (connfd < 0) 
		return ;
    else
	{
		//Register the new client in our memory buffer dedicated just for
		//client info, the 1024 is due to select system call limitation to
		//inspect no more than 1024 fd.
		for (t_client *client = g_lclient; client < g_lclient + 1024; client++)
		{
			//If we got a slot available in our buffer, register it.
			if (client->fd == 0)
			{
				client->fd = connfd;
				client->id = g_id++;
				//Add the new client to the "all fds to inspect" set.
				FD_SET(connfd, &g_afds);
				//If the new fd is greather than the highest fd setled in the
				//g_xfds sets then set it to the new one.
				if (connfd > g_hfd)
					g_hfd = connfd;
				//Set the message to send to everyone with the proper id.
				sprintf(tmpmsg, CARRV, client->id);
				//Broadcast the new client to everyone.
				ft_broadcast_message(client, tmpmsg);
				//Go out of here!
				break;
			}
		}
	}
}

void	ft_run_server(void)
{
	struct timeval	timeout; //Needed to don't block select system call.
	int				ready;//Max number of fd in all g_xfds sets

	FD_SET(g_sockfd, &g_afds);//Set to check g_sockfd listening to inspection.
	bzero(&timeout, sizeof(timeout));//set to zero every field in the struct.
	g_hfd = g_sockfd;//The max fd to be checked is g_sockfd so far.
	while (1)
	{
		g_rfds = g_wfds = g_afds;//Refresh the fd to be checked.
		//Inspect all the fd setled in all g_xfds sets.
		ready = select(g_hfd + 1, &g_rfds, &g_wfds, 0, &timeout);
		//If any is available to make I/0 keep going.
		if (ready <= 0)
			continue ;
		//If we got the passive socket available to read, set a new client.
		else if (FD_ISSET(g_sockfd, &g_rfds))
			ft_new_connection();
		//Otherwise make I/O operations.
		else
			ft_read_messages(), ft_send_messages();
	}
}

int main(int argc, char **argv) {
	int	port;
	struct sockaddr_in servaddr; 

	if (argc != 2)
		ft_error(E_WARG);
	port = atoi(*(argv + 1));
	// socket create and verification, exists only in name space.
	g_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sockfd == -1) 
		ft_error(E_FATAL);
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(g_sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) 
		ft_error(E_FATAL);
	//Listen and make passive mode.
	if (listen(g_sockfd, 10) != 0)
		ft_error(E_FATAL);
	//Run the server.
	ft_run_server();
}
