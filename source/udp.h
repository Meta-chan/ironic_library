/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef _WIN32
	#include <unistd.h>
	#include <fcntl.h>
#endif

ir::UDP::UDP() noexcept
{
	IP::init();
}

ir::UDP::UDP(bool v6) noexcept
{
	IP::init();
	init(v6);
}

ir::UDP::UDP(IP ip) noexcept
{
	assert(ip.ok());
	init(ip);
}

bool ir::UDP::init(bool v6) noexcept
{
	finalize();

	//Create socket
	_socket = socket(v6 ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_socket == INVALID_SOCKET) return false;

	//Set non-blocking mode
	#ifdef _WIN32
		unsigned long nonblocking = 1;
		if (ioctlsocket(_socket, FIONBIO, &nonblocking) == SOCKET_ERROR) { finalize(); return false; }
	#else
		int options = fcntl(_socket, F_GETFL, NULL);
		if (options < 0) { finalize(); return false; }
		if (fcntl(_socket, F_SETFL, options | O_NONBLOCK) < 0) { finalize(); return false; }
	#endif

	return true;
}

bool ir::UDP::init(IP ip) noexcept
{
	finalize();
	assert(ip.ok());

	//Create socket
	_ip = ip;
	_socket = socket(ip.v6() ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_socket == INVALID_SOCKET) return false;

	//Bind socket
	if (bind(_socket, ip.ip(), ip.size()) == SOCKET_ERROR) { finalize(); return false; }

	//Set non-blocking mode
	#ifdef _WIN32
		unsigned long nonblocking = 1;
		if (ioctlsocket(_socket, FIONBIO, &nonblocking) == SOCKET_ERROR) { finalize(); return false; }
	#else
		int options = fcntl(_socket, F_GETFL, NULL);
		if (options < 0) { finalize(); return false; }
		if (fcntl(_socket, F_SETFL, options | O_NONBLOCK) < 0) { finalize(); return false; }
	#endif

	return true;
}

bool ir::UDP::ok() const noexcept
{
	return _socket != INVALID_SOCKET;
}

ir::IP ir::UDP::ip() const noexcept
{
	assert(ok());
	if (_ip.ok()) return _ip;
	sockaddr_in6 address;
	socklen_t address_size = sizeof(sockaddr_in6);
	if (getsockname(_socket, (sockaddr*)&address, &address_size) == SOCKET_ERROR) return IP();
	else
	{
		_ip = IP((sockaddr*)&address);
		return _ip;
	}
}

bool ir::UDP::send(IP ip, const void *data, size_t size, size_t mstimeout) noexcept
{
	assert(ok());

	//Wait for write access
	fd_set write_set;
	FD_ZERO(&write_set);
	FD_SET(_socket, &write_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)(_socket + 1), nullptr, &write_set, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	//Send data
	if (sendto(_socket, (const char*)data, (int)size, 0, ip.ip(), ip.size()) < size) return false;

	return true;
}

bool ir::UDP::receive(IP *ip, void *data, size_t *size, size_t mstimeout) noexcept
{
	assert(ok());

	//Wait for read access
	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(_socket, &read_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)(_socket + 1), &read_set, nullptr, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	//Receive data
	sockaddr_in6 address;
	socklen_t address_size = sizeof(sockaddr_in6);
	int received = recvfrom(_socket, (char*)data, (int)*size, 0, (sockaddr*)&address, &address_size);
	if (received == SOCKET_ERROR) return false;
	if (ip != nullptr) *ip = IP((sockaddr*)&address);
	*size = received;

	return true;
}

bool ir::UDP::receive(IP *ip, void *data, size_t size, size_t mstimeout) noexcept
{
	return receive(ip, data, &size, mstimeout);
}

bool ir::UDP::pick(IP *ip, void *data, size_t *size, size_t mstimeout) noexcept
{
	assert(ok());

	//Wait for read access
	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(_socket, &read_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)(_socket + 1), &read_set, nullptr, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	//Receive data
	sockaddr_in6 address;
	socklen_t address_size = sizeof(sockaddr_in6);
	int received = recvfrom(_socket, (char*)data, (int)*size, MSG_PEEK, (sockaddr*)&address, &address_size);
	if (received == SOCKET_ERROR) return false;
	if (ip != nullptr) *ip = IP((sockaddr*)&address);
	*size = received;

	return true;
}

bool ir::UDP::pick(IP *ip, void *data, size_t size, size_t mstimeout) noexcept
{
	return pick(ip, data, &size, mstimeout);
}

void ir::UDP::finalize() noexcept
{
	if (_socket != INVALID_SOCKET)
	{
		#ifdef _WIN32
			shutdown(_socket, SD_BOTH);
			closesocket(_socket);
		#else
			shutdown(_socket, SHUT_RDWR);
			close(_socket);
		#endif
		_socket = INVALID_SOCKET;
	}
}

ir::UDP::~UDP() noexcept
{
	finalize();
}