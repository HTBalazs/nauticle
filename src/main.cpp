/*
    Copyright a
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
    
#include <string>
#include "nauticle.h"
#include "pmYAML_processor.h"

// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Triangulation_vertex_base_with_info_2.h>
// #include <CGAL/Triangulation_face_base_2.h>
// #include <CGAL/Triangulation_data_structure_2.h>
// #include <CGAL/Triangulation_2.h>

using namespace Nauticle;

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef CGAL::Triangulation_vertex_base_with_info_2<int,K>                      Vb;
// typedef CGAL::Triangulation_face_base_2<K>    Fbb;
// typedef CGAL::Triangulation_data_structure_2<Vb,Fbb>               TDS;
// typedef CGAL::Triangulation_2<K,TDS>         Triangulation;
// typedef Triangulation::Vertex_circulator Vertex_circulator;
// typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
// typedef Triangulation::Finite_faces_iterator Finite_faces_iterator;

int main(int argc, char* argv[]) {

  // // std::ifstream in("triangulation_prog1.cin");
  // // std::istream_iterator<Triangulation::Point> begin(in);
  // // std::istream_iterator<Triangulation::Point> end;

  // std::vector<double> x = {1, 3, 4, 9, 7, 5, 6, 10};
  // std::vector<double> y = {0, 2, 5, 8, 4, 2, 3, 1};
  // 
  // Triangulation t;
  // for (int i=0; i<x.size(); i++) {
  //       t.insert(Triangulation::Point(x[i],y[i]));
  // }
  // // t.insert(begin, end);

  // Finite_vertices_iterator vci = t.finite_vertices_begin();
  // Finite_vertices_iterator donei = t.finite_vertices_end();
  // Finite_faces_iterator fci = t.finite_faces_begin();
  // Finite_faces_iterator donefi = t.finite_faces_end();

  // int index = 0;
  // do { 
  //       std::cout << vci->point() << std::endl;
  //       vci->info() = index;
  //       index = index+1;	
  // }while(++vci != donei);
 
  // int faceindex = 0;
  // do { 
  //       std::cout << "face index: " << faceindex << std::endl;
  //       std::cout << "  0: " << fci->vertex(0)->info() << std::endl;
  //       std::cout << "  1: " << fci->vertex(1)->info() << std::endl;
  //       std::cout << "  2: " << fci->vertex(2)->info() << std::endl;
  //       faceindex = faceindex + 1;
  // }while(++fci != donefi);
  // 
  // return 0;

	std::string default_yaml_name = pmYAML_processor::get_default_file_name();
	std::string default_working_dir = "";
	bool command_switch = false;

// /// Do not modify anything below this line. /// //
	std::string yaml_name = default_yaml_name;
	std::string working_dir = default_working_dir;
	bool exec = false;
	size_t num_threads = std::thread::hardware_concurrency();;
	auto exec_fptr=[&](){
		if(exec) {
			std::shared_ptr<pmSimulation> simulation = std::make_shared<pmSimulation>();
			simulation->set_working_directory(working_dir);
			simulation->read_file(yaml_name);
			simulation->execute(num_threads);
		}
	};
	if(argc<2) {
		exec = true;
		exec_fptr();
	} else {
		pmCommand_parser cp{argc, argv, command_switch};
		for(int i=1; i<argc; i++) {
			if(cp.get_arg(i)=="-wsres") {
				pmCommand_parser::print_header();
				pmWorkspace::print_reserved_names();
			} else if(cp.get_arg(i)=="-help") {
				pmCommand_parser::print_command_list();
			} else if(cp.get_arg(i)=="-logfile") {
				ProLog::pLogger::logfile = cp.get_arg(++i);
				exec = true;
			} else if(cp.get_arg(i)=="-numthreads") {
				num_threads = stoi(cp.get_arg(++i));
				exec = true;
			} else if(cp.get_arg(i)=="-version") {
				pmCommand_parser::print_version();
			} else if(cp.get_arg(i)=="-wdir") {
				if(argc>i+1) {
					working_dir = cp.get_arg(++i);
					exec = true;
				}
			} else if(cp.get_arg(i)=="-yamlname") {
				if(argc>i+1) {
					yaml_name = cp.get_arg(++i);
					exec = true;
				}
			} else {
				ProLog::pLogger::log<ProLog::WHT>("Don't know what to do with \"%s\"\n",cp.get_arg(i).c_str());
			}
		}
		exec_fptr();
	}
	return 0;
}
