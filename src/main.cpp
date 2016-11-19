/*
    Copyright 2016 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
#include <string>
#include "pm.h"
#include "pmGrid.h"

int main(int argc, char* argv[]) {

	std::string default_xml_name = "config.xml";
	std::string default_working_dir = "";
	bool command_switch = false;

// /// Do not modify anything below this line. /// //
	std::string xml_name = default_xml_name;
	std::string working_dir = default_working_dir;
	bool exec = false;
	size_t num_threads = 8;
	auto exec_fptr=[&](){ if(exec){ pmCase::execute(xml_name,working_dir,num_threads); } };
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
			} else if(cp.get_arg(i)=="-numthreads") {
				num_threads = stoi(cp.get_arg(i+1));
				exec = true;
				i++;
			} else if(cp.get_arg(i)=="-version") {
				pLogger::logf("Version number: %i.%i\n",MAJOR_VERSION,MINOR_VERSION);
			} else if(cp.get_arg(i)=="-wdir") {
				if(argc>i+1) {
					working_dir = cp.get_arg(i+1);
					exec = true;
					i++;
				}
			}  else if(cp.get_arg(i)=="-xmlname") {
				if(argc>i+1) {
					xml_name = cp.get_arg(i+1);
					exec = true;
					i++;
				}
			} else {
				pLogger::log<WHT>("Don't know what to do with \"%s\"\n",cp.get_arg(i).c_str());
			}
		}
		exec_fptr();
	}
	return 0;
}