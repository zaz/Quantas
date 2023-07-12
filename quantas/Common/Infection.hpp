/*
Copyright 2023

This file is part of QUANTAS.
QUANTAS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
QUANTAS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with QUANTAS. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef Infection_hpp
#define Infection_hpp

#include <functional>
#include "Peer.hpp"

using std::function;

namespace quantas {

	// TODO: test this whole thing
	// TODO: move to Infect.hpp
	// XXX QUESTION should this class even exist?
	template<class type_msg>  // XXX typedef cannot be a template
	class Infection {
		function<void(Peer<type_msg>*,function<void()>)> _infection;
		// allow for creating infections that don't take performComputation as an argument
		function<void(Peer<type_msg>*)> _fn;
	public:
		Infection(function<void(Peer<type_msg>*,function<void()>)> fn) : _infection(fn) {}
		Infection(function<void(Peer<type_msg>*)> fn) : _fn(fn) {}

		/**
		 * An infection is a higher-order function:
		 *
		 * @param the peer that we are performing computation on
		 * @param the original performComputation function
         * FIXME contradicts infect.hpp
		 * @return a modified version of performComputation
		 */
		void operator()(Peer<type_msg>* peer, function<void()> performComputation) {
			if (_infection != nullptr)
				// XXX: do we need to use bind so peer is properly bound to this?
				_infection(peer, performComputation);
			else
				_fn(peer, performComputation);
		}
	};

}

#endif /* Infection_hpp */
