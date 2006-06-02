#pragma once
#include <vtkInteractorStyleImage.h>

class vtkInteractorStyleImage2 : public vtkInteractorStyleImage
{
public:
	vtkTypeRevisionMacro(vtkInteractorStyleImage2,vtkInteractorStyleImage);
	static vtkInteractorStyleImage2 *New();

	// from vtkInteractorStyleTrackballCamera
	virtual void Spin(void);

protected:
	vtkInteractorStyleImage2(void);
	virtual ~vtkInteractorStyleImage2(void);

	int SpinEnabled;

private:
	vtkInteractorStyleImage2(const vtkInteractorStyleImage2&);  // Not implemented.
	void operator=(const vtkInteractorStyleImage2&);  // Not implemented.

};
