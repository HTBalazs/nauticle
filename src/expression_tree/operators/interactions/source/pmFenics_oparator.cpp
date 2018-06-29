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
 
#include "pmFenics_operator.h"
#include "Color_define.h"

using namespace Nauticle;
 
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator::pmFenics_operator(std::array<std::shared_ptr<pmExpression>,5> op) {
    operand = std::move(op);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator::pmFenics_operator(pmFenics_operator const& other) {
    this->assigned = false;
    for(int i=0; i<this->operand.size(); i++) {
        this->operand[i] = other.operand[i]->clone();
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator::pmFenics_operator(pmFenics_operator&& other) {
    this->psys = std::move(other.psys);
    this->assigned = std::move(other.assigned);
    this->operand = std::move(other.operand);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator& pmFenics_operator::operator=(pmFenics_operator const& other) {
    if(this!=&other) {
        this->assigned = false;
        for(int i=0; i<this->operand.size(); i++) {
            this->operand[i] = other.operand[i]->clone();
        }
    }
    return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator& pmFenics_operator::operator=(pmFenics_operator&& other) {
    if(this!=&other) {
        this->psys = std::move(other.psys);
        this->assigned = std::move(other.assigned);
        this->operand = std::move(other.operand);
    }
    return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmFenics_operator::clone_impl() const {
    return std::make_shared<pmFenics_operator>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFenics_operator> pmFenics_operator::clone() const {
    return std::static_pointer_cast<pmFenics_operator, pmExpression>(clone_impl());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints N-body content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFenics_operator::print() const {
    ProLog::pLogger::logf<NAUTICLE_COLOR>("fenics");
    print_operands();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmFenics_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
    if(!assigned) { ProLog::pLogger::error_msgf("FEM model is not assigned to any particle system.\n"); }
    std::shared_ptr<pmParticle_system> ps = psys.lock();
    // create vector of forces
    float gid = operand[1]->evaluate(i,level)[0];
    std::vector<double> q;
    q.push_back(0);
    for(int j=0; j<ps->get_field_size(); j++) {
        if(gid==operand[0]->evaluate(j,level)[0]) {
            q.push_back(operand[2]->evaluate(j,level)[1]); // hydrostatic
        }
    }
    q.push_back(0);
    double dt = operand[4]->evaluate(0,level)[0];
    // std::vector<std::shared_ptr<Elem>> elem = problem.calculation(q, dt);
    int k=0;
    // for(int j=0; j<ps->get_field_size(); j++) {
    //     if(gid==operand[0]->evaluate(j,level)[0]) {
    //         pmTensor position{2,1,0};
    //         position[0] = -0.5+elem[k]->x; // hydrostatic
    //         position[1] = -1.025+elem[k]->w; // hydrostatic
    //         ps->set_value(position,j);  
 
    //         pmTensor velocity{2,1,0};
    //         velocity[1] = elem[k]->wdot; // hydrostatic
    //         std::dynamic_pointer_cast<pmField>(operand[3])->set_value(velocity,j);
    //         k++;
    //     }
    // }
    return pmTensor{1,1,1};
}
 
int pmFenics_operator::get_field_size() const {
    return psys.lock()->get_field_size();
}

#include "Color_undefine.h"