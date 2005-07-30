#pragma once
#include <vtkimplicitplanewidget.h>

class CGridLineWidget : public vtkImplicitPlaneWidget
{
public:
  // Description:
  // Instantiate the object.
  static CGridLineWidget *New();

  vtkTypeRevisionMacro(CGridLineWidget,vtkImplicitPlaneWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void SetEnabled(int);

protected:
	CGridLineWidget(void);
	~CGridLineWidget(void);

private:
  CGridLineWidget(const CGridLineWidget&);  //Not implemented
  void operator=(const CGridLineWidget&);  //Not implemented
};
