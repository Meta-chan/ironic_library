/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include "ip.h"

#ifndef IR_UDP
#define IR_UDP

namespace ir
{	
	class UDP
	{
	private:
		mutable IP _ip;
		SOCKET _socket = INVALID_SOCKET;

	public:
		///Creates empty UDP socket
		UDP()				noexcept;
		///Creates UDP socket
		///@param v6 True if IPv6, false if IPv4
		UDP(bool v6)		noexcept;
		///Creates UDP socket with given IP
		///@param ip IP address
		UDP(IP ip)			noexcept;
		///Creates UDP socket
		///@param v6 True if IPv6, false if IPv4
		bool init(bool v6)	noexcept;
		///Creates UDP socket with given IP
		///@param ip IP address
		bool init(IP ip)	noexcept;
		///Returns if UDP socket is ok
		bool ok()			const noexcept;
		///Returns IP address of UDP socket
		IP ip()				const noexcept;
		///Send data to another UDP socket
		///@param ip IP of UDP socket to send data to
		///@param data Data to send
		///@param size Size of data to send
		///@param mstimeout Timeout in milliseconds
		bool send(IP ip, const void *data, size_t size, size_t mstimeout = (size_t)-1)	noexcept;
		///Receives data
		///@param ip Buffer to receive IP address
		///@param data Buffer to receive data
		///@param size Buffer to store buffer size and receive data size
		///@param mstimeout Timeout in milliseconds
		bool receive(IP *ip, void *data, size_t *size, size_t mstimeout = (size_t)-1)	noexcept;
		///Receives data from UDP socket without removing it from queue
		///@param ip Buffer to receive IP address
		///@param data Buffer to receive data
		///@param size Buffer to store buffer size and receive data size
		///@param mstimeout Timeout in milliseconds
		bool pick(IP *ip, void *data, size_t *size, size_t mstimeout = (size_t)-1)		noexcept;
		///Finalizes UDP socket and frees data
		void finalize()		noexcept;
		///Destroys UDP socket
		~UDP()				noexcept;
	};
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_UDP) : !defined(IR_EXCLUDE_UDP)
	#ifndef IR_INCLUDE

	#elif IR_INCLUDE == 'a'
		#ifndef IR_UDP_SOURCE
			#define IR_UDP_SOURCE
			#include "../../source/udp.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_UDP