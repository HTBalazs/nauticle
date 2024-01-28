#ifndef _RUNTIME_INTERFACE_H_
#define _RUNTIME_INTERFACE_H_

#include "c2c/c2Interface.h"
#include "pmWorkspace.h"
#include <string>
#include <functional>

namespace Nauticle {
	class pmRuntime_interface : public c2c::c2Interface {
	protected:
		template<class T> static T coefficientwise_operation(T const& a, T const& b, std::function<double(double,double)> op);
	public:
		virtual bool solve(double const& current_time, size_t const& num_threads, std::string const& name="")=0;
		virtual void initialize()=0;
		virtual void set_workspace(std::shared_ptr<pmWorkspace> ws)=0;
	};

	template<class T> /*static*/ T pmRuntime_interface::coefficientwise_operation(T const& a, T const& b, std::function<double(double,double)> op) {
		T c = a;
		for(int i=0; i<a.rows(); i++) {
			for(int j=0; j<a.cols(); j++) {
				c(i,j) = op(a(i,j),b(i,j));
			}
		}
		return c;
	}

	template<> /*static*/ double pmRuntime_interface::coefficientwise_operation(double const& a, double const& b, std::function<double(double,double)> op) {
		return op(a,b);
	}
}

#endif // _RUNTIME_INTERFACE_H_