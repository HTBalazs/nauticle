/*
    Copyright 2016-2020 Balazs Havasi-Toth
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
    
#ifndef _PM_RANDOM_H_
#define _PM_RANDOM_H_

#include "prolog/pLogger.h"
#include "pmTensor.h"
#include <Eigen/Eigen>
#include <cstdlib>
#include <random>

namespace Nauticle {
    /** This namespace contains functions for random number generation.
    */
    enum RANDOM_TYPE {UNIFORM, NORMAL, LOGNORMAL};
    /////////////////////////////////////////////////////////////////////////////////////////
    /// Generates a random number with R_TYPE distribution using the parameters p1 and p2.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <RANDOM_TYPE R_TYPE> 
    inline Eigen::Vector2d random(Eigen::Vector2d const& p1, Eigen::Vector2d const& p2) {
        Eigen::Vector2d rand_obj = p1;
        for(int i=0; i<p1.rows(); i++) {
            for(int j=0; j<p1.cols(); j++) {
                rand_obj(i,j) = random<R_TYPE,double>(p1(i,j), p2(i,j));
            }
        }
        return rand_obj;
    }

    template <RANDOM_TYPE R_TYPE> 
    inline Eigen::Vector3d random(Eigen::Vector3d const& p1, Eigen::Vector3d const& p2) {
        Eigen::Vector3d rand_obj = p1;
        for(int i=0; i<p1.rows(); i++) {
            for(int j=0; j<p1.cols(); j++) {
                rand_obj(i,j) = random<R_TYPE,double>(p1(i,j), p2(i,j));
            }
        }
        return rand_obj;
    }

    template <RANDOM_TYPE R_TYPE> 
    inline Eigen::Matrix2d random(Eigen::Matrix2d const& p1, Eigen::Matrix2d const& p2) {
        Eigen::Matrix2d rand_obj = p1;
        for(int i=0; i<p1.rows(); i++) {
            for(int j=0; j<p1.cols(); j++) {
                rand_obj(i,j) = random<R_TYPE,double>(p1(i,j), p2(i,j));
            }
        }
        return rand_obj;
    }

    template <RANDOM_TYPE R_TYPE> 
    inline Eigen::Matrix3d random(Eigen::Matrix3d const& p1, Eigen::Matrix3d const& p2) {
        Eigen::Matrix3d rand_obj = p1;
        for(int i=0; i<p1.rows(); i++) {
            for(int j=0; j<p1.cols(); j++) {
                rand_obj(i,j) = random<R_TYPE,double>(p1(i,j), p2(i,j));
            }
        }
        return rand_obj;
    }

    template <RANDOM_TYPE R_TYPE>
    inline double random(double const& p1, double const& p2) {
        static bool pseudo = true;
        if(!pseudo) {
            srand(time(NULL));
            pseudo = false;
        }
        static std::default_random_engine generator;
        switch(R_TYPE) {
            default:
            case UNIFORM:   { std::uniform_real_distribution<double> uniform(p1,p2);
                            return uniform(generator); }
            case NORMAL:    { std::normal_distribution<double> nm(p1,p2);
                            return nm(generator); }
            case LOGNORMAL:     { std::lognormal_distribution<double> lognormal(p1,p2);
                                return lognormal(generator); }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Generates a uniform distributed random tensor between p1 and p2. If the 
    /// dimensions of the given tensors do not match it returns an empty tensor.
    /////////////////////////////////////////////////////////////////////////////////////////
    template <RANDOM_TYPE R_TYPE> 
    inline pmTensor random(Nauticle::pmTensor const& p1, Nauticle::pmTensor const& p2) {
        Nauticle::pmTensor random_tensor{p1.get_numrows(), p1.get_numcols()};
        for(int i=0; i<p1.numel(); i++) {
            random_tensor[i] = random<R_TYPE>((double)p1[i], (double)p2[i]);
        }
        return random_tensor;
    }

    inline double rnd_proba() {
        return 4.5;
    }
}

#endif //_PM_RANDOM_H_