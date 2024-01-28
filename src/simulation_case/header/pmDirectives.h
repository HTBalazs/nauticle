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

#define HASH_KEY "\treturn flatten(maximum-minimum, grid_pos, 0);"

#define UPDATE_NEIGHBORS \
"\tstd::fill(pidx.begin(),pidx.end(),-1);\n \
\tstd::fill(cidx.begin(),cidx.end(),-1);\n \
\tfor(int i=0; i<"+pmSymbol::prefix+"v_numparticles; i++) {\n \
\t\tVector grid_crd = grid_coordinates("+pmSymbol::prefix+"psys_r[i][0]);\n \
\t\tint hkey = hash_key(grid_crd);\n \
\t\tif(hkey>cidx.size() || hkey<0) {\n \
\t\t\treturn false;\n \
\t\t}\n \
\t\tif(cidx[hkey]!=-1) {\n \
\t\t\tpidx[i] = cidx[hkey];\n \
\t\t}\n \
\t\tcidx[hkey] = i;\n \
\t}\n \
\tup_to_date = true;\n \
\treturn up_to_date;"

#define RESTRICT_PARTICLES \
"\tif(up_to_date) { return; }\n \
\tVector cell_number = maximum-minimum;\n \
\tfor(int i=0; i<"+pmSymbol::prefix+"psys_r.size(); i++) {\n \
\t\tVector g = grid_coordinates("+pmSymbol::prefix+"psys_r[i][0]);\n \
\t\tVector shift = (g-pmRuntime_interface::coefficientwise_operation(g, cell_number,[](double a, double b){return a-std::floor(a/b)*b;}))"+std::string(dimensions==1?"":".array()")+" * cell_size"+std::string(dimensions==1?"":".array()")+";\n \
\t\t"+pmSymbol::prefix+"f_periodic_jump[i][0] += (shift"+std::string(dimensions==1?"":".array()")+"/(cell_number"+std::string(dimensions==1?"":".array()")+"*cell_size"+std::string(dimensions==1?"":".array()")+")).matrix();\n \
\t\tsize_t deletable = 0;\n \
\t\tfor(int j=0; j<"+pmSymbol::prefix+"psys_r[i][0].size(); j++) {\n \
\t\t\t"+pmSymbol::prefix+"psys_r[i][0][j] -= shift[j]*(boundary[j]!=2);\n \
\t\t\tdeletable += (shift[j]!=0 && boundary[j]==2);\n \
\t\t}\n \
\t\tif(deletable>0) {\n \
\t\t\tdel.push_back(i);\n \
\t\t}\n \
\t}"

#define PARALLEL \
"\tstd::vector<std::thread> th;\n \
\tint p_end = "+pmSymbol::prefix+"psys_r.size();\n \
\tint number_of_threads = std::min((int)num_threads,p_end);\n \
\tint ppt = p_end/number_of_threads;\n \
\tfor(int i=0; i<p_end; i+=ppt) {\n \
\t\tth.push_back(std::thread{process, i, i+ppt});\n \
\t}\n \
\tfor(auto& it:th) {\n \
\t\tit.join();\n \
\t}"

#define FLATTEN \
"\tif(i>=2) { return 0.0; }\n \
\treturn std::round((cells[i])*flatten(cells, grid_pos, i+1)+grid_pos[i]);"

#define GRID_COORDINATES "\treturn ((point.array()/cell_size.array()).floor()-minimum.array()).round();"

#define INTERACTION \
"\tT result{};\n \
\tif constexpr (std::is_same<T, Vector>::value) {\n \
\t\tresult = Vector{0,0};\n \
\t}\n \
\tVector gi = grid_coordinates("+pmSymbol::prefix+"psys_r[i][0]);\n \
\tfor(auto const& it:cell_iterator) {\n \
\t\tfor(int j=cidx[hash_key(gi+it)]; j!=-1; j=pidx[j]) {\n \
\t\t\tVector rel_pos = "+pmSymbol::prefix+"psys_r[j][0] - "+pmSymbol::prefix+"psys_r[i][0];\n \
\t\t\tresult += contribute(rel_pos, i, j);\n \
\t\t}\n \
\t}\n \
\treturn result;"




