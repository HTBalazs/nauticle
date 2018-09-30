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

#ifndef _PARTICLE_MERGER_H_
#define _PARTICLE_MERGER_H_

#include "pmWorkspace.h"
#include "pmParticle_modifier.h"
#include "pmInteraction.h"
#include <utility>
#include <memory>
#include <vector>

namespace Nauticle {
    /** This class implements the pairwise particle merging.
    */
	class pmParticle_merger : public pmParticle_modifier {
        /** This class implements the search for nearest neighbor as an interaction.
        */  
        class pmNearest_neighbor : public pmInteraction<0> {
        protected:
            virtual std::shared_ptr<pmExpression> clone_impl() const override;
        public:
            pmTensor evaluate(int const& i, size_t const& level=0) const override;
            void print() const override {}
        };

    private:
        std::shared_ptr<pmNearest_neighbor> nearest;
        std::shared_ptr<pmField> velocity;
        static size_t counter;
    protected:
        virtual std::shared_ptr<pmParticle_modifier> clone_impl() const override;
    private:
        void make_pairs(std::pair<std::vector<size_t>,std::vector<size_t>>& pairs, std::vector<size_t> candidates) const;
    public:
        pmParticle_merger();
		void update() override;
        virtual void set_workspace(std::shared_ptr<pmWorkspace> ws) override;
        void set_velocity(std::shared_ptr<pmField> vel);
        std::shared_ptr<pmField> const& get_velocity() const;
        void print() const override;
        std::shared_ptr<pmParticle_merger> clone() const;
	};
}

#endif // _PARTICLE_MERGER_H_