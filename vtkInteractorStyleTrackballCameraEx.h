#pragma once
#include <vtkinteractorstyletrackballcamera.h>

class vtkInteractorStyleTrackballCameraEx : public vtkInteractorStyleTrackballCamera
{
public:
	vtkTypeRevisionMacro(vtkInteractorStyleTrackballCameraEx,vtkInteractorStyleTrackballCamera);

	static vtkInteractorStyleTrackballCameraEx *New();

	virtual void HighlightProp3D(vtkProp3D *prop3D);

protected:
	vtkInteractorStyleTrackballCameraEx(void);
	virtual ~vtkInteractorStyleTrackballCameraEx(void);

private:
	vtkInteractorStyleTrackballCameraEx(const vtkInteractorStyleTrackballCameraEx&);  // Not implemented.
	void operator=(const vtkInteractorStyleTrackballCameraEx&);  // Not implemented.
};
