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

#ifndef _PM_PARTICLE_MERGER_H_
#define _PM_PARTICLE_MERGER_H_

#include "pmWorkspace.h"
#include "pmParticle_resolve.h"
#include "pmInteraction.h"
#include <utility>
#include <memory>
#include <vector>
#include <tuple>

namespace Nauticle {
    /** This class implements the pairwise particle merging.
    */
	class pmParticle_merger : public pmParticle_resolve {
        /** This class implements the search for nearest neighbor as an interaction.
        */  
        class pmNearest_neighbor : public pmInteraction<0> {
        protected:
            std::shared_ptr<pmExpression> neighbor_condition;
            std::shared_ptr<pmExpression> max_neighbor_distance;
            virtual std::shared_ptr<pmExpression> clone_impl() const override;
        public:
            void set_neighbor_condition(std::shared_ptr<pmExpression> ncd);
            void set_max_neighbor_distance(std::shared_ptr<pmExpression> mnd);
            std::shared_ptr<pmExpression> get_neighbor_condition() const;
            std::shared_ptr<pmExpression> get_max_neighbor_distance() const;
            pmTensor evaluate(int const& i, size_t const& level=0) const override;
            void print() const override {}
        };

    private:
        std::shared_ptr<pmNearest_neighbor> nearest;
        std::shared_ptr<pmField> velocity;
    protected:
        virtual std::shared_ptr<pmParticle_modifier> clone_impl() const override;
    private:
        void make_tuples(std::tuple<std::vector<size_t>,std::vector<size_t>,std::vector<size_t>>& tuples, std::vector<size_t> candidates) const;
    public:
        pmParticle_merger();
        void update(size_t const& num_threads) override;
        virtual void set_workspace(std::shared_ptr<pmWorkspace> ws) override;
        void set_velocity(std::shared_ptr<pmField> vel);
        void set_neighbor_condition(std::shared_ptr<pmExpression> ncd);
        void set_max_neighbor_distance(std::shared_ptr<pmExpression> mnd);
        std::shared_ptr<pmField> const& get_velocity() const;
        void print() const override;
        std::shared_ptr<pmParticle_merger> clone() const;
	};
}

#endif //_PM_PARTICLE_MERGER_H_