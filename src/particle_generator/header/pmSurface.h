#include "pmTensor.h"
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>
#include <vector>
#include <memory>

namespace Nauticle {
	class pmSurface {
	protected:
		std::vector<std::string> file_name;
		std::vector<vtkSmartPointer<vtkPolyData>> poly_data;
	public:
		void add_file_name(std::string const& fn);
		void print() const;
		void cut(std::vector<pmTensor>& grid) const;
		void update();
	};
}























