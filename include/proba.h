#ifndef _PROBA_H_
#define _PROBA_H_

class Built_case {
	std::shared_ptr<pmWorkspace> workspace;
	std::vector<std::shared_ptr<pmEquation>> equations;
public:
	void solve();
};

void Built_case::solve() {
	// eq0
	double T = workspace->get_value("t", 0)[0];
	double dt = workspace->get_value("dt", 0)[0];
	workspace->get_instance("t").lock()->set_value(pmTensor{T+dt}, 0);
	// eq1 value="T_dot=a*sph_L00(T,mass,rho0,Wp22220,csize)" condition="eq(gid,0)"
	for
}


#endif // _PROBA_H_