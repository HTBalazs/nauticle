/////////////////////////////////////////////////////////////////////////////
// Name:        pmFile_parser.h
// Purpose:     pmFile_parser class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _FILE_PARSER_H_
#define _FILE_PARSER_H_

#include <string>
#include "pmNoncopyable.h"

class pmFile_parser : public pmNoncopyable {
protected:
	std::fstream file;
	std::string name;
	std::string filecontent;
public:
	virtual ~pmFile_parser() {}
	virtual void read_file(std::string const& filename)=0;
	virtual void print() const=0;
};

#endif //_FILE_PARSER_H_