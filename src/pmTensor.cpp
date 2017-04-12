/*
    Copyright 2016-2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#include <cmath>
#include "pmTensor.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor() {
	rows = 0;
	columns = 0;
	elements = nullptr;
	scalar = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(double const& s) {
	rows = 1;
	columns = 1;
	elements = new double[1];
	elements[0] = s;
	scalar = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(int const& r, int const& c, double const& init/*=0*/, bool const& sc/*=true*/) {
	if(r<1 || c<1) { return; }
	rows = r;
	columns = c;
	elements = new double[r*c];
	for(int i=0; i<r*c; i++) {
		elements[i] = init;
	}
	scalar = r*c>1 ? false : sc;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(pmTensor const& other) {
	this->rows = other.rows;
	this->columns = other.columns;
	this->scalar = other.scalar;
	elements = new double[rows*columns];
	memcpy(this->elements, other.elements, sizeof(double)*rows*columns);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(pmTensor&& other) {
	this->rows = std::move(other.rows);
	this->columns = std::move(other.columns);
	this->elements = other.elements;
	this->scalar = other.scalar;
	other.elements = nullptr;
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
/*static*/ pmTensor pmTensor::Tensor(int const& num_components) {
	switch(num_components) {
		case 1: return pmTensor{1,1,0};
		case 2: return pmTensor{2,1,0};
		case 3: return pmTensor{3,1,0};
		case 4: return pmTensor{2,2,0};
		case 9: return pmTensor{3,3,0};
		default: pLogger::error_msgf("Cannot create tensor with %i components.\n", num_components); break;
	}
	return pmTensor{1,1,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a scalar initialized to 0.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::Tensor11() {
	return pmTensor{1,1,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a 1 by 2 vector initialized to 0.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::Tensor12() {
	return pmTensor{1,2,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a 2 by 1 initialized to 0.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::Tensor21() {
	return pmTensor{2,1,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a 1 by 3 initialized to 0.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::Tensor13() {
	return pmTensor{1,3,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a 3 by 1 initialized to 0.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::Tensor31() {
	return pmTensor{3,1,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a 2 by 2 initialized to 0.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::Tensor22() {
	return pmTensor{2,2,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a 3 by 3 initialized to 0.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::Tensor33() {
	return pmTensor{3,3,0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator=(pmTensor const& other) {
	if(this!=&other) {
		this->rows = other.rows;
		this->columns = other.columns;
		this->scalar = other.scalar;
		delete [] elements;
		elements = new double[rows*columns];
		memcpy(this->elements, other.elements, sizeof(double)*rows*columns);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator=(pmTensor&& other) {
	if(this!=&other) {
		this->rows = std::move(other.rows);
		this->columns = std::move(other.columns);
		this->scalar = other.scalar;
		delete [] elements;
		this->elements = other.elements;
		other.elements = nullptr;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Destructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::~pmTensor() {
	delete [] elements;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Plus and equal operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator+=(pmTensor const& other) {
	*this = *this + other;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Minus and equal operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator-=(pmTensor const& other) {
	*this = *this - other;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Multiply and equal operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator*=(pmTensor const& other) {
	*this = *this * other;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Divide and equal operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator/=(pmTensor const& other) {
	*this = *this / other;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Increments all elements (prefix).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator++() {
	// *this += double{1};
	// return *this;
	return this->operator+=(double{1});
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Decrements all elements (prefix).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator--() {
	// *this -= double{1};
	// return *this;
	return this->operator-=(double{1});
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Increments all elements (postfix).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::operator++(int) {
	pmTensor tensor{*this};	
	(*this)++;
	return tensor;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Decrements all elements (postfix).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::operator--(int) {
	pmTensor tensor{*this};	
	(*this)--;
	return tensor;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of rows.
/////////////////////////////////////////////////////////////////////////////////////////
int pmTensor::get_numrows() const {
	return rows;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of columns.
/////////////////////////////////////////////////////////////////////////////////////////
int pmTensor::get_numcols() const {
	return columns;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements indexing with () brackets.
/////////////////////////////////////////////////////////////////////////////////////////
double pmTensor::operator()(int const& i, int const& j) const {
	if(i*columns+j>=numel() || i>=rows || j>=columns) { return double{0}; }
	return elements[i*columns+j];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements continuous indexing with () brackets.
/////////////////////////////////////////////////////////////////////////////////////////
double pmTensor::operator()(int const& i) const {
	if(i>=numel()) { return double{0}; }
	return elements[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements continuous indexing with [] brackets.
/////////////////////////////////////////////////////////////////////////////////////////
double& pmTensor::operator[](int const& i) {
	if(i>=numel()) { pLogger::error_msgf("Index is out of bounds.\n"); }
	return elements[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements continuous indexing with [] brackets.
/////////////////////////////////////////////////////////////////////////////////////////
double const& pmTensor::operator[](int const& i) const {
	if(i>=numel()) { pLogger::error_msgf("Index is out of bounds.\n"); }
	return elements[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is a column vector (number of rows=1 and columns<rows)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_column() const {
	return (columns==1 && rows>columns);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is a row vector (number of columns=1 and rows<columns)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_row() const {
	return (rows==1 && rows<columns);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is vector (row or column).
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_vector() const {
	return is_row() || is_column() || (!scalar && numel()==1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is scalar (number of rows=1 and columns=1)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_scalar() const {
	return scalar;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is empty (number of rows=0 and columns=0)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_empty() const {
	return rows==0 && columns==0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if all elements are zero.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_zero() const {
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
bool pmTensor::is_square() const {
	return rows==columns;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is singular.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_singular() const {
	pmTensor det = determinant();
	if(det.is_empty()) { return true; }
	return std::abs(det[0])<1e-12;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Converts vector to row. If the object is scalar or tensor, it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::to_row() const {
	pmTensor tensor = *this;
	if(this->is_column()) {
		return tensor.transpose();
	}
	return tensor;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Converts vector to column. If the object is scalar or tensor, it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::to_column() const {
	pmTensor tensor = *this;
	if(this->is_row()) {
		return tensor.transpose();
	}
	return tensor;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the product of the tensor elements.
/////////////////////////////////////////////////////////////////////////////////////////
double pmTensor::productum() const {
	double prod{elements[0]};
	for(int i=1; i<numel(); i++) {
		prod *= elements[i];
	}
	return prod;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the summation of the tensor elements.
/////////////////////////////////////////////////////////////////////////////////////////
double pmTensor::summation() const {
	double sum = elements[0];
	for(int i=1; i<numel(); i++) {
		sum += elements[i];
	}
	return sum;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the total number of elements in the tensor (number of rows*columns).
/////////////////////////////////////////////////////////////////////////////////////////
int pmTensor::numel() const {
	return rows*columns;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the transpose of the tensor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::transpose() const {
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
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::trace() const {
	if(rows!=columns) {
		pLogger::error_msgf("Tensor must be square.\n");
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
/////////////////////////////////////////////////////////////////////////////////////////
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
pmTensor pmTensor::sub_tensor(int r, int c) const {
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
pmTensor pmTensor::determinant() const {
	if(!is_square() || is_empty()) { 
		pLogger::error_msgf("Matrix is not square, determinant does not exist.\n");
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
pmTensor pmTensor::adjugate() const {
	if(!is_square() || is_empty()) {
		pLogger::error_msgf("Matrix is not square, adjugate does not exist.\n");
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
pmTensor pmTensor::inverse() const {
	if(!is_square() || is_empty()) { 
		pLogger::error_msgf("Matrix is not square, inverse does not exist.\n");
		return pmTensor{0,0};
	}
	if(numel()==1) { return 1/elements[0]; }
	if(is_singular()) { 
		pLogger::warning_msgf("Matrix is singular, inverse cannot be calculated with real values. Identity is returned.\n");
		return make_identity(columns);
	}
	pmTensor inv;
	return 1.0/determinant()*adjugate();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints tensor content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTensor::print() const {
	for(int i=0; i<rows; i++) {
		if(i!=0) pLogger::logf(" | ");
		for(int j=0; j<columns; j++) {
			if(j!=0) pLogger::logf(" ; ");
			pLogger::logf<WHT>("%g", elements[i*columns+j]);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the tensor by value.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::copy() const {
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Division term by term.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::divide_term_by_term(pmTensor const& rhs) const {
	if(this->rows!=rhs.rows || this->columns!=rhs.columns) { 
		pLogger::error_msgf("Tensor dimensions do not agree for term by term division.\n");
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
pmTensor pmTensor::multiply_term_by_term(pmTensor const& rhs) const {
	if(this->rows!=rhs.rows || this->columns!=rhs.columns) { 
		pLogger::error_msgf("Tensor dimensions do not agree for term by term division.");
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
double pmTensor::norm() const {
	if(this->is_vector()) {
		return sqrt((this->multiply_term_by_term(*this)).summation());
	} else if(this->is_scalar()) {
		return std::abs(elements[0]);
	}
	return double{0};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Fills the tensor with the given value. The size remains unchanged.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTensor::fill(double const& s) {
	for(int i=0; i<numel(); i++){
		elements[i] = s;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a tensor with the given sizes, filled by value.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::make_tensor(int const& r, int const& c, double const& value) {
	return pmTensor{r,c,value};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a tensor with the size of base, filled with value.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::make_tensor(pmTensor const& base, double const& value) {
	pmTensor tensor{base};
	tensor.fill(value);
	return tensor;
}
/*static*/ pmTensor pmTensor::make_identity(int const& size) {
	if(size<1) { pLogger::error_msgf("Tensor size must be at least one by one."); }
	pmTensor tensor{size,size,0};
	for(int i=0; i<size*size; i+=size+1) {
		tensor[i] = 1.0;
	}
	return tensor;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes tensor data to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTensor::write_to_string(std::ostream& os) const {
	if(numel()==1 && !scalar) {
		os << (*this)[0] << "*e_i";
	}
	for(int i=0; i<rows; i++) {
		if(i!=0) os << "|";
		for(int j=0; j<columns; j++) {
			if(j!=0) os << ";";
			os << (*this)[i*columns+j];
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the scalar value to sc. If numel>1 it does not changes scalar.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTensor::set_scalar(bool const& sc) {
	scalar = numel()>1 ? scalar : sc;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reflects tensor in directions where guide is nonzero.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::reflect(pmTensor const& guide) const {
	if(scalar) { return *this; }
	pmTensor R = make_tensor(guide.numel(),guide.numel(),0);
	for(int i=0; i<guide.numel(); i++) {
		R[i*rows+i] = guide[i]!=0 ? -1 : 1;
	}
	return R*(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a tensor appended with zeros to the size row*col.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmTensor::append(int const& row, int const& col) const {
	if(this->rows==row && this->columns==col) {
		return *this;
	}
	if(this->rows>row || this->columns>col) {
		pLogger::error_msgf("Appended size is smaller than the original.\n");
	}
	pmTensor tensor{row,col,0};
	memcpy(&tensor.elements[0], &this->elements[0], sizeof(double)*this->numel());
	return tensor;
}
























