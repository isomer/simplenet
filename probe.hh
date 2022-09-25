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
#ifndef PROBE_H
#define PROBE_H 1
#include <memory>
#include <sys/socket.h>
#include <string>

class BaseConnectionProbe {
public:
	virtual int send(void *data, size_t len) = 0;
	virtual int recv(void *data, size_t len) = 0;
};

class ConnectionProbe : BaseConnectionProbe {
protected:
	int fd_;
public:
	ConnectionProbe(int fd) : fd_(fd) {}
	int send(void *data, size_t len);
	int recv(void *data, size_t len);

	virtual ~ConnectionProbe();
};

class ConnectionlessProbe {
protected:
	int fd_;

public:
	ConnectionlessProbe(int fd) : fd_(fd) {}
	virtual int send(void *data, size_t len, struct sockaddr *sa, socklen_t socklen);
	virtual int recv(void *data, size_t len, struct sockaddr *sa, socklen_t *socklen);

	virtual ~ConnectionlessProbe();
};

class Listener {
protected:
	int fd_;
public:
	Listener(int fd) : fd_(fd) {};
	virtual std::unique_ptr<ConnectionProbe> accept(struct sockaddr *addr, socklen_t *len);
	virtual ~Listener();
};

class ConnectionOrConnectionless {
public:
	virtual class ConnectionOrientated *as_connection() { return nullptr; }
	virtual class Connectionless *as_connectionless() { return nullptr; }
};

class ConnectionOrientated : virtual public ConnectionOrConnectionless {
public:
	virtual std::unique_ptr<ConnectionProbe> connect(int argc, char *argv[]) = 0;
	virtual std::unique_ptr<Listener> listen(int argc, char *argv[]) = 0;
	ConnectionOrientated *as_connection() { return this; }
};

class Connectionless : virtual public ConnectionOrConnectionless {
public:
	virtual std::unique_ptr<ConnectionlessProbe> create(int argc, char *argv[]) = 0;
	virtual std::unique_ptr<ConnectionlessProbe> listen(int argc, char *argv[]) = 0;
	Connectionless *as_connectionless() { return this; }
};

class Prober : virtual public ConnectionOrConnectionless {
protected:
	int features;
public:
	virtual std::string &name() = 0;
	virtual ~Prober();
};

#endif
