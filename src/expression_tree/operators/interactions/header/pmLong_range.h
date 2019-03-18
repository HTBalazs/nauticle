/*
    Copyright 2016-2019 Balazs Toth
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
    
#ifndef _LONG_RANGE_H_
#define _LONG_RANGE_H_

#include <memory>
#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "pmKernel.h"

namespace Nauticle {
    /** This abstract class implements the conventianal Smoothed Particle Hydrodynamics
    //  through interactions between particles.
    */
    class pmLong_range {
    protected:
        class pmPairs {
            std::vector<pmHysteron> hysteron;
            std::vector<double> initial_length;
            std::vector<int> first;
            std::vector<int> second;
            pmIdentifier<int> ID;
            mutable std::vector<int> sorted_first;
            mutable std::vector<int> sorted_second;
            mutable std::vector<int> start_first;
            mutable std::vector<int> end_first;
            mutable std::vector<int> start_second;
            mutable std::vector<int> end_second;
        private:
            void sort_lists(std::vector<int>& sorted_pair_idx, std::vector<int> const& pair_vec) const;
            void update_helper_vectors(std::vector<int>& start, std::vector<int>& end, std::vector<int> const& sorted_pair_idx, std::vector<int> const& pair_vec, int const& N) const;
            void update_pairs(std::vector<int>& pair_vec, std::vector<int> const& sorted_pair_idx, std::vector<int> const& start, std::vector<int> const& end, std::vector<int> const& sorted_particle_idx) const;
        public:
            pmPairs()=default;
            pmPairs(std::vector<int> const& fst, std::vector<int> const& snd);
            void sort_pairs(std::vector<int> const& sorted_particle_idx);
            void add_pair(int const& i1, int const& i2, double const& l0);
            int get_pair_index(int const& i1, int const& i2) const;
            int get_pair_id(int const& i1, int const& i2) const;
            void delete_pair(int const& id);
            double get_initial_length(int const& id) const;
            void reset();
            int size() const;
            void print() const;
            std::vector<int> const& get_first() const;
            std::vector<int> const& get_second() const;
            pmIdentifier<int> const& get_id() const;
            pmHysteron& get_hysteron(int const& i);
            pmHysteron const& get_hysteron(int const& i) const;
        };
        pmPairs pairs;
    public:
        pmLong_range() {}
        virtual ~pmLong_range() {}
        pmPairs const& get_pairs() const;
        void sort_pairs(std::vector<int> const& sorted_idx);
    };
}

#endif //_LONG_RANGE_H_
