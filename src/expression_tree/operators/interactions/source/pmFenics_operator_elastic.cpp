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
 
#include "pmFenics_operator_elastic.h"
#include "Color_define.h"
#include <algorithm>

using namespace Nauticle;
 
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_elastic::pmFenics_operator_elastic(std::array<std::shared_ptr<pmExpression>,7> op) {
    problem = std::make_shared<Problem>();
    operand = std::move(op);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_elastic::pmFenics_operator_elastic(pmFenics_operator_elastic const& other) {
    this->assigned = false;
    this->problem = other.problem;
    for(int i=0; i<this->operand.size(); i++) {
        this->operand[i] = other.operand[i]->clone();
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_elastic::pmFenics_operator_elastic(pmFenics_operator_elastic&& other) {
    this->psys = std::move(other.psys);
    this->problem = std::move(other.problem);
    this->assigned = std::move(other.assigned);
    this->operand = std::move(other.operand);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFenics_operator_elastic& pmFenics_operator_elastic::operator=(pmFenics_operator_elastic const& other) {
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
pmFenics_operator_elastic& pmFenics_operator_elastic::operator=(pmFenics_operator_elastic&& other) {
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
std::shared_ptr<pmExpression> pmFenics_operator_elastic::clone_impl() const {
    return std::make_shared<pmFenics_operator_elastic>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFenics_operator_elastic> pmFenics_operator_elastic::clone() const {
    return std::static_pointer_cast<pmFenics_operator_elastic, pmExpression>(clone_impl());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints N-body content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFenics_operator_elastic::print() const {
    ProLog::pLogger::logf<NAUTICLE_COLOR>("fe_elastic");
    print_operands();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmFenics_operator_elastic::evaluate(int const& i, size_t const& level/*=0*/) const {
    if(!assigned) { ProLog::pLogger::error_msgf("FEM model is not assigned to any particle system.\n"); }
    std::shared_ptr<pmParticle_system> ps = psys.lock();
    // create vector of forces
    int solid_id = operand[2]->evaluate(i,level)[0];
    std::vector<int> id;
    std::vector<double> qx;
    std::vector<double> qy;
    for(int j=0; j<ps->get_field_size(); j++) {
        if(solid_id==(int)operand[1]->evaluate(j,level)[0]) {
            pmTensor q = operand[3]->evaluate(j,level);
            qx.push_back(q[0]);
            qy.push_back(q[1]);
            id.push_back((int)operand[0]->evaluate(j,level)[0]);
        }
    }
    std::vector<int> nauticle2fenics;
    nauticle2fenics.resize(id.size());
    std::iota(nauticle2fenics.begin(),nauticle2fenics.end(),0);
    pmSort::sort_by_vector(nauticle2fenics,id,pmSort::ascending);
    pmSort::reorder(qx,nauticle2fenics);
    pmSort::reorder(qy,nauticle2fenics);
    
    std::vector<double> q;
    for(int i=0; i<qx.size(); i++) {
        q.push_back(qx[i]);
    }
    for(int i=0; i<qy.size(); i++) {
        q.push_back(qy[i]);
    }
    // calc
    double dt = operand[6]->evaluate(0,level)[0];
    std::vector<double> px;
    std::vector<double> py;
    std::vector<double> vx;
    std::vector<double> vy;
    problem->calculation(q, dt, px, py, vx, vy);
    
    std::vector<int> fenics2nauticle;
    fenics2nauticle.resize(id.size());
    std::iota(fenics2nauticle.begin(),fenics2nauticle.end(),0);
    pmSort::sort_by_vector(fenics2nauticle,nauticle2fenics,pmSort::ascending);
    pmSort::reorder(px,fenics2nauticle);
    pmSort::reorder(py,fenics2nauticle);
    
    int k=0;
    for(int j=0; j<ps->get_field_size(); j++) {
        if(solid_id==(int)operand[1]->evaluate(j,level)[0]) {
            pmTensor velocity{2,1,0};
            velocity[0] = vx[k];
            velocity[1] = vy[k];
            pmTensor position{2,1,0};
            position[0] = px[k];
            position[1] = py[k];
            std::dynamic_pointer_cast<pmField>(operand[4])->set_value(position,j);
            std::dynamic_pointer_cast<pmField>(operand[5])->set_value(velocity,j);
            k++;
        }
    }

    return pmTensor{1,1,1};
}
 
int pmFenics_operator_elastic::get_field_size() const {
    return 1;
}

#include "Color_undefine.h"