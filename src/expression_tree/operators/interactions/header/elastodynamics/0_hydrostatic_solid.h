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

  The content of this file is based on the former work (FEniCS elastodynamics demo):

  // Copyright (C) 2009 Mirko Maraldi and Garth N. Wells
  //
  // This file is part of DOLFIN.
  //
  // DOLFIN is free software: you can redistribute it and/or modify
  // it under the terms of the GNU Lesser General Public License as published by
  // the Free Software Foundation, either version 3 of the License, or
  // (at your option) any later version.
  //
  // DOLFIN is distributed in the hope that it will be useful,
  // but WITHOUT ANY WARRANTY; without even the implied warranty of
  // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  // GNU Lesser General Public License for more details.
  //
  // You should have received a copy of the GNU Lesser General Public License
  // along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
  //
  // Modified by Anders Logg, 2011
  //
  // First added:  2009-01-22
  // Last changed: 2011-06-28
*/

#ifndef _SOLID_H_
#define _SOLID_H_

//*
#define SAINT_VENANT_KIRCHHOFF
//*/#define MOONEY_RIVLIN

#include <dolfin.h>
#include "ElastoDynamics.h"
#include <iostream>

using namespace dolfin;

class PressureBoundary : public SubDomain
{
	bool inside(const Array<double>& x, bool on_boundary) const
	{        
		if (!(-0.49+DOLFIN_EPS > x[0] || 0.49-DOLFIN_EPS < x[0]) && on_boundary)
			return true;
		else
			return false;
	}
};

class ClampedBoundary : public SubDomain
{
	bool inside(const Array<double>& x, bool on_boundary) const
	{
		if ((-0.49+DOLFIN_EPS > x[0] || 0.49-DOLFIN_EPS < x[0]) && on_boundary)
			return true;
		else
			return false;
	}
};

// Acceleration update
inline void update_a(Function& a, const Function& u, const Function& a0,
              const Function& v0,  const Function& u0,
              double beta, double dt)
{
  // a = 1/(2*beta)*((u - u0 - v0*dt)/(0.5*dt*dt) - (1-2*beta)*a0)
  *a.vector()  = *u.vector();
  *a.vector() -= *u0.vector();
  *a.vector() *= 1.0/dt;
  *a.vector() -= *v0.vector();
  *a.vector() *= 1.0/((0.5-beta)*dt);
  *a.vector() -= *a0.vector();
  *a.vector() *= (0.5-beta)/beta;
}

// Velocity update
inline void update_v(Function& v, const Function& a, const Function& a0,
              const Function& v0, double gamma, double dt)
{
  // v = dt * ((1-gamma)*a0 + gamma*a) + v0
  *v.vector()  = *a0.vector();
  *v.vector() *= (1.0-gamma)/gamma;
  *v.vector() += *a.vector();
  *v.vector() *= dt*gamma;
  *v.vector() += *v0.vector();
}

// Class for communication
class Elem {
public:
  int index;
  double x0, y0; // original position
  double ux, uy; // deformation
  double vx, vy; // velocity
  double ax, ay; // acceleration
  double x, y; // new position
  Elem() {}
  void compute_new_position() {
    x = x0 + ux;
    y = y0 + uy;    
  }
};

class Problem {
  std::shared_ptr<const dolfin::Mesh> mesh;
  std::shared_ptr<ElastoDynamics::FunctionSpace> V;
  std::shared_ptr<const Constant> rho;
  std::shared_ptr<const Constant> eta;
#ifdef SAINT_VENANT_KIRCHHOFF
  std::shared_ptr<const Constant> lambda;
  std::shared_ptr<const Constant> mu;
#endif // SAINT_VENANT_KIRCHHOFF
#ifdef MOONEY_RIVLIN
  std::shared_ptr<const Constant> C10;
  std::shared_ptr<const Constant> C01;
  std::shared_ptr<const Constant> K;
#endif //MOONEY_RIVLIN
  std::shared_ptr<const Constant> alpha_m;
  std::shared_ptr<const Constant> alpha_f;
  std::shared_ptr<const Constant> beta;
  std::shared_ptr<const Constant> gamma;
  std::shared_ptr<const Constant> f;
  std::shared_ptr<const Constant> dt;
  std::shared_ptr<Function> u;
  std::shared_ptr<Function> u0;
  std::shared_ptr<Function> v;
  std::shared_ptr<Function> v0;
  std::shared_ptr<Function> a;
  std::shared_ptr<Function> a0;
  std::shared_ptr<File> file_u;
public:
  Problem() {
    set_log_active(false);
    parameters["reorder_dofs_serial"]=false;
    double _rho = 2700.0;   // mass density
    double _eta = 250;     // damping coefficient
    double _alpha_m = 0.2;
    double _alpha_f = 0.4;
    double _beta = 0.36;
    double _gamma = 0.7;
    double nu = 0.34;       // Poisson ratio

    rho = std::make_shared<const Constant>(_rho);
    eta = std::make_shared<const Constant>(_eta); // damping coefficient
#ifdef SAINT_VENANT_KIRCHHOFF
    double E = 6.75e10;         // Youngs modulus
    lambda = std::make_shared<const Constant>((nu*E)/((1.0 + nu)*(1.0 - 2.0 * nu))); // Lame coefficient
    mu = std::make_shared<const Constant>(E/(2.0*(1.0 + nu))); // Lame coefficient
#endif // SAINT_VENANT_KIRCHHOFF
#ifdef MOONEY_RIVLIN
    double c10 = -379310.3;
    double c01 = 1327586.2;
    C10 = std::make_shared<const Constant>(c10);
    C01 = std::make_shared<const Constant>(c01);
    K = std::make_shared<const Constant>(2*(c01+c10)/(1-2*nu)); 
#endif //MOONEY_RIVLIN
    alpha_m = std::make_shared<const Constant>(_alpha_m);
    alpha_f = std::make_shared<const Constant>(_alpha_f);
    beta = std::make_shared<const Constant>(_beta);
    gamma = std::make_shared<const Constant>(_gamma);
    f = std::make_shared<Constant>(0.0, 0.0);

    // Create mesh
    mesh = std::make_shared<Mesh>("solid.xml");

    // Create function space
    V = std::make_shared<ElastoDynamics::FunctionSpace>(mesh);
    u = std::make_shared<Function>(V); // displacement
    u0 = std::make_shared<Function>(V);
    v = std::make_shared<Function>(V);  // velocity
    v0 = std::make_shared<Function>(V);
    a = std::make_shared<Function>(V);  // acceleration
    a0 = std::make_shared<Function>(V);

    file_u = std::make_shared<File>("u.pvd", "compressed");


    u0->vector()->zero();
    v0->vector()->zero();
    a0->vector()->zero();
  }

  void calculation(std::vector<double> const force, double const& _dt, std::vector<double>& px, std::vector<double>& py, std::vector<double>& vx, std::vector<double>& vy) {
    auto p = std::make_shared<Function>(V);
    auto p0 = std::make_shared<Function>(V);
    (p->vector())->set_local(force);
    (p0->vector())->set_local(force);
    dt = std::make_shared<const Constant>(_dt);    // time step

    // Neumann Boundary conditions
    PressureBoundary pressure_boundary;
    auto pressure_boundary_function = std::make_shared<MeshFunction<std::size_t>>(mesh, 1);
    pressure_boundary.mark(*pressure_boundary_function, 3);

    // Dirichlet boundary conditions
    auto clamped_boundary_dirichlet = std::make_shared<ClampedBoundary>();
    auto zero = std::make_shared<Constant>(0.0, 0.0);
    DirichletBC bc(V, zero, clamped_boundary_dirichlet);

    // Create forms
    ElastoDynamics::LinearForm F(V);
    ElastoDynamics::JacobianForm J(V,V);

    // Attach functions
#ifdef SAINT_VENANT_KIRCHHOFF
    F.lmbda = lambda; 
    F.mu = mu;
#endif // SAINT_VENANT_KIRCHHOFF
#ifdef MOONEY_RIVLIN
    F.C10 = C10;
    F.C01 = C01;
    F.K = K;
#endif // MOONEY_RIVLIN
    F.rho = rho;
    F.eta = eta;
    F.beta = beta;
    F.gamma = gamma;
    F.dt = dt;
    F.alpha_m = alpha_m;
    F.alpha_f = alpha_f;
    F.u = u;
    F.u0 = u0;
    F.v0 = v0;
    F.p0 = p0;
    F.a0 = a0;
    F.p = p;
    F.f = f;

#ifdef SAINT_VENANT_KIRCHHOFF
    J.lmbda = lambda;
    J.mu = mu;
#endif // SAINT_VENANT_KIRCHHOFF
#ifdef MOONEY_RIVLIN
    J.C10 = C10;
    J.C01 = C01;
    J.K = K;
#endif // MOONEY_RIVLIN
    J.rho = rho;
    J.eta = eta;
    J.beta = beta;
    J.gamma = gamma;
    J.dt = dt;
    J.alpha_m = alpha_m;
    J.alpha_f = alpha_f;
    J.u = u;
    
    // Attach subdomains
    J.ds = pressure_boundary_function;
    F.ds = pressure_boundary_function;
 
    // Create solver Parameters
    Parameters params("nonlinear_variational_solver");
    Parameters newton_params("newton_solver");
    newton_params.add("absolute_tolerance", 1e-5);
    newton_params.add("linear_solver","mumps");
    params.add(newton_params);

    solve(F == 0, *u, bc, J, params);
    update_a(*a, *u, *a0, *v0, *u0, *beta, *dt);
    update_v(*v, *a, *a0, *v0, *gamma, *dt);
    *u0 = *u; *v0 = *v; *a0 = *a;
    
    static int count=0;
    if(count%200==0) *file_u << *u;
    count++;

    std::vector<std::shared_ptr<Elem>> elem = transform(u,v,a);
    px.reserve(elem.size());
    py.reserve(elem.size());
    vx.reserve(elem.size());
    vy.reserve(elem.size());
    for(int i=0; i<elem.size(); i++) {
      px.push_back(elem[i]->x);
      py.push_back(elem[i]->y);
      vx.push_back(elem[i]->vx);
      vy.push_back(elem[i]->vy);
    }
  }

  std::vector<std::shared_ptr<Elem>> transform(std::shared_ptr<Function> u, std::shared_ptr<Function> v, std::shared_ptr<Function> a) const
  {
    std::vector<std::shared_ptr<Elem>> cells;
 
    for (MeshEntityIterator e(*mesh, 0); !e.end(); ++e)
    {
      // get vertex
      Vertex vertex(*mesh,e->index());
      Point po = vertex.point();

      // get values at the vertex
      Array<double> coords(2);
      coords[0] = po.x();
      coords[1] = po.y();
      Array<double> uvals(2);
      Array<double> vvals(2);
      Array<double> avals(2);
      u->eval(uvals, coords);
      v->eval(vvals, coords);
      a->eval(avals, coords);
       
      // create an Elem
      std::shared_ptr<Elem> cll = std::make_shared<Elem>();
      cll->index = e->index();
      cll->x0 = po.x();
      cll->y0 = po.y();
      cll->ux = uvals[0];
      cll->uy = uvals[1];      
      cll->vx = vvals[0];
      cll->vy = vvals[1];      
      cll->ax = avals[0];
      cll->ay = avals[1];     
      cll->compute_new_position(); 
      cells.push_back(cll);
    }
 
    return cells;
  }
};

#endif // _SOLID_H_