#pragma once
#include <vtkinteractorstyletrackballcamera.h>

class vtkInteractorStyleTrackballCameraEx : public vtkInteractorStyleTrackballCamera
{
public:
	vtkTypeRevisionMacro(vtkInteractorStyleTrackballCameraEx,vtkInteractorStyleTrackballCamera);

	static vtkInteractorStyleTrackballCameraEx *New();
	virtual void Delete();

	virtual void HighlightProp3D(vtkProp3D *prop3D);

	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();
	virtual void OnChar();
	virtual void OnKeyPress();

	vtkSetMacro(PickWithMouse,int);
	///void SetPickWithMouse(float p);
	vtkGetMacro(PickWithMouse,int);

protected:
	vtkInteractorStyleTrackballCameraEx(void);
	virtual ~vtkInteractorStyleTrackballCameraEx(void);

	//vtkAssemblyPath *AssemblyPath;
	int LastEventPosition[2];
	vtkProp *LastProp;

private:
	vtkInteractorStyleTrackballCameraEx(const vtkInteractorStyleTrackballCameraEx&);  // Not implemented.
	void operator=(const vtkInteractorStyleTrackballCameraEx&);  // Not implemented.

	int PickWithMouse;
};
