/*
Copyright 2023

This file is part of QUANTAS.
QUANTAS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
QUANTAS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with QUANTAS. If not, see <https://www.gnu.org/licenses/>.
*/

// An Infection is a function that takes a Peer and modifies its behavior.
//
// An infected peer is simply a peer that has modified behavior.
// The modified behavior may be in accordance with the protocol, or it may not.
// In the latter case, the peer is said to be Byzantine.

#ifndef infect_hpp
#define infect_hpp

#include <map>
#include <functional>
#include "Peer.hpp"
#include "../PBFTPeer/PBFTPeer.hpp"

using std::map; using std::function; using std::string;

// Here be dragons. Some of the Infections we define below deliberately do
// abnormal things, such as ignoring their paramaters.
#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace quantas {

	template<class type_msg>
	map<string, function<void(Peer<type_msg>* peer)>> infection = {

		{ "crash", [](Peer<type_msg>* peer) {
			peer->computationPerformer = [](Peer<type_msg>* peer) {};
		} },

		{ "censor", [](Peer<type_msg>* peer) {
			peer->submitTransPerformer = [](Peer<type_msg>* peer, int tranID) {};
		} },

		// pick a uniform random number of peers and send the commit message to
		// only those nodes
		{ "equivocate", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg); };
		} },

		{ "equivocate00", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .00); };
		} },

		// same as above, but send commit to 1% of peers
		{ "equivocate01", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .01); };
		} },

		{ "equivocate02", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .02); };
		} },

		{ "equivocate03", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .03); };
		} },

		{ "equivocate04", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .04); };
		} },

		{ "equivocate05", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .05); };
		} },

		{ "equivocate05", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .05); };
		} },

		{ "equivocate06", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .06); };
		} },

		{ "equivocate07", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .07); };
		} },

		{ "equivocate08", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .08); };
		} },

		{ "equivocate09", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .09); };
		} },

		{ "equivocate10", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .10); };
		} },

		{ "equivocate15", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .15); };
		} },

		{ "equivocate20", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .20); };
		} },

		{ "equivocate25", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .25); };
		} },

		{ "equivocate30", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .30); };
		} },

		{ "equivocate35", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .35); };
		} },

		{ "equivocate40", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .40); };
		} },

		{ "equivocate45", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .45); };
		} },

		// same as above, but send commit to 50% of peers
		{ "equivocate50", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .50); };
		} },

		{ "equivocate55", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .55); };
		} },

		{ "equivocate60", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .60); };
		} },

		{ "equivocate65", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .65); };
		} },

		{ "equivocate70", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .70); };
		} },

		{ "equivocate75", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .75); };
		} },

		{ "equivocate80", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .80); };
		} },

		{ "equivocate85", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .85); };
		} },

		{ "equivocate90", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .90); };
		} },

		{ "equivocate95", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, .95); };
		} },

		{ "equivocate100", [](Peer<type_msg>* peer) {
			peer->sendMsg = [](Peer<type_msg>* peer, type_msg msg)
				{ peer->NetworkInterface<type_msg>::randomMulticast(msg, 1); };
		} }

	};

}

#endif /* infect_hpp */
