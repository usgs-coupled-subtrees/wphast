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

	void RotateZ(float);
	void SetOrientation(float x, float y, float z);

// COMMENT: {5/10/2006 9:26:25 PM}	virtual void PlaceWidget(float bounds[6]);
	virtual void SetEnabled(int);

	//{{
	vtkPoints* GetPoints(void)
	{
		return this->Points;
	}
	//}}

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
