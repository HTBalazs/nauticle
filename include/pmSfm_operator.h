/*
    Copyright 2017 Balazs Toth
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
    
#ifndef _SOCIAL_FORCE_MODEL_OPERATOR_H_
#define _SOCIAL_FORCE_MODEL_OPERATOR_H_

#include <cstdlib>
#include <array>
#include <string>
#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "commonutils/Common.h"
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class implements the social model for crowd simulation.
	//  It requires a pmParticle_system assigned to it.
	*/
	class pmSfm_operator : public pmInteraction<10> {
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmSfm_operator() {}
		pmSfm_operator(std::array<std::shared_ptr<pmExpression>,10> op);
		pmSfm_operator(pmSfm_operator const& other);
		pmSfm_operator(pmSfm_operator&& other);
		pmSfm_operator& operator=(pmSfm_operator const& other);
		pmSfm_operator& operator=(pmSfm_operator&& other);
		virtual ~pmSfm_operator() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmSfm_operator> clone() const;
	};
}

#endif // _SOCIAL_FORCE_MODEL_OPERATOR_H_