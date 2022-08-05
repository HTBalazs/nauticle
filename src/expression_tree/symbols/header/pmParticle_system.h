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
    
#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include <string>
#include "pmSort.h"
#include "pmDomain.h"
#include "pmField.h"
#include "pmHysteron.h"
#include "pmIdentifier.h"

namespace Nauticle {
	/** This class manages a cloud of nodes (particles) and forms a spatial domain 
	//  for the governing equations.
	//  Neighbour search and cloud/grid generation is integrated inside.
	*/
	class pmParticle_system final : public pmField, public pmDomain {
		std::vector<int> pidx;
		std::shared_ptr<pmField> periodic_jump;
		bool up_to_date=false;
	private:
		void restrict_particles(std::vector<size_t>& del) const;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmParticle_system(std::vector<pmTensor> const& value, pmDomain const& dm);
		bool operator==(pmParticle_system const& rhs) const;
		bool operator!=(pmParticle_system const& rhs) const;
		virtual ~pmParticle_system() {}
		std::vector<int> const& get_cell_content(pmTensor const& grid_crd, int& index) const;
		void set_expired();
		virtual void set_value(pmTensor const& value, int const& i=0) override;
		void print() const override;
		void printv() const override;
		std::shared_ptr<pmParticle_system> clone() const;
		void set_field_size(size_t const& N) override;
		bool is_position() const override;
		virtual void delete_member(size_t const& i) override;
		virtual void delete_set(std::vector<size_t> const& indices) override;
		virtual void add_member(pmTensor const& v=pmTensor{}) override;
		virtual void duplicate_member(size_t const& i) override;
		void restrict_particles(std::vector<size_t>& del);
		bool is_up_to_date() const;
		bool update(std::vector<size_t>& del);
		std::shared_ptr<pmField> get_periodic_jump() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmParticle_system const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif //_PARTICLE_SYSTEM_H_