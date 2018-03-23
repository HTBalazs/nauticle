/*
    Copyright 2016-2018 Balazs Toth
    This file is part of Nauticle.

    Nauticle is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Nauticle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Nauticle.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/nauticleproject/
*/
    
#ifndef _DISSIPATIVE_PARTICLE_DYNAMICS_OPERATOR_H_
#define _DISSIPATIVE_PARTICLE_DYNAMICS_OPERATOR_H_

#include <cstdlib>
#include <array>
#include <string>
#include "pmFilter.h"
#include "prolog/pLogger.h"
#include "commonutils/Common.h"
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class implements the social model for crowd simulation.
	//  It requires a pmParticle_system assigned to it.
	*/
	class pmDpd_operator : public pmFilter<7> {
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmDpd_operator() {}
		pmDpd_operator(std::array<std::shared_ptr<pmExpression>,7> op);
		pmDpd_operator(pmDpd_operator const& other);
		pmDpd_operator(pmDpd_operator&& other);
		pmDpd_operator& operator=(pmDpd_operator const& other);
		pmDpd_operator& operator=(pmDpd_operator&& other);
		virtual ~pmDpd_operator() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmDpd_operator> clone() const;
	};
}

#endif // _DISSIPATIVE_PARTICLE_DYNAMICS_OPERATOR_H_