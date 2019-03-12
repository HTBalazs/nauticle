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

#include "Color_define.h"

namespace Nauticle {
    /** This abstract class implements the conventianal Smoothed Particle Hydrodynamics
    //  through interactions between particles.
    */
    template <size_t S>
    class pmLong_range : public pmInteraction<S> {
    protected:
        class pmMesh {
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
            void sort_lists(std::vector<int>& sorted_link_idx, std::vector<int> const& link_vec) const;
            void update_helper_vectors(std::vector<int>& start, std::vector<int>& end, std::vector<int> const& sorted_link_idx, std::vector<int> const& link_vec, int const& N) const;
            void update_links(std::vector<int>& link_vec, std::vector<int> const& sorted_link_idx, std::vector<int> const& start, std::vector<int> const& end, std::vector<int> const& sorted_particle_idx) const;
        public:
            pmMesh() {}
            pmMesh(std::vector<int> const& fst, std::vector<int> const& snd);
            void sort_mesh(std::vector<int> const& sorted_particle_idx);
            void add_link(int const& i1, int const& i2, double const& l0);
            int get_link_index(int const& i1, int const& i2) const;
            int get_link_id(int const& i1, int const& i2) const;
            void delete_link(int const& id);
            double get_initial_length(int const& id) const;
            void reset();
            int size() const;
            void print() const;
            std::vector<int> const& get_first() const;
            std::vector<int> const& get_second() const;
            pmIdentifier<int> const& get_id() const;
            pmHysteron const& get_hysteron(int const& i) const;
        };
        pmMesh mesh;
    public:
        pmLong_range() {}
        virtual ~pmLong_range() {}
        pmMesh const& get_mesh() const;
        virtual void update(size_t const& level=0) override;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Constructor.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    pmLong_range<S>::pmMesh::pmMesh(std::vector<int> const& fst, std::vector<int> const& snd) : first{fst}, second{snd} {
        initial_length.resize(first.size());
        ID.resize(first.size());
        hysteron.resize(first.size());
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Sorts a 0..N vector by link_vec and copies the result into sorted_link_idx.
    /// The content of sorted_link_idx (if there's any) is destroyed.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    void pmLong_range<S>::pmMesh::sort_lists(std::vector<int>& sorted_link_idx, std::vector<int> const& link_vec)  const {
        sorted_link_idx.resize(link_vec.size());
        std::iota(sorted_link_idx.begin(), sorted_link_idx.end(), 0);
        std::vector<int> copy = link_vec;
        pmSort::sort_by_vector(sorted_link_idx, copy, pmSort::ascending);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Fills the helper start and end vectors. Needs to be called for first and second 
    /// elements of the links separately.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    void pmLong_range<S>::pmMesh::update_helper_vectors(std::vector<int>& start, std::vector<int>& end, std::vector<int> const& sorted_link_idx, std::vector<int> const& link_vec, int const& N)  const {
        start.resize(N);
        end.resize(N);
        std::fill(start.begin(),start.end(),0xFFFFFFFF);
        std::fill(end.begin(),end.end(),0xFFFFFFFF);
        int count = 0;
        for(int i=0; i<N; i++) {
            int count_start = count;
            for(int j=count; j<sorted_link_idx.size(); j++) {
                if(link_vec[sorted_link_idx[j]]==i) {
                    count++;
                }
            }
            int count_end = count-1;
            if(count_start<=count_end) {
                start[i] = count_start;
                end[i] = count_end;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Updates the list of links according to the sorted_particle_idx. Needs to be called 
    ///for first and second elements of the links separately.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    void pmLong_range<S>::pmMesh::update_links(std::vector<int>& link_vec, std::vector<int> const& sorted_link_idx, std::vector<int> const& start, std::vector<int> const& end, std::vector<int> const& sorted_particle_idx)  const {
        for(int i=0; i<sorted_particle_idx.size(); i++) {
            int old_id = sorted_particle_idx[i];
            int new_id = i;
            int start_id = start[old_id];
            int end_id = end[old_id];
            if(start_id!=0xFFFFFFFF) {
                for(int j=start_id; j<=end_id; j++) {
                    link_vec[sorted_link_idx[j]] = new_id;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Adds a link given by two particle indices to the mesh.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    void pmLong_range<S>::pmMesh::add_link(int const& i1, int const& i2, double const& l0) {
        first.push_back(i1);
        second.push_back(i2);
        initial_length.push_back(l0);
        ID.add_id(ID.size()+1);
        hysteron.push_back(pmHysteron{});
    }

    template <size_t S>
    int pmLong_range<S>::pmMesh::get_link_index(int const& i1, int const& i2) const {
        for(int i=0; i<first.size(); i++) {
            if((first[i]==i1 && second[i]==i2) || (first[i]==i2 && second[i]==i1)) {
                return i;
            }
        }
        return -1;
    }

    template <size_t S>
    double pmLong_range<S>::pmMesh::get_initial_length(int const& i) const {
        return initial_length[i];
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the ith link.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    void pmLong_range<S>::pmMesh::delete_link(int const& i) {
        first.erase(first.begin()+i);
        second.erase(second.begin()+i);
        initial_length.erase(initial_length.begin()+i);
        ID.delete_id(i);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Performs the sorting of the list of links based on the given sorted_particle_idx.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    void pmLong_range<S>::pmMesh::sort_mesh(std::vector<int> const& sorted_particle_idx) {
        sort_lists(sorted_first, first);
        sort_lists(sorted_second, second);
        update_helper_vectors(start_first, end_first, sorted_first, first, sorted_particle_idx.size());
        update_helper_vectors(start_second, end_second, sorted_second, second, sorted_particle_idx.size());
        update_links(first, sorted_first, start_first, end_first, sorted_particle_idx);
        update_links(second, sorted_second, start_second, end_second, sorted_particle_idx);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Clears all data.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    void pmLong_range<S>::pmMesh::reset() {
        first.clear();
        second.clear();
        sorted_first.clear();
        sorted_second.clear();
        start_first.clear();
        end_first.clear();
        start_second.clear();
        end_second.clear();
        initial_length.clear();
        ID.reset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the number of links in the mesh.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    int pmLong_range<S>::pmMesh::size() const {
        return first.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the ith id.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <size_t S>
    int pmLong_range<S>::pmMesh::get_link_id(int const& i1, int const& i2) const {
        for(int i=0; i<first.size(); i++) {
            if((first[i]==i1 && second[i]==i2) || (first[i]==i2 && second[i]==i1)) {
                return ID[i];
            }
        }
        return -1;
    }


    template <size_t S>
    void pmLong_range<S>::pmMesh::print() const {
        if(first.size()!=0) {
            ProLog::pLogger::logf<ProLog::LYW>("\n            Particle links: ");
            ProLog::pLogger::logf<NAUTICLE_COLOR>("%i", first.size());
        }
    }

    template <size_t S>
    std::vector<int> const& pmLong_range<S>::pmMesh::get_first() const {
        return first;
    }

    template <size_t S>
    std::vector<int> const& pmLong_range<S>::pmMesh::get_second() const {
        return second;
    }

    template <size_t S>
    pmIdentifier<int> const& pmLong_range<S>::pmMesh::get_id() const {
        return ID;
    }

    template <size_t S>
    pmHysteron const& pmLong_range<S>::pmMesh::get_hysteron(int const& i) const {
        return hysteron[i];
    }

    template <size_t S>
    typename pmLong_range<S>::pmMesh const& pmLong_range<S>::get_mesh() const {
        return mesh;
    }

    template <size_t S>
    void pmLong_range<S>::update(size_t const& level/*=0*/) {
        mesh.sort_mesh(this->psys.lock()->get_sorted_idx());
    }
}

#include "Color_undefine.h"

#endif //_LONG_RANGE_H_
