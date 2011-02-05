#include "StdAfx.h"
#include "GridRotateAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "GridActor.h"

#include <vtkCamera.h>

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGridRotateAction::CGridRotateAction(CWPhastDoc* pDoc, CGridActor* pActor, const CGridKeyword& gridKeyword)
: WPhastDoc(pDoc)
, NewGridKeyword(gridKeyword)
{
	pActor->GetGridKeyword(this->OriginalGridKeyword);
}

CGridRotateAction::~CGridRotateAction(void)
{
}

void CGridRotateAction::Execute(void)
{
	this->WPhastDoc->SetGridKeyword(this->NewGridKeyword);
	if (POSITION pos = this->WPhastDoc->GetFirstViewPosition())
	{
		CWPhastView *pView = (CWPhastView*) this->WPhastDoc->GetNextView(pos);
		ASSERT_VALID(pView);
		vtkCamera *camera = pView->GetRenderer()->GetActiveCamera();

		double midgrid[3];
		for (int i = 0; i < 3; i++)
		{
			if (this->NewGridKeyword.m_grid[0].uniform && (!this->NewGridKeyword.m_grid[0].uniform_expanded))
			{
				midgrid[i] = (this->NewGridKeyword.m_grid[i].coord[0] + this->NewGridKeyword.m_grid[i].coord[1]) / 2.0;
			}
			else
			{
				midgrid[i] = (this->NewGridKeyword.m_grid[0].coord[0] + this->NewGridKeyword.m_grid[0].coord[this->NewGridKeyword.m_grid[0].count_coord - 1]) / 2.0;
			}
		}

		double *scale = this->WPhastDoc->GetScale();
		const CUnits& units = this->WPhastDoc->GetUnits();

		vtkTransform *trans = vtkTransform::New();
		
		trans->Identity();
		trans->Scale(
			1./(scale[0] * units.horizontal.input_to_si),
			1./(scale[1] * units.horizontal.input_to_si),
			1./(scale[2] * units.vertical.input_to_si)
			);
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->Translate(
			-(midgrid[0]),
			-(midgrid[1]),
			-(midgrid[2])
			);
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->RotateZ(-(this->NewGridKeyword.m_grid_angle - this->OriginalGridKeyword.m_grid_angle));
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->Translate(
			(midgrid[0]),
			(midgrid[1]),
			(midgrid[2])
			);
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->Scale(
			scale[0] * units.horizontal.input_to_si,
			scale[1] * units.horizontal.input_to_si,
			scale[2] * units.vertical.input_to_si
			);
		camera->ApplyTransform(trans);

		trans->Delete();

		pView->GetInteractor()->Render();
	}
	this->WPhastDoc->UpdateAllViews(0);
}

void CGridRotateAction::UnExecute(void)
{
	this->WPhastDoc->SetGridKeyword(this->OriginalGridKeyword);
	if (POSITION pos = this->WPhastDoc->GetFirstViewPosition())
	{
		CWPhastView *pView = (CWPhastView*) this->WPhastDoc->GetNextView(pos);
		ASSERT_VALID(pView);
		vtkCamera *camera = pView->GetRenderer()->GetActiveCamera();

		// determine grid midpoint
		//
		double midgrid[3];
		for (int i = 0; i < 3; i++)
		{
			if (this->NewGridKeyword.m_grid[0].uniform && (!this->NewGridKeyword.m_grid[0].uniform_expanded))
			{
				midgrid[i] = (this->NewGridKeyword.m_grid[i].coord[0] + this->NewGridKeyword.m_grid[i].coord[1]) / 2.0;
			}
			else
			{
				midgrid[i] = (this->NewGridKeyword.m_grid[0].coord[0] + this->NewGridKeyword.m_grid[0].coord[this->NewGridKeyword.m_grid[0].count_coord - 1]) / 2.0;
			}
		}

		double *scale = this->WPhastDoc->GetScale();
		const CUnits& units = this->WPhastDoc->GetUnits();

		vtkTransform *trans = vtkTransform::New();

		trans->Identity();
		trans->Scale(
			scale[0] * units.horizontal.input_to_si,
			scale[1] * units.horizontal.input_to_si,
			scale[2] * units.vertical.input_to_si
			);
		trans->Inverse();
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->Translate(
			(midgrid[0]),
			(midgrid[1]),
			(midgrid[2])
			);
		trans->Inverse();
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->RotateZ(-(this->NewGridKeyword.m_grid_angle - this->OriginalGridKeyword.m_grid_angle));
		trans->Inverse();
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->Translate(
			-(midgrid[0]),
			-(midgrid[1]),
			-(midgrid[2])
			);
		trans->Inverse();
		camera->ApplyTransform(trans);

		trans->Identity();
		trans->Scale(
			1./(scale[0] * units.horizontal.input_to_si),
			1./(scale[1] * units.horizontal.input_to_si),
			1./(scale[2] * units.vertical.input_to_si)
			);
		trans->Inverse();
		camera->ApplyTransform(trans);

		trans->Delete();

		pView->GetInteractor()->Render();
	}
	this->WPhastDoc->UpdateAllViews(0);
}

void CGridRotateAction::Apply(const CGridKeyword& gridKeyword)
{
	this->NewGridKeyword = gridKeyword;
	this->Execute();
}
