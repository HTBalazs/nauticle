clear 
close all
clc

R = 2;
[x,y] = meshgrid(linspace(-R,R,100));
r = sqrt(x.*x+y.*y);


eps = 2;
kappa = 0.5;
C1=0;
C2=1;
delta = pi/2;
psi = C1+C2*(1/4*r.^4-1/2*eps*r.^2)+kappa*(x.*sin(delta)-y*cos(delta));



contourf(x,y,psi,200);
axis equal