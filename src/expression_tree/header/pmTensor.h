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
    
#ifndef _TENSOR_H_
#define _TENSOR_H_

#include <iostream>
#include <cmath>
#include "prolog/pLogger.h"
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class represents a 0th, 1st or 2nd order tensor of arbitrary shape.
	//  This general mathematical object helps to build equations of scalars, vectors
	//  or tensors with automatic conversions.
	*/
	class pmTensor final { 
		int rows;
		int columns;
		double elements[9];
	public:
		pmTensor();
		pmTensor(double const& s);
		pmTensor(int const& r, int const& c, double const& init=0);
		pmTensor(pmTensor const& other);
		pmTensor(pmTensor&& other);
		static pmTensor Tensor(int const& num_components);
		static pmTensor Tensor11();
		static pmTensor Tensor12();
		static pmTensor Tensor21();
		static pmTensor Tensor13();
		static pmTensor Tensor31();
		static pmTensor Tensor22();
		static pmTensor Tensor33();
		pmTensor& operator=(pmTensor const& other);
		pmTensor& operator=(pmTensor&& other);
		pmTensor& operator+=(pmTensor const& other);
		pmTensor& operator-=(pmTensor const& other);
		pmTensor& operator*=(pmTensor const& other);
		pmTensor& operator/=(pmTensor const& other);
		pmTensor& operator++();
		pmTensor& operator--();
		pmTensor operator++(int);
		pmTensor operator--(int);
		double operator()(int const& i, int const& j) const;
		double operator()(int const& i) const;
		double& operator[](int const& i);
		double const& operator[](int const& i) const;
		~pmTensor();
		int get_numrows() const;
		int get_numcols() const;
		bool is_column() const;
		bool is_row() const;
		bool is_vector() const;
		bool is_scalar() const;
		bool is_empty() const;
		bool is_zero() const;
		bool is_square() const;
		bool is_singular() const;
		double productum() const;
		double summation() const;
		int numel() const;
		pmTensor transpose() const;
		pmTensor trace() const;
		void print() const;
		pmTensor copy() const;
		pmTensor divide_term_by_term(pmTensor const& rhs) const;
		pmTensor multiply_term_by_term(pmTensor const& rhs) const;
		double norm() const;
		double tensor_norm() const;
		void fill(double const& value);
		pmTensor to_row() const;
		pmTensor to_column() const;
		static pmTensor make_tensor(int const& r, int const& c, double const& value);
		static pmTensor make_tensor(pmTensor const& base, double const& value);
		static pmTensor make_identity(int const& size);
		void write_to_string(std::ostream& os) const;
		pmTensor sub_tensor(int rmin, int rmax, int cmin, int cmax) const;
		pmTensor sub_tensor(int r, int c) const;
		pmTensor determinant() const;
		pmTensor adjugate() const;
		pmTensor inverse() const;
		pmTensor reflect_perpendicular(pmTensor const& guide) const;
		pmTensor append(int const& row, int const& col) const;
		bool is_integer() const;
		pmTensor deQ() const;
		pmTensor deR() const;
		pmTensor deR(pmTensor& Q) const;
		pmTensor eigensystem() const;
		pmTensor diagonalize() const;
		pmTensor eigenvalues() const;
		double min() const;
		double max() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of << operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmTensor const* obj) {
		obj->write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of << operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmTensor const& obj) {
		obj.write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of + operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator+(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel()==0) { return rhs; }
		if(rhs.numel()==0) { return lhs; }
		if(lhs.get_numrows()!=rhs.get_numrows() || lhs.get_numcols()!=rhs.get_numcols()) {
			ProLog::pLogger::error_msgf("Unable to add tensors of different sizes or types.\n");
		}
		pmTensor tensor = pmTensor{lhs};
		for(int i=0; i<tensor.numel(); i++) {
			tensor[i] += rhs[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of - operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator-(pmTensor const& rhs) {
		pmTensor tensor{rhs};
		for(int i=0; i<tensor.numel(); i++) {
			tensor[i] = -tensor[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of - operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator-(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel()==0) { return -rhs; }
		if(rhs.numel()==0) { return lhs; }
		if(lhs.get_numrows()!=rhs.get_numrows() || lhs.get_numcols()!=rhs.get_numcols()) {
			ProLog::pLogger::error_msgf("Unable to subtract tensors of different sizes or types.\n");
		}
		pmTensor tensor = pmTensor{lhs};
		for(int i=0; i<tensor.numel(); i++) {
			tensor[i] -= rhs[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of + operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator+(pmTensor const& rhs) {
		return rhs;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of * operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator*(pmTensor const& lhs, double const& rhs) {
		pmTensor tensor{lhs};
		for(int i=0; i<lhs.numel(); i++) {
			tensor[i] *= rhs;
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of * operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator*(double const& lhs, pmTensor const& rhs) {
		return rhs*lhs;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of / operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator/(pmTensor const& lhs, double const& rhs) {
		pmTensor t{lhs};
		for(int i=0; i<lhs.numel(); i++) {
			t[i] = lhs[i]/rhs;
		}
		return t;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of * operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator*(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel()==1 && rhs.numel()!=1) {
			return lhs[0]*rhs;
		}
		if(lhs.numel()!=1 && rhs.numel()==1) {
			return lhs*rhs[0];
		}
		if(lhs.numel()!=1 && rhs.numel()!=1) {
			if(lhs.get_numcols()!=rhs.get_numrows()) {
				ProLog::pLogger::error_msgf("Unable to multiply these tensors.\n");
				return pmTensor{};
			}
		}
		int imax = lhs.get_numrows();
		int jmax = rhs.get_numcols();
		pmTensor tensor{imax, jmax, 0};
		for(int i=0; i<imax; i++) {
			for(int j=0; j<jmax; j++) {
				double sum = 0;
				for(int k=0; k<lhs.get_numcols(); k++) {
					sum += lhs(i,k)*rhs(k,j);
				}
				tensor[i*jmax+j] = sum;
			}
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of / operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator/(pmTensor const& lhs, pmTensor const& rhs) {
		if(rhs.is_scalar()) {
			return lhs/rhs[0];
		} else {
			ProLog::pLogger::error_msgf("Unable to divide by a non-scalar.\n");
			return pmTensor{};
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of && operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator&&(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"and\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] && rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of || operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator||(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"or\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] || rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of != operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator!=(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"xor\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] != rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of ! operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator!(pmTensor const& tensor) {
		if(!tensor.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"not\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)!tensor[0]};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of > operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator>(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"gt\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] > rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of < operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator<(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"lt\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] < rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of > operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator>=(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"gte\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] > rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of < operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator<=(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			ProLog::pLogger::error_msgf("Logical \"lte\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] < rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of == operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool operator==(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel() == rhs.numel()) {
			for(int i=0; i<lhs.numel(); i++) {
				if(lhs[i] != rhs[i]) {
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sin operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sin(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::sin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of cos operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cos(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::cos(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of tan operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor tan(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::tan(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of cot operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cot(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::cos(tmp[i])/std::sin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of asin operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor asin(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::asin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of acos operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor acos(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::acos(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of atan operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor atan(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::atan(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of atan2 operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor atan2(pmTensor const& tensor1, pmTensor const& tensor2) {
		pmTensor tmp{tensor1};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::atan2(tensor1[i],tensor2[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of acot operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor acot(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::acos(tmp[i])/std::asin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sinh operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sinh(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::sinh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of cosh operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cosh(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::cosh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of tanh operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor tanh(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::tanh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of coth operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor coth(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = 1/std::tanh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of min operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor min(pmTensor const& T1, pmTensor const& T2) {
		if(T1.get_numrows()!=T2.get_numrows() || T1.get_numcols()!=T2.get_numcols()) {
			ProLog::pLogger::error_msgf("Tensor dimensions do not agree.\n");
		}
		pmTensor tensor{T1.get_numrows(), T1.get_numcols()};
		for(int i=0; i< T1.numel(); i++) {
			tensor[i] = std::min(T1[i], T2[i]);
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of max operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor max(pmTensor const& T1, pmTensor const& T2) {
		if(T1.get_numrows()!=T2.get_numrows() || T1.get_numcols()!=T2.get_numcols()) {
			ProLog::pLogger::error_msgf("Tensor dimensions do not agree.\n");
		}
		pmTensor tensor{T1.get_numrows(), T1.get_numcols()};
		for(int i=0; i< T1.numel(); i++) {
			tensor[i] = std::max(T1[i], T2[i]);
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of mod operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor mod(pmTensor const& T1, pmTensor const& T2) {
		if(T1.get_numrows()!=T2.get_numrows() || T1.get_numcols()!=T2.get_numcols()) {
			ProLog::pLogger::error_msgf("Tensor dimensions do not agree.\n");
		}
		pmTensor tensor{T1.get_numrows(), T1.get_numcols()};
		for(int i=0; i< T1.numel(); i++) {
			tensor[i] = (int)T1[i] % (int)T2[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of abs operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor abs(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::abs(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of floor operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor floor(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::floor(tmp[i]+NAUTICLE_EPS);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of trunc operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor trunc(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::trunc(tmp[i]+NAUTICLE_EPS);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of round operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor round(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::round(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sqrt operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sqrt(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::sqrt(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of log operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor log(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::log(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of matrix log operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor logm(pmTensor const& tensor) {
		if(tensor.is_square()) {
			ProLog::pLogger::error_msgf("Matrix logarithm cannot be calculated to non-square matrix.");
		} else if(tensor.is_scalar()) {
			return log(tensor);
		}
		pmTensor V = tensor.eigensystem();
		pmTensor D = log(V.inverse()*(tensor)*V);
		return V*D*V.inverse();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of exp operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor exp(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::exp(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sgn operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sgn(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = (0.0 < tmp[i])-(tmp[i] < 0.0);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of pow operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pow(pmTensor const& T1, pmTensor const& T2) {
		if(!T2.is_scalar()) {
			ProLog::pLogger::error_msgf("Unable to evaluate expression with non-scalar power.");
		}
		if(T1.is_scalar()) {
			return pmTensor{(double)std::pow(T1[0],T2[0])};
		}
		int power = (int)T2[0];
		pmTensor tensor{T1};
		for(int i=0; i<power-1; i++) {
			tensor = tensor * T1;
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the cross product of the given vectors.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cross(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_column() || !rhs.is_column() || lhs.numel()<3 || rhs.numel()<3) {
			ProLog::pLogger::error_msgf("Cross product requires [3 by 1] vectors.\n");
		}
		pmTensor tensor{3,1,0};
		tensor[0] = lhs[1]*rhs[2]-lhs[2]*rhs[1];
		tensor[1] = lhs[2]*rhs[0]-lhs[0]*rhs[2];
		tensor[2] = lhs[0]*rhs[1]-lhs[1]*rhs[0];
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor::pmTensor() {
		rows = 0;
		columns = 0;
		// elements = nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor::pmTensor(double const& s) {
		rows = 1;
		columns = 1;
		// elements = new double[1];
		elements[0] = s;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor::pmTensor(int const& r, int const& c, double const& init/*=0*/) {
		if(r<1 || c<1) { return; }
		rows = r;
		columns = c;
		// elements = new double[r*c];
		for(int i=0; i<r*c; i++) {
			elements[i] = init;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor::pmTensor(pmTensor const& other) {
		this->rows = other.rows;
		this->columns = other.columns;
		// elements = new double[rows*columns];
		memcpy(this->elements, other.elements, sizeof(double)*rows*columns);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor::pmTensor(pmTensor&& other) {
		this->rows = std::move(other.rows);
		this->columns = std::move(other.columns);
		// this->elements = other.elements;
		// other.elements = nullptr;
		memcpy(this->elements, other.elements, sizeof(double)*rows*columns);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a pmTensor object with num_components filled with zeros.
	/// If num_components equals:
	///  1: pmTensor is a scalar
	///  2: pmTensor is a 2 by 1 vector
	///  3: pmTensor is a 3 by 1 vector
	///  4: pmTensor is 2 by 2 matrix
	///  9: pmTensor is 3 by 3 matrix.
	/// Otherwise error occurs.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor(int const& num_components) {
		switch(num_components) {
			case 1: return pmTensor{1,1,0};
			case 2: return pmTensor{2,1,0};
			case 3: return pmTensor{3,1,0};
			case 4: return pmTensor{2,2,0};
			case 9: return pmTensor{3,3,0};
			default: ProLog::pLogger::error_msgf("Cannot create tensor with %i components.\n", num_components); break;
		}
		return pmTensor{1,1,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a scalar initialized to 0.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor11() {
		return pmTensor{1,1,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a 1 by 2 vector initialized to 0.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor12() {
		return pmTensor{1,2,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a 2 by 1 initialized to 0.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor21() {
		return pmTensor{2,1,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a 1 by 3 initialized to 0.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor13() {
		return pmTensor{1,3,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a 3 by 1 initialized to 0.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor31() {
		return pmTensor{3,1,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a 2 by 2 initialized to 0.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor22() {
		return pmTensor{2,2,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generates a 3 by 3 initialized to 0.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::Tensor33() {
		return pmTensor{3,3,0};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator=(pmTensor const& other) {
		if(this!=&other) {
			this->rows = other.rows;
			this->columns = other.columns;
			// delete [] elements;
			// elements = new double[rows*columns];
			memcpy(this->elements, other.elements, sizeof(double)*rows*columns);
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator=(pmTensor&& other) {
		if(this!=&other) {
			this->rows = std::move(other.rows);
			this->columns = std::move(other.columns);
			// delete [] elements;
			// this->elements = other.elements;
			// other.elements = nullptr;
			memcpy(this->elements, other.elements, sizeof(double)*rows*columns);
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Destructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor::~pmTensor() {
		// delete [] elements;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Plus and equal operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator+=(pmTensor const& other) {
		*this = *this + other;
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Minus and equal operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator-=(pmTensor const& other) {
		*this = *this - other;
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Multiply and equal operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator*=(pmTensor const& other) {
		*this = *this * other;
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Divide and equal operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator/=(pmTensor const& other) {
		*this = *this / other;
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Increments all elements (prefix).
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator++() {
		// *this += double{1};
		// return *this;
		return this->operator+=(pmTensor{this->rows,this->columns,1.0});
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Decrements all elements (prefix).
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor& pmTensor::operator--() {
		// *this -= double{1};
		// return *this;
		return this->operator-=(pmTensor{this->rows,this->columns,1.0});
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Increments all elements (postfix).
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::operator++(int) {
		pmTensor tensor{*this};	
		++(*this);
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Decrements all elements (postfix).
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::operator--(int) {
		pmTensor tensor{*this};	
		--(*this);
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the number of rows.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline int pmTensor::get_numrows() const {
		return rows;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the number of columns.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline int pmTensor::get_numcols() const {
		return columns;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implements indexing with () brackets.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double pmTensor::operator()(int const& i, int const& j) const {
		if(i*columns+j>=numel() || i>=rows || j>=columns) { return double{0}; }
		return elements[i*columns+j];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implements continuous indexing with () brackets.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double pmTensor::operator()(int const& i) const {
		if(i>=numel()) { return double{0}; }
		return elements[i];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implements continuous indexing with [] brackets.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double& pmTensor::operator[](int const& i) {
		if(i>=numel()) { ProLog::pLogger::error_msgf("Index is out of bounds.\n"); }
		return elements[i];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implements continuous indexing with [] brackets.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double const& pmTensor::operator[](int const& i) const {
		if(i>=numel()) { ProLog::pLogger::error_msgf("Index is out of bounds.\n"); }
		return elements[i];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the tensor is a column vector (number of rows=1 and columns<rows)
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_column() const {
		return (columns==1 && rows>columns);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the tensor is a row vector (number of columns=1 and rows<columns)
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_row() const {
		return (rows==1 && rows<columns);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the tensor is vector (row or column).
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_vector() const {
		return is_row() || is_column();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the tensor is scalar (number of rows=1 and columns=1)
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_scalar() const {
		return numel()==1 ? true : false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the tensor is empty (number of rows=0 and columns=0)
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_empty() const {
		return rows==0 && columns==0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if all elements are zero.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_zero() const {
		if(is_empty()) { return false; }
		for(int i=0; i<numel(); i++) {
			if(elements[i]!=0) {
				return false;
			}
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the tensor is square (numcols=numrows)
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_square() const {
		return rows==columns;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the tensor is singular.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_singular() const {
		pmTensor det = determinant();
		if(det.is_empty()) { return true; }
		return std::abs(det[0])<NAUTICLE_EPS;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Converts vector to row. If the object is scalar or tensor, it does nothing.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::to_row() const {
		pmTensor tensor = *this;
		if(this->is_column()) {
			return tensor.transpose();
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Converts vector to column. If the object is scalar or tensor, it does nothing.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::to_column() const {
		pmTensor tensor = *this;
		if(this->is_row()) {
			return tensor.transpose();
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the product of the tensor elements.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double pmTensor::productum() const {
		double prod{elements[0]};
		for(int i=1; i<numel(); i++) {
			prod *= elements[i];
		}
		return prod;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the summation of the tensor elements.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double pmTensor::summation() const {
		double sum = elements[0];
		for(int i=1; i<numel(); i++) {
			sum += elements[i];
		}
		return sum;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the total number of elements in the tensor (number of rows*columns).
	/////////////////////////////////////////////////////////////////////////////////////////
	inline int pmTensor::numel() const {
		return rows*columns;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the transpose of the tensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::transpose() const {
		pmTensor tensor{columns, rows};
		for(int i=0; i<rows; i++) {
			for(int j=0; j<columns; j++) {
				tensor[j*rows+i] = (*this)(i,j);
			}
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Calculates the sum of the diagonal elements. Returns empty tensor if the tensor is 
	/// not square.
	inline /////////////////////////////////////////////////////////////////////////////////////////
	pmTensor pmTensor::trace() const {
		if(rows!=columns) {
			ProLog::pLogger::error_msgf("Matrix is not square, trace does not exist.\n");
			return pmTensor{};
		}
		pmTensor tensor{1,1};
		for(int i=0; i<rows; i++) {
			tensor[0] += (*this)(i,i);
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the subtensor given by the ranges. If ranges are invalid, it returns an empty
	/// tensor.
	inline /////////////////////////////////////////////////////////////////////////////////////////
	pmTensor pmTensor::sub_tensor(int rmin, int rmax, int cmin, int cmax) const {
		if(rmin<0 || cmin<0 || rmax>=rows || cmax>=columns || rmax<rmin || cmax<cmin) { return pmTensor{0,0}; }
		int scolumns = cmax-cmin+1;
		int srows = rmax-rmin+1;
		pmTensor tensor{srows,scolumns,0};
		int sidx = 0;
		for(int i=rmin; i<=rmax; i++) {
			for(int j=cmin; j<=cmax; j++, sidx++) {
				tensor.elements[sidx] = this->operator()(i,j);
			}
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the subtensor without the r row and c column.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::sub_tensor(int r, int c) const {
		if(r<0 || c<0 || r>=rows || c>=columns) { return pmTensor{0,0}; }
		pmTensor tensor{rows-1,columns-1,0};
		int sidx = 0;
		for(int i=0; i<rows; i++) {
			if(i==r) { continue; }
			for(int j=0; j<columns; j++) {
				if(j==c) { continue; }
				tensor.elements[sidx] = this->operator()(i,j);
				sidx++;
			}
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the determinant of the tensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::determinant() const {
		if(!is_square() || is_empty()) { 
			ProLog::pLogger::error_msgf("Matrix is not square, determinant does not exist.\n");
			return pmTensor{0,0};
		}
		if(numel()==1) { return elements[0]; }
		if(numel()==4) { return elements[0]*elements[3]-elements[1]*elements[2]; }
		pmTensor det{1,1,0};
		int sign = 1;
		for(int i=0; i<rows; i++) {
			det += sign*this->operator()(i,0)*sub_tensor(i,0).determinant();
			sign = -sign;
		}
		return det;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the adjugate of the tensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::adjugate() const {
		if(!is_square() || is_empty()) {
			ProLog::pLogger::error_msgf("Matrix is not square, adjugate does not exist.\n");
			return pmTensor{0,0};
		}
		pmTensor tensor{rows, columns, 0};
		for(int i=0; i<rows; i++) {
			for(int j=0; j<columns; j++) {
				tensor[i*columns+j] = ((i%2)*2-1)*((j%2)*2-1)*sub_tensor(i,j).determinant()[0];
			}
		}
		return tensor.transpose();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the inverse of the tensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::inverse() const {
		if(!is_square() || is_empty()) { 
			ProLog::pLogger::error_msgf("Matrix is not square, inverse does not exist.\n");
			return pmTensor{0,0};
		}
		if(numel()==1) { return 1/elements[0]; }
		if(is_singular()) { 
			ProLog::pLogger::warning_msgf("Matrix is singular, inverse cannot be calculated with real values. Identity is returned.\n");
			return make_identity(columns);
		}
		pmTensor inv;
		return 1.0/determinant()*adjugate();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints tensor content.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline void pmTensor::print() const {
		for(int i=0; i<rows; i++) {
			if(i!=0) ProLog::pLogger::logf(" | ");
			for(int j=0; j<columns; j++) {
				if(j!=0) ProLog::pLogger::logf(" ; ");
				ProLog::pLogger::logf<ProLog::WHT>("%g", elements[i*columns+j]);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the tensor by value.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::copy() const {
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Division term by term.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::divide_term_by_term(pmTensor const& rhs) const {
		if(this->rows!=rhs.rows || this->columns!=rhs.columns) { 
			ProLog::pLogger::error_msgf("Tensor dimensions do not agree for term by term division.\n");
		}
		pmTensor tensor{*this};
		for(int i=0; i<numel(); i++) {
			tensor[i] /= rhs[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Multiplication term by term.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::multiply_term_by_term(pmTensor const& rhs) const {
		if(this->rows!=rhs.rows || this->columns!=rhs.columns) { 
			ProLog::pLogger::error_msgf("Tensor dimensions do not agree for term by term division.\n");
		}
		pmTensor tensor{*this};
		for(int i=0; i<numel(); i++) {
			tensor[i] *= rhs[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Vector length. If not vector, is returns zero. If scalar, it returns the scalar value.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double pmTensor::norm() const {
		if(this->is_vector()) {
			return std::sqrt((this->multiply_term_by_term(*this)).summation());
		} else if(this->is_scalar()) {
			return std::abs(elements[0]);
		}
		return double{0};
	}

	inline double pmTensor::tensor_norm() const {
		if(rows==columns) {
			return std::sqrt(2.0f*((*this)*(*this).transpose()).trace()[0]);
		} else {
			ProLog::pLogger::error_msgf("Tensor norm cannot be calculated to non-square matrix");
		}
		return 0.0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Fills the tensor with the given value. The size remains unchanged.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline void pmTensor::fill(double const& value) {
		for(int i=0; i<numel(); i++){
			elements[i] = value;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns a tensor with the given sizes, filled by value.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::make_tensor(int const& r, int const& c, double const& value) {
		return pmTensor{r,c,value};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns a tensor with the size of base, filled with value.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::make_tensor(pmTensor const& base, double const& value) {
		pmTensor tensor{base};
		tensor.fill(value);
		return tensor;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns an identity tensor of the given size.
	/////////////////////////////////////////////////////////////////////////////////////////
	/*static*/ inline pmTensor pmTensor::make_identity(int const& size) {
		if(size<1) { ProLog::pLogger::error_msgf("Tensor size must be at least one by one."); }
		pmTensor tensor{size,size,0};
		for(int i=0; i<size*size; i+=size+1) {
			tensor[i] = 1.0;
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Writes tensor data to string.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline void pmTensor::write_to_string(std::ostream& os) const {
		for(int i=0; i<rows; i++) {
			if(i!=0) os << "|";
			for(int j=0; j<columns; j++) {
				if(j!=0) os << ";";
				os << (*this)[i*columns+j];
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Reflects tensor in directions where guide is nonzero.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::reflect_perpendicular(pmTensor const& guide) const {
		if(is_scalar()) { return *this; }
		pmTensor R = make_tensor(guide.numel(),guide.numel(),0);
		for(int i=0; i<guide.numel(); i++) {
			R[i*rows+i] = guide[i]!=0 ? -1 : 1;
		}
		return R*(*this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns a tensor appended with zeros to the size row*col.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::append(int const& row, int const& col) const {
		if(this->rows==row && this->columns==col) {
			return *this;
		}
		if(this->rows>row || this->columns>col) {
			ProLog::pLogger::error_msgf("Appended size is smaller than the original.\n");
		}
		pmTensor tensor{row,col,0};
		memcpy(&tensor.elements[0], &this->elements[0], sizeof(double)*this->numel());
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if all the elements os the tensor are integers.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool pmTensor::is_integer() const {
		for(int i=0; i<numel(); i++) {
			double rem = std::abs(elements[i]) - std::round(std::abs(elements[i]));
			if(rem>NAUTICLE_EPS || rem<-NAUTICLE_EPS) {
				return false;
			}
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Performs QR decomposition and returns Q.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::deQ() const {
		if(columns!=rows) {
			ProLog::pLogger::error_msgf("Orthonormal tensor cannot be calculated to non-square matrix.");
		} else if(this->numel()==1) {
			return elements[0];
		} else if(this->numel()==4) {
			pmTensor x1 = this->sub_tensor(0,1,0,0);
			pmTensor x2 = this->sub_tensor(0,1,1,1);
			pmTensor y1 = x1;
			pmTensor y2 = x2 - y1*((y1.to_row()*x2)/(y1.to_row()*y1));
			pmTensor z1 = -y1/y1.norm();
			pmTensor z2 = y2/y2.norm();
			pmTensor Q{2,2,0};
			memcpy(Q.elements, z1.elements, sizeof(double)*2);
			memcpy(&Q.elements[2], z2.elements, sizeof(double)*2);
			return Q.transpose();
		} else if(numel()==9) {
			pmTensor x1 = this->sub_tensor(0,2,0,0);
			pmTensor x2 = this->sub_tensor(0,2,1,1);
			pmTensor x3 = this->sub_tensor(0,2,2,2);
			pmTensor y1 = x1;
			pmTensor y2 = x2 - y1*((y1.to_row()*x2)/(y1.to_row()*y1));
			pmTensor y3 = x3 - y1*((y1.to_row()*x3)/(y1.to_row()*y1)) - y2*((y2.to_row()*x3)/(y2.to_row()*y2));
			pmTensor z1 = -y1/y1.norm();
			pmTensor z2 = -y2/y2.norm();
			pmTensor z3 = -y3/y3.norm();
			pmTensor Q{3,3,0};
			memcpy(Q.elements, z1.elements, sizeof(double)*3);
			memcpy(&Q.elements[3], z2.elements, sizeof(double)*3);
			memcpy(&Q.elements[6], z3.elements, sizeof(double)*3);
			return Q.transpose();
		}
		return pmTensor{};
	}

	// inline void pmTensor::QR(pmTensor& Q, pmTensor& R) const {
	// 	pmTensor A = *this;
	// 	Q = make_identity(rows);
	// 	R = A;
 //        for(int j=0; j<columns; j++) {
 //        	double normx = R.sub_tensor(j,rows-1,j,j).norm();
 //        	double r = R[j*columns+j];
 //        	int s = -r/std::abs(r);
 //        	double u1 = R(j,j) - s*normx;
 //        	pmTensor w = R.sub_tensor(j,rows-1,j,j)/u1;
 //        	w[0]  = 1;
 //        	double tau = -s*u1/normx;
 //        	pmTensor subR = R.sub_tensor(j,rows-1,0,columns-1);
 //        	pmTensor subQ = Q.sub_tensor(0,rows-1,j,columns-1);
 //        	subR = (tau*w)*(w*subR);
 //        	subQ = (subQ*w)*(tau*w);
 //        	for(int i=j;i<rows;i++) {
 //                for(int k=0;k<columns;k++) {
 //                    R[i*columns+k] = R[i*columns+k]-subR[(i-j)*columns+k];
 //                    Q[k*rows+i] = Q[k*rows+i]-subQ[k*rows+i-j];
 //                }
 //        	}
 //        }
 //    }

	// inline pmTensor pmTensor::deQ() const {
	// 	pmTensor Q;
	// 	pmTensor R;
	// 	QR(Q,R);
	// 	return Q;
	// }

	// inline pmTensor pmTensor::deR() const {
	// 	pmTensor Q;
	// 	pmTensor R;
	// 	QR(Q,R);
	// 	return R;
	// }

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Performs QR decomposition and returns R.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::deR() const {
		return this->deQ().transpose()*(*this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Performs QR decomposition writes Q tensor into the given Q and returns R.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::deR(pmTensor& Q) const {
		Q = this->deQ();
		return Q.transpose()*(*this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the matrix with the eigenvectors in its columns.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::eigensystem() const {
		if(rows!=columns) {
			ProLog::pLogger::error_msgf("Eigensystem cannot be calculated to non-square matrix.");
		}
		pmTensor A = *this;
		pmTensor U = make_identity(rows);
		for(int i=1;i<20;i++) {
			pmTensor Q;
			pmTensor R = A.deR(Q);
			// pmTensor Q;
			// pmTensor R;
			// QR(Q,R);
			A = R*Q;
			U = U*Q;
		}
		return U;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Transorms the tensor to diagonal form.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::diagonalize() const {
		pmTensor V = this->eigensystem();
		return V.inverse()*(*this)*V;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the diagonal form og the tensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmTensor::eigenvalues() const {
		if(rows!=columns) {
			ProLog::pLogger::error_msgf("Eigenvalues cannot be calculated to non-square matrix.");
		}
		return this->diagonalize();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the minimum element of the tensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double pmTensor::min() const {
		double min_value = elements[0];
		for(int i=1; i<numel(); i++) {
			if(min_value<elements[i]) {
				min_value = elements[i];
			}
		}
		return min_value;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the maximum element of the tensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline double pmTensor::max() const {
		double max_value = elements[0];
		for(int i=1; i<numel(); i++) {
			if(max_value>elements[i]) {
				max_value = elements[i];
			}
		}
		return max_value;
	}
}


#endif //_TENSOR_H_
