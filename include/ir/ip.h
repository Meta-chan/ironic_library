/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_IP
#define IR_IP

#include "types.h"
#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2ipdef.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif

namespace ir
{
///@addtogroup net Networking
///@{
	
	///IPv4 or IPv6 address
	class IP
	{
	private:
#ifdef _WIN32
		static bool _initialized;
#endif
		static uint32 _flow_info;

		union
		{
			sockaddr_in _ip4;
			sockaddr_in6 _ip6;
		};
		bool _ok = false;

	public:
		static bool init()		noexcept;
		static void finalize()	noexcept;

		///Creates default (invalid) IP address
		IP()														noexcept;
		///Creates IP address of device
		///@param global `false` to create local address, `true` to create global address
		///@param ip6 `true` to create IPv6 address, `false` to create IPv4 address
		///@param port TCP port
		IP(bool global, bool ip6, uint16 port)						noexcept;
		///Creates IP address from UNIX `sockaddr` pointer with `sin_family` of `AF_INET` or `AF_INET6`
		IP(const sockaddr *ip)										noexcept;
		///Creates IPv4 address from UNIX `sockaddr_in`
		IP(sockaddr_in address)										noexcept;
		///Creates IPv6 address from UNIX `sockaddr_in6`
		IP(sockaddr_in6 address)									noexcept;
		///Creates IP address from string representation
		///@param address String representation of IP
		///@param lookup If true, URI can be passed as @c address, DNS lookup will be performed
		///@param ip6 @c true to create IPv6 address, false to create IPv4 address
		///@param port TCP port
		IP(const char *address, bool lookup, bool ip6, uint16 port)	noexcept;
		///Returns if address was created successfully
		bool ok()													const noexcept;
		///Returns if address is IPv6 address
		bool v6()													const noexcept;
		///Returns UNIX `sockaddr_in` for IPv4 address
		sockaddr_in ip4()											const noexcept;
		///Returns UNIX `sockaddr_in6` for IPv6 address
		sockaddr_in6 ip6()											const noexcept;
		///Returns pointer to UNIX `sockaddr`
		const sockaddr *ip()										const noexcept;
		///Returns size of UNIX `sockaddr`
		int size()													const noexcept;
		///Compares address with another address
		int32 compare(IP ip)										const noexcept;
	};

	///Compares two IP addresses
	bool operator==(IP ip1, IP ip2)									noexcept;
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_IP) : !defined(IR_EXCLUDE_IP)
	#ifndef IR_INCLUDE

	#elif IR_INCLUDE == 'a'
		#ifndef IR_IP_SOURCE
			#define IR_IP_SOURCE
			#include "../../source/ip.h"
		#endif
	#endif
#endif

#endif	///#ifndef IR_IP
