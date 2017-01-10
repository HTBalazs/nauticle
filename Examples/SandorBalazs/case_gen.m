clear
close all
clc

% constants
radius = 1;
csize = 0.1;
dx = csize/2.1;
thickness = csize;
rho0 = 1000;
mass = dx^2*rho0;
dt = 0.0001;
phi = 0;
eps = 0.95;
kappa = 4*sqrt(3)/9*eps^3;
% variables
T = 0;
% fields
d = 'magnitude(r)';
h = 'd^4-eps^2*d^2+eps^2-1'; % átirni!
hx = '4*d^2*elem(r,0,0)-2*eps^2*elem(r,0,0)';
hy = '4*d^2*elem(r,0,1)-2*eps^2*elem(r,0,1)';
dh = 'hx|hy';
u = '0';
u_dot = '0';

%% case assemlbing
% generate points
k=2;
for rd=dx:dx:radius+thickness
    cf = rd*2*pi;
    nt = round(cf/dx);
    dphi = cf/nt/rd;
    for alpha=0:dphi:2*pi-dphi
        x(k) = rd*cos(alpha);
        y(k) = rd*sin(alpha);
        k=k+1;
    end
end
k=k-1;

%% generate vtk file
fileID = fopen('initial_condition.vtk','w');

fprintf(fileID, '# vtk DataFile Version 4.0\nvtk output\nASCII\nDATASET POLYDATA\n');
fprintf(fileID, 'FIELD FieldData %i\n', 4);
fprintf(fileID, 'domain 1 3 string\n');
fprintf(fileID, 'domain_min:%i|%i\n', -round((radius+thickness)/csize)-1, -round((radius+thickness)/csize)-1);
fprintf(fileID, 'domain_max:%i|%i\n', round((radius+thickness)/csize)+1, round((radius+thickness)/csize)+1);
fprintf(fileID, 'cell_size:%1.6f\n\n', csize);
fprintf(fileID, 'variables 1 1 string\n');
fprintf(fileID, 'T:0\n\n');
fprintf(fileID, 'constants 1 10 string\n');
fprintf(fileID, 'radius:%g\n', radius);
fprintf(fileID, 'csize:%g\n', csize);
fprintf(fileID, 'dx:%g\n', dx);
fprintf(fileID, 'thickness:%g\n', thickness);
fprintf(fileID, 'rho0:%g\n', 1000);
fprintf(fileID, 'mass:%g\n', dx^2*rho0);
fprintf(fileID, 'dt:%g\n', dt);
fprintf(fileID, 'phi:%g\n', phi);
fprintf(fileID, 'kappa:%g\n', kappa);
fprintf(fileID, 'eps:%g\n\n', eps);
fprintf(fileID, 'functions 1 3 string\n');
fprintf(fileID, 'eq0:T=(T+dt)\n');
fprintf(fileID, 'eq1:u_dot=if(gid,0,(h*sph_L00(u,mass,rho0,Wp22220)-transpose(dh)*sph_G00(u,mass,rho0,Wp22220))+kappa*(hx*sin(phi)-hy*cos(phi)))\n');
fprintf(fileID, 'p2:u=u+int1(u_dot,dt)\n\n');


fprintf(fileID, 'POINTS %i float\n', k);
for i=1:k
    fprintf(fileID, '%1.6e %1.6e %1.6e\n', x(i), y(i), 0);
end
fprintf(fileID,'\n');

fprintf(fileID, 'VERTICES %i %i\n', k*2, k*2);
for i=1:k
	fprintf(fileID, '1 %i\n', i);
end

fprintf(fileID, 'POINT_DATA %i\n', k);
fprintf(fileID, 'SCALARS id float\nLOOKUP_TABLE default\n');
for i=1:k
    fprintf(fileID, '%i\n', i);
end

fprintf(fileID, 'FIELD FieldData %i\n', 8);
fprintf(fileID, 'r 3 %i float\n', k);
for i=1:k
    fprintf(fileID, '%1.6e %1.6e %1.6e\n', x(i), y(i), 0);
end

fprintf(fileID, 'gid 1 %i float\n', k);
for i=1:k
	d = sqrt(x(i)^2+y(i)^2);
	gid = 0;
	if d>radius
		gid = 1;
	end
    fprintf(fileID, '%i\n', gid);
end

fprintf(fileID, 'u 1 %i float\n', k);
for i=1:k
    fprintf(fileID, '%i\n', 0);
end

fprintf(fileID, 'u_dot 1 %i float\n', k);
for i=1:k
    fprintf(fileID, '%i\n', 0);
end

fprintf(fileID, 'h 1 %i float\n', k);
h(1) = eps^2-1;
for i=1:k
	d = sqrt(x(i)^2+y(i)^2);
	h = d^4-eps^2*d^2+eps^2-1;
    fprintf(fileID, '%1.6e\n', h);
end

fprintf(fileID, 'hx 1 %i float\n', k);
for i=1:k
	d = sqrt(x(i)^2+y(i)^2);
	hx = 4*d^2*x(i)-2*eps^2*x(i);	
    fprintf(fileID, '%1.6e\n', hx);
end

fprintf(fileID, 'hy 1 %i float\n', k);
for i=1:k
	d = sqrt(x(i)^2+y(i)^2);
	hy = 4*d^2*y(i)-2*eps^2*y(i);	
    fprintf(fileID, '%1.6e\n', hy);
end

fprintf(fileID, 'dh 3 %i float\n', k);
for i=1:k
	d = sqrt(x(i)^2+y(i)^2);
	hx = 4*d*x(i)-2*x(i)*eps^2;	
	hy = 4*d*y(i)-2*y(i)*eps^2;	
    fprintf(fileID, '%1.6e %1.6e %1.6e\n', hx, hy, 0);
end

% fclose(fileID);
% plot(x,y,'r.');
% axis equal;









