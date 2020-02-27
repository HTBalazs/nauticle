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

#include <vector>
#include "Eigen/Eigen"

namespace nauticle {
	template<typename T>
	class pmCells {
	protected:
		std::vector<size_t> cell_start;
		std::vector<size_t> cell_end;
		std::vector<T> cell_iterator;
	public:
		void update_cells(std::vector<size_t> const& hash_key) {
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
		std::vector<size_t> const& get_start() const {
			return cell_start;
		}
		std::vector<size_t> const& get_end() const {
			return cell_end;
		}
		std::vector<T> const& get_cell_iterator() const {
			return cell_iterator;
		}
	};

	template<size_t S>
	class pmBinary_domain {};

	template<>
	class pmBinary_domain<1> : public pmCells<int> {
		double minimum;
		double maximum;
		size_t cell_num;
		double cell_size;
		size_t boundary;
	public:
		pmBinary_domain()=default;
		pmBinary_domain(double bmin, double bmax, size_t cn, size_t bnd) : minimum{bmin}, maximum{bmax}, cell_num{cn}, boundary{bnd} {
			cell_size = (maximum-minimum) / cell_num;
			cell_start.resize(cell_num);
			cell_end.resize(cell_num);
			cell_iterator = {-1, 0, 1};
		}
		size_t compute_hash_key(double pos) const {
			return std::round(std::floor(pos/cell_size)-minimum/cell_size);
		}
		void compute_hash_key(std::vector<double> const& pos, std::vector<size_t>& hash_key) const {
			hash_key.resize(pos.size());
			for(int i=0; i<pos.size(); i++) {
				hash_key[i] = compute_hash_key(pos[i]);
			}
		}
		size_t get_dimensions() const {
			return 1;
		}
		size_t get_boundary() const {
			return boundary;
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
	public:
		pmBinary_domain()=default;
		pmBinary_domain(Eigen::Vector2d const& bmin, Eigen::Vector2d const& bmax, Eigen::Vector2i const cn, Eigen::Vector2i const& bnd) : minimum{bmin}, maximum{bmax}, cell_num{cn}, boundary{bnd} {
			cell_size = (maximum-minimum).cwiseQuotient(cell_num.cast<double>());
			cell_start.resize(cell_num.prod());
			cell_end.resize(cell_num.prod());
			cell_iterator = {{-1,-1}, {0,-1}, {1,-1}, {-1,0}, {0,0}, {1,0}, {-1,1}, {0,1}, {1,1}};
		}
		size_t compute_hash_key(Eigen::Vector2d const& pos) const {
			Eigen::Vector2i grid_pos = round2(floor2(pos.cwiseQuotient(cell_size))-minimum.cwiseQuotient(cell_size).cast<int>());
			return std::round(cell_num(0)*grid_pos(1)+grid_pos(0));
		}
		void compute_hash_key(std::vector<Eigen::Vector2d> const& pos, std::vector<size_t>& hash_key) const {
			hash_key.resize(pos.size());
			for(int i=0; i<pos.size(); i++) {
				hash_key[i] = compute_hash_key(pos[i]);
			}
		}
		size_t get_dimensions() const {
			return 2;
		}
		Eigen::Vector2i get_boundary() const {
			return boundary;
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
	public:
		pmBinary_domain()=default;
		pmBinary_domain(Eigen::Vector3d const& bmin, Eigen::Vector3d const& bmax, Eigen::Vector3i const cn, Eigen::Vector3i const& bnd) : minimum{bmin}, maximum{bmax}, cell_num{cn}, boundary{bnd} {
			cell_size = (maximum-minimum).cwiseQuotient(cell_num.cast<double>());
			cell_start.resize(cell_num.prod());
			cell_end.resize(cell_num.prod());
			cell_iterator = {{-1,-1,-1},{0,-1,-1},{1,-1,-1},{-1,0,-1},{0,0,-1},{1,0,-1},{-1,1,-1},{0,1,-1},{1,1,-1},{-1,-1,0},{0,-1,0},{1,-1,0},{-1,0,0},{0,0,0},{1,0,0},{-1,1,0},{0,1,0},{1,1,0},{-1,-1,1},{0,-1,1},{1,-1,1},{-1,0,1},{0,0,1},{1,0,1},{-1,1,1},{0,1,1},{1,1,1}};
		}
		size_t compute_hash_key(Eigen::Vector3d const& pos) const {
			Eigen::Vector3i grid_pos = round3(floor3(pos.cwiseQuotient(cell_size))-minimum.cwiseQuotient(cell_size).cast<int>());
			return std::round(cell_num(0)*(cell_num(1)*grid_pos(2)+grid_pos(1))+grid_pos(0));
		}
		void compute_hash_key(std::vector<Eigen::Vector3d> const& pos, std::vector<size_t>& hash_key) const {
			hash_key.resize(pos.size());
			for(int i=0; i<pos.size(); i++) {
				hash_key[i] = compute_hash_key(pos[i]);
			}
		}
		size_t get_dimensions() const {
			return 3;
		}
		Eigen::Vector3i get_boundary() const {
			return boundary;
		}
	};
}

#endif //_CELLS_H_