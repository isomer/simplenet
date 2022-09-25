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
#include "probe.hh"
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;

Prober::~Prober() {}

int ConnectionProbe::send(void *data, size_t len) {
	return write(fd_, data, len);
}

int ConnectionProbe::recv(void *data, size_t len) {
	return read(fd_, data, len);
}

ConnectionProbe::~ConnectionProbe() { close(fd_); }

int ConnectionlessProbe::send(void *data, size_t len, struct sockaddr *sa, socklen_t socklen) {
	return sendto(fd_, data, len, 0, sa, socklen);
}

int ConnectionlessProbe::recv(void *data, size_t len, struct sockaddr *sa, socklen_t *socklen) {
	int ret = ::recvfrom(fd_, data, len, 0, sa, socklen);
	if (ret == -1)
		cerr << "read: " << strerror(errno) << endl;
	return ret;
}

ConnectionlessProbe::~ConnectionlessProbe() { close(fd_); }

std::unique_ptr<ConnectionProbe> Listener::accept(struct sockaddr *addr, socklen_t *len) {
	int new_fd;
	if ((new_fd = ::accept(fd_, addr, len)) == -1) {
		cerr << "accept: " << strerror(errno) << endl;
		return std::unique_ptr<ConnectionProbe>(nullptr);
	}

	return std::make_unique<ConnectionProbe>(new_fd);
}

Listener::~Listener() { close(fd_); }
