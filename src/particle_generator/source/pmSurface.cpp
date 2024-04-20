#include "pmSurface.h"
#include "pmRandom.h"
#include "commonutils/Common.h"
#include <vtkPoints.h>
#include <vtkSTLReader.h>
#include <vtkSelectEnclosedPoints.h>

using namespace Nauticle;
using namespace ProLog;

void pmSurface::add_file_name(std::string const& fn) {
	file_name.push_back(fn);
}

void pmSurface::print() const {

}

void pmSurface::update() {
	for(auto const& it:file_name) {
		vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
		reader->SetFileName(it.c_str());
		reader->Update();
		poly_data.push_back(reader->GetOutput());
	}
}

void pmSurface::cut(std::vector<pmTensor>& grid) const {
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for(int i=0; i<grid.size(); i++) {
		points->InsertNextPoint(grid[i][0],grid[i][1],grid[i][2]);
	}

	vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
	pointsPolydata->SetPoints(points);

	// Points inside test.
	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints->SetInputData(pointsPolydata);
	selectEnclosedPoints->SetSurfaceData(poly_data[0]);
	selectEnclosedPoints->SetTolerance(1e-10);
	selectEnclosedPoints->Update();

	std::vector<size_t> del;
	del.reserve(grid.size());
	for(unsigned int i = 0; i < grid.size(); ++i) {
		if(selectEnclosedPoints->IsInside(i) != 1) {
			del.push_back(i);
		}
	}
	Common::delete_indices(grid,del);
}




