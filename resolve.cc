/* Copyright 2022 Perry Lorier
 *
 *   Licensed under the Apache License, Version 2.0 (the "License"); you may
 *   not use this file except in compliance with the License.  You may obtain a
 *   copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "resolve.hh"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

class BindAction : public ResolveAction {
public:
	int apply(addrinfo &ai);
};

int BindAction::apply(addrinfo &ai) {
	int fd = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
	if (fd == -1) {
		std::cerr << "socket(" << ai.ai_family << "," << ai.ai_socktype << "," << ai.ai_protocol << "): " << strerror(errno) << std::endl;
		return -1;
	}
	const int one = 1;
	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1) {
		std::cerr << "SO_REUSEADDR: " << strerror(errno) << std::endl;
	}

	if (bind(fd, ai.ai_addr, ai.ai_addrlen) == -1) {
		std::cerr << "bind: " << strerror(errno) << std::endl;
		return -1;
	}
	return fd;
}

int resolve_socket_bind(const char *hostname, const char *port, int socktype, int protocol) {
	BindAction action{};
	return resolve(hostname, port, socktype, protocol, action);
}

class ConnectAction: public ResolveAction {
public:
	int apply(addrinfo &ai);
};

int ConnectAction::apply(addrinfo &ai) {
	int fd = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
	if (fd == -1) {
		std::cerr << "socket(" << ai.ai_family << "," << ai.ai_socktype << "," << ai.ai_protocol << "): " << strerror(errno) << std::endl;
		return -1;
	}
	if (connect(fd, ai.ai_addr, ai.ai_addrlen) == -1) {
		std::cerr << "bind: " << strerror(errno) << std::endl;
		return -1;
	}
	return fd;
}

int resolve_socket_connect(char const *hostname, char const *port, int socktype, int protocol) {
	ConnectAction action{};
	return resolve(hostname, port, socktype, protocol, action);
}

class SendAction : public ResolveAction {
	const void *buffer_;
	size_t len_;
	ConnectionlessProbe &probe_;
public:
	SendAction(const void *buffer, size_t len, ConnectionlessProbe &probe)
		: buffer_(buffer), len_(len), probe_(probe)
	{};
	int apply(addrinfo &ai);
};

int SendAction::apply(addrinfo &ai) {
	return probe_.send((void*)buffer_, len_, ai.ai_addr, ai.ai_addrlen);
}

int resolve_send(const void *buffer, size_t len, ConnectionlessProbe &probe, const char *hostname, const char *port) {
	SendAction action(buffer, len, probe);
	return resolve(hostname, port, SOCK_DGRAM, 0, action);
}

int resolve(const char *hostname, const char *port, int socktype, int protocol, ResolveAction &action) {
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints = (addrinfo) {
		.ai_flags = 0,
		.ai_family = AF_UNSPEC,
		.ai_socktype = socktype,
		.ai_protocol = protocol,
		.ai_addrlen = 0,
		.ai_addr = nullptr,
		.ai_canonname = nullptr,
		.ai_next = nullptr,

	};
	addrinfo *res;
	int ret;
	switch (ret = getaddrinfo(hostname, port, &hints, &res)) {
		case 0:
			break; /* success */
		case EAI_SYSTEM:
			cerr << "getaddrinfo(" << hostname << "," << port << "): " << strerror(errno) << endl;
			return -1;
		default:
			cerr << "getaddrinfo(" << hostname << "," << port << "): " << gai_strerror(ret) << endl;
	}

	for(addrinfo *it = res; it != nullptr; it = it->ai_next) {
		ret = action.apply(*it);
		if (ret >= 0)
			return ret;
	}
	return -1;
}

