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

using namespace Nauticle;

int main(int argc, char* argv[]) {
	std::string default_yaml_name = pmYAML_processor::get_default_file_name();
	std::string default_working_dir = "";
	bool command_switch = false;

// /// Do not modify anything below this line. /// //
	std::string yaml_name = default_yaml_name;
	std::string working_dir = default_working_dir;
	bool exec = false;
	size_t num_threads = std::thread::hardware_concurrency();;
	auto exec_fptr=[&](){ if(exec){ pmCase_manager::execute(yaml_name,working_dir,num_threads); } };
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