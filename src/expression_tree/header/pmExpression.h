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
    
#ifndef _PM_EXPRESSION_H_
#define _PM_EXPRESSION_H_

#include "pmTensor.h"
#include "c2c/c2CPP_type.h"
#include "c2c/c2CPP_class_member_function.h"
#include <memory>
#include <cmath>
#include <vector>

namespace Nauticle {
    class pmParticle_system;

    /** This interface represents an algebraic expression as an expression tree.
    */
    class pmExpression {
    protected:
        std::string name = "";
    	virtual std::shared_ptr<pmExpression> clone_impl() const=0;
    public:
    	virtual ~pmExpression() {}
    	virtual pmTensor evaluate(int const&, size_t const& level=0) const=0;
    	virtual void print() const=0;
        virtual int get_field_size() const=0;
    	virtual std::string const& get_name() const;
        virtual void set_storage_depth(size_t const& d)=0;
    	virtual size_t get_storage_depth() const;
    	std::shared_ptr<pmExpression> clone() const;
    	virtual void assign(std::shared_ptr<pmParticle_system> ps) {}
    	virtual bool is_assigned() const=0;
    	virtual void write_to_string(std::ostream& os) const=0;
        virtual bool is_symmetric() const;
        virtual bool is_position() const;
        virtual void update(size_t const& level=0) {}
        virtual bool is_interaction() const;
        virtual void delete_member(size_t const& i) {}
        virtual void delete_set(std::vector<size_t> const& indices) {}
        virtual int get_precedence() const { return 0; };
#ifdef JELLYFISH
        virtual c2c::c2CPP_type get_cpp_type() const { return c2c::c2CPP_type{"double"}; };
        virtual std::string get_cpp_evaluation(std::string const& idx, size_t const& level=0) const { return ""; };
        virtual c2c::c2CPP_class_member_function generate_cpp_evaluator_function() const { return c2c::c2CPP_class_member_function{"T","U",false,""}; }
#endif // JELLYFISH
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Implementaton of << operator.
    /////////////////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<(std::ostream& os, pmExpression const* obj) {
        obj->write_to_string(os);
        return os;
    }
}


#endif //_PM_EXPRESSION_H_