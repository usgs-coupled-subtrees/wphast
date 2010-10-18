#pragma once

#ifndef __vtkCellPicker2_h
#define __vtkCellPicker2_h

#include "vtkPicker.h"

class vtkCellPicker2 : public vtkCellPicker
{
public:
	static vtkCellPicker2 *New();
	vtkTypeRevisionMacro(vtkCellPicker2, vtkCellPicker);
// COMMENT: {9/9/2008 9:06:48 PM}	void PrintSelf(ostream& os, vtkIndent indent);
// COMMENT: {9/9/2008 9:06:48 PM}
	virtual int Pick(double selectionX, double selectionY, double selectionZ, vtkRenderer *renderer);

protected:
	vtkCellPicker2(void);
	~vtkCellPicker2(void);

private:
	vtkCellPicker2(const vtkCellPicker2&);  // Not implemented.
	void operator=(const vtkCellPicker2&);  // Not implemented.
};

#endif
