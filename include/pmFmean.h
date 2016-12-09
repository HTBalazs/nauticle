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
    
#ifndef _FMEAN_H_
#define _FMEAN_H_

#include "pmFsearch.h"

class pmFmean : public pmFsearch {
	std::shared_ptr<pmExpression> clone_impl() const;
private:
	void process(pmTensor& value, Eval_type eval_type=current) const override;
public:
	pmFmean(std::shared_ptr<pmExpression>);
	pmFmean(pmFmean const& other);
	pmFmean(pmFmean&& other);
	pmFmean& operator=(pmFmean const& other);
	pmFmean& operator=(pmFmean&& other);
	std::shared_ptr<pmFmean> clone() const;
	void print() const;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmFmean const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FMEAN_H_