/////////////////////////////////////////////////////////////////////////////
// Name:        pmNoncopyable.h
// Purpose:     pmNoncopyable class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

/** This class has all its copy functions (constructor and assignment)
//  deleted which makes any object of its derived classes Noncopyable.
*/
class pmNoncopyable {
public:
	pmNoncopyable() {}
	pmNoncopyable(pmNoncopyable const&)=delete;
	pmNoncopyable& operator=(pmNoncopyable const&)=delete;
	virtual ~pmNoncopyable() {}
};

#endif //_NONCOPYABLE_H_