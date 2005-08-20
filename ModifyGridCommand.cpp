#include "StdAfx.h"
#include "ModifyGridCommand.h"

CModifyGridCommand* CModifyGridCommand::New(void)
{
	return new CModifyGridCommand;
}

CModifyGridCommand::CModifyGridCommand(void)
: Interactor(0)
{
	TRACE("CModifyGridCommand ctor\n");
}

CModifyGridCommand::~CModifyGridCommand(void)
{
	TRACE("CModifyGridCommand dtor\n");

	if (this->Interactor)
	{
		//this->Interactor->RemoveObserver(this);
		//this->Interactor->UnRegister(this);
	}
}

void CModifyGridCommand::SetInteractor(vtkRenderWindowInteractor* iren)
{
	if (iren == this->Interactor)
	{
		return;
	}

	// if we already have an Interactor then stop observing it
	if (this->Interactor)
	{
		//this->Interactor->UnRegister(this);
	}

	this->Interactor = iren;
    //this->Interactor->Register(this);

	// these methods increment reference count
	//
	this->Interactor->AddObserver(vtkCommand::LeftButtonPressEvent,   this, 10);
	this->Interactor->AddObserver(vtkCommand::MouseMoveEvent,         this, 10);
	this->Interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, this, 10);
}

void CModifyGridCommand::Execute(vtkObject *caller, unsigned long eventId, void *callData)
{
	switch ( eventId ) 
	{
		case vtkCommand::LeftButtonPressEvent:
			//this->OnLeftButtonPressEvent(caller, callData);
			TRACE("CModifyGridCommand::LeftButtonPressEvent\n");
			this->SetAbortFlag(1);
			break;

		case vtkCommand::MouseMoveEvent:
			//this->OnMouseMoveEvent(caller, callData);
			TRACE("CModifyGridCommand::MouseMoveEvent\n");
			this->SetAbortFlag(1);
			break;

		case vtkCommand::LeftButtonReleaseEvent:
			//this->OnLeftButtonReleaseEvent(caller, callData);
			TRACE("CModifyGridCommand::LeftButtonReleaseEvent\n");
			this->SetAbortFlag(1);
			break;

		default:
			break;
	}
}
