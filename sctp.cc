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
#include "sctp.hh"
#include "resolve.hh"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <iostream>

std::unique_ptr<Listener> SctpProber::listen(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << name() << " listen <ip> <port>" << std::endl;
		return std::unique_ptr<Listener>(nullptr);
	}

	int fd;


	if ((fd = resolve_socket_bind(argv[0], argv[1], SOCK_STREAM, IPPROTO_SCTP)) == -1)
		return NULL;

	if (::listen(fd, 8) == -1) {
		std::cerr << name() << " listen: " << strerror(errno) << std::endl;
		return std::unique_ptr<Listener>(nullptr);
	}

	return std::make_unique<Listener>(fd);
}


std::unique_ptr<ConnectionProbe> SctpProber::connect(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << name() << " listen <ip> <port>" << std::endl;
		return std::unique_ptr<ConnectionProbe>(nullptr);
	}

	int fd;

	if ((fd = resolve_socket_connect(argv[0], argv[1], SOCK_STREAM, IPPROTO_SCTP)) == -1)
		return NULL;

	const int one = 1;
	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1) {
		std::cerr << name() << " SO_REUSEADDR: " << strerror(errno) << std::endl;
	}

	return std::make_unique<ConnectionProbe>(fd);
}
