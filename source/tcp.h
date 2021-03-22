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

ir::TCPClient::TCPClient() noexcept
{
	IP::init();
}

ir::TCPClient::TCPClient(bool v6) noexcept
{
	IP::init();
	init(v6);
}

ir::TCPClient::TCPClient(IP ip) noexcept
{
	assert(ip.ok());
	init(ip);
}

bool ir::TCPClient::init(bool v6) noexcept
{
	finalize();

	//Create socket
	_socket = socket(v6 ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

bool ir::TCPClient::init(IP ip) noexcept
{
	finalize();
	assert(ip.ok());

	//Create socket
	_ip = ip;
	_socket = socket(ip.v6() ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

bool ir::TCPClient::ok() const noexcept
{
	return _socket != INVALID_SOCKET;
}

ir::IP ir::TCPClient::ip() const noexcept
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

bool ir::TCPClient::connect(IP ip, size_t mstimeout) noexcept
{
	//Does not work when server is started after client, not sure why

	assert(ok());
	_server_ip = ip;

	//Start connection
	::connect(_socket, ip.ip(), ip.size());

	//Wait for write access
	fd_set write_set;
	FD_ZERO(&write_set);
	FD_SET(_socket, &write_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)_socket + 1, nullptr, &write_set, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	return true;
}

ir::IP ir::TCPClient::server() const noexcept
{
	assert(ok());
	return _server_ip;
}

bool ir::TCPClient::send(const void *data, size_t size, size_t mstimeout) noexcept
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
	if (::send(_socket, (const char*)data, (int)size, 0) < size) return false;

	return true;
}

bool ir::TCPClient::receive(void *data, size_t size, size_t mstimeout) noexcept
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
	if (::recv(_socket, (char*)data, (int)size, 0) < size) return false;

	return true;
}

bool ir::TCPClient::pick(void *data, size_t size, size_t mstimeout) noexcept
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
	if (::recv(_socket, (char*)data, (int)size, MSG_PEEK) < size) return false;

	return true;
}

void ir::TCPClient::finalize() noexcept
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

ir::TCPClient::~TCPClient() noexcept
{
	finalize();
}

//================================================================================================================

ir::TCPServer::TCPServer() noexcept
{
}

ir::TCPServer::TCPServer(IP ip) noexcept
{
	assert(ip.ok());
	init(ip);
}

bool ir::TCPServer::init(IP ip) noexcept
{
	finalize();
	assert(ip.ok());
	_ip = ip;

	//Create socket
	_socket = socket(ip.v6() ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) return false;

	//Bind socket
	if (bind(_socket, ip.ip(), sizeof(sockaddr_in)) == SOCKET_ERROR) { finalize(); return false; }

	//Listening socket
	if (listen(_socket, 5) != 0) { finalize(); return false; }

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

bool ir::TCPServer::ok() const noexcept
{
	return _socket != INVALID_SOCKET;
}

ir::IP ir::TCPServer::ip() const noexcept
{
	assert(ok());
	return _ip;
}

bool ir::TCPServer::accept(size_t mstimeout) noexcept
{
	assert(ok());
	
	//Wait for read
	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(_socket, &read_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)(_socket + 1), &read_set, nullptr, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	//Accept
	socklen_t new_address_size = sizeof(sockaddr_in6);
	sockaddr_in6 new_address;
	SOCKET new_socket = ::accept(_socket, (sockaddr*)&new_address, &new_address_size);
	if (new_socket == INVALID_SOCKET) return false;

	//Set to non-blocking mode
	#ifdef _WIN32
		unsigned long nonblocking = 1;
		if (ioctlsocket(new_socket, FIONBIO, &nonblocking) == SOCKET_ERROR) return false;
	#else
		int options = fcntl(new_socket, F_GETFL, NULL);
		if (options < 0) return false;
		if (fcntl(new_socket, F_SETFL, options | O_NONBLOCK) < 0) return false;
	#endif

	//Add client to list
	Client new_client;
	new_client.ip = IP((sockaddr*)&new_address);
	new_client.socket = new_socket;
	return _clients.push_back(new_client);
}

size_t ir::TCPServer::clinet_number() const noexcept
{
	return _clients.size();
}

ir::IP ir::TCPServer::client(size_t i) const noexcept
{
	return _clients[i].ip;
}

bool ir::TCPServer::send(size_t i, const void *data, size_t size, size_t mstimeout) noexcept
{
	assert(ok());

	//Wait for write
	fd_set write_set;
	FD_ZERO(&write_set);
	FD_SET(_clients[i].socket, &write_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)(_clients[i].socket + 1), nullptr, &write_set, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	//Receive data
	if (::send(_clients[i].socket, (const char*)data, (int)size, 0) < size)
	{
		#ifdef _WIN32
			shutdown(_clients[i].socket, SD_BOTH);
			closesocket(_clients[i].socket);
		#else
			shutdown(_clients[i].socket, SHUT_RDWR);
			close(_clients[i].socket);
		#endif
		_clients.erase(i);
		return false;
	}

	return true;
}

bool ir::TCPServer::receive(size_t i, void *data, size_t size, size_t mstimeout) noexcept
{
	assert(ok());

	//Wait for read
	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(_clients[i].socket, &read_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)(_clients[i].socket + 1), &read_set, nullptr, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	//Receive data
	if (::recv(_clients[i].socket, (char*)data, (int)size, 0) < size) return false;

	return true;
}

bool ir::TCPServer::pick(size_t i, void *data, size_t size, size_t mstimeout) noexcept
{
	assert(ok());

	//Wait for read
	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(_clients[i].socket, &read_set);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * (long)mstimeout;
	if (select((int)(_clients[i].socket + 1), &read_set, nullptr, nullptr, (mstimeout == (size_t)-1) ? nullptr : &timeout) != 1) return false;

	//Receive data
	if (::recv(_clients[i].socket, (char*)data, (int)size, MSG_PEEK) < size) return false;

	return true;
}

void ir::TCPServer::finalize() noexcept
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		#ifdef _WIN32
			shutdown(_clients[i].socket, SD_BOTH);
			closesocket(_clients[i].socket);
		#else
			shutdown(_clients[i].socket, SHUT_RDWR);
			close(_clients[i].socket);
		#endif
	}
	_clients.clear();
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

ir::TCPServer::~TCPServer() noexcept
{
	finalize();
}