/*
    Copyright 2016-2019 Balazs Toth
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

#include "pmRandom.h"


/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a uniform distributed random number between minimum and maximum.
/////////////////////////////////////////////////////////////////////////////////////////
double Nauticle::pmRandom::random(double const& minimum, double const& maximum) {
	static bool gen=true;
	if(gen) {
		srand(time(NULL));
		gen=false;
	}
	if(minimum>=maximum) {
		ProLog::pLogger::warning_msgf("Random number cannot be generated if the range is incorrect. Returns zero.\n");
		return 0.0;
	}
	return ((double)(rand()%10000))/1e4*(maximum-minimum)+minimum;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a uniform distributed random tensor between minimum and maximum. If the 
/// dimensions of the given tensors do not match it returns an empty tensor.
/////////////////////////////////////////////////////////////////////////////////////////
Nauticle::pmTensor Nauticle::pmRandom::random(Nauticle::pmTensor const& minimum, Nauticle::pmTensor const& maximum) {
	if(minimum.get_numrows()!=maximum.get_numrows() || minimum.get_numcols()!=maximum.get_numcols()) {
		ProLog::pLogger::error_msgf("Random numbers cannot be generated if the dimensions of the limits does not match.\n");
		return Nauticle::pmTensor{};
	}
	Nauticle::pmTensor random_tensor{minimum.get_numrows(), minimum.get_numcols()};
	for(int i=0; i<minimum.numel(); i++) {
		random_tensor[i] = Nauticle::pmRandom::random((double)minimum[i], (double)maximum[i]);
	}
	return random_tensor;
}