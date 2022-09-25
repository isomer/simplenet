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
#ifndef UDPLITE_H
#define UDPLITE_H 1
#include "probe.hh"
#include <memory>
#include <sys/socket.h>
#include <string>

class UdpLiteProber : public Prober, public Connectionless {
	std::string name_;
public:
	UdpLiteProber() : name_("udplite") {};
	std::string &name() { return name_; }
	std::unique_ptr<ConnectionlessProbe> listen(int argc, char *argv[]);
	std::unique_ptr<ConnectionlessProbe> create(int argc, char *argv[]);
};


#endif
