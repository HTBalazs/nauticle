# Introduction #
**LEMPS** (Langrangian Engine for Methods of ParticleS) is a particle based **general purpose** explicit numerical solver. It is designed to make the implementation of particle based models easy and efficient. Some of the particle based solvers are already implemented (SPH, basic DEM, n-body problem) and **new contributions are welcome**.  
The implemented schemes can be employed to build **system of differential and algebraic equations** depending on functions which are interpreted in a spatial distribution of particles.
# Installation #

**LEMPS** has a few dependencies which are the following open-source libraries:  
VTK 7.0.0 (http://www.vtk.org/download/)  
ProLog (https://bitbucket.org/BalazsToth/prolog)  
HandyXML (https://bitbucket.org/BalazsToth/handyxml)  
CommonUtils (https://bitbucket.org/BalazsToth/commonutils)  

After the installation of the libraries above the simple method to build LEMPS is: 

```
#!shell

cmake CMakeLists.txt
sudo make
```
It generates an executable in the **<LEMPS dir>/bin** directory with the name **pmsimple**.
For more information about the usage please type

```
#!shell

./pmsimple -help
```
int the terminal or open the **<LEMPS dir>/examples** directory.

# Documentation #

For detailed programming reference please visit:

# Examples #