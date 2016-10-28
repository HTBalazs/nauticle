/////////////////////////////////////////////////////////////////////////////
// Name:        pmTensor.cpp
// Purpose:     pmTensor class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "pmTensor.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor() {
	rows = 0;
	columns = 0;
	elements = nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(float const& s) {
	rows = 1;
	columns = 1;
	elements = new float[1];
	elements[0] = s;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(int const& r, int const& c, float const& init/*=0*/) {
	if(r<1 || c<1) { return; }
	rows = r;
	columns = c;
	elements = new float[r*c];
	for(int i=0; i<r*c; i++) {
		elements[i] = init;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(pmTensor const& other) {
	this->rows = other.rows;
	this->columns = other.columns;
	elements = new float[rows*columns];
	memcpy(this->elements, other.elements, sizeof(float)*rows*columns);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor::pmTensor(pmTensor&& other) {
	this->rows = std::move(other.rows);
	this->columns = std::move(other.columns);
	this->elements = other.elements;
	other.elements = nullptr;
}

/*static*/ pmTensor pmTensor::Tensor(int const& num_components) {
	switch(num_components) {
		case 1: return pmTensor{1,1,0};
		case 2: return pmTensor{2,1,0};
		case 3: return pmTensor{3,1,0};
		case 4: return pmTensor{2,2,0};
		case 9: return pmTensor{3,3,0};
		default: pLogger::error_msgf("Cannot create tensor with %i components.\n", num_components); break;
	}
}

/*static*/ pmTensor pmTensor::Tensor11() {
	return pmTensor{1,1,0};
}

/*static*/ pmTensor pmTensor::Tensor12() {
	return pmTensor{1,2,0};
}

/*static*/ pmTensor pmTensor::Tensor21() {
	return pmTensor{2,1,0};
}

/*static*/ pmTensor pmTensor::Tensor13() {
	return pmTensor{1,3,0};
}

/*static*/ pmTensor pmTensor::Tensor31() {
	return pmTensor{3,1,0};
}

/*static*/ pmTensor pmTensor::Tensor22() {
	return pmTensor{2,2,0};
}

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
		delete [] elements;
		elements = new float[rows*columns];
		memcpy(this->elements, other.elements, sizeof(float)*rows*columns);
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
	// *this += float{1};
	// return *this;
	return this->operator+=(float{1});
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Decrements all elements (prefix).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor& pmTensor::operator--() {
	// *this -= float{1};
	// return *this;
	return this->operator-=(float{1});
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
float pmTensor::operator()(int const& i, int const& j) const {
	if(i*columns+j>=numel()) { return float{0}; }
	return elements[i*columns+j];
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Implements continuous indexing with () brackets.
/////////////////////////////////////////////////////////////////////////////////////////
float pmTensor::operator()(int const& i) const {
	if(i>=numel()) { return float{0}; }
	return elements[i];
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Implements continuous indexing with [] brackets.
/////////////////////////////////////////////////////////////////////////////////////////
float& pmTensor::operator[](int const& i) {
	if(i>=numel()) { pLogger::error_msgf("Index is out of bounds.\n"); }
	return elements[i];
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Implements continuous indexing with [] brackets.
/////////////////////////////////////////////////////////////////////////////////////////
float const& pmTensor::operator[](int const& i) const {
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
	return is_row() || is_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is scalar (number of rows=1 and columns=1)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_scalar() const {
	return rows==1 && columns==1;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the tensor is empty (number of rows=0 and columns=0)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmTensor::is_empty() const {
	return rows==0 && columns==0;
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
float pmTensor::productum() const {
	float prod{elements[0]};
	for(int i=1; i<numel(); i++) {
		prod *= elements[i];
	}
	return prod;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the summation of the tensor elements.
/////////////////////////////////////////////////////////////////////////////////////////
float pmTensor::summation() const {
	float sum = elements[0];
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
float pmTensor::norm() const {
	if(this->is_vector()) {
		return sqrt((this->multiply_term_by_term(*this)).summation());
	} else if(this->is_scalar()) {
		return std::abs(elements[0]);
	}
	return float{0};
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Fills the tensor with the given value. The size remains unchanged.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTensor::fill(float const& scalar) {
	for(int i=0; i<numel(); i++){
		elements[i] = scalar;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a tensor with the given sizes, filled by value.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::make_tensor(int const& r, int const& c, float const& value) {
	return pmTensor{r,c,value};
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a tensor with the size of base, filled with value.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ pmTensor pmTensor::make_tensor(pmTensor const& base, float const& value) {
	pmTensor tensor{base};
	tensor.fill(value);
	return tensor;
}
/*static*/ pmTensor pmTensor::make_identity(int const& size) {
	if(size<1) { pLogger::error_msgf("Tensor size must be at least one by one."); }
	pmTensor tensor{size,size,0};
	for(int i=0; i<size*size; i+=size+1) {
		tensor[i] = 1.0f;
	}
	return tensor;
}

void pmTensor::write_to_string(std::ostream& os) const {
	for(int i=0; i<rows; i++) {
		if(i!=0) os << "|";
		for(int j=0; j<columns; j++) {
			if(j!=0) os << ":";
			os << (*this)[i*columns+j];
		}
	}
}