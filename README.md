![nauticle_logo_small.png](https://bitbucket.org/repo/yraAqM/images/4040395855-nauticle_logo_small.png)

# Introduction #
**Nauticle** is a parallel particle based **general purpose** explicit numerical solver under the GNU Lesser General Public License (LGPL). It is designed to make the implementation of particle based models easy and efficient. Some of the particle based solvers are already implemented (n-body problem, SPH, DEM, DVM, SFM) and **new contributions are welcome**.
The implemented schemes can be employed to build **system of differential and algebraic equations** depending on functions which are interpreted over a spatial distribution of particles.
# Installation #

**Nauticle** has a few dependencies which are the following open-source libraries:  
- [VTK 7.0.0](http://www.vtk.org/download/)  
- [ProLog](https://bitbucket.org/BalazsToth/prolog)  
- [HandyXML](https://bitbucket.org/BalazsToth/handyxml)  
- [CommonUtils](https://bitbucket.org/BalazsToth/commonutils)  

Nauticle can be installed manually or by the automated installation script available in the Installation repository. Currently the automated installation works only on MAC OS systems or Linux distributions that support APT.
The Installation script can be downloaded from here.

# User's Guide and Documentation #
The detailed [guide](https://bitbucket.org/nauticleproject/guide) and [source code documentation](https://bitbucket.org/nauticleproject/documentation/overview) is provided for users as well as developers.

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

The simulations above with the results included are available [here](https://bitbucket.org/nauticleproject/examples). The results can be visualized in Paraview v5.2.0 or later.

# License #
The Nauticle code is open-source and freely distributable under the terms of GNU Lesser General Public License v3:  
  
Copyright 2016-2017 Balazs Toth  
  
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