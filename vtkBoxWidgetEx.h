#pragma once
#include <vtkBoxWidget.h>

class vtkBoxWidgetEx : public vtkBoxWidget
{
public:
	vtkTypeRevisionMacro(vtkBoxWidgetEx, vtkBoxWidget);
	static vtkBoxWidgetEx *New();

	vtkSetMacro(HexPickerEnabled, int);
	vtkGetMacro(HexPickerEnabled, int);
	vtkBooleanMacro(HexPickerEnabled, int);

protected:
	// Handles the events
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

	// ProcessEvents() dispatches to these methods.
	void OnLeftButtonDown();

	int HexPickerEnabled;

protected:
	vtkBoxWidgetEx(void);
	virtual ~vtkBoxWidgetEx(void);

};
