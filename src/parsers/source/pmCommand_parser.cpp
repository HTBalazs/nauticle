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
    
#include "pmCommand_parser.h"
#include "pmVersion.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCommand_parser::pmCommand_parser(int argc_, char * argv_[],bool switches_on_) {
	argc=argc_;
	argv.resize(argc);
	std::copy(argv_,argv_+argc,argv.begin());
	switches_on=switches_on_;
	if(switches_on) {
		std::vector<std::string>::iterator it1,it2;
		it1=argv.begin();
		it2=it1+1;
		while (true) {
			if (it1==argv.end()) break;
			if (it2==argv.end()) break;
			if ((*it1)[0]=='-') {
				switch_map[*it1]=*(it2);
			}
			it1++;
			it2++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the argument by number.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmCommand_parser::get_arg(int i) {
	if(i>=0&&i<argc) {
		return argv[i];
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the argument by switch.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmCommand_parser::get_arg(std::string s) {
	if (!switches_on) {
		return "";
	}
	if (switch_map.find(s)!=switch_map.end()) {
		return switch_map[s];
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the list of available commands.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmCommand_parser::print_command_list() {
	print_header();
	ProLog::pLogger::log<ProLog::WHT>("Nauticle can recieve the following optional commands:\n");
	ProLog::pLogger::log<ProLog::WHT>("1) -help                  Display Nauticle information.\n");
	ProLog::pLogger::log<ProLog::WHT>("2) -wsres                 Lists all reserved names in workspace.\n");
	ProLog::pLogger::log<ProLog::WHT>("3) -numthreads <number>   Defines the number of threads to use. Default is detected.\n");
	ProLog::pLogger::log<ProLog::WHT>("4) -yamlname <filename>    Defines the name of the XML input file.\n");
	ProLog::pLogger::log<ProLog::WHT>("5) -logfile <filename>    Defines the name of the output log file.\n");
	ProLog::pLogger::log<ProLog::WHT>("6) -wdir <directory>      Defines the working directory. FULL path of an EXISTING directory is required.\n");
	ProLog::pLogger::log<ProLog::WHT>("7) -version               Prints the version number.\n");
	ProLog::pLogger::line_feed(1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints header.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmCommand_parser::print_header() {
	static bool already_written=false;
	if(already_written) { return; }
	print_version();
	ProLog::pLogger::log<ProLog::WHT>("Copyright 2016-2018 Balazs Toth\n\nNauticle is free software: you can redistribute it and/or modify\nit under the terms of the GNU Lesser General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nNauticle is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU Lesser General Public License for more details.\n\nYou should have received a copy of the GNU Lesser General Public License\nalong with Nauticle.  If not, see <http://www.gnu.org/licenses/>.\n\nFor more information please visit: https://bitbucket.org/nauticleproject/\n");
	ProLog::pLogger::line_feed(1);
	already_written = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints version of Nauticle.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmCommand_parser::print_version() {
	ProLog::pLogger::log<ProLog::WHT>("Nauticle version %i.%i.%i\n", MAJOR_VERSION, MINOR_VERSION, RELEASE_DATE);
}