![nauticle_logo_small.png](https://bitbucket.org/repo/yraAqM/images/4040395855-nauticle_logo_small.png)

# Introduction #
**Nauticle** is a parallel particle based **general purpose** explicit numerical solver under the GNU Lesser General Public License (LGPL). It is designed to make the implementation of particle based models easy and efficient. Some of the particle based solvers are already implemented (n-body problem, SPH, DEM, DVM, SFM) and **new contributions are welcome**.
The implemented schemes can be employed to build **system of differential and algebraic equations** depending on functions which are interpreted over a spatial distribution of particles.
# Installation #

**Nauticle** has a few dependencies which are the following open-source libraries:  
VTK 7.0.0 (http://www.vtk.org/download/)  
ProLog (https://bitbucket.org/BalazsToth/prolog)  
HandyXML (https://bitbucket.org/BalazsToth/handyxml)  
CommonUtils (https://bitbucket.org/BalazsToth/commonutils)  

Nauticle can be installed manually or by the automated installation script available in the Installation repository. Currently the automated installation works only on MAC OS systems or Linux distributions that support APT.
The Installation script can be downloaded from here.

# User's Guide and Documentation #
The detailed guide and source code documentation is provided for users as well as developers.

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

The simulations above with the results included are available here. The results can be visualized in Paraview v5.2.0 or later.