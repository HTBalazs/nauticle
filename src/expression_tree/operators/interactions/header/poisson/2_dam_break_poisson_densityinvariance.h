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
#include <vtkGenericDataObjectWriter.h>

#include <dolfin.h>
#include <iostream>
#include <fstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <CGAL/Triangulation_data_structure_2.h>

#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include "DPressureUpdate.h"

using namespace dolfin;

// Traits
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

// Vertex type
typedef CGAL::Triangulation_vertex_base_with_info_2<int,K>                      Vbb;
typedef CGAL::Alpha_shape_vertex_base_2<K,Vbb> Vb;

// Cell type
typedef CGAL::Triangulation_face_base_2<K>    Fbb;
typedef CGAL::Alpha_shape_face_base_2<K,Fbb> Fb;

// Triangulation
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Delaunay_triangulation_2<K,TDS>         Triangulation;
typedef CGAL::Alpha_shape_2<Triangulation> Alpha_shape_2;

// Iterators
typedef Alpha_shape_2::Vertex_circulator Vertex_circulator;
typedef Alpha_shape_2::Finite_vertices_iterator Finite_vertices_iterator;
typedef Alpha_shape_2::Finite_faces_iterator Finite_faces_iterator;
typedef typename Alpha_shape_2::Alpha_shape_edges_iterator Alpha_shape_edges_iterator;

// Define Dirichlet boundary domain
class Boundary_poisson : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    if(!(-0.99+DOLFIN_EPS > x[0] || 0.99-DOLFIN_EPS < x[0] || -0.49+DOLFIN_EPS > x[1]) && on_boundary)
      return true;
    else
      return false;
  }
};

// Define Neumann boundary domain
class Neumann_Boundary_poisson : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const {
    if((-0.99+DOLFIN_EPS > x[0] || 0.99-DOLFIN_EPS < x[0] || -0.49+DOLFIN_EPS > x[1]) && on_boundary)
      return true;
    else
      return false;
  }
};

// Define Expression for Neumann BC
class dUdN : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const {
    values[0] = 0;
  }
};

class Problem_poisson {
  std::shared_ptr<const dolfin::Mesh> mesh;
  std::shared_ptr<DPressureUpdate::FunctionSpace> DQ;
  std::shared_ptr<Function> p;
  std::shared_ptr<File> file_p;
  std::shared_ptr<File> file_p_xml;
  std::shared_ptr<File> file_mesh;
  std::shared_ptr<File> file_mesh_pvd;
  std::shared_ptr<const Constant> dt;
  std::shared_ptr<const Constant> rho;
  std::vector<int> vindices;
  std::vector<int> excluded_vindices;

public:
  Problem_poisson() {
    set_log_active(false);
    parameters["reorder_dofs_serial"] = false;
    file_p = std::make_shared<File>("p.pvd", "ascii");
		file_p_xml = std::make_shared<File>("p.xml");
		file_mesh = std::make_shared<File>("mesh.xml");
		file_mesh_pvd = std::make_shared<File>("mesh.pvd", "ascii");
  }

	// DEBUGGER Write std::vector into file
  template <class Type>
	void writefile(std::vector<Type> const& vec, std::string const& filename) {
		ofstream file;
		file.open(filename);
		for(int ii=0; ii<vec.size(); ii++) {
			file << vec[ii] << std::endl;
		}
		file.close();
	}

	// DEBUGGER Write everything into files for further inspection
	void dump(std::vector<double> const& x, std::vector<double> const& y, std::vector<int> const& cells0, std::vector<int> const& cells1, std::vector<int> const& cells2){
		writefile<double>(x,"x.txt");
		writefile<double>(y,"y.txt");
		writefile<int>(cells0,"cells0.txt");
		writefile<int>(cells1,"cells1.txt");
		writefile<int>(cells2,"cells2.txt");
		*file_mesh << *mesh;
		*file_mesh_pvd << *mesh;
	}

  // Ez a fuggveny torol minden "indices" elemet a data-bol, de ugy, 
  // hogy tudja, hogy a data sebesseget tartalmazza, igy ket helyen is vegez torlest.
  template <typename T> void delete_indices(std::vector<T>& data, std::vector<int> const& indices, bool isvector) {
    std::vector<bool> marked_elements(data.size(), false);
    std::vector<T> temp_buffer;
    temp_buffer.reserve(data.size()-indices.size());
    for(auto const& it:indices) {
      marked_elements[it] = true;
    }
		if (isvector) {
    	for(auto const& it:indices) {
    	  marked_elements[it+data.size()/2] = true;
    	}
		}
    for(int i=0; i<data.size(); i++) {
      if(!marked_elements[i]) {
        temp_buffer.push_back(data[i]);
      }
    }
    data = temp_buffer;
  }

	// Convert old indices to new indices
  int oldtonew(int old) {
    std::vector<int>::iterator it = std::find(vindices.begin(), vindices.end(), old);
    return std::distance(vindices.begin(), it);
  }

  // Ez a fuggveny visszater egy olyan vektorral, amelyikben azok az indexek vannak, 
  // amelyek a vindicesbol hianyoznak.
  void store_deleted_incides(std::vector<int>& excluded_vindices, std::vector<int> const& vindices, int const& size) {
    for(int i=1;i<vindices.size(); i++) {
      if(1<(vindices[i]-vindices[i-1])) {
        for(int j=vindices[i-1]+1;j<vindices[i];j++) {
          excluded_vindices.push_back(j);
        }
      }
    }
    for(int i=vindices.back()+1; i<size; i++) {
      excluded_vindices.push_back(i);
    }
  }

  // Ez a fuggveny feltolti a pressure vektort nullakkal ott, ahol hianyzo vertexek voltak.
  void refill_with_excluded_indices(std::vector<double>& pressure, std::vector<int> const& excluded_vindices) {
    for(int i=0; i<excluded_vindices.size(); i++) {
      pressure.insert(pressure.begin()+excluded_vindices[i],0.0);
    }
  }

	// Create Delaunay triangulation
  void create_mesh(std::vector<double> const& x, std::vector<double> const& y, double const& alpha) {
    // Add points, calculate triangulation
    Triangulation tri;
    for (int i=0; i<x.size(); i++) {
        tri.insert(Triangulation::Point(x[i],y[i]));
    }
    Alpha_shape_2 t(tri,alpha,Alpha_shape_2::GENERAL);
    // Alpha_shape_2 t(tri,alpha,Alpha_shape_2::REGULARIZED);
	
    // Add indices to vertices
    Finite_vertices_iterator vci = t.finite_vertices_begin();
    Finite_vertices_iterator donei = t.finite_vertices_end();
    int index = 0;
    do { 
			vci->info() = index;
			index = index+1;	
    }while(++vci != donei);
    
    // Get faces and cell indices
    std::vector<int> _cells0;
    std::vector<int> _cells1;
    std::vector<int> _cells2;
    vindices.clear(); // empty vindices vector
    excluded_vindices.clear(); // empty vindices vector
    Finite_faces_iterator ffi = t.finite_faces_begin();
    Finite_faces_iterator donefi = t.finite_faces_end();
    do {
      if(t.classify(ffi) == Alpha_shape_2::INTERIOR) {
        vindices.push_back(ffi->vertex(0)->info());
        vindices.push_back(ffi->vertex(1)->info());
        vindices.push_back(ffi->vertex(2)->info());

        _cells0.push_back(ffi->vertex(0)->info());
        _cells1.push_back(ffi->vertex(1)->info());
        _cells2.push_back(ffi->vertex(2)->info());
      }
    }while(++ffi != donefi);
    std::sort(vindices.begin(), vindices.end());
    std::vector<int>::iterator iter = std::unique(vindices.begin(), vindices.end());
    vindices.resize(std::distance(vindices.begin(),iter));
    store_deleted_incides(excluded_vindices, vindices, x.size());

    std::vector<int> cells0;
    std::vector<int> cells1;
    std::vector<int> cells2;
    for (int i = 0; i<_cells0.size(); i++) {
        cells0.push_back(oldtonew(_cells0[i]));
        cells1.push_back(oldtonew(_cells1[i]));
        cells2.push_back(oldtonew(_cells2[i]));
    }

    dolfin_mesh(cells0,cells1,cells2,x,y);
  }

	// Convert mesh to dolfin::Mesh
	void dolfin_mesh(std::vector<int> cells0, std::vector<int> cells1, std::vector<int> cells2, std::vector<double> const& x, std::vector<double> const& y) {
    // Init mesh and editor
    Mesh msh;
    MeshEditor editor;
    editor.open(msh, "triangle", 2, 2); // 2D mesh

    // Create cells
    int n_cells = cells1.size();
    editor.init_cells(n_cells);
    for (int i=0; i<n_cells; i++) {
      	editor.add_cell(i,cells0[i],cells1[i],cells2[i]);
    }
    
    // writefile(vindices,"vindices.txt");
		if (vindices.size() != x.size()) {
		//	std::cout << x.size() - vindices.size() << " points are excluded from the mesh." << std::endl;
			// dump(x,y,cells0,cells1,cells2);
			// writefile<int>(vindices,"vindices.txt");
			// writefile<int>(excluded_vindices,"excluded_vindices.txt");
		}
    // Add vertices to the mesh
    int n_vertices = vindices.size();
    editor.init_vertices(n_vertices);
    int counter = 0;
    for (auto const& iit:vindices) {
        editor.add_vertex(counter++,x[iit],y[iit]);
    }
    editor.close();

    mesh = std::make_shared<Mesh>(msh);
	}	

  void calculation(std::vector<double>& _rhs, std::vector<double>& pressure, double const& _rho, double const& _dt) {
    // Create function space
    DQ = std::make_shared<DPressureUpdate::FunctionSpace>(mesh);
    p = std::make_shared<Function>(DQ); // pressure
    rho = std::make_shared<const Constant>(_rho);  

    // Convert values from arguments
    auto rhs = std::make_shared<Function>(DQ);       // right-handside of density invariance equation
    delete_indices(_rhs, excluded_vindices,false);
    rhs->vector()->set_local(_rhs);

    // Define Dirichlet boundary conditions
    auto boundary = std::make_shared<Boundary_poisson>();
    auto zero = std::make_shared<Constant>(0.0);
    DirichletBC bc(DQ, zero, boundary);

    // Define Neumann boundary conditions
    Neumann_Boundary_poisson neumann_boundary;
    auto boundary_function = std::make_shared<MeshFunction<std::size_t>>(mesh, 1);
    neumann_boundary.mark(*boundary_function, 1);
    auto g = std::make_shared<dUdN>();

    // Create forms
    DPressureUpdate::BilinearForm a2(DQ,DQ);
    DPressureUpdate::LinearForm L2(DQ);

    // Set coefficients
    L2.v0 = rhs;
    L2.g = g;
    // a2.rho = rho;
    L2.ds = boundary_function;
    a2.ds = boundary_function;
    static int count = 0;

    solve(a2 == L2, *p, bc);
    if(count%100==0) {
      *file_p << *p;
    }
    count++;
    tran(p,pressure);
    refill_with_excluded_indices(pressure, excluded_vindices);
  }

	// Transform Function back to std::vector
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
