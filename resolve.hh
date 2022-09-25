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
#ifndef RESOLVE_HH
#define RESOLVE_HH 1
#include <string>
#include "probe.hh"
#include <netdb.h>

class ResolveAction {
public:
	virtual int apply(addrinfo &ai) = 0;
};

int resolve(const char *hostname, const char *port, int socktype, int protocol, ResolveAction &action);

int resolve_socket_bind(const char *hostname, const char *port, int socktype, int protocol);
int resolve_socket_connect(const char *hostname, const char *port, int socktype, int protocol);
int resolve_send(const void *buffer, size_t len, ConnectionlessProbe &probe, const char *hostname, const char *port);

#endif
