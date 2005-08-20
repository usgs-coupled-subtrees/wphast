#pragma once
#include <vtkCommand.h>

class vtkRenderWindowInteractor;

class CModifyGridCommand : public vtkCommand
{
public:
// COMMENT: {8/19/2005 5:06:41 PM}	vtkTypeRevisionMacro(CModifyGridCommand, vtkCommand);

	static CModifyGridCommand *New();

public:
	virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData);

	void SetInteractor(vtkRenderWindowInteractor* iren);

protected:
	CModifyGridCommand(void);
	virtual ~CModifyGridCommand(void);

protected:
	vtkRenderWindowInteractor *Interactor;

private:
	CModifyGridCommand(const CModifyGridCommand&);  // Not implemented.
	void operator=(const CModifyGridCommand&);      // Not implemented.
};
