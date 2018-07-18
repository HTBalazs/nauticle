
namespace Nauticle {
	class pmCompiled_simulation {
	private:
		std::shared_ptr<pmConstant> Lx;
		std::shared_ptr<pmConstant> Ly;
		std::shared_ptr<pmConstant> H;
		std::shared_ptr<pmConstant> W;
		std::shared_ptr<pmConstant> csize;
		std::shared_ptr<pmConstant> dx;
		std::shared_ptr<pmConstant> h;
		std::shared_ptr<pmConstant> rho0;
		std::shared_ptr<pmConstant> mass;
		std::shared_ptr<pmConstant> g;
		std::shared_ptr<pmConstant> c;
		std::shared_ptr<pmConstant> fluid;
		std::shared_ptr<pmConstant> wall;
		std::shared_ptr<pmConstant> CFL;
		std::shared_ptr<pmVariable> dt;
		std::shared_ptr<pmVariable> T;
		std::shared_ptr<pmField> a;
        std::shared_ptr<pmField> v;
        std::shared_ptr<pmField> rho;
        std::shared_ptr<pmField> rho_dot;
        std::shared_ptr<pmField> p;
        std::shared_ptr<pmParticle_system> r;
        std::shared_ptr<pmSph_operator<DIVERGENCE,0,0,5>> OP_001;
    public:
    	void synchronize_workspace(std::shared_ptr<pmWorkspace>& ws) {
			Lx = ws->get_instance("Lx").lock();
			Ly = ws->get_instance("Ly").lock();
			H = ws->get_instance("H").lock();
			W = ws->get_instance("W").lock();
			csize = ws->get_instance("csize").lock();
			dx = ws->get_instance("dx").lock();
			h = ws->get_instance("h").lock();
			rho0 = ws->get_instance("rho0").lock();
			mass = ws->get_instance("mass").lock();
			g = ws->get_instance("g").lock();
			c = ws->get_instance("c").lock();
			fluid = ws->get_instance("fluid").lock();
			wall = ws->get_instance("wall").lock();
			CFL = ws->get_instance("CFL").lock();
			dt = ws->get_instance("dt").lock();
			T = ws->get_instance("T").lock();
			a = ws->get_instance("a").lock();
			v = ws->get_instance("v").lock();
			rho = ws->get_instance("rho").lock();
			rho_dot = ws->get_instance("rho_dot").lock();
			p = ws->get_instance("p").lock();
			r = ws->get_instance("r").lock();
    	}
    	void set_operators(std::shared_ptr<pmEquation> eqs) {
    		OP_001 = ;
    	}
    	void solve() {
    		T.set_value(T.get_value()+dt.get_value());
    		for(int i=0; i<N; i++) {
      			rho_dot.set_value(-rho.get_value()*OP_001->evaluate(i,level),i);
    		}

    	}
	};
}