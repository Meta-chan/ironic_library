/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_UDP_IMPLEMENTATION
#define IR_UDP_IMPLEMENTATION

ir::UDP::_close() noexcept
{
	#ifdef _WIN32
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	#else
		close(_socket);
		_socket = -1;
	#endif
}

ir::UDP::UDP(bool v6) noexcept
{
	if (!IP::init()) return;
	_socket = socket(v6 ? AF_NET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

ir::UDP::UDP(IP ip) noexcept
{
	if (!IP::init()) return;
	_socket = socket(ip.v6() ? AF_NET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (!ok()) return;
	bind(_socket, ip.ip(), ip.ip_size());
}

bool ir::UDP::ok() const noexcept
{
	#ifdef _WIN32
		return _socket != INVALID_SOCKET;
	#else
		return _socket >= 0;
	#endif
}

IP ir::UDP::ip() const noexcept
{
	if (!ok()) return IP();
	sockaddr_in6 name;
	int namelen = sizeof(sockaddr_in6);
	if (getsockname(_socket, (sockaddr*)&name, &namelen) != 0)
	{
		_close();
		return IP();
	}
	IP result = IP(name);
	if (!result.ok()) _close();
	return result;
}

bool ir::UDP::send(IP ip, const void *data, size_t size, unsigned int mstimeout = (unsigned int)-1) noexcept
{
	if (!ok()) return false;
	return sendto(_socket, data, (int)size, 0, ip.ip(), ip.ip_size()) == size;
	
}

bool ir::UDP::pick(IP ip, void *data, size_t size, unsigned int mstimeout) noexcept
{
	
}

bool pick(void *data, size_t size, unsigned int mstimeout) noexcept
{}

bool receive(IP ip, void *data, size_t size, unsigned int mstimeout) noexcept
{
	if (!ok()) return false;
	if (mstimeout == 0)
	{
		
	}
	else if (mstimeout == (unsigned int)-1)
	{}
	else
	{
		FD_SET set;
		FD_ZERO(&set);
		FD_SET(_socket, &set);
		TIMEVAL time;
		time.tv_sec = 0;
		time.tv_usec = 1000 * mstimeout;
	}
}

bool receive(void *data, size_t size, unsigned int mstimeout) noexcept
{}

ir::UDP::~UDP() noexcept
{
	if (ok()) _close();
}

#if (defined(IR_IMPLEMENT) || defined(IR_UDP_IMPLEMENT)) && defined(IR_UDP_NOT_IMPLEMENT)
	#include "../implementation/ir_net/ir_udp_implementation.h"
#endif

#endif	//#ifndef IR_UDP_IMPLEMENTATION