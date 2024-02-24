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

#ifndef _PM_HELPER_H_
#define _PM_HELPER_H_

#include <Eigen/Eigen>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <functional>

namespace Nauticle {
	template <class T, class U>
	inline U get(T const& obj, int const& i);
	
	template <class U>
	inline U get(std::vector<U> const& obj, int const& i) {
		return obj[i];
	}

	template <class U>
	inline U get(U const& obj, int const& i) {
		return obj;
	}

	template <class T>
	inline double normal(T const& obj) {
		return obj.norm();
	}
	
	template <>
	inline double normal(double const& obj) {
		return obj;
	}

	template <class T>
	inline std::string num2str(T num) {
		std::stringstream ss;
		ss << std::defaultfloat << std::setprecision(17) << num;
		return ss.str();
	}

	template <>
	inline std::string num2str(int num) {
		return std::to_string(num);
	}

	template <class T>
	inline T cwise_operation(T const& lhs, T const& rhs, std::function<double(double,double)> op) {
		T res = lhs;
		for(int i=0; i<lhs.size(); i++) {
			res[i] = op(lhs[i],rhs[i]);
		}
		return res;
	}

	template <class T>
	inline T cwise_operation(T const& val, std::function<double(double)> op) {
		T res = val;
		for(int i=0; i<val.size(); i++) {
			res[i] = op(val[i]);
		}
		return res;
	}

	template <class T>
	inline T add(T const& mx1, T const& mx2) {
		return mx1+mx2;
	}

	template <class T>
	inline T add(T const& mx, double const& sc) {
		return (mx.array()+sc).matrix();
	}

	template <class T>
	inline T add(double const& sc, T const& mx) {
		return add(mx,sc);
	}

	template <class T>
	inline T sub(T const& mx1, T const& mx2) {
		return mx1-mx2;
	}

	template <class T>
	inline T sub(T const& mx, double const& sc) {
		return (mx.array()-sc).matrix();
	}

	template <class T>
	inline T sub(double const& sc, T const& mx) {
		return sub(mx,sc);
	}

	template <typename T> int sign(T val) {
		return (T{0} < val) - (val < T{0});
	}

	template <typename T> int euler_integrator(T val1, T val2, double ts) {
		return val1+val2*ts;
	}

	template <class T>
	Eigen::SelfAdjointEigenSolver<T> eigensystem(T const& mat) {
		return Eigen::SelfAdjointEigenSolver<T>{mat};
	}

	template <class T>
	T eigenvectors(T const& mat) {
		return eigensystem(mat).eigenvectors();
	}

	template <size_t S>
	Eigen::Matrix<double,S,1> eigenvalues(Eigen::Matrix<double,S,S> const& mat) {
		return eigensystem(mat).eigenvalues();
	}

	template <class T, bool FULL=false>
	void qRDecomposition(T const& A,T& Q, T& R) {
		T thinQ{A.rows(),A.cols()};
		T q{A.rows(),A.rows()};
	 
		Eigen::HouseholderQR<T> householderQR(A);
		q = householderQR.householderQ();
		thinQ.setIdentity();
		Q = householderQR.householderQ() * thinQ;
		if(FULL) {
			R = Q.transpose()*A;
		}
	}

	template <class T>
	T deQ(T const& A) {
		T Q;
		T R;
		qRDecomposition<T,false>(A, Q, R);
		return Q;
	}

	template <class T>
	T deR(T const& A) {
		T Q;
		T R;
		qRDecomposition<T,true>(A, Q, R);
		return R;
	}

	inline double value_limiter(double const& value, double const& minimum, double const& maximum) {
		return (value>maximum?maximum:(value<minimum?minimum:value));
	}

	template <int R, int C>
	inline Eigen::Matrix<double,R,C> transpose(Eigen::Matrix<double,C,R> const& mat) {
		return mat.transpose();
	}

	inline double transpose(double const& mat) {
		return mat;
	}

	template <class T>
	inline T create(double const& v=0);

	template <>
	inline double create<double>(double const& v/*=0*/) {
		return v;
	}

	template <>
	inline Eigen::Vector2d create<Eigen::Vector2d>(double const& v/*=0*/) {
		return Eigen::Vector2d{v,v};
	}

	template <>
	inline Eigen::Vector3d create<Eigen::Vector3d>(double const& v/*=0*/) {
		return Eigen::Vector3d{v,v,v};
	}

	template <>
	inline Eigen::Matrix2d create<Eigen::Matrix2d>(double const& v/*=0*/) {
		Eigen::Matrix2d mat{};
		mat << v,v,v,v;
		return mat;
	}

	template <>
	inline Eigen::Matrix3d create<Eigen::Matrix3d>(double const& v/*=0*/) {
		Eigen::Matrix3d mat{};
		mat << v,v,v,v,v,v,v,v,v;
		return mat;
	}
}

#endif // _PM_HELPER_H_




































