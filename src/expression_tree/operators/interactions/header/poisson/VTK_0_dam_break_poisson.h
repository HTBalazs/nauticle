/*  Copyright 2016-2018 Balazs Toth
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

#ifndef _POISSON_H_
#define _POISSON_H_

#include <vtkVersion.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkGenericCell.h>
#include <vtkPoints.h>
#include <vtkTriangle.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkLine.h>
#include <vtkCellLocator.h>
#include <vtkSmartPointer.h>
#include <vtkDelaunay2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkVertexGlyphFilter.h>
#include <dolfin.h>
#include "PressureUpdate.h"
#include <iostream>
#include <fstream>

#include <vtkGenericDataObjectWriter.h>

using namespace dolfin;

// Define boundary domain
class Boundary_poisson : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    if(!(-0.99+DOLFIN_EPS > x[0] || 0.99-DOLFIN_EPS < x[0] || -0.49+DOLFIN_EPS > x[1]) && on_boundary)
      return true;
    else
      return false;
   	// return on_boundary;
  }
};

// Define boundary domain
class Neumann_Boundary_poisson : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const {
    if((-0.99+DOLFIN_EPS > x[0] || 0.99-DOLFIN_EPS < x[0] || -0.49+DOLFIN_EPS > x[1]) && on_boundary)
      return true;
    else
      return false;
	// return on_boundary;
  }
};

class dUdN : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const {
    values[0] = 0;
  }
};

// Class for communication
class Elem_poisson {
public:
  int index;
  double x0, y0; // original position
  double p;      // pressure
  Elem_poisson() {}
};

class Problem_poisson {
  std::shared_ptr<const dolfin::Mesh> mesh;
  std::shared_ptr<PressureUpdate::FunctionSpace> Q;
  std::shared_ptr<PressureUpdate::CoefficientSpace_v0> V;
  std::shared_ptr<Function> p;
  std::shared_ptr<File> file_p;
  std::shared_ptr<File> file_p_xml;
  std::shared_ptr<File> file_v_xml;
  std::shared_ptr<File> file_v_pvd;
  std::shared_ptr<File> file_mesh;
  std::shared_ptr<File> file_mesh_pvd;
  std::shared_ptr<const Constant> dt;
  std::shared_ptr<const Constant> rho;
  std::vector<int> vindices;

public:
  Problem_poisson() {
    set_log_active(false);
	// set_log_active(true);
	// set_log_level(1);
    parameters["reorder_dofs_serial"] = false;
    file_p = std::make_shared<File>("p.pvd", "ascii");
	file_p_xml = std::make_shared<File>("p.xml");
	file_v_xml = std::make_shared<File>("v.xml");
	file_v_pvd = std::make_shared<File>("v.pvd", "ascii");
	file_mesh = std::make_shared<File>("mesh.xml");
	file_mesh_pvd = std::make_shared<File>("mesh.pvd", "ascii");
  }

  void create_mesh(std::vector<double> const& x, std::vector<double> const& y, double const& alpha) {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for(int i=0; i<x.size(); i++) {
      points->InsertNextPoint(x[i], y[i], 0.0);
    }
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
    delaunay->SetAlpha(alpha);
    delaunay->SetInputData(polydata);
    delaunay->Update();

    polydata = delaunay->GetOutput();
	vtkGenericDataObjectWriter* writer = vtkGenericDataObjectWriter::New();
	writer->SetFileName("polydata.vtk");
	writer->SetInputData(polydata);          //polydata is my polydata object
	writer->Update();
	writer->Write();
    size_t size = polydata->GetNumberOfPoints();

    std::vector<size_t> cells0;
    std::vector<size_t> cells1;
    std::vector<size_t> cells2;

    vtkCellArray* cellarray = polydata->GetPolys();
    vtkIdType numCells = cellarray->GetNumberOfCells();
    vtkIdType cellLocation = 0; // the index into the cell array  
    for(vtkIdType i = 0; i < numCells; i++) {
      vtkIdType numIds; // to hold the size of the cell
      vtkIdType *pointIds; // to hold the ids in the cell
      cellarray->GetCell(cellLocation, numIds, pointIds);
      cellLocation += 1 + numIds;
      cells0.push_back(pointIds[0]);
      cells1.push_back(pointIds[1]);
      cells2.push_back(pointIds[2]);
    }

    // Init mesh and editor
    Mesh msh;
    MeshEditor editor;
    editor.open(msh, "triangle", 2, 2); // 2D mesh

    // Create cells
	std::vector<int>::iterator iter;
    int n_cells = cells1.size();
    editor.init_cells(n_cells);
    for (int i=0; i<n_cells; i++) {
      editor.add_cell(i,cells0[i],cells1[i],cells2[i]);
  	  vindices.push_back(cells0[i]);
  	  vindices.push_back(cells1[i]);
  	  vindices.push_back(cells2[i]);
      // if(cells0[i]==2404 || cells1[i]==2404 || cells2[i]==2404) {
      //   std::cout << std::endl;
      //   std::cout << i << ":" << std::endl;
      //   std::cout << cells0[i] << std::endl;
      //   std::cout << cells1[i] << std::endl;
      //   std::cout << cells2[i] << std::endl;
      // }
    }
	std::sort(vindices.begin(), vindices.end());
	iter = std::unique(vindices.begin(), vindices.end());
	vindices.resize(std::distance(vindices.begin(),iter));
  // ofstream file;
  // file.open("vindices.txt");
  // for(auto const& it:vindices) {
  //   file << it << std::endl;
  // }
  // file.close();

	int n_vertices = vindices.size();
	editor.init_vertices(n_vertices);
	for (auto const& iit:vindices) {
		editor.add_vertex(iit,x[iit],y[iit]);
	}	
    editor.close();
    
	// // Create vertices
    // int n_vertices = x.size();
    // editor.init_vertices(n_vertices);
    // for (int i=0; i<n_vertices; i++) {
    //   editor.add_vertex(i,x[i],y[i]);
    // }

    // // Create cells
    // int n_cells = cells1.size();
    // editor.init_cells(n_cells);
    // for (int i=0; i<n_cells; i++) {
    //   editor.add_cell(i,cells0[i],cells1[i],cells2[i]);
    // }
    // editor.close();

    mesh = std::make_shared<Mesh>(msh);
  }

  void calculation(std::vector<double> const& _v0, std::vector<double>& pressure, double const& _rho, double const& _dt) {
    // Create function space
    Q = std::make_shared<PressureUpdate::FunctionSpace>(mesh);
    V = std::make_shared<PressureUpdate::CoefficientSpace_v0>(mesh);
    p = std::make_shared<Function>(Q); // pressure
    rho = std::make_shared<const Constant>(_rho);  
    dt = std::make_shared<const Constant>(_dt);    // time step

    // Convert values from arguments
    auto v0 = std::make_shared<Function>(V);       // approximate velocity
	std::vector<double> v0resized;
	for (auto it:vindices) {
		v0resized.push_back(_v0[it]);	
	}
	for (auto it:vindices) {
		v0resized.push_back(_v0[_v0.size()/2+it]);	
	}
    (v0->vector())->set_local(v0resized);

    // Define Dirichlet boundary conditions
    auto boundary = std::make_shared<Boundary_poisson>();
    auto zero = std::make_shared<Constant>(0.0);
    DirichletBC bc(Q, zero, boundary);

    // Define Neumann boundary conditions
    Neumann_Boundary_poisson neumann_boundary;
    auto boundary_function = std::make_shared<MeshFunction<std::size_t>>(mesh, 1);
    neumann_boundary.mark(*boundary_function, 1);
    auto g = std::make_shared<dUdN>();

    // Create forms
    PressureUpdate::BilinearForm a2(Q, Q);
    PressureUpdate::LinearForm L2(Q);

    // Set coefficients
    L2.v0 = v0;
    L2.rho = rho;
    L2.dt = dt;
    L2.g = g;
	L2.ds = boundary_function;
	a2.ds = boundary_function;
    static int count = 0;
 //    if(count%100==0) {
	// *file_mesh_pvd << *mesh;
 //    *file_v_pvd << *v0;
	//  std::cout << "1" << std::endl;
	//  static int counter = 0;
	//  counter++;
	//  if(counter==4) {
	// 	std::cout << _v0.size() << std::endl;
	// 	ofstream file;
	//   	file.open("velocity.txt");
	// 	for(int ii=0; ii<_v0.size(); ii++) {
	// 		file << _v0[ii] << std::endl;
	// 	}
	// 	file.close();
	//  }
	//  *file_v_xml << *v0;
	//  std::cout << "2" << std::endl;
   // }
	// std::cout << "1" << std::endl;
		// std::cout << "2" << std::endl;
	// std::cout << "3" << std::endl;
	// std::cout << "4" << std::endl;
	// std::cout << "5" << std::endl;

    if(count%100==0) {
      *file_mesh << *mesh;
      *file_p << *p;
    }
    solve(a2 == L2, *p, bc);
    count++;
    tran(p,pressure);
  }

  void tran(std::shared_ptr<Function> calcp, std::vector<double>& pressure) 
  {
    for (MeshEntityIterator e(*mesh, 0); !e.end(); ++e)
    {
      // get vertex
      Vertex vertex(*mesh,e->index());
      Point po = vertex.point();

      // get values at the vertex
      Array<double> coords(2);
      Array<double> pvals(1);
      coords[0] = po.x();
      coords[1] = po.y();
      calcp->eval(pvals, coords);
      pressure.push_back(pvals[0]);
    }
  }

};

#endif // _POISSON_H_
