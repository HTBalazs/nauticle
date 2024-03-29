/*
    Copyright 2016-2020 Balazs Havasi-Toth
    This file is part of Nauticle.

    Nauticle is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nauticle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Nauticle.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/nauticleproject/
*/

#ifndef _PM_PARTICLE_SPLITTER_H_
#define _PM_PARTICLE_SPLITTER_H_

#include "pmParticle_resolve.h"
#include <memory>
#include <vector>

namespace Nauticle {
    /** This class implements the particle splitting.
    */
	class pmParticle_splitter : public pmParticle_resolve {
        std::shared_ptr<pmExpression> separation_parameter;
        std::shared_ptr<pmExpression> smoothing_ratio;
        std::shared_ptr<pmExpression> daughters;
        std::shared_ptr<pmExpression> parent;
        std::shared_ptr<pmExpression> rotation;
        std::shared_ptr<pmField> active;
        std::shared_ptr<pmField> passive;
    protected:
        virtual std::shared_ptr<pmParticle_modifier> clone_impl() const override;
    public:
		void update(size_t const& num_threads) override;
        void print() const override;
        void set_smoothing_ratio(std::shared_ptr<pmExpression> sr);
        void set_separation_parameter(std::shared_ptr<pmExpression> sp);
        void set_daughters(std::shared_ptr<pmExpression> dr);
        void set_parent(std::shared_ptr<pmExpression> pr);
        void set_rotation(std::shared_ptr<pmExpression> rot);
        void set_passive(std::shared_ptr<pmField> psv);
        void set_active(std::shared_ptr<pmField> atv);
        std::shared_ptr<pmParticle_splitter> clone() const;
	};
}

#endif //_PM_PARTICLE_SPLITTER_H_