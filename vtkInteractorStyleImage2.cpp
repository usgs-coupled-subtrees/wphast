#include "StdAfx.h"
#include "vtkInteractorStyleImage2.h"


#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

vtkCxxRevisionMacro(vtkInteractorStyleImage2, "");
vtkStandardNewMacro(vtkInteractorStyleImage2);


vtkInteractorStyleImage2::vtkInteractorStyleImage2(void)
: SpinEnabled(0)
{
}

vtkInteractorStyleImage2::~vtkInteractorStyleImage2(void)
{
}

void vtkInteractorStyleImage2::Spin()
{
	if (this->SpinEnabled) Superclass::Spin();
}
