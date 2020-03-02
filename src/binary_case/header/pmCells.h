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

#ifndef _CELLS_H_  
#define _CELLS_H_

#include "Eigen/Eigen"
#include <vector>

namespace Nauticle {
	
	template<typename T>
	class pmCells {
	protected:
		mutable std::vector<size_t> cell_start;
		mutable std::vector<size_t> cell_end;
		std::vector<T> cell_iterator;
	public:
		void update_cells(std::vector<size_t> const& hash_key) const;
		std::vector<size_t> const& get_start() const;
		std::vector<size_t> const& get_end() const;
		std::vector<T> const& get_cell_iterator() const;
	};

	template<typename T> void pmCells<T>::update_cells(std::vector<size_t> const& hash_key) const {
		std::fill(cell_start.begin(),cell_start.end(),0xFFFFFFFF);
		std::fill(cell_end.begin(),cell_end.end(),0xFFFFFFFF);
		cell_start[hash_key[0]] = 0;
		cell_end[hash_key[0]] = 0;
		for(int i=1; i<hash_key.size(); i++){
			if(hash_key[i] != hash_key[i-1]) {
				cell_start[hash_key[i]] = i;
				cell_end[hash_key[i-1]] = i-1;
			}
			if(hash_key.size() == i+1) {
				cell_end[hash_key[i]] = i;
			}
		}
	}
	template<typename T> std::vector<size_t> const& pmCells<T>::get_start() const {
		return cell_start;
	}
	template<typename T> std::vector<size_t> const& pmCells<T>::get_end() const {
		return cell_end;
	}
	template<typename T> std::vector<T> const& pmCells<T>::get_cell_iterator() const {
		return cell_iterator;
	}


	template<size_t S>
	class pmBinary_domain {};

	template<>
	class pmBinary_domain<1> : public pmCells<int> {
		double minimum;
		double maximum;
		size_t cell_num;
		double cell_size;
		size_t boundary;
		void init_cells() {
			cell_start.resize(cell_num);
			cell_end.resize(cell_num);
			cell_iterator = {-1, 0, 1};
		}
	public:
		pmBinary_domain()=default;
		pmBinary_domain(double bmin, double bmax, size_t cn, size_t bnd) : minimum{bmin}, maximum{bmax}, cell_num{cn}, boundary{bnd} {
			cell_size = (maximum-minimum) / cell_num;
			this->init_cells();
		}
		pmBinary_domain& operator=(pmDomain const& dm) {
			minimum = dm.get_physical_minimum()[0];
			maximum = dm.get_physical_maximum()[0];
			cell_size = dm.get_cell_size()[0];
			boundary = dm.get_boundary()[0];
			cell_num = (dm.get_maximum()-dm.get_minimum())[0];
			this->init_cells();
			return *this;
		}
		size_t compute_hash_key(double pos) const {
			return this->grid_pos(pos);
		}
		void compute_hash_key(std::vector<double> const& pos, std::vector<size_t>& hash_key) const {
			hash_key.resize(pos.size());
			for(int i=0; i<pos.size(); i++) {
				hash_key[i] = compute_hash_key(pos[i]);
			}
			this->update_cells(hash_key);
		}
		size_t get_dimensions() const {
			return 1;
		}
		size_t get_boundary() const {
			return boundary;
		}
		size_t get_number_of_cells() const {
			return cell_num;
		}
		double get_cell_size() const {
			return cell_size;
		}
		size_t grid_pos(double pos) const {
			return std::round(std::floor(pos/cell_size)-minimum/cell_size);
		}
		double get_minimum() const {
			return minimum;
		}
		double get_maximum() const {
			return maximum;
		}
	};

	template<typename T>
	Eigen::Vector2i floor2(T const& vec) {
		Eigen::Vector2i flvec;
		flvec(0) = std::floor((double)vec(0));
		flvec(1) = std::floor((double)vec(1));
		return flvec;
	}

	template<typename T>
	Eigen::Vector2i round2(T const& vec) {
		Eigen::Vector2i flvec;
		flvec(0) = std::round((double)vec(0));
		flvec(1) = std::round((double)vec(1));
		return flvec;
	}

	template<>
	class pmBinary_domain<2> : public pmCells<Eigen::Vector2i> {
		Eigen::Vector2d minimum;
		Eigen::Vector2d maximum;
		Eigen::Vector2i cell_num;
		Eigen::Vector2d cell_size;
		Eigen::Vector2i boundary;
		void init_cells() {
			cell_start.resize(cell_num.prod());
			cell_end.resize(cell_num.prod());
			cell_iterator = {{-1,-1}, {0,-1}, {1,-1}, {-1,0}, {0,0}, {1,0}, {-1,1}, {0,1}, {1,1}};
		}
	public:
		pmBinary_domain()=default;
		pmBinary_domain(Eigen::Vector2d const& bmin, Eigen::Vector2d const& bmax, Eigen::Vector2i const cn, Eigen::Vector2i const& bnd) : minimum{bmin}, maximum{bmax}, cell_num{cn}, boundary{bnd} {
			cell_size = (maximum-minimum).cwiseQuotient(cell_num.cast<double>());
			this->init_cells();
		}
		pmBinary_domain& operator=(pmDomain const& dm) {
			minimum = {dm.get_physical_minimum()[0],dm.get_physical_minimum()[1]};
			maximum = {dm.get_physical_maximum()[0],dm.get_physical_maximum()[1]};
			cell_size = {dm.get_cell_size()[0],dm.get_cell_size()[1]};
			boundary = {dm.get_boundary()[0],dm.get_cell_size()[1]};
			cell_num = {(dm.get_maximum()-dm.get_minimum())[0],(dm.get_maximum()-dm.get_minimum())[1]};
			this->init_cells();
			return *this;
		}
		size_t compute_hash_key(Eigen::Vector2d const& pos) const {
			Eigen::Vector2i grid_pos = this->grid_pos(pos);
			return std::round(cell_num(0)*grid_pos(1)+grid_pos(0));
		}
		void compute_hash_key(std::vector<Eigen::Vector2d> const& pos, std::vector<size_t>& hash_key) const {
			hash_key.resize(pos.size());
			for(int i=0; i<pos.size(); i++) {
				hash_key[i] = compute_hash_key(pos[i]);
			}
			this->update_cells(hash_key);
		}
		size_t get_dimensions() const {
			return 2;
		}
		Eigen::Vector2i get_boundary() const {
			return boundary;
		}
		Eigen::Vector2i get_number_of_cells() const {
			return cell_num;
		}
		Eigen::Vector2d get_cell_size() const {
			return cell_size;
		}
		Eigen::Vector2i grid_pos(Eigen::Vector2d pos) const {
			return round2(floor2(pos.cwiseQuotient(cell_size))-minimum.cwiseQuotient(cell_size).cast<int>());
		}
		Eigen::Vector2d get_minimum() const {
			return minimum;
		}
		Eigen::Vector2d get_maximum() const {
			return maximum;
		}
	};

	template<typename T>
	Eigen::Vector3i floor3(T const& vec) {
		Eigen::Vector3i flvec;
		flvec(0) = std::floor((double)vec(0));
		flvec(1) = std::floor((double)vec(1));
		flvec(2) = std::floor((double)vec(2));
		return flvec;
	}

	template<typename T>
	Eigen::Vector3i round3(T const& vec) {
		Eigen::Vector3i flvec;
		flvec(0) = std::round((double)vec(0));
		flvec(1) = std::round((double)vec(1));
		flvec(2) = std::round((double)vec(2));
		return flvec;
	}

	template<>
	class pmBinary_domain<3> : public pmCells<Eigen::Vector3i> {
		Eigen::Vector3d minimum;
		Eigen::Vector3d maximum;
		Eigen::Vector3i cell_num;
		Eigen::Vector3d cell_size;
		Eigen::Vector3i boundary;
		void init_cells() {
			cell_start.resize(cell_num.prod());
			cell_end.resize(cell_num.prod());
			cell_iterator = {{-1,-1,-1},{0,-1,-1},{1,-1,-1},{-1,0,-1},{0,0,-1},{1,0,-1},{-1,1,-1},{0,1,-1},{1,1,-1},{-1,-1,0},{0,-1,0},{1,-1,0},{-1,0,0},{0,0,0},{1,0,0},{-1,1,0},{0,1,0},{1,1,0},{-1,-1,1},{0,-1,1},{1,-1,1},{-1,0,1},{0,0,1},{1,0,1},{-1,1,1},{0,1,1},{1,1,1}};
		}
	public:
		pmBinary_domain()=default;
		pmBinary_domain(Eigen::Vector3d const& bmin, Eigen::Vector3d const& bmax, Eigen::Vector3i const cn, Eigen::Vector3i const& bnd) : minimum{bmin}, maximum{bmax}, cell_num{cn}, boundary{bnd} {
			cell_size = (maximum-minimum).cwiseQuotient(cell_num.cast<double>());
			this->init_cells();
		}
		pmBinary_domain& operator=(pmDomain const& dm) {
			minimum = {dm.get_physical_minimum()[0],dm.get_physical_minimum()[1],dm.get_physical_minimum()[2]};
			maximum = {dm.get_physical_maximum()[0],dm.get_physical_maximum()[1],dm.get_physical_maximum()[2]};
			cell_size = {dm.get_cell_size()[0],dm.get_cell_size()[1],dm.get_cell_size()[2]};
			boundary = {(int)dm.get_boundary()[0],(int)dm.get_cell_size()[1],(int)dm.get_cell_size()[2]};
			cell_num = {(int)(dm.get_maximum()-dm.get_minimum())[0],(int)(dm.get_maximum()-dm.get_minimum())[1],(int)(dm.get_maximum()-dm.get_minimum())[2]};
			this->init_cells();
			return *this;
		}
		size_t compute_hash_key(Eigen::Vector3d const& pos) const {
			Eigen::Vector3i grid_pos = this->grid_pos(pos);
			return std::round(cell_num(0)*(cell_num(1)*grid_pos(2)+grid_pos(1))+grid_pos(0));
		}
		void compute_hash_key(std::vector<Eigen::Vector3d> const& pos, std::vector<size_t>& hash_key) const {
			hash_key.resize(pos.size());
			for(int i=0; i<pos.size(); i++) {
				hash_key[i] = compute_hash_key(pos[i]);
			}
			this->update_cells(hash_key);
		}
		size_t get_dimensions() const {
			return 3;
		}
		Eigen::Vector3i get_boundary() const {
			return boundary;
		}
		Eigen::Vector3i get_number_of_cells() const {
			return cell_num;
		}
		Eigen::Vector3d get_cell_size() const {
			return cell_size;
		}
		Eigen::Vector3i grid_pos(Eigen::Vector3d pos) const {
			return round3(floor3(pos.cwiseQuotient(cell_size))-minimum.cwiseQuotient(cell_size).cast<int>());
		}
		Eigen::Vector3d get_minimum() const {
			return minimum;
		}
		Eigen::Vector3d get_maximum() const {
			return maximum;
		}
	};
}

#endif //_CELLS_H_