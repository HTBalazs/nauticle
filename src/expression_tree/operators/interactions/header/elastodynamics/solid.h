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

#include <dolfin.h>
#include "ElastoDynamics.h"
#include <iostream>

using namespace dolfin;

// External load
class Pressure : public Expression
{
public:

  Pressure()
    : Expression(2) {}
  void eval(Array<double>& values, const Array<double>& x) const {
    values[0] = 0.0;
    values[1] = 0.001;
  }

};

// Right boundary
class RightBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    if (1.0 - x[0] < DOLFIN_EPS && on_boundary)
      return true;
    else
      return false;
  }
};

class LeftBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    if (x[0] < DOLFIN_EPS)
      return true;
    else
      return false;
  }
};

class TopBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    if (0.05 - x[1] < DOLFIN_EPS && on_boundary)
      return true;
    else
      return false;
  }
};

// Acceleration update
void update_a(Function& a, const Function& u, const Function& a0,
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
void update_v(Function& v, const Function& a, const Function& a0,
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
  double x,y;
  double u,v;
  Elem() {}
};

class Problem {
  std::shared_ptr<const dolfin::Mesh> mesh;
  std::shared_ptr<ElastoDynamics::FunctionSpace> V;
  std::shared_ptr<const Constant> rho;
  std::shared_ptr<const Constant> eta;
  std::shared_ptr<const Constant> lambda;
  std::shared_ptr<const Constant> mu;
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
  std::shared_ptr<Pressure> p;
  std::shared_ptr<Pressure> p0;

public:
  Problem() {
    double _rho = 1.0;  // mass density
    double _eta = 0.25; // damping coefficient
    double E = 1.0;     // Youngs modulus
    double nu = 0.4;     // Poisson ratio
    double _alpha_m = 0.2;
    double _alpha_f = 0.4;
    double _beta = 0.36;
    double _gamma = 0.7;
    double _dt = 1.0/32.0;

    rho = std::make_shared<const Constant>(_rho);
    eta = std::make_shared<const Constant>(_eta); // damping coefficient
    lambda = std::make_shared<const Constant>((nu*E)/((1.0 + nu)*(1.0 - 2.0 * nu))); // Lame coefficient
    mu = std::make_shared<const Constant>(E/(2.0*(1.0 + nu))); // Lame coefficient
    alpha_m = std::make_shared<const Constant>(_alpha_m);
    alpha_f = std::make_shared<const Constant>(_alpha_f);
    beta = std::make_shared<const Constant>(_beta);
    gamma = std::make_shared<const Constant>(_gamma);
    f = std::make_shared<Constant>(0.0, 0.0);

    // Create mesh
    mesh = std::make_shared<Mesh>("solid.xml");
    // Point x0{0.0, 0.0};
    // Point x1{1, 0.05};
    // mesh = std::make_shared<Mesh>(RectangleMesh(x0, x1, 50, 5, "right"));

    // Create function space
    V = std::make_shared<ElastoDynamics::FunctionSpace>(mesh);
    u = std::make_shared<Function>(V); // displacement
    u0 = std::make_shared<Function>(V);
    v = std::make_shared<Function>(V);  // velocity
    v0 = std::make_shared<Function>(V);
    a = std::make_shared<Function>(V);  // acceleration
    a0 = std::make_shared<Function>(V);

    file_u = std::make_shared<File>("u.pvd", "compressed");

    p = std::make_shared<Pressure>();
    p0 = std::make_shared<Pressure>();

    u0->vector()->zero();
    v0->vector()->zero();
    a0->vector()->zero();
  }

  std::vector<std::shared_ptr<Elem>> calculation(double _dt) {
    dt = std::make_shared<const Constant>(_dt);    // time step

    // Neumann Boundary conditions
    TopBoundary top_boundary;
    auto top_boundary_function = std::make_shared<MeshFunction<std::size_t>>(mesh, 1);
    top_boundary.mark(*top_boundary_function, 3);
    // Dirichlet boundary conditions
    auto left_boundary = std::make_shared<LeftBoundary>();
    auto right_boundary = std::make_shared<RightBoundary>();
    auto zero = std::make_shared<Constant>(0.0, 0.0);
    DirichletBC bc(V, zero, left_boundary);
    DirichletBC bc2(V, zero, right_boundary);

    // Create forms
    ElastoDynamics::BilinearForm a_form(V, V);
    ElastoDynamics::LinearForm L(V);

    // Attach functions
    a_form.lmbda = lambda; a_form.mu = mu; a_form.rho = rho;
    a_form.eta = eta;
    a_form.beta = beta;
    a_form.gamma = gamma;
    a_form.dt = dt;
    a_form.alpha_m = alpha_m;
    a_form.alpha_f = alpha_f;

    L.lmbda = lambda; L.mu = mu; L.rho = rho;
    L.eta = eta;
    L.beta = beta;
    L.gamma = gamma;
    L.dt = dt;
    L.alpha_m = alpha_m;
    L.alpha_f = alpha_f;
    L.u0 = u0; L.v0 = v0; L.a0 = a0;
    L.f = f;
    L.p = p;
    L.p0 = p0;

    // Attach subdomains
    a_form.ds = top_boundary_function;
    L.ds = top_boundary_function;

    solve(a_form == L, *u, {&bc, &bc2});
    update_a(*a, *u, *a0, *v0, *u0, *beta, *dt);
    update_v(*v, *a, *a0, *v0, *gamma, *dt);
    *u0 = *u; *v0 = *v; *a0 = *a;
    *file_u << *u;

    return transform(u);
  }

  std::vector<std::shared_ptr<Elem>> transform(std::shared_ptr<Function> u) const
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
      Array<double> cvals(2);
      u->eval(cvals, coords);
       
      // create an Elem
      std::shared_ptr<Elem> cll = std::make_shared<Elem>();
      cll->index = e->index();
      cll->x = po.x();
      cll->y = po.y();
      cll->u = cvals[0];
      cll->v = cvals[1];      
      cells.push_back(cll);
    }
 
    return cells;
  }
};


int main(int argc, char* argv[])
{
  init(argc, argv);
  Problem problem;
  std::vector<std::shared_ptr<Elem>> elems;
  double dt = 1.0/32.0;
  for(int i=0; i<100; i++) {
    elems.push_back(problem.calculation(dt));
  }



  return 0;
}
