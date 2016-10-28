/////////////////////////////////////////////////////////////////////////////
// Name:        pmFile.h
// Purpose:     pmFile class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _FILE_H_
#define _FILE_H_

#include <cstdarg>
#include <string>

class pmFile final {
	FILE* file;
public:
	pmFile()=delete;
	pmFile(std::string const& file_name, std::string const& mode);
	~pmFile();
	void fprint(char const* fmt, ...);
	void vfprint(char const* fmt, va_list args);
};

#endif // _FILE_H_