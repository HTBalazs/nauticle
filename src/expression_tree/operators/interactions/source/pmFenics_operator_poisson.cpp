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
 
#include "pmFenics_operator_poisson.h"
#include "Color_define.h"
#include <algorithm>

using namespace Nauticle;
 
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_poisson::pmFenics_operator_poisson(std::array<std::shared_ptr<pmExpression>,6> op) {
    problem = std::make_shared<Problem_poisson>();
    operand = std::move(op);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_poisson::pmFenics_operator_poisson(pmFenics_operator_poisson const& other) {
    this->assigned = false;
    this->problem = other.problem;
    for(int i=0; i<this->operand.size(); i++) {
        this->operand[i] = other.operand[i]->clone();
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_poisson::pmFenics_operator_poisson(pmFenics_operator_poisson&& other) {
    this->psys = std::move(other.psys);
    this->problem = std::move(other.problem);
    this->assigned = std::move(other.assigned);
    this->operand = std::move(other.operand);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_poisson& pmFenics_operator_poisson::operator=(pmFenics_operator_poisson const& other) {
    if(this!=&other) {
        this->assigned = false;
        this->problem = other.problem;
        for(int i=0; i<this->operand.size(); i++) {
            this->operand[i] = other.operand[i]->clone();
        }
    }
    return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_poisson& pmFenics_operator_poisson::operator=(pmFenics_operator_poisson&& other) {
    if(this!=&other) {
        this->psys = std::move(other.psys);
        this->problem = std::move(other.problem);
        this->assigned = std::move(other.assigned);
        this->operand = std::move(other.operand);
    }
    return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmFenics_operator_poisson::clone_impl() const {
    return std::make_shared<pmFenics_operator_poisson>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFenics_operator_poisson> pmFenics_operator_poisson::clone() const {
    return std::static_pointer_cast<pmFenics_operator_poisson, pmExpression>(clone_impl());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints N-body content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFenics_operator_poisson::print() const {
    ProLog::pLogger::logf<NAUTICLE_COLOR>("fe_poisson");
    print_operands();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmFenics_operator_poisson::evaluate(int const& i, size_t const& level/*=0*/) const {
    if(!assigned) { ProLog::pLogger::error_msgf("FEM model is not assigned to any particle system.\n"); }
    std::shared_ptr<pmParticle_system> ps = psys.lock();
    // create vector of forces
    double alpha = operand[3]->evaluate(i,level)[0];
    double rho = operand[4]->evaluate(i,level)[0];
    double dt = operand[5]->evaluate(i,level)[0];
    std::vector<double> x;
    std::vector<double> y;
    for(int j=0; j<ps->get_field_size(); j++) {
        pmTensor posj = operand[0]->evaluate(j,level);
        x.push_back(posj[0]);
        y.push_back(posj[1]);
    }
    std::vector<double> vx;
    std::vector<double> vy;

    std::vector<double> v;
    for(int j=0; j<ps->get_field_size(); j++) {
        pmTensor velj = operand[1]->evaluate(j,level);
        vx.push_back(velj[0]);
        vy.push_back(velj[1]);
    }
    vx.insert(vx.end(), vy.begin(), vy.end());
    problem->create_mesh(x, y, alpha);
    std::vector<double> p;
    problem->calculation(vx, p, rho, dt);

    int k=0;
    for(int j=0; j<ps->get_field_size(); j++) {
        pmTensor pressure{1,1,0};
        pressure[0] = p[k];
        std::dynamic_pointer_cast<pmField>(operand[2])->set_value(pressure,j);
        k++;
    }

    return pmTensor{1,1,1};
}
 
int pmFenics_operator_poisson::get_field_size() const {
    return 1;
}

#include "Color_undefine.h"