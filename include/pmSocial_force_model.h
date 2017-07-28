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
#include "pmFilter.h"
#include "prolog/pLogger.h"
#include "commonutils/Common.h"
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class implements the socal model for crowd simulation.
	//  It requires a pmParticle_system assigned to it.
	*/
	class pmSocial_force_model : public pmFilter<8> {
		std::string op_name;
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmSocial_force_model() {}
		pmSocial_force_model(std::array<std::shared_ptr<pmExpression>,8> op);
		pmSocial_force_model(pmSocial_force_model const& other);
		pmSocial_force_model(pmSocial_force_model&& other);
		pmSocial_force_model& operator=(pmSocial_force_model const& other);
		pmSocial_force_model& operator=(pmSocial_force_model&& other);
		virtual ~pmSocial_force_model() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmSocial_force_model> clone() const;
		virtual void write_to_string(std::ostream& os) const override;
	};
}

#endif // _SOCIAL_FORCE_MODEL_OPERATOR_H_