// include LEMPS
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
	auto exec_fptr=[&](){if(exec){pmCase::execute(xml_name,working_dir);}};
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