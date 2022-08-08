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
    
#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "pmTensor.h"

namespace Nauticle {
	template <typename T>
	class pmQuaternion {
		T w;
		T x;
		T y;
		T z;
	public:
		pmQuaternion() : w{0.0}, x{0.0}, y{0.0}, z{0.0} {}
		pmQuaternion(T const& ww, T const& xx, T const& yy, T const& zz) : w{ww}, x{xx}, y{yy}, z{zz} {}
		bool operator==(pmQuaternion const& rhs) const;
		bool operator!=(pmQuaternion const& rhs) const;
		pmQuaternion operator+(pmQuaternion const& rhs) const;
		pmQuaternion operator-(pmQuaternion const& rhs) const;
		pmQuaternion operator*(pmQuaternion const& rhs) const;
		pmQuaternion& operator+=(pmQuaternion const& rhs);
		pmQuaternion& operator-=(pmQuaternion const& rhs);
		pmQuaternion& operator*=(pmQuaternion const& rhs);
		T dot_product(pmQuaternion const& rhs) const;
		pmQuaternion inverse() const;
		static pmQuaternion vector2quaternion(pmTensor const& vec);
		static pmTensor quaternion2vector(pmQuaternion const& quat);
		static pmQuaternion make_rotation_quaternion(pmTensor const& axis, double const& angle);
	};

	template <typename T>
	bool pmQuaternion<T>::operator==(pmQuaternion<T> const& rhs) const {
		if(this->w==rhs.w && this->x==rhs.x && this->y==rhs.y && this->z==rhs.z) {
			return true;
		}
		return false;
	}

	template <typename T>
	bool pmQuaternion<T>::operator!=(pmQuaternion<T> const& rhs) const {
		return !pmQuaternion<T>::operator==(rhs);
	}

	template <typename T>
	pmQuaternion<T> pmQuaternion<T>::operator+(pmQuaternion<T> const& rhs) const {
		return pmQuaternion<T>{this->w+rhs.w,this->x+rhs.x,this->y+rhs.y,this->z+rhs.z};
	}

	template <typename T>
	pmQuaternion<T> pmQuaternion<T>::operator-(pmQuaternion<T> const& rhs) const {
		return pmQuaternion<T>{this->w-rhs.w,this->x-rhs.x,this->y-rhs.y,this->z-rhs.z};
	}

	template <typename T>
	pmQuaternion<T> pmQuaternion<T>::operator*(pmQuaternion<T> const& rhs) const {
		pmQuaternion<T> q;
		q.w = rhs.w * this->w - rhs.x * this->x - rhs.y * this->y - rhs.z * this->z;
		q.x = rhs.w * this->x + rhs.x * this->w - rhs.y * this->z + rhs.z * this->y;
		q.y = rhs.w * this->y + rhs.x * this->z + rhs.y * this->w - rhs.z * this->x;
		q.z = rhs.w * this->z - rhs.x * this->y + rhs.y * this->x + rhs.z * this->w;
		return q;
	}

	template <typename T>
	pmQuaternion<T>& pmQuaternion<T>::operator+=(pmQuaternion<T> const& rhs) {
		*this = *this + rhs;
		return *this;
	}

	template <typename T>
	pmQuaternion<T>& pmQuaternion<T>::operator-=(pmQuaternion<T> const& rhs) {
		*this = *this - rhs;
		return *this;
	}

	template <typename T>
	pmQuaternion<T>& pmQuaternion<T>::operator*=(pmQuaternion<T> const& rhs) {
		*this = *this * rhs;
		return *this;
	}

	template <typename T>
	T pmQuaternion<T>::dot_product(pmQuaternion const& rhs) const {
		return (this->w * rhs.w) + (this->x * rhs.x) + (this->y * rhs.y) + (this->z * rhs.z);
	}

	template <typename T>
	pmQuaternion<T> pmQuaternion<T>::vector2quaternion(pmTensor const& vec) {
		return pmQuaternion{0.0, vec[0], vec[1], vec[2]};
	}

	template <typename T>
	pmTensor pmQuaternion<T>::quaternion2vector(pmQuaternion const& quat) {
		pmTensor t{3,1,0};
		t[0] = quat.x;
		t[1] = quat.y;
		t[2] = quat.z;
		return t;
	}

	template <typename T>
	pmQuaternion<T> pmQuaternion<T>::inverse() const {
		return pmQuaternion{this->w,-this->x,-this->y,-this->z};
	}

	template <typename T>
	pmQuaternion<T> pmQuaternion<T>::make_rotation_quaternion(pmTensor const& axis, double const& angle) {
		return pmQuaternion{std::cos(angle/2.0),axis[0]*std::sin(angle/2.0),axis[1]*std::sin(angle/2.0),axis[2]*std::sin(angle/2.0)};
	}
}

#endif //_QUATERNION_H_