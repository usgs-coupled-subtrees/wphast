#pragma once
#include <vtkBoxWidget.h>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class vtkBoxWidgetEx : public vtkBoxWidget
{
public:
	vtkTypeRevisionMacro(vtkBoxWidgetEx, vtkBoxWidget);
	static vtkBoxWidgetEx *New();

	vtkSetMacro(HexPickerEnabled, int);
	vtkGetMacro(HexPickerEnabled, int);
	vtkBooleanMacro(HexPickerEnabled, int);

// COMMENT: {5/10/2006 9:26:25 PM}	virtual void PlaceWidget(float bounds[6]);

protected:
	// Handles the events
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

	// ProcessEvents() dispatches to these methods.
	void OnLeftButtonDown();

	int HexPickerEnabled;
	virtual void SizeHandles();
	double SizeHandles(double factor);

protected:
	vtkBoxWidgetEx(void);
	virtual ~vtkBoxWidgetEx(void);

};
