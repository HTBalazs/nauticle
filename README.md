![logo_small.png](https://bitbucket.org/repo/yraAqM/images/386051192-logo_small.png)

# Introduction #
**LEMPS** (Langrangian Engine for Methods of ParticleS) is a parallel particle based **general purpose** explicit numerical solver under the GNU Lesser General Public License (LGPL). It is designed to make the implementation of particle based models easy and efficient. Some of the particle based solvers are already implemented (SPH, basic DEM, n-body problem) and **new contributions are welcome**.  
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

## Three body-problem : 8 shaped choreography ##
![anim.gif](https://bitbucket.org/repo/yraAqM/images/2159458539-anim.gif)
## Two dimensional heat conduction problem ##
![2d_heat_square.png](https://bitbucket.org/repo/yraAqM/images/3091408574-2d_heat_square.png)
## Two dimensional basic DEM simulation ##
![anim.gif](https://bitbucket.org/repo/yraAqM/images/1876226342-anim.gif)
## Relaxation (e.g. for initial configuration)
![tessellation.gif](https://bitbucket.org/repo/yraAqM/images/647679836-tessellation.gif)