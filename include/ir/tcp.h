/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_TCP
#define IR_TCP

#include "ip.h"
#include "quiet_vector.h"
#include <stddef.h>

namespace ir
{
///@addtogroup net Networking
///@{
	
	class TCPClient
	{
	private:
		#ifndef _WIN32
			typedef int SOCKET;
			static const int INVALID_SOCKET = -1;
			static const int SOCKET_ERROR = 1;
		#endif

		mutable IP _ip;
		IP _server_ip;
		SOCKET _socket = INVALID_SOCKET;

	public:
		///Creates empty TCP client
		TCPClient()			noexcept;
		///Creates TCP client
		///@param v6 True if IPv6, false if IPv4
		TCPClient(bool v6)	noexcept;
		///Creates TCP client with given IP address
		///@param ip IP address
		TCPClient(IP ip)	noexcept;
		///Initializes TCP client
		///@param v6 True if IPv6, false if IPv4
		bool init(bool v6)	noexcept;
		///Initializes TCP client with given IP address
		///@param ip IP address
		bool init(IP ip)	noexcept;
		///Returns if TCP client is ok
		bool ok()			const noexcept;
		///Returns IP address of TCP client
		IP ip()				const noexcept;
		///Connects TCP client to server @n
		///@param ip TCP server IP
		bool connect(IP ip, size_t mstimeout = (size_t)-1)						noexcept;
		///Returns IP address of connected TCP server
		IP server()																const noexcept;
		///Sends data to server
		///@param data Data to send
		///@param size Size of data to send
		///@param mstimeout Timeout in milliseconds
		bool send(const void *data, size_t size, size_t mstimeout = (size_t)-1)	noexcept;
		///Receives data from server
		///@param data Data to receive
		///@param size Size of data to receive
		///@param mstimeout Timeout in milliseconds
		bool receive(void *data, size_t size, size_t mstimeout = (size_t)-1)	noexcept;
		///Receives data from server without removing it from queue
		///@param data Data to receive
		///@param size Size of data to receive
		///@param mstimeout Timeout in milliseconds
		bool pick(void *data, size_t size, size_t mstimeout = (size_t)-1)		noexcept;
		///Finalizes TCP client and frees resources
		void finalize()		noexcept;
		///Destroys TCP client
		~TCPClient()		noexcept;
	};
	
	class TCPServer
	{
	private:
		#ifndef _WIN32
			typedef int SOCKET;
			static const int INVALID_SOCKET = -1;
			static const int SOCKET_ERROR = -1;
		#endif

		struct Client
		{
			IP ip;
			SOCKET socket;
		};

		IP _ip;
		QuietVector<Client> _clients;
		SOCKET _socket = INVALID_SOCKET;

	public:
		///Creates empty TCP server
		TCPServer()									noexcept;
		///Creates TCP server
		///@param ip IP address of server
		TCPServer(IP ip)							noexcept;
		///Initializes TCP server
		///@param ip IP address of server
		bool init(IP ip)							noexcept;
		///Returns if TCP server is ok
		bool ok()									const noexcept;
		///Returns IP address of TCP server
		IP ip()										const noexcept;
		///Accepts connect request from clients
		bool accept(size_t mstimeout = (size_t)-1)	noexcept;
		///Returns client number
		size_t clinet_number()						const noexcept;
		///Returns client's IP addresses
		///@param i Client's number
		IP client(size_t i)							const noexcept;
		///Receives data from client
		///@param i Client's number
		///@param data Data to send
		///@param size Size of data to send
		///@param mstimeout Timeout in milliseconds
		bool send(size_t i, const void *data, size_t size, size_t mstimeout = (size_t)-1)	noexcept;
		///Sends data to client
		///@param i Client's number
		///@param data Data to receive
		///@param size Size of data to receive
		///@param mstimeout Timeout in milliseconds
		bool receive(size_t i, void *data, size_t size, size_t mstimeout = (size_t)-1)		noexcept;
		///Receives data from client without removing it from queue
		///@param i Client's number
		///@param data Data to receive
		///@param size Size of data to receive
		///@param mstimeout Timeout in milliseconds
		bool pick(size_t i, void *data, size_t size, size_t mstimeout = (size_t)-1)			noexcept;
		///Finalizes TCP server and frees resources
		void finalize()	noexcept;
		///Destroys TCP server
		~TCPServer()	noexcept;
	};
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_TCP) : !defined(IR_EXCLUDE_TCP)
	#ifndef IR_INCLUDE

	#elif IR_INCLUDE == 'a'
		#ifndef IR_TCP_SOURCE
			#define IR_TCP_SOURCE
			#include "../../source/tcp.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_TCP