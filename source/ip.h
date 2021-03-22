/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifdef _WIN32
	#include <ws2tcpip.h>
	#include <iphlpapi.h>
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "iphlpapi.lib")
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <ifaddrs.h>
#endif
#include <assert.h>
#include <string.h>

#ifdef _WIN32
	bool ir::IP::_initialized = false;
#endif
ir::uint32 ir::IP::_flow_info = 0;

//If you are reading this, you shall know...
//I have no idea what interface does Windows or Linux uses to connect to Internet
//I have no idea what is default interface
//I have no idea what you may want
//I could have got Internet interface through accessing to 8.8.8.8
//I could have made a possibility to specify interface
//But come on, it is Ironic!
//And I have never seen IPv6 in my life.
//IPv6 fucntions might work, but they are not tested. It is all I can do to speed up IPv6 era.
//Good luck!

bool ir::IP::init() noexcept
{
	#ifdef _WIN32
		if (_initialized) return true;
		WSADATA wsadata;
		if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) return false;
		_initialized = true;
	#endif
	return true;
}

void ir::IP::finalize() noexcept
{
	#ifdef _WIN32
		_initialized = false;
		WSACleanup();
	#endif
}

ir::IP::IP() noexcept
{
	memset(this, 0, sizeof(IP));
	init();
}

ir::IP::IP(bool global, bool ip6, unsigned short port) noexcept
{
	memset(this, 0, sizeof(IP));
	if (!init()) return;
	if (!global)
	{
		if (!ip6)
		{
			_ip4.sin_family = AF_INET;
			_ip4.sin_port = htons(port);
			inet_pton(AF_INET, "127.0.0.1", &_ip4.sin_addr);
			_ok = true;
		}
		else
		{
			_ip6.sin6_family = AF_INET6;
			_ip6.sin6_port = htons(port);
			_ip6.sin6_flowinfo = _flow_info++;
			inet_pton(AF_INET, "::1", &_ip6.sin6_addr);
			#ifdef _WIN32
				_ip6.sin6_scope_struct.Zone = 0;
				_ip6.sin6_scope_struct.Level = SCOPE_LEVEL::ScopeLevelGlobal;
			#else
				//?
			#endif
			_ok = true;
		}
	}
	else
	{
		#ifdef _WIN32
			char buffer[16 * 1024];
			IP_ADAPTER_ADDRESSES *addresses = (IP_ADAPTER_ADDRESSES*)buffer;
			unsigned long buffer_size = 16 * 1024;
			if (GetAdaptersAddresses(ip6 ? AF_INET6 : AF_INET, 0, nullptr, addresses, &buffer_size) != NO_ERROR) return;
			for (IP_ADAPTER_ADDRESSES *address = addresses; !_ok && address != nullptr; address = address->Next)
			{
				if (address->OperStatus != IF_OPER_STATUS::IfOperStatusUp) continue;
				for (IP_ADAPTER_UNICAST_ADDRESS_LH *unicast = address->FirstUnicastAddress; unicast != nullptr; unicast = unicast->Next)
				{
					if (!ip6 && unicast->Address.lpSockaddr->sa_family == AF_INET)
					{
						_ip4 = *((sockaddr_in*)unicast->Address.lpSockaddr);
						_ip4.sin_port = htons(port);
						_ok = true;
						break;
					}
					else if (ip6 && unicast->Address.lpSockaddr->sa_family == AF_INET6)
					{
						_ip6 = *((sockaddr_in6*)unicast->Address.lpSockaddr);
						_ip6.sin6_port = htons(port);
						_ip6.sin6_scope_struct.Zone = 0;
						_ip6.sin6_scope_struct.Level = SCOPE_LEVEL::ScopeLevelGlobal;
						_ok = true;
						break;
					}
				}
			}
		#else
			ifaddrs *addresses;
			if (getifaddrs(&addresses) < 0) return;
			for (ifaddrs *address = addresses; !_ok && address != nullptr; address = address->ifa_next)
			{
				if ((address->ifa_flags & IFF_UP) == 0) continue;
				if ((address->ifa_flags & IFF_LOOPBACK) != 0) continue;
				if (!ip6 && address->ifa_addr->sa_family == AF_INET)
				{
					_ip4 = *((sockaddr_in*)address->ifa_addr);
					_ip4.sin_port = htons(port);
					_ok = true;
					break;
				}
				else if (ip6 && address->ifa_addr->sa_family == AF_INET6)
				{
					_ip6 = *((sockaddr_in6*)address->ifa_addr);
					_ip6.sin6_port = htons(port);
					_ok = true;
					break;
				}
			}
			freeifaddrs(addresses);
		#endif
	}
}

ir::IP::IP(const sockaddr *ip) noexcept
{
	memset(this, 0, sizeof(IP));
	if (!init()) return;
	if (ip->sa_family == AF_INET) { _ip4 = *((sockaddr_in*)ip); _ok = true; }
	else if (ip->sa_family == AF_INET6) { _ip6 = *((sockaddr_in6*)ip); _ok = true; }
}

ir::IP::IP(sockaddr_in address) noexcept
{
	memset(this, 0, sizeof(IP));
	if (!init()) return;
	assert(address.sin_family == AF_INET);
	_ip4 = address;
	_ok = true;
}

ir::IP::IP(sockaddr_in6 address) noexcept
{
	memset(this, 0, sizeof(IP));
	if (!init()) return;
	assert(address.sin6_family == AF_INET6);
	_ip6 = address;
	_ok = true;
}

ir::IP::IP(const char *address, bool lookup, bool ip6, unsigned short port) noexcept
{
	memset(this, 0, sizeof(IP));
	if (!init()) return;
	if (!lookup)
	{
		if (!ip6)
		{
			_ip4.sin_family = AF_INET;
			if (inet_pton(AF_INET, address, &_ip4.sin_addr) != 1) return;
			_ip4.sin_port = htons(port);
			_ok = true;
		}
		else
		{
			_ip4.sin_family = AF_INET6;
			if (inet_pton(AF_INET6, address, &_ip6.sin6_addr) != 1) return;
			_ip6.sin6_port = htons(port);
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
			if (getaddrinfo(address, "80", &hints, &result) != 0) return;
			for (addrinfo *a = result; a != nullptr; a = a->ai_next)
			{
				if (a->ai_addr->sa_family != AF_INET)
				{
					_ip4 = *((sockaddr_in*)a->ai_addr);
					_ip4.sin_port = htons(port);
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
			if (getaddrinfo(address, "80", &hints, &result) != 0) return;
			for (addrinfo *a = result; a != nullptr; a = a->ai_next)
			{
				if (a->ai_addr->sa_family != AF_INET6)
				{
					_ip6 = *((sockaddr_in6*)a->ai_addr);
					_ip6.sin6_port = htons(port);
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

bool ir::IP::v6() const noexcept
{
	assert(_ok);
	return _ip6.sin6_family == AF_INET6;
}

sockaddr_in ir::IP::ip4() const noexcept
{
	assert(_ok);
	assert(_ip4.sin_family == AF_INET);
	return _ip4;
}

sockaddr_in6 ir::IP::ip6() const noexcept
{
	assert(_ok);
	assert(_ip6.sin6_family == AF_INET6);
	return _ip6;
}

const sockaddr *ir::IP::ip() const noexcept
{
	assert(_ok);
	return (const sockaddr*)&_ip4;
}

int ir::IP::size() const noexcept
{
	assert(_ok);
	return _ip6.sin6_family == AF_INET6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
}

ir::int32 ir::IP::compare(IP ip) const noexcept
{
	if (!ok() && !ip.ok()) return 0;
	else if (ok() && !ip.ok()) return 1;
	else if (!ok() && ip.ok()) return -1;
	else if (!v6() && !ip.v6())
	{
		return memcmp(&_ip4, &ip._ip4, sizeof(sockaddr_in));
	}
	else if (v6() && !ip.v6()) return 1;
	else if (!v6() && ip.v6()) return -1;
	else
	{
		return memcmp(&_ip6, &ip._ip6, sizeof(sockaddr_in6));
	}
}

bool ir::operator==(IP ip1, IP ip2) noexcept
{
	return ip1.compare(ip2) == 0;
}