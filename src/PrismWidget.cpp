#include "StdAfx.h"
#include "PrismWidget.h"

#include "Resource.h"  // IDC_NULL
#include "srcinput/Prism.h"

vtkCxxRevisionMacro(CPrismWidget, "$Revision$");
vtkStandardNewMacro(CPrismWidget);

#include "vtkCellPicker2.h"

#include "ZoneActor.h"
#include "Global.h"
#include "Utilities.h"

static double SelectedColor[3] = {1, 0, 0};

double RadiusFactor = 0.008;

CPrismWidget::CPrismWidget(void)
: LinePicker(0)
, GhostSphereSource(0)
, GhostPolyDataMapper(0)
, GhostActor(0)
, LineIndex(-1)
, TubeVisibility(0)
, HandleHover(0)
{
	TRACE("%s, in\n", __FUNCTION__);
	this->State = CPrismWidget::Start;

	ASSERT(this->EventCallbackCommand);
	this->EventCallbackCommand->SetCallback(CPrismWidget::ProcessEvents);

	this->HandleProperty = vtkProperty::New();

	this->SelectedProperty = vtkProperty::New();
	this->SelectedProperty->SetColor(SelectedColor);
	this->SelectedProperty->SetAmbient(1.0);
	this->SelectedProperty->SetDiffuse(1.0);
	this->SelectedProperty->SetColor(SelectedColor);
	this->SelectedProperty->SetEdgeColor(SelectedColor);
	this->SelectedProperty->SetAmbientColor(SelectedColor);
	this->SelectedProperty->SetRepresentationToWireframe();

	this->SelectedHandleProperty = vtkProperty::New();
	this->SelectedHandleProperty->SetColor(1, 0, 0);

	this->HandlePicker = vtkCellPicker::New();
	this->HandlePicker->SetTolerance(0.001);
	this->HandlePicker->PickFromListOn();

	this->LinePicker = vtkCellPicker2::New();
	this->LinePicker->SetTolerance(0.007);
	this->LinePicker->PickFromListOn();

	this->CurrentHandle[0] = this->CurrentHandle[1] = NULL;
	this->ZoneActor = NULL;

	this->GhostSphereSource = vtkSphereSource::New();
	this->GhostSphereSource->SetPhiResolution(10);
	this->GhostSphereSource->SetThetaResolution(10);

	this->GhostPolyDataMapper = vtkPolyDataMapper::New();
	this->GhostPolyDataMapper->SetInput(this->GhostSphereSource->GetOutput());

	this->GhostActor = vtkActor::New();
	this->GhostActor->SetMapper(this->GhostPolyDataMapper);
	this->GhostActor->GetProperty()->SetColor(1, 1, 1);
	this->GhostActor->GetProperty()->SetOpacity(0.33);
	this->GhostActor->VisibilityOff();		


	TRACE("%s, out\n", __FUNCTION__);
}

CPrismWidget::~CPrismWidget(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->HandleProperty)
	{
		this->HandleProperty->Delete();
		this->HandleProperty = 0;
	}
	if (this->SelectedProperty)
	{
		this->SelectedProperty->Delete();
		this->SelectedProperty = 0;
	}
	if (this->SelectedHandleProperty)
	{
		this->SelectedHandleProperty->Delete();
		this->SelectedHandleProperty = 0;
	}
	if (this->HandlePicker)
	{
		this->HandlePicker->Delete();
		this->HandlePicker = 0;
	}
	if (this->LinePicker)
	{
		this->LinePicker->Delete();
		this->LinePicker = 0;
	}
	if (this->GhostSphereSource)
	{
		this->GhostSphereSource->Delete();
		this->GhostSphereSource = 0;
	}
	if (this->GhostPolyDataMapper)
	{
		this->GhostPolyDataMapper->Delete();
		this->GhostPolyDataMapper = 0;
	}
	if (this->GhostActor)
	{
		this->GhostActor->Delete();
		this->GhostActor = 0;
	}
	this->FreeHandles();
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::SetEnabled(int enabling)
{
	const bool TurnOffTopAndBottomActors = true;

	TRACE("%s, in\n", __FUNCTION__);
	if ( ! this->Interactor )
	{
		vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
		TRACE("%s, out\n", __FUNCTION__);
		return;
	}

	if ( enabling ) //------------------------------------------------------------
	{
		vtkDebugMacro(<<"Enabling widget");

		if ( this->Enabled ) //already enabled, just return
		{
			TRACE("%s, out\n", __FUNCTION__);
			return;
		}

		if ( ! this->CurrentRenderer )
		{
			this->CurrentRenderer = this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]);
			if (this->CurrentRenderer == NULL)
			{
				TRACE("%s, out\n", __FUNCTION__);
				return;
			}
		}

		this->Enabled = 1;

		// listen to the following events
		vtkRenderWindowInteractor *i = this->Interactor;
		i->AddObserver(vtkCommand::MouseMoveEvent,
            this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::LeftButtonPressEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::LeftButtonReleaseEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::RightButtonPressEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::RightButtonReleaseEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::KeyPressEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::KeyReleaseEvent,
			this->EventCallbackCommand, this->Priority);

		// camera events
		//
		if (vtkCamera* camera = this->CurrentRenderer->GetActiveCamera())
		{
			camera->AddObserver(vtkCommand::ModifiedEvent, this->EventCallbackCommand);
		}

		if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(this->Prop3D))
		{
			if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
			{
				Data_source::DATA_SOURCE_TYPE s = p->perimeter.Get_source_type();
				if (s == Data_source::POINTS)
				{
					if (TurnOffTopAndBottomActors)
					{
						ASSERT(pZoneActor->TopActors.size() == 1);
						pZoneActor->TopActors[0]->SetVisibility(0);

						ASSERT(pZoneActor->TopOutlineActors.size() == 1);
						pZoneActor->TopOutlineActors[0]->SetVisibility(0);

						ASSERT(pZoneActor->BottomActors.size() == 1);
						pZoneActor->BottomActors[0]->SetVisibility(0);

						ASSERT(pZoneActor->BottomOutlineActors.size() == 1);
						pZoneActor->BottomOutlineActors[0]->SetVisibility(0);
					}

					ASSERT(pZoneActor->OutlinePerimeterActors.size() == 1);
					pZoneActor->OutlinePerimeterActors[0]->SetProperty(this->SelectedProperty);

					// size handles
					this->SizeHandles();
					
					// add handles to renderer
					this->AddHandleActors();
				}
			}
		}

		this->InvokeEvent(vtkCommand::EnableEvent, NULL);

	}

	else //disabling-------------------------------------------------------------
	{
		vtkDebugMacro(<<"Disabling widget");

		if ( ! this->Enabled ) //already disabled, just return
		{
			TRACE("%s, out\n", __FUNCTION__);
			return;
		}

		this->Enabled = 0;

		if (this->CurrentRenderer)
		{
			if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(this->Prop3D))
			{
				if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
				{
					Data_source::DATA_SOURCE_TYPE s = p->perimeter.Get_source_type();
					if (s == Data_source::POINTS)
					{
						if (TurnOffTopAndBottomActors)
						{
							ASSERT(pZoneActor->TopActors.size() == 1);
							ASSERT(pZoneActor->TopOutlineActors.size() == 1);
							if (pZoneActor->GetVisibility())
							{
								pZoneActor->TopActors[0]->SetVisibility(pZoneActor->TopVisibility);
								pZoneActor->TopOutlineActors[0]->SetVisibility(pZoneActor->TopVisibility);
							}

							ASSERT(pZoneActor->BottomActors.size() == 1);
							ASSERT(pZoneActor->BottomOutlineActors.size() == 1);
							if (pZoneActor->GetVisibility())
							{
								pZoneActor->BottomActors[0]->SetVisibility(pZoneActor->BottomVisibility);
								pZoneActor->BottomOutlineActors[0]->SetVisibility(pZoneActor->BottomVisibility);
							}
						}

						ASSERT(pZoneActor->OutlinePerimeterActors.size() == 1);
						pZoneActor->OutlinePerimeterActors[0]->SetProperty(pZoneActor->OutlineActor->GetProperty());
					}
				}
				// remove handles from renderer
				this->RemoveHandleActors();
			}

			// camera events
			//
			if (vtkCamera* camera = this->CurrentRenderer->GetActiveCamera())
			{
				camera->RemoveObserver(this->EventCallbackCommand);
			}

		}

		// don't listen for events any more
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
		this->InvokeEvent(vtkCommand::DisableEvent, NULL);
	}

	this->Interactor->Render();
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::PlaceWidget(double bounds[6])
{
	TRACE("%s, in\n", __FUNCTION__);
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	TRACE("%s, in\n", __FUNCTION__);
	this->Superclass::PrintSelf(os, indent);
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* callData)
{
// COMMENT: {9/2/2008 3:35:01 PM}	TRACE("%s, in\n", __FUNCTION__);
	CPrismWidget* self = reinterpret_cast<CPrismWidget *>(clientdata);

	if (!self || !self->Prop3D || !self->Interactor) return;

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = self->Interactor->FindPokedRenderer(X, Y);
	if (ren != self->CurrentRenderer)
	{
		TRACE("%s, out\n", __FUNCTION__);
		return;
	}

	switch(event)
	{
		case vtkCommand::ModifiedEvent:
			self->OnModifiedEvent(object, callData);
			ASSERT(callData == NULL);
			break;

		case vtkCommand::LeftButtonPressEvent:
			self->OnLeftButtonDown();
			break;
		case vtkCommand::LeftButtonReleaseEvent:
			self->OnLeftButtonUp();
			break;
		case vtkCommand::MiddleButtonPressEvent:
// COMMENT: {8/25/2008 9:54:52 PM}			self->OnMiddleButtonDown();
			break;
		case vtkCommand::MiddleButtonReleaseEvent:
// COMMENT: {8/25/2008 9:54:56 PM}			self->OnMiddleButtonUp();
			break;
		case vtkCommand::RightButtonPressEvent:
			self->OnRightButtonDown();
			break;
		case vtkCommand::RightButtonReleaseEvent:
			self->OnRightButtonUp();
			break;
		case vtkCommand::MouseMoveEvent:
			self->OnMouseMove();
			break;

	}
// COMMENT: {9/2/2008 3:35:07 PM}	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::OnModifiedEvent(vtkObject* caller, void* callData)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(caller && vtkCamera::SafeDownCast(caller));
	if (this->CurrentRenderer)
	{
		this->SizeHandles();
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::SizeHandles(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	double radius = CGlobal::ComputeHandleRadius(this->CurrentRenderer);
	std::vector<vtkSphereSource*>::iterator iterSphereSource = this->SphereSources.begin();
	for (; iterSphereSource != this->SphereSources.end(); ++iterSphereSource)
	{
		(*iterSphereSource)->SetRadius(radius);
	}
	this->GhostSphereSource->SetRadius(radius);
	std::vector<vtkTubeFilter*>::iterator iterTubeFilter = this->TubeFilters.begin();
	for (; iterTubeFilter != this->TubeFilters.end(); ++iterTubeFilter)
	{
		(*iterTubeFilter)->SetRadius(radius / 2.0);
	}
	this->LinePicker->SetTolerance(0.007/sqrt(this->ZoneActor->GetScale()[0]*this->ZoneActor->GetScale()[1]));
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::SetProp3D(vtkProp3D* prop3D)
{
	TRACE("%s, in\n", __FUNCTION__);
	bool bCreate = true;
	if (this->Prop3D == prop3D)
	{
		if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(this->Prop3D))
		{
			vtkPoints *points = pZoneActor->PrismSidesPolyData[0]->GetPoints();
			if (points && (points->GetNumberOfPoints() == this->SphereSources.size()))
			{
				bCreate = false;
			}
		}
	}

	this->Superclass::SetProp3D(prop3D);

	if (this->ZoneActor = CZoneActor::SafeDownCast(this->Prop3D))
	{
		if (bCreate)
		{
			this->FreeHandles();
			this->CreateHandles();

			if (this->GetEnabled() && this->CurrentRenderer)
			{
				// add handles to pick list and renderer
				this->AddHandleActors();
			}
		}
		else
		{
			this->PositionHandles();
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::PositionHandles(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(this->Prop3D))
	{
		vtkPoints *points = pZoneActor->PrismSidesPolyData[0]->GetPoints();
		if (points)
		{
			ASSERT(points->GetNumberOfPoints() == this->SphereSources.size());
			ASSERT(points->GetNumberOfPoints() == this->PolyDataMappers.size());
			ASSERT(points->GetNumberOfPoints() == this->Actors.size());
			ASSERT(points->GetNumberOfPoints() == this->LineSources.size());
			ASSERT(points->GetNumberOfPoints() == this->LineActors.size());
			ASSERT(points->GetNumberOfPoints() == this->LineMappers.size());
			ASSERT(points->GetNumberOfPoints() == this->TubeFilters.size());

			double ptB[3];
			double ptT[3];
			double x[3];
			std::vector<vtkSphereSource*>::iterator iterSphereSource = this->SphereSources.begin();
			std::vector<vtkLineSource*>::iterator iterLineSource = this->LineSources.begin();
			for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i, ++iterSphereSource)
			{
				points->GetPoint(i, x);
				if (vtkTransform *user = dynamic_cast<vtkTransform*>(pZoneActor->GetUserTransform()))
				{
					user->TransformPoint(x, x);
				}
				else
				{
					ASSERT(FALSE);
				}
				(*iterSphereSource)->SetCenter(x);
				if (i > 1)
				{
					if (i % 2)
					{
						(*iterLineSource)->SetPoint1(ptT[0], ptT[1], ptT[2]);
						(*iterLineSource)->SetPoint2(x[0], x[1], x[2]);
					}
					else
					{
						(*iterLineSource)->SetPoint1(ptB[0], ptB[1], ptB[2]);
						(*iterLineSource)->SetPoint2(x[0], x[1], x[2]);
					}
					++iterLineSource;
				}
				if (i % 2)
				{
					ptT[0] = x[0];
					ptT[1] = x[1];
					ptT[2] = x[2];
				}
				else
				{
					ptB[0] = x[0];
					ptB[1] = x[1];
					ptB[2] = x[2];
				}
			}
			for (vtkIdType i = 0; i < 2; ++i, ++iterLineSource)
			{
				points->GetPoint(i, x);

				if (vtkTransform *user = dynamic_cast<vtkTransform*>(pZoneActor->GetUserTransform()))
				{
					user->TransformPoint(x, x);
				}
				else
				{
					ASSERT(FALSE);
				}

				if (i % 2)
				{
					(*iterLineSource)->SetPoint1(ptT[0], ptT[1], ptT[2]);
					(*iterLineSource)->SetPoint2(x[0], x[1], x[2]);
				}
				else
				{
					(*iterLineSource)->SetPoint1(ptB[0], ptB[1], ptB[2]);
					(*iterLineSource)->SetPoint2(x[0], x[1], x[2]);
				}
			}
			this->SizeHandles();
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::FreeHandles(void)
{
	TRACE("%s, in\n", __FUNCTION__);

	this->RemoveHandleActors();

	std::vector<vtkActor*>::iterator iActor = this->Actors.begin();
	for (; iActor != this->Actors.end(); ++iActor)
	{
		(*iActor)->Delete();
	}
	this->Actors.clear();

	std::vector<vtkPolyDataMapper*>::iterator iMapper = this->PolyDataMappers.begin();
	for (; iMapper != this->PolyDataMappers.end(); ++iMapper)
	{
		(*iMapper)->Delete();
	}
	this->PolyDataMappers.clear();

	std::vector<vtkSphereSource*>::iterator iSource = this->SphereSources.begin();
	for (; iSource != this->SphereSources.end(); ++iSource)
	{
		(*iSource)->Delete();
	}
	this->SphereSources.clear();

	std::vector<vtkLineSource*>::iterator iterLineSource = this->LineSources.begin();
	for(; iterLineSource != this->LineSources.end(); ++iterLineSource)
	{
		(*iterLineSource)->Delete();
	}
	this->LineSources.clear();

	std::vector<vtkTubeFilter*>::iterator iterTubeFilter = this->TubeFilters.begin();
	for(; iterTubeFilter != this->TubeFilters.end(); ++iterTubeFilter)
	{
		(*iterTubeFilter)->Delete();
	}
	this->TubeFilters.clear();

	std::vector<vtkPolyDataMapper*>::iterator iterLinePolyDataMapper = this->LineMappers.begin();
	for(; iterLinePolyDataMapper != this->LineMappers.end(); ++iterLinePolyDataMapper)
	{
		(*iterLinePolyDataMapper)->Delete();
	}
	this->LineMappers.clear();

	std::vector<vtkActor*>::iterator iterLineActor = this->LineActors.begin();
	for(; iterLineActor != this->LineActors.end(); ++iterLineActor)
	{
		(*iterLineActor)->Delete();
	}
	this->LineActors.clear();

	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::CreateHandles(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(this->Prop3D))
	{
		if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
		{
			Data_source::DATA_SOURCE_TYPE s = p->perimeter.Get_source_type();
			if (s == Data_source::POINTS)
			{
				// add to pick list
				if (this->LinePicker)
				{
					this->LinePicker->GetPickList()->RemoveAllItems();
					ASSERT(pZoneActor->OutlinePerimeterActors.size() == 1);
				}

				vtkPoints *points = pZoneActor->PrismSidesPolyData[0]->GetPoints();
				if (points)
				{
					double radius = CGlobal::ComputeHandleRadius(this->CurrentRenderer);
					double x[3];
					double ptB[3];
					double ptT[3];
					this->GhostSphereSource->SetRadius(radius);
					for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
					{
						points->GetPoint(i, x);

						if (vtkTransform *user = dynamic_cast<vtkTransform*>(pZoneActor->GetUserTransform()))
						{
							user->TransformPoint(x, x);
						}
						else
						{
							ASSERT(FALSE);
						}

						vtkSphereSource *sphereSource = vtkSphereSource::New();
						sphereSource->SetPhiResolution(10);
						sphereSource->SetThetaResolution(10);
						sphereSource->SetRadius(radius);
						sphereSource->SetCenter(x[0], x[1], x[2]);
						this->SphereSources.push_back(sphereSource);

						vtkPolyDataMapper *polyDataMapper = vtkPolyDataMapper::New();
						polyDataMapper->SetInput(sphereSource->GetOutput());
						this->PolyDataMappers.push_back(polyDataMapper);

						vtkActor *actor = vtkActor::New();
						actor->SetMapper(polyDataMapper);
						this->Actors.push_back(actor);

						if (this->CurrentRenderer && this->GetEnabled())
						{
							this->CurrentRenderer->AddActor(actor);
						}
						if (i > 1)
						{
							if (i % 2)
							{
								vtkLineSource *lineSource = vtkLineSource::New();
								this->LineSources.push_back(lineSource);
								lineSource->SetPoint1(ptT[0], ptT[1], ptT[2]);
								lineSource->SetPoint2(x[0], x[1], x[2]);

								vtkTubeFilter *tubeFilter = vtkTubeFilter::New();
								tubeFilter->SetNumberOfSides(8);
								tubeFilter->SetInput(lineSource->GetOutput());
								tubeFilter->SetRadius(radius / 2.0);
								this->TubeFilters.push_back(tubeFilter);

								vtkPolyDataMapper *polyDataMapper = vtkPolyDataMapper::New();
								polyDataMapper->SetInput(tubeFilter->GetOutput());
								this->LineMappers.push_back(polyDataMapper);

								vtkActor *actor = vtkActor::New();		
								actor->SetMapper(polyDataMapper);
								actor->SetVisibility(this->TubeVisibility);
								this->LineActors.push_back(actor);
								this->LinePicker->AddPickList(actor);
							}
							else
							{
								vtkLineSource *lineSource = vtkLineSource::New();
								this->LineSources.push_back(lineSource);
								lineSource->SetPoint1(ptB[0], ptB[1], ptB[2]);
								lineSource->SetPoint2(x[0], x[1], x[2]);

								vtkTubeFilter *tubeFilter = vtkTubeFilter::New();
								tubeFilter->SetNumberOfSides(8);
								tubeFilter->SetInput(lineSource->GetOutput());
								tubeFilter->SetRadius(radius / 2.0);
								this->TubeFilters.push_back(tubeFilter);

								vtkPolyDataMapper *polyDataMapper = vtkPolyDataMapper::New();
								polyDataMapper->SetInput(tubeFilter->GetOutput());
								this->LineMappers.push_back(polyDataMapper);

								vtkActor *actor = vtkActor::New();		
								actor->SetMapper(polyDataMapper);
								actor->SetVisibility(this->TubeVisibility);
								this->LineActors.push_back(actor);
								this->LinePicker->AddPickList(actor);
							}
						}
						if (i % 2)
						{
							ptT[0] = x[0];
							ptT[1] = x[1];
							ptT[2] = x[2];
						}
						else
						{
							ptB[0] = x[0];
							ptB[1] = x[1];
							ptB[2] = x[2];
						}
					}
					for (vtkIdType i = 0; i < 2; ++i)
					{
						points->GetPoint(i, x);

						if (vtkTransform *user = dynamic_cast<vtkTransform*>(pZoneActor->GetUserTransform()))
						{
							user->TransformPoint(x, x);
						}
						else
						{
							ASSERT(FALSE);
						}

						if (i % 2)
						{
							vtkLineSource *lineSource = vtkLineSource::New();
							this->LineSources.push_back(lineSource);
							lineSource->SetPoint1(ptT[0], ptT[1], ptT[2]);
							lineSource->SetPoint2(x[0], x[1], x[2]);

							vtkTubeFilter *tubeFilter = vtkTubeFilter::New();
							tubeFilter->SetNumberOfSides(8);
							tubeFilter->SetInput(lineSource->GetOutput());
							tubeFilter->SetRadius(radius / 2.0);
							this->TubeFilters.push_back(tubeFilter);

							vtkPolyDataMapper *polyDataMapper = vtkPolyDataMapper::New();
							polyDataMapper->SetInput(tubeFilter->GetOutput());
							this->LineMappers.push_back(polyDataMapper);

							vtkActor *actor = vtkActor::New();		
							actor->SetMapper(polyDataMapper);
							actor->SetVisibility(this->TubeVisibility);
							this->LineActors.push_back(actor);
							this->LinePicker->AddPickList(actor);
						}
						else
						{
							vtkLineSource *lineSource = vtkLineSource::New();
							this->LineSources.push_back(lineSource);
							lineSource->SetPoint1(ptB[0], ptB[1], ptB[2]);
							lineSource->SetPoint2(x[0], x[1], x[2]);

							vtkTubeFilter *tubeFilter = vtkTubeFilter::New();
							tubeFilter->SetNumberOfSides(8);
							tubeFilter->SetInput(lineSource->GetOutput());
							tubeFilter->SetRadius(radius / 2.0);
							this->TubeFilters.push_back(tubeFilter);

							vtkPolyDataMapper *polyDataMapper = vtkPolyDataMapper::New();
							polyDataMapper->SetInput(tubeFilter->GetOutput());
							this->LineMappers.push_back(polyDataMapper);

							vtkActor *actor = vtkActor::New();		
							actor->SetMapper(polyDataMapper);
							actor->SetVisibility(this->TubeVisibility);
							this->LineActors.push_back(actor);
							this->LinePicker->AddPickList(actor);
						}
					}
				}
			}
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::AddHandleActors(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(this->CurrentRenderer);
	ASSERT(this->HandlePicker);
	if (this->CurrentRenderer)
	{
		// add handles to pick list and renderer
		std::vector<vtkActor*>::iterator iActor = this->Actors.begin();
		for (; iActor != this->Actors.end(); ++iActor)
		{
			this->CurrentRenderer->AddActor(*iActor);
			this->HandlePicker->AddPickList(*iActor);
		}
		this->CurrentRenderer->AddActor(this->GhostActor);
		std::vector<vtkActor*>::iterator itActor = this->LineActors.begin();
		for (; itActor != this->LineActors.end(); ++itActor)
		{
			this->CurrentRenderer->AddActor(*itActor);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::RemoveHandleActors(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->CurrentRenderer)
	{
		std::vector<vtkActor*>::iterator iActor = this->Actors.begin();
		for (; iActor != this->Actors.end(); ++iActor)
		{
			ASSERT(this->CurrentRenderer);
			if (this->CurrentRenderer)
			{
				this->CurrentRenderer->RemoveActor(*iActor);
			}
			if (this->HandlePicker)
			{
				this->HandlePicker->DeletePickList(*iActor);
			}
		}
		this->CurrentRenderer->RemoveActor(this->GhostActor);
		std::vector<vtkActor*>::iterator itActor = this->LineActors.begin();
		for (; itActor != this->LineActors.end(); ++itActor)
		{
			if (this->CurrentRenderer)
			{
				this->CurrentRenderer->RemoveActor(*itActor);
			}
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::OnLeftButtonDown(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X, Y);
	if (ren != this->CurrentRenderer)
	{
		this->State = CPrismWidget::Outside;
		TRACE("%s, out\n", __FUNCTION__);
		return;
	}

	vtkAssemblyPath *path;
	ASSERT(this->HandlePicker);
	this->HandlePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
	path = this->HandlePicker->GetPath();
	if (path != NULL /***{9/10/2008 2:52:16 PM} && !(::GetAsyncKeyState(VK_CONTROL) < 0) {9/10/2008 2:52:16 PM}***/)
	{
		this->State = CPrismWidget::Moving;
		this->HandlePicker->GetPickPosition(this->LastPickPosition);
		this->ValidPick = 1;

		ASSERT(this->Actors.size() == this->SphereSources.size());

		// determine picked handle
		vtkIdType id = 0;
		std::vector<vtkActor*>::iterator iactor = this->Actors.begin();
		std::vector<vtkSphereSource*>::iterator sactor = this->SphereSources.begin();
		for (; iactor != this->Actors.end(); ++iactor, ++sactor, ++id)
		{
			if ((*iactor) == path->GetFirstNode()->GetViewProp())
			{
				if (id % 2)
				{
					this->CurrentHandle[1] = (*iactor);
					--iactor;
					this->CurrentHandle[0] = (*iactor);
					this->CurrentId[1] = id;
					this->CurrentId[0] = id-1;

				}
				else
				{
					this->CurrentHandle[0] = (*iactor);
					++iactor;
					this->CurrentHandle[1] = (*iactor);
					this->CurrentId[0] = id;
					this->CurrentId[1] = id+1;
				}
				this->ScaledZPlane = (*sactor)->GetCenter()[2];
				this->CurrentHandle[0]->SetProperty(this->SelectedHandleProperty);
				this->CurrentHandle[1]->SetProperty(this->SelectedHandleProperty);
				break;
			}
		}
	}
	else
	{
		if (this->LinePicker)
		{
			this->LinePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
			vtkAssemblyPath *path = this->LinePicker->GetPath();
			if (path != NULL)
			{
				this->LinePicker->GetPickPosition(this->LastPickPosition);

				double pickPoint[4];
				pickPoint[0] = this->LastPickPosition[0];
				pickPoint[1] = this->LastPickPosition[1];
				pickPoint[2] = this->LastPickPosition[2];

				this->LineIndex = -1;
				std::vector<vtkActor*>::iterator it = this->LineActors.begin();
				for (size_t i = 0; it != this->LineActors.end(); ++it, ++i)
				{
					if (this->LinePicker->GetActor() == (*it))
					{
						this->LineIndex = i;
					}
				}
				ASSERT(this->LineIndex != -1);

				// calculate closest intersection
				//
				double pt1[3];
				double pt2[3];
				this->LineSources[this->LineIndex]->GetPoint1(pt1);
				this->LineSources[this->LineIndex]->GetPoint2(pt2);
				double magsq = (pt2[0] - pt1[0]) * (pt2[0] - pt1[0]) + (pt2[1] - pt1[1]) * (pt2[1] - pt1[1]);
				double u = (((pickPoint[0] - pt1[0]) * (pt2[0] - pt1[0])) + ((pickPoint[1] - pt1[1]) * (pt2[1] - pt1[1]))) / magsq;
				this->PickPoint[0] = pt1[0] + u * (pt2[0] - pt1[0]);
				this->PickPoint[1] = pt1[1] + u * (pt2[1] - pt1[1]);

				// unscale pickpoint
				//
				this->ZoneActor->GetUserTransform()->GetInverse()->TransformPoint(this->PickPoint, this->PickPoint);

				this->EventCallbackCommand->SetAbortFlag(1);
				this->InvokeEvent(CPrismWidget::InsertPointEvent, NULL);
			}
			this->GhostActor->VisibilityOff();
			this->Interactor->Render();
		}

		this->State = CPrismWidget::Outside;
		TRACE("%s, in\n", __FUNCTION__);
		return;
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);

	ASSERT(this->Interactor);
	this->Interactor->Render();
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::OnLeftButtonUp(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->State == CPrismWidget::Outside || this->State == CPrismWidget::Start)
	{
		TRACE("%s, out\n", __FUNCTION__);
		return;
	}

	ASSERT(this->HandleProperty);

	this->State = CPrismWidget::Start;
	ASSERT(this->CurrentHandle[0]);
	if (this->CurrentHandle[0])
	{
		this->CurrentHandle[0]->SetProperty(this->HandleProperty);
		this->CurrentHandle[0] = NULL;
	}
	ASSERT(this->CurrentHandle[1]);
	if (this->CurrentHandle[1])
	{
		this->CurrentHandle[1]->SetProperty(this->HandleProperty);
		this->CurrentHandle[1] = NULL;
	}

	// TODO check for crossing
	this->SizeHandles();

	this->EventCallbackCommand->SetAbortFlag(1);
	this->EndInteraction();
	this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);

	ASSERT(this->Interactor);
	this->Interactor->Render();
	TRACE("%s, out\n", __FUNCTION__);
}

void CPrismWidget::OnMouseMove(void)
{
// COMMENT: {9/2/2008 3:33:22 PM}	TRACE("%s, in\n", __FUNCTION__);

	// See whether we're active
	if (this->State == CPrismWidget::Outside || this->State == CPrismWidget::Start)
	{
		// check for handle hit first
		//
		int X = this->Interactor->GetEventPosition()[0];
		int Y = this->Interactor->GetEventPosition()[1];
		ASSERT(this->HandlePicker);
		this->HandlePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
		vtkAssemblyPath *path = this->HandlePicker->GetPath();
		if (path != NULL)
		{
			this->GhostActor->VisibilityOff();
			this->Interactor->Render();
// COMMENT: {9/10/2008 2:52:16 PM}			if (!(::GetAsyncKeyState(VK_CONTROL) < 0))
			{
				this->HandleHover = 1;
				TRACE("IDC_SIZEALL 1\n");
				::SetCursor(::AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
			}
// COMMENT: {9/10/2008 2:52:21 PM}			else
// COMMENT: {9/10/2008 2:52:21 PM}			{
// COMMENT: {9/10/2008 2:52:21 PM}				this->HandleHover = 0;
// COMMENT: {9/10/2008 2:52:21 PM}				::SetCursor(::AfxGetApp()->LoadStandardCursor(IDC_ARROW));
// COMMENT: {9/10/2008 2:52:21 PM}			}
			return;
		}
		this->HandleHover = 0;

		if (this->LinePicker)
		{
			this->LinePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
			vtkAssemblyPath *path = this->LinePicker->GetPath();
			if (path != NULL && !(::GetAsyncKeyState(VK_LBUTTON) < 0) && !(::GetAsyncKeyState(VK_RBUTTON) < 0) && !(::GetAsyncKeyState(VK_MBUTTON) < 0))
			{
				double *pickPoint = this->LinePicker->GetPickPosition();

				size_t i = 0;
				for (; i < this->LineActors.size(); ++i)
				{
					if (this->LinePicker->GetActor() == this->LineActors[i])
					{
						// calculate closest intersection
						//
						double pt1[3];
						double pt2[3];
						this->LineSources[i]->GetPoint1(pt1);
						this->LineSources[i]->GetPoint2(pt2);
						double magsq = (pt2[0] - pt1[0]) * (pt2[0] - pt1[0]) + (pt2[1] - pt1[1]) * (pt2[1] - pt1[1]);
						double u = (((pickPoint[0] - pt1[0]) * (pt2[0] - pt1[0])) + ((pickPoint[1] - pt1[1]) * (pt2[1] - pt1[1]))) / magsq;
						pickPoint[0] = pt1[0] + u * (pt2[0] - pt1[0]);
						pickPoint[1] = pt1[1] + u * (pt2[1] - pt1[1]);
						break;
					}
				}
				ASSERT(i < this->LineActors.size());

				this->GhostSphereSource->SetCenter(pickPoint[0], pickPoint[1], pickPoint[2]);
				this->GhostActor->VisibilityOn();
				this->Interactor->Render();
			}
			else
			{
				this->GhostActor->VisibilityOff();
				this->Interactor->Render();
			}
		}
// COMMENT: {9/2/2008 3:33:29 PM}		TRACE("%s, out\n", __FUNCTION__);
		return;
	}

	if (this->State == CPrismWidget::Moving)
	{
		ASSERT(this->HandleHover);
		ASSERT(this->CurrentHandle[0]);
		if (this->CurrentHandle[0])
		{
			int FixedCoord = 2; // always z-axis
			double pickPoint[4];
			CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, FixedCoord, this->ScaledZPlane, pickPoint);

			// unscale pickpoint
			//
			this->ZoneActor->GetUserTransform()->GetInverse()->TransformPoint(pickPoint, this->WorldSIPoint);

			ASSERT(this->ZoneActor);
			ASSERT(this->ZoneActor->PrismSidesPolyData[0]);
			vtkPoints *points = this->ZoneActor->PrismSidesPolyData[0]->GetPoints();
			if (points)
			{
				double x[3];
				points->GetPoint(this->CurrentId[0], x);
				points->SetPoint(this->CurrentId[0], this->WorldSIPoint[0], this->WorldSIPoint[1], x[2]);
				points->GetPoint(this->CurrentId[1], x);
				points->SetPoint(this->CurrentId[1], this->WorldSIPoint[0], this->WorldSIPoint[1], x[2]);
				points->Modified();
				this->PositionHandles();
			}
			TRACE("IDC_SIZEALL 2\n");
			::SetCursor(::AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		}
	}

	// Interact, if desired
	this->InvokeEvent(vtkCommand::InteractionEvent, NULL);

	ASSERT(this->Interactor);
	this->GhostActor->VisibilityOff();
	this->Interactor->Render();
	TRACE("%s, out\n", __FUNCTION__);
}

vtkPoints* CPrismWidget::GetPoints(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->ZoneActor && this->ZoneActor->PrismSidesPolyData.size() == 1)
	{
		TRACE("%s, out\n", __FUNCTION__);
		return this->ZoneActor->PrismSidesPolyData[0]->GetPoints();
	}
	TRACE("%s, out\n", __FUNCTION__);
	return 0;
}

#ifdef WIN32
// COMMENT: {9/10/2008 1:50:36 PM}#include <winuser.h> // IDC_HAND
BOOL CPrismWidget::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (this->Enabled)
	{
		if (this->GhostActor->GetVisibility())
		{
			if ((::GetAsyncKeyState(VK_SHIFT) < 0))
			{
				// show cross-hairs if shift is down
				::SetCursor(::AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			}
			else if ((::GetAsyncKeyState(VK_CONTROL) < 0))
			{
				// show hands if control is down
				::SetCursor(::AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649)));
			}
			else
			{
				::SetCursor(::AfxGetApp()->LoadCursor(IDC_NULL));		
			}
			return TRUE;
		}
		if (this->HandleHover)
		{
			TRACE("IDC_SIZEALL 3\n");
			::SetCursor(::AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
			return TRUE;
		}
	}
	return FALSE;
}
#endif // WIN32

vtkPoints* CPrismWidget::GetInsertPoints(void)
{
	vtkPoints *points = vtkPoints::New();
	if (this->ZoneActor && this->ZoneActor->PrismSidesPolyData.size() == 1)
	{
		vtkPoints *pts = this->ZoneActor->PrismSidesPolyData[0]->GetPoints();
		if (pts)
		{
			double x[3];
			for (vtkIdType i = 0; i < pts->GetNumberOfPoints(); ++i)
			{
				pts->GetPoint(i, x);
				points->InsertNextPoint(x);
				if (((this->LineIndex / 2) * 2) + 1 == i)
				{
					points->InsertNextPoint(this->PickPoint[0], this->PickPoint[1], 0.0);
					points->InsertNextPoint(this->PickPoint[0], this->PickPoint[1], 0.0);
				}
			}			
		}
	}
	return points;
}

void CPrismWidget::OnRightButtonDown(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X, Y);
	if (ren != this->CurrentRenderer)
	{
		this->State = CPrismWidget::Outside;
		TRACE("%s, out\n", __FUNCTION__);
		return;
	}

	vtkAssemblyPath *path;
	ASSERT(this->HandlePicker);
	this->HandlePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
	path = this->HandlePicker->GetPath();
	if (path != NULL)
	{
		this->State = CPrismWidget::Deleting;
		this->HandlePicker->GetPickPosition(this->LastPickPosition);
		this->ValidPick = 1;

		ASSERT(this->Actors.size() == this->SphereSources.size());

		// determine picked handle
		vtkIdType id = 0;
		std::vector<vtkActor*>::iterator iactor = this->Actors.begin();
		std::vector<vtkSphereSource*>::iterator sactor = this->SphereSources.begin();
		for (; iactor != this->Actors.end(); ++iactor, ++sactor, ++id)
		{
			if ((*iactor) == path->GetFirstNode()->GetViewProp())
			{
				if (id % 2)
				{
					this->CurrentHandle[1] = (*iactor);
					--iactor;
					this->CurrentHandle[0] = (*iactor);
					this->CurrentId[1] = id;
					this->CurrentId[0] = id-1;

				}
				else
				{
					this->CurrentHandle[0] = (*iactor);
					++iactor;
					this->CurrentHandle[1] = (*iactor);
					this->CurrentId[0] = id;
					this->CurrentId[1] = id+1;
				}
				this->ScaledZPlane = (*sactor)->GetCenter()[2];
				this->CurrentHandle[0]->SetProperty(this->SelectedHandleProperty);
				this->CurrentHandle[1]->SetProperty(this->SelectedHandleProperty);
				this->Interactor->Render();
				break;
			}
		}
		this->EventCallbackCommand->SetAbortFlag(1);
	}

}

void CPrismWidget::OnRightButtonUp(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->State == CPrismWidget::Outside || this->State == CPrismWidget::Start)
	{
		TRACE("%s, out\n", __FUNCTION__);
		return;
	}
	if (this->State == CPrismWidget::Deleting)
	{
		::SetCursor(::AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		int X = this->Interactor->GetEventPosition()[0];
		int Y = this->Interactor->GetEventPosition()[1];

		this->CurrentHandle[0]->SetProperty(this->HandleProperty);
		this->CurrentHandle[1]->SetProperty(this->HandleProperty);

		vtkRenderer *ren = this->Interactor->FindPokedRenderer(X, Y);
		if (ren != this->CurrentRenderer)
		{
			this->HandleHover = 0;
			this->State = CPrismWidget::Outside;
			TRACE("%s, out\n", __FUNCTION__);
			return;
		}

		vtkAssemblyPath *path;
		ASSERT(this->HandlePicker);
		this->HandlePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
		path = this->HandlePicker->GetPath();
		if (path != NULL && (this->HandlePicker->GetActor() == this->CurrentHandle[0] || this->HandlePicker->GetActor() == this->CurrentHandle[1]))
		{
			this->HandleHover = 0;
			this->InvokeEvent(CPrismWidget::DeletePointEvent, NULL);
		}
		this->EventCallbackCommand->SetAbortFlag(1);
		this->State = CPrismWidget::Start;
	}
}

vtkPoints* CPrismWidget::GetDeletePoints(void)
{
	vtkPoints *points = vtkPoints::New();
	if (this->ZoneActor && this->ZoneActor->PrismSidesPolyData.size() == 1)
	{
		vtkPoints *pts = this->ZoneActor->PrismSidesPolyData[0]->GetPoints();
		if (pts)
		{
			double x[3];
			for (vtkIdType i = 0; i < pts->GetNumberOfPoints(); ++i)
			{
				if (i == this->CurrentId[0]) continue;
				if (i == this->CurrentId[1]) continue;
				pts->GetPoint(i, x);
				points->InsertNextPoint(x);
			}			
		}
	}
	return points;
}
