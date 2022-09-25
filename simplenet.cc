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
#include "dccp.hh"
#include "mptcp.hh"
#include "sctp.hh"
#include "tcp.hh"
#include "udp.hh"
#include "udplite.hh"
#include "resolve.hh"
#include <memory>
#include <vector>
#include <optional>
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;

Prober * find_prober(vector<unique_ptr<Prober>> &probers, string_view name) {
	for(auto &it: probers) {
		if (it->name() == name) {
			return &*it;
		}
	}
	return nullptr;
}


int main(int argc, char *argv[]) {

	vector<unique_ptr<Prober>> probers{};
	probers.push_back(make_unique<TcpProber>());
	probers.push_back(make_unique<UdpProber>());
	probers.push_back(make_unique<DccpProber>());
	probers.push_back(make_unique<SctpProber>());
	probers.push_back(make_unique<UdpLiteProber>());
	probers.push_back(make_unique<MpTcpProber>());

	if (argc < 3) {
		cerr << "usage: " << argv[0] << " <protocol> listen|connect ..." << endl;
		cerr << "known protocols:";
		for(auto &it: probers) {
			cerr << " " << it->name();
		}
		cerr << endl;
		return 1;
	}

	Prober *pr = find_prober(probers, argv[1]);

	if (!pr) {
		cerr << "could not find prober called " << argv[1] << endl;
		return 1;
	}

	if (strcmp(argv[2], "listen") == 0) {
		if (ConnectionOrientated *conn = pr->as_connection()) {
			unique_ptr<Listener> l = conn->listen(argc-3, &argv[3]);
			if (!l)
				return 1;

			struct sockaddr_storage ss;
			socklen_t socklen = sizeof(ss);

			unique_ptr<ConnectionProbe> p = l->accept((struct sockaddr *)&ss, &socklen);

			/* TOOD: Report the connecting address */

			char buffer[1024];
			int len;
			while ((len = p->recv(buffer, sizeof(buffer))) > 0) {
				p->send(buffer, len);
			}
			return len == -1 ? 1 : 0;
		} else if (Connectionless *connless = pr->as_connectionless()) {
			unique_ptr<ConnectionlessProbe> p = connless->listen(argc-3, &argv[3]);
			if (!p)
				return 1;
			char buffer[1024];
			struct sockaddr_storage addr;
			socklen_t addrlen = sizeof(addr);
			int len = p->recv(buffer, sizeof(buffer), (struct sockaddr *)&addr, &addrlen);
			/* TODO: Report the connecting address */
			if (len != -1) {
				p->send(buffer, len, (struct sockaddr *)&addr, addrlen);
			}
			return 0;
		} else {
			cerr << "Don't know how to drive this protocol" << endl;
		}
	} else if (strcmp(argv[2], "connect") == 0) {
		if (ConnectionOrientated *conn = pr->as_connection()) {
			unique_ptr<ConnectionProbe> p = conn->connect(argc-3, &argv[3]);
			if (!p)
				return 1;

			char buffer[1024];
			while (fgets(buffer, sizeof(buffer), stdin)) {
				int len = p->send(buffer, strlen(buffer));
				if (len == -1) {
					cerr << "send: " << strerror(errno) << endl;
					return 1;
				} else if (len != (int)strlen(buffer)) {
					cerr << "sent " << len << "/" << strlen(buffer) << " bytes" << endl;
					return 1;
				}

				len = p->recv(buffer, sizeof(buffer));
				if (len == -1) {
					cerr << "failed to recv: " << strerror(errno) << endl;
					return 1;
				}
				write(1, buffer, len);
			}
			return 0;
		} if (Connectionless *connless = pr->as_connectionless()) {
			unique_ptr<ConnectionlessProbe> p = connless->create(argc-3, &argv[3]);
			if (!p)
				return 1;

			char msg[] = "Hello World\n";

			int len = resolve_send(msg, strlen(msg), *p, argv[3], argv[4]);

			char buffer[1024];
			struct sockaddr_storage sa;
			socklen_t salen = sizeof(sa);
			len = p->recv(buffer, sizeof(buffer), (sockaddr*) &sa, &salen);
			if (len == -1) {
				cerr << "recv: " << strerror(errno) << endl;
				return 1;
			}
			write(1, buffer, len);

		} else {
			cerr << "Don't know how to drive this protocol" << endl;
		}
	} else {
		cerr << "Unknown driver " << argv[2] << endl;
		return 1;
	}
}
