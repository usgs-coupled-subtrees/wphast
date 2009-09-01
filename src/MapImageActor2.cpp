#include "StdAfx.h"
#include "MapImageActor2.h"

#include <fstream>

#include <vtkObjectFactory.h>

#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageShiftScale.h>

#include "Global.h"
#include "WorldTransform.h"

vtkCxxRevisionMacro(CMapImageActor2, "$Revision: 346 $");
vtkStandardNewMacro(CMapImageActor2);

// Note: No header files should follow the next three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMapImageActor2::CMapImageActor2(void)
{
	this->m_MapImageReader2 = NULL;
	this->m_MapShiftScale = vtkImageShiftScale::New();
	this->m_Scale[0] = this->m_Scale[1] = this->m_Scale[2] = 1.0;
	this->m_DataSpacing[0] = this->m_DataSpacing[1] = this->m_DataSpacing[2] = 1.0;
	this->m_UpperLeft[0] = this->m_UpperLeft[1] = this->m_UpperLeft[2] = 0.0;
	this->InterpolateOff();
}

CMapImageActor2::~CMapImageActor2(void)
{
	if (this->m_MapImageReader2) this->m_MapImageReader2->Delete();
	if (this->m_MapShiftScale) this->m_MapShiftScale->Delete();
}

int CMapImageActor2::SetFileName(const char *filename)
{
	if (this->m_MapImageReader2) this->m_MapImageReader2->Delete();

	vtkImageReader2Factory *factory = vtkImageReader2Factory::New();
	this->m_MapImageReader2 = factory->CreateImageReader2(filename);
	factory->Delete();

	// ASSERT(this->m_MapImageReader2);
	if (this->m_MapImageReader2) {

		this->m_MapImageReader2->SetFileName( filename );
		this->m_MapImageReader2->Update( );
		double *origin = this->m_MapImageReader2->GetOutput()->GetOrigin();
		TRACE("origin before setinput = %g %g %g\n", origin[0], origin[1], origin[2]);
// COMMENT: {8/8/2009 1:01:41 AM}		this->m_MapShiftScale->SetInput( this->m_MapImageReader2->GetOutput() );
// COMMENT: {8/8/2009 1:01:41 AM}		this->m_MapShiftScale->SetOutputScalarTypeToUnsignedChar( );
// COMMENT: {8/8/2009 1:01:41 AM}// COMMENT: {8/4/2009 6:29:25 PM}		this->SetInput( this->m_MapShiftScale->GetOutput() );
		this->SetInput( this->m_MapImageReader2->GetOutput() );
		origin = this->m_MapImageReader2->GetOutput()->GetOrigin();
		TRACE("origin after setinput = %g %g %g\n", origin[0], origin[1], origin[2]);

		return 1; // success
	}
	return 0; // failure
}

int CMapImageActor2::SetWorldTransform(const CWorldTransform &wtrans)
{
	if (this->m_MapImageReader2)
	{
		this->UpdateImageReader2();

		const double* upperleft   = wtrans.GetUpperLeft();
		const double* dataspacing = wtrans.GetDataSpacing();
		for (int i = 0; i < 3; i++)
		{
			this->m_UpperLeft[i] = upperleft[i];
			this->m_DataSpacing[i] = dataspacing[i];
		}
		int *dims = this->m_MapImageReader2->GetDataExtent();
		ASSERT(dims[0] == 0 && dims[2] == 0);
		int cy = dims[3];
		ASSERT(cy > 0);

		double yLowerLeft = this->m_UpperLeft[1] - (double(cy) * this->m_DataSpacing[1]);
		this->m_MapImageReader2->SetDataOrigin(
			this->m_UpperLeft[0],
			yLowerLeft,
			this->m_UpperLeft[2]
			);

		ASSERT(this->m_Scale[0] = 1.0);
		ASSERT(this->m_Scale[1] = 1.0);
		ASSERT(this->m_Scale[2] = 1.0);
		this->UpdateImageReader2();
	}
	else
	{
		::AfxMessageBox("SetFileName must be called before SetWorldTransform");
		return 0; // failure
	}

	return 1; // success
}

int CMapImageActor2::SetWorldFileName(const char *filename)
{
	ASSERT(FALSE); // use SetWorldTransform instead

	/////////////////////////////////////////////////
	//from http://support.esri.com
	//Article ID: 17489
	//Software:  ArcGIS - ArcInfo 8.0.1, 8.0.2 ArcInfo Workstation 7.0.4, 7.1.1, 7.1.2, 7.2.1 ArcView GIS 3.0, 3.0a, 3.0b, 3.1, 3.2, 3.2a
	//Platforms: N/A
	//
	//Question
	//What is the format of the world file used for georeferencing images?
	//
	//
	//Answer
	//The world file is an ASCII text file associated with an image and contains the following lines:
	//
	//Line 1: x-dimension of a pixel in map units
	//Line 2: rotation parameter
	//Line 3: rotation parameter
	//Line 4: NEGATIVE of y-dimension of a pixel in map units
	//Line 5: x-coordinate of center of upper left pixel
	//Line 6: y-coordinate of center of upper left pixel
	/////////////////////////////////////////////////

	double dummy;
	std::ifstream ifs;
	ifs.open(filename);
	ifs >> this->m_DataSpacing[0];
	ifs >> dummy;
	ASSERT(dummy == 0.0);
	ifs >> dummy;
	ASSERT(dummy == 0.0);

	ifs >> this->m_DataSpacing[1];
	ASSERT(this->m_DataSpacing[1] < 0.0);
	this->m_DataSpacing[1] = fabs(this->m_DataSpacing[1]);

	ifs >> this->m_UpperLeft[0];
	ifs >> this->m_UpperLeft[1];
	this->m_UpperLeft[2] = 0.0;

	if (this->m_MapImageReader2)
	{
		this->UpdateImageReader2();

		//{{
		ASSERT(this->m_Scale[0] = 1.0);
		ASSERT(this->m_Scale[1] = 1.0);
		ASSERT(this->m_Scale[2] = 1.0);
		int *dims = this->m_MapImageReader2->GetDataExtent();
		ASSERT(dims[0] == 0 && dims[2] == 0);
		int cy = dims[3];
		ASSERT(cy > 0);

		double yLowerLeft = this->m_UpperLeft[1] - (cy - 1) * this->m_DataSpacing[1];
		this->m_MapImageReader2->SetDataOrigin(
			this->m_UpperLeft[0],
			yLowerLeft,
			this->m_UpperLeft[2]
			);
		//}}
	}
	else
	{
		::AfxMessageBox("SetFileName must be called before SetWorldFileName");
		return 0; // failure
	}

	return 1; // success
}

int CMapImageActor2::PlaceMap(double xPos, double yPos, double zPos, double angle)
{
	//{{
	int *dims = this->GetDataExtent();
	ASSERT(dims[0] == 0 && dims[2] == 0);
	int cy = dims[3];
	ASSERT(cy > 0);

	double yLowerLeft = this->m_UpperLeft[1] - (cy - 1) * this->m_DataSpacing[1];
	//}}

	//{{
	this->m_UpperLeft[2] = zPos;
	//}}

	this->m_MapImageReader2->SetDataOrigin(this->m_UpperLeft[0] - xPos, yLowerLeft - yPos, zPos);
	return 1;
}

int* CMapImageActor2::GetDataExtent(void)const
{
	static const int extent[6];
	if (this->m_MapImageReader2) {
		this->m_MapImageReader2->Update();
		return this->m_MapImageReader2->GetDataExtent();
	}
	return (int*)extent;
}

void CMapImageActor2::UpdateImageReader2(void)
{
	if (this->m_MapImageReader2)
	{
// COMMENT: {3/22/2004 6:59:41 PM}		ASSERT(this->m_Scale[0] == 1.0); // Scale is only partially implemented
// COMMENT: {3/22/2004 6:59:41 PM}		ASSERT(this->m_Scale[1] == 1.0);
// COMMENT: {3/22/2004 6:59:41 PM}		ASSERT(this->m_Scale[2] == 1.0);

		this->m_MapImageReader2->Update();

		this->m_MapImageReader2->SetDataSpacing(
			this->m_DataSpacing[0] * this->m_Scale[0],
            this->m_DataSpacing[1] * this->m_Scale[1],
			this->m_DataSpacing[2] * this->m_Scale[2]);


		//{{
		double* origin = this->m_MapImageReader2->GetDataOrigin();
		double newOrigin[3];
		for (int i = 0; i < 3; ++i) {
			newOrigin[i] = origin[i] * this->m_Scale[i];
		}
		this->m_MapImageReader2->SetDataOrigin(newOrigin[0], newOrigin[1], newOrigin[2]);
		//}}



// COMMENT: {3/22/2004 7:05:48 PM}		int *dims = this->m_MapImageReader2->GetDataExtent();
// COMMENT: {3/22/2004 7:05:48 PM}		ASSERT(dims[0] == 0 && dims[2] == 0);
// COMMENT: {3/22/2004 7:05:48 PM}		int cy = dims[3];
// COMMENT: {3/22/2004 7:05:48 PM}		ASSERT(cy > 0);
// COMMENT: {3/22/2004 7:05:48 PM}
// COMMENT: {3/22/2004 7:05:48 PM}		double yLowerLeft = this->m_UpperLeft[1] - (cy - 1) * this->m_DataSpacing[1];
// COMMENT: {3/22/2004 7:05:48 PM}// COMMENT: {3/22/2004 7:00:55 PM}		this->m_MapImageReader2->SetDataOrigin(
// COMMENT: {3/22/2004 7:05:48 PM}// COMMENT: {3/22/2004 7:00:55 PM}			this->m_UpperLeft[0] * this->m_Scale[0],
// COMMENT: {3/22/2004 7:05:48 PM}// COMMENT: {3/22/2004 7:00:55 PM}			yLowerLeft * this->m_Scale[1],
// COMMENT: {3/22/2004 7:05:48 PM}// COMMENT: {3/22/2004 7:00:55 PM}			this->m_UpperLeft[2] * this->m_Scale[2]
// COMMENT: {3/22/2004 7:05:48 PM}// COMMENT: {3/22/2004 7:00:55 PM}			);
// COMMENT: {3/22/2004 7:05:48 PM}		this->m_MapImageReader2->SetDataOrigin(
// COMMENT: {3/22/2004 7:05:48 PM}			this->m_UpperLeft[0],
// COMMENT: {3/22/2004 7:05:48 PM}			yLowerLeft,
// COMMENT: {3/22/2004 7:05:48 PM}			this->m_UpperLeft[2]
// COMMENT: {3/22/2004 7:05:48 PM}			);
	}
}

void CMapImageActor2::SetScale(double xScale, double yScale, double zScale)
{
	this->m_Scale[0] = xScale;
	this->m_Scale[1] = yScale;
	this->m_Scale[2] = zScale;

	this->UpdateImageReader2();
}
