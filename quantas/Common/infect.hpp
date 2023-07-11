/*
Copyright 2023

This file is part of QUANTAS.
QUANTAS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
QUANTAS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with QUANTAS. If not, see <https://www.gnu.org/licenses/>.
*/

// This set of functions handles "infecting" a peer.
// An infected peer is simply a peer that has modified behavior.
// The modified behavior may be in accordance with the protocol, or it may not.
// In the latter case, the peer is said to be Byzantine.

#ifndef infect_hpp
#define infect_hpp

#include <chrono>
#include <thread>
#include <fstream>

#include "Simulation.hpp"
#include "Peer.hpp"
#include "LogWriter.hpp"
#include "BS_thread_pool.hpp"


using std::ofstream;
using std::thread;

namespace quantas {
	template<class type_msg>

	void crash(Peer<type_msg>* peer) {
		auto doNothing = [](Peer<type_msg>* p) {};
		peer->replaceWrapper(doNothing);
	}
}

#endif /* infect_hpp */
