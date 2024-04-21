#include "pmTensor.h"
#include "pmBackground.h"
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkAppendFilter.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkCellArray.h>
#include <vtkIntArray.h>
#include <string>
#include <vector>
#include <memory>
#include <array>

namespace Nauticle {
	class pmSolid : public pmBackground {
	protected:
		static size_t counter;
		vtkSmartPointer<vtkSTLReader> reader;
		std::shared_ptr<pmField> normal_field;
		std::shared_ptr<pmField> potential_field;
		std::shared_ptr<pmField> wall_velocity;
		std::shared_ptr<pmExpression> center;
		std::shared_ptr<pmExpression> rotation;
		std::shared_ptr<pmExpression> thickness;
		double previous_thickness;
		size_t export_counter = 0;
		mutable bool exprt = true;
	protected:
		void solidify();
		void read_file() override;
		void transform(double const& dt);
		template<class T> void remove_duplicates(std::vector<T>& normals) const;
		pmTensor get_vertex_normal(pmTensor const& vertex, std::vector<pmTensor> const& normals, int i1, int i2, int i3) const;
	public:
		pmSolid() { counter++; }
		void print() const override;
		void set_thickness(std::shared_ptr<pmExpression> thk);
		void set_field(std::shared_ptr<pmField> fld) override {}
		void set_normal_field(std::shared_ptr<pmField> nrm);
		void set_potential_field(std::shared_ptr<pmField> pot);
		void set_wall_velocity(std::shared_ptr<pmField> wall_vel);
		void set_center(std::shared_ptr<pmExpression> ctr);
		void set_rotation(std::shared_ptr<pmExpression> rot);
		void initialize() override;
		void update(double const& dt) override;
		void interpolate() override;
		void write_geometry(std::string const& fn) const override;
	};
}























