![nauticle_logo_small.png](https://bitbucket.org/repo/yraAqM/images/4040395855-nauticle_logo_small.png)

# Introduction #
**Nauticle** is a parallel particle based **general purpose** explicit numerical solver under the GNU Lesser General Public License (LGPL). It is designed to make the implementation of particle based models easy and efficient. Some of the particle based solvers are already implemented (n-body problem, SPH, DEM, DVM, SFM) and **new contributions are welcome**.
The implemented schemes can be employed to build **system of differential and algebraic equations** depending on functions which are interpreted over a spatial distribution of particles.

# Installation #

**Nauticle** has a few dependencies which are the following open-source libraries:  
- [VTK 7.0.0](http://www.vtk.org/download/)  
- [ProLog](https://bitbucket.org/BalazsToth/prolog)  
- [yaml-cpp](https://github.com/jbeder/yaml-cpp/)  
- [CommonUtils](https://bitbucket.org/BalazsToth/commonutils)  

Nauticle can be installed manually or by the automated installation script available in the Installation repository. Currently the automated installation works only on MAC OS systems or Linux distributions that support APT.
The Installation script can be downloaded here.

# User's Guide and Documentation #
The detailed [source code documentation](https://bitbucket.org/nauticleproject/documentation/downloads) is provided for users as well as developers. The User's guide is available [here](https://bitbucket.org/nauticleproject/guide/downloads/).

# Examples #
The few examples discussed in the User's guide cover the following simulations:  
  1. Couette-flow (SPH)  
  2. Phase separation on a sphere with Cahn-Hilliard equation (SPH)  
  3. Particle damper (DEM)  
  4. Breaking of free-jet (DVM)  
  5. Solar System (N-body problem)  
  6. Dam-break flow (SPH)  
  7. Melting solid phase (SPH)  
  8. Building evacuation (SFM)  
  9. Molecular dynamics (MC)  

The simulations above with the results included are available [here](https://bitbucket.org/nauticleproject/examples). The results can be visualized in Paraview v5.2.0 or later.

Please visit the Nauticle [YouTube-channel](https://www.youtube.com/channel/UCh3Y325U289BiAqJ6K0Lfqw) to see the visualizations of the examples.

# Version 1.0.180524: #
The most important features and bug-fixes:  
- Implementation of the Lennard-Jones interaction.  
- Fix kernel bug in SPH inertia tensor.  
- domain.vtk file output for all simulations.  
- Redesigned kernel function class.  
- Renamed class pmCase_manager to class pmSimulation.  
- Implement arithmetic function "limit" in the SFL-tree.  
- Zeroth and first order kernel implementations.  

# Version 1.0.180207: #
The most important features and bug-fixes:  
- Fix the bug of particle interaction switch for SPH with variable smoothing length (commit:5f46690).  
- Domains support for different cell sizes in different directions.  
- Implementation of matrix-logarithm function ("logm").  
- Implementation of eigen-system calculation (eigenvectors and eigenvalues).  
- Extension of position field for symmetric and periodic boundary conditions in SPH operators.  
- Change configuration file type from XML to YAML for better readability.  
- New SPH operator for tensile instability correction (commit: 42cc737).  
- New SPH operator for artificial viscosity (commit: 8c98566).  
- SPH operator for the calculation of particle-inertia tensor (commit: dc0c2ae).  

# License #
The Nauticle code is open-source and freely distributable under the terms of GNU Lesser General Public License v3:  
  
Copyright 2016-2018 Balazs Toth  
  
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