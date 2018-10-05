/*
    Copyright 2016-2018 Balazs Toth
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

#ifndef _PARTICLE_SPLITTER_H_
#define _PARTICLE_SPLITTER_H_

#include "pmWorkspace.h"
#include "pmParticle_modifier.h"
#include <memory>
#include <vector>

namespace Nauticle {
    /** This class implements the particle splitting.
    */
	class pmParticle_splitter : public pmParticle_modifier {
        double epsilon = 0.4;
        double alpha = 0.9;
        size_t daughters = 6;
        bool parent = true;
        static size_t counter;
    protected:
        virtual std::shared_ptr<pmParticle_modifier> clone_impl() const override;
    public:
        pmParticle_splitter();
		void update() override;
        void print() const override;
        void set_alpha(double alp);
        void set_epsilon(double eps);
        void set_daughters(size_t dau);
        void set_parent(bool par);
        std::shared_ptr<pmParticle_splitter> clone() const;
	};
}

#endif // _PARTICLE_SPLITTER_H_