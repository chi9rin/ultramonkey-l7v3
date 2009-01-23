#ifndef __L7AG_SOCKET_H__
#define __L7AG_SOCKET_H__

class	l7ag_socket
{
	private:
		int	sockfd;
	public:
		l7ag_socket(){};
		virtual ~l7ag_socket(){};

		void	connect();
		void	disconnect();
		void	send();
		void	recv();
};

#endif //__L7AG_SOCKET_H__
