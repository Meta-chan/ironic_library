/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_IP_IMPLEMENTATION
#define IR_IP_IMPLEMENTATION

#include <ir_md5.h>
#include <ir_container/ir_vector.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <assert.h>

ir::IP::IP() noexcept
{}

ir::IP::IP(bool global, bool ip6, unsigned short port) noexcept
{
	memset(this, 0, sizeof(IP));
	if (!global)
	{
		if (!ip6)
		{
			_ip4.sin_family = AF_INET;
			_ip4.sin_port = port;
			inet_pton(AF_INET, "127.0.0.1", &_ip4.sin_addr);
			_ok = true;
		}
		else
		{
			_ip6.sin6_family = AF_INET6;
			_ip6.sin6_port = port;
			inet_pton(AF_INET, "::1", &_ip6.sin6_addr);
			unsigned int hash[4];
			md5(&_ip6.sin6_addr, sizeof(IN6_ADDR) * 8, hash);	//No collision check yet. Should use map
			_ip6.sin6_flowinfo = hash[0];
			_ip6.sin6_scope_struct.Zone = 0;
			_ip6.sin6_scope_struct.Level = SCOPE_LEVEL::ScopeLevelGlobal;
			_ok = true;
		}
	}
	else
	{
		char buffer[16 * 1024];
		IP_ADAPTER_ADDRESSES *addresses = (IP_ADAPTER_ADDRESSES*)buffer;
		unsigned long buffer_size = 16 * 1024;
		if (GetAdaptersAddresses(AF_INET, 0, nullptr, addresses, &buffer_size) != NO_ERROR) return;
		for (IP_ADAPTER_ADDRESSES *address = addresses; !_ok && address != nullptr; address = address->Next)
		{
			//If you are reading this, you shall know...
			//I have no idea what interface does windows use to connect to Internet
			//I have no idea what is default interface
			//I have no idea what you may want
			//I could have got Internet interface through accessing to 8.8.8.8...
			//But I don't like if thing requires Internet but doesn't need it
			//I could have made a possibility to specify interface
			//But come on, it is Ironic!
			//And I have never seen IPv6 in my life.
			//The code could probably function with IPv6, but it is not tested.
			//Good luck.
			if (address->OperStatus != IF_OPER_STATUS::IfOperStatusUp) continue;
			for (IP_ADAPTER_UNICAST_ADDRESS_LH *unicast = address->FirstUnicastAddress; unicast != nullptr; unicast = unicast->Next)
			{
				if (!ip6 && unicast->Address.lpSockaddr->sa_family == AF_INET)
				{
					_ip4 = *((sockaddr_in*)unicast->Address.lpSockaddr);
					_ip4.sin_port = port;
					_ok = true;
					return;
				}
				else if (ip6 && unicast->Address.lpSockaddr->sa_family == AF_INET6)
				{
					_ip6 = *((sockaddr_in6*)unicast->Address.lpSockaddr);
					_ip6.sin6_port = port;
					_ok = true;
					return;
				}
			}
		}
	}
}

ir::IP::IP(sockaddr_in address) noexcept
{
	_ip4 = address;
	_ok = true;
}

ir::IP::IP(sockaddr_in6 address) noexcept
{
	_ip6 = address;
	_ok = true;
}

ir::IP::IP(const String address, bool lookup, bool ip6, unsigned short port) noexcept
{
	memset(this, 0, sizeof(IP));
	if (!lookup)
	{
		if (!ip6)
		{
			_ip4.sin_family = AF_INET;
			if (inet_pton(AF_INET, address.data(), &_ip4.sin_addr) != 1) return;
			_ip4.sin_port = port;
			_ok = true;
		}
		else
		{
			_ip4.sin_family = AF_INET6;
			if (inet_pton(AF_INET6, address.data(), &_ip6.sin6_addr) != 1) return;
			_ip6.sin6_port = port;
			_ok = true;
		}
	}
	else
	{
		if (!ip6)
		{
			addrinfo hints;
			memset(&hints, 0, sizeof(addrinfo));
			hints.ai_family = AF_INET;
			addrinfo *result;
			if (getaddrinfo(address.data(), "80", &hints, &result) != 0) return;
			for (addrinfo *a = result; a != nullptr; a = a->ai_next)
			{
				if (a->ai_addr->sa_family != AF_INET)
				{
					_ip4 = *((sockaddr_in*)a->ai_addr);
					_ip4.sin_port = port;
					_ok = true;
					break;
				}
			}
			freeaddrinfo(result);
		}
		else
		{
			addrinfo hints;
			memset(&hints, 0, sizeof(addrinfo));
			hints.ai_family = AF_INET6;
			addrinfo *result;
			if (getaddrinfo(address.data(), "80", &hints, &result) != 0) return;
			for (addrinfo *a = result; a != nullptr; a = a->ai_next)
			{
				if (a->ai_addr->sa_family != AF_INET6)
				{
					_ip6 = *((sockaddr_in6*)a->ai_addr);
					_ip6.sin6_port = port;
					_ok = true;
					break;
				}
			}
			freeaddrinfo(result);
		}
	}
}

bool ir::IP::ok() const noexcept
{
	return _ok;
}

bool ir::IP::ip6() const noexcept
{
	assert(_ok);
	return _ip6.sin6_family == AF_INET6;
}

sockaddr_in ir::IP::get_ip4() const noexcept
{
	assert(_ok);
	assert(_ip4.sin_family == AF_INET);
	return _ip4;
}

sockaddr_in6 ir::IP::get_ip6() const noexcept
{
	assert(_ok);
	assert(_ip6.sin6_family == AF_INET6);
	return _ip6;
}

#endif	//#ifndef IR_IP_IMPLEMENTATION