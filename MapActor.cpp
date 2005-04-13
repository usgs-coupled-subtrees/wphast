#include "StdAfx.h"
#include "MapActor.h"
#include <fstream>

#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>

#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include <vtkTransform.h>
#include <vtkObjectFactory.h>

#include "Global.h"
#include "WorldTransform.h"
#include "SiteMap.h"

vtkCxxRevisionMacro(CMapActor, "$Revision$");
vtkStandardNewMacro(CMapActor);


CMapActor::CMapActor(void)
: m_ImageReader2(0)
, m_Texture(0)
, m_PlaneSource(0)
, m_PolyDataMapper(0)
{
	this->m_Texture = vtkTexture::New();
	//{{
	this->m_Texture->InterpolateOff();
	//}}
	this->m_PlaneSource = vtkPlaneSource::New();
	this->m_PolyDataMapper = vtkPolyDataMapper::New();
	this->m_PolyDataMapper->SetInput(this->m_PlaneSource->GetOutput());


	// get temp file
	TCHAR szTempDirectory[MAX_PATH];
	::GetTempPath(MAX_PATH, szTempDirectory);
	::GetTempFileName(szTempDirectory, _T("WPH"), 0, this->m_szTempFileName);	
}

CMapActor::~CMapActor(void)
{
	// SetTexture is req'd so that m_ImageReader's dtor
	// is called within Delete and we can delete m_szTempFileName
	this->SetTexture(NULL);

	if (this->m_Texture)        this->m_Texture->Delete();
	if (this->m_ImageReader2)   this->m_ImageReader2->Delete();
	if (this->m_PlaneSource)    this->m_PlaneSource->Delete();
	if (this->m_PolyDataMapper) this->m_PolyDataMapper->Delete();
	try
	{
		CFile::Remove(this->m_szTempFileName);
	}
	catch (CFileException* pEx)
	{
		ASSERT(FALSE);
		pEx->Delete();
	}
}

int CMapActor::SetFileName(const char *filename)
{
	if (this->m_ImageReader2) this->m_ImageReader2->Delete();

	vtkImageReader2Factory *factory = vtkImageReader2Factory::New();
	this->m_ImageReader2 = factory->CreateImageReader2(filename);
	factory->Delete();

	if (this->m_ImageReader2)
	{
		if (::strcmp(filename, this->m_szTempFileName) != 0) {
			VERIFY(::CopyFile(filename, this->m_szTempFileName, FALSE));
		}

		this->m_ImageReader2->SetFileName( this->m_szTempFileName );
		this->m_ImageReader2->Update( );
		this->m_Texture->SetInput( this->m_ImageReader2->GetOutput() );
		this->SetMapper( this->m_PolyDataMapper );
		this->SetTexture( this->m_Texture );

		return 1; // success
	}
	return 0; // failure
}

int CMapActor::SetWorldTransform(const CWorldTransform &wtrans)
{
	if (this->m_ImageReader2)
	{
		const double *dataSpacing = wtrans.GetDataSpacing();
		const double *upperLeft = wtrans.GetUpperLeft();

		this->m_XDataSpacing = dataSpacing[0];
		this->m_YDataSpacing = dataSpacing[1];
		this->m_XUpperLeft   = upperLeft[0];
		this->m_YUpperLeft   = upperLeft[1];

// COMMENT: {5/18/2004 3:29:40 PM}		this->m_XDataSpacing = wtrans.x_dimension;
// COMMENT: {5/18/2004 3:29:40 PM}		this->m_YDataSpacing = wtrans.y_dimension;
// COMMENT: {5/18/2004 3:29:40 PM}		this->m_XUpperLeft   = wtrans.x_coor_upper_left;
// COMMENT: {5/18/2004 3:29:40 PM}		this->m_YUpperLeft   = wtrans.y_coor_upper_left;

		this->m_ImageReader2->Update();

		//BUGBUG Does this do anything???
		this->m_ImageReader2->SetDataSpacing(this->m_XDataSpacing, this->m_YDataSpacing, 1);

		int *dataExtent = this->m_ImageReader2->GetDataExtent();
		int cx = dataExtent[1] - dataExtent[0];
		int cy = dataExtent[3] - dataExtent[2];
		int cz = dataExtent[5] - dataExtent[4];
		ASSERT(cx > 0);
		ASSERT(cy > 0);
		ASSERT(cz == 0);

		float xMin = this->m_XUpperLeft;
		float xMax = cx * this->m_XDataSpacing + this->m_XUpperLeft;

		float yMin = this->m_YUpperLeft - cy * this->m_YDataSpacing;
		float yMax = this->m_YUpperLeft;

		this->m_PlaneSource->SetOrigin(xMin, yMin, 0.0);
		this->m_PlaneSource->SetPoint1(xMax, yMin, 0.0);
		this->m_PlaneSource->SetPoint2(xMin, yMax, 0.0);
	}
	else
	{
		::AfxMessageBox("SetFileName must be called before SetWorldTransform");
		return 0; // failure
	}

	return 1; // success
}

int CMapActor::SetWorldFileName(const char *filename)
{
	ASSERT(FALSE); // use SetWorldTransform
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
	ifs >> this->m_XDataSpacing;
	ifs >> dummy;
	ASSERT(dummy == 0.0);
	ifs >> dummy;
	ASSERT(dummy == 0.0);
	ifs >> this->m_YDataSpacing;
	ASSERT(this->m_YDataSpacing < 0.0);
	this->m_YDataSpacing = fabs(this->m_YDataSpacing);

	ifs >> this->m_XUpperLeft;
	ifs >> this->m_YUpperLeft;

	if (this->m_ImageReader2)
	{
		this->m_ImageReader2->Update();
		this->m_ImageReader2->SetDataSpacing(this->m_XDataSpacing, this->m_YDataSpacing, 1);

// COMMENT: {3/22/2004 8:28:20 PM}		int *dims = this->m_ImageReader2->GetDataExtent();
// COMMENT: {3/22/2004 8:28:20 PM}		ASSERT(dims[0] == 0 && dims[2] == 0);
// COMMENT: {3/22/2004 8:28:20 PM}		int cx = dims[1];
// COMMENT: {3/22/2004 8:28:20 PM}		ASSERT(cx > 0);
// COMMENT: {3/22/2004 8:28:20 PM}		int cy = dims[3];
// COMMENT: {3/22/2004 8:28:20 PM}		ASSERT(cy > 0);
// COMMENT: {3/22/2004 8:28:20 PM}
// COMMENT: {3/22/2004 8:28:20 PM}		double xMax = this->m_XUpperLeft + (cx - 1) * this->m_XDataSpacing;
// COMMENT: {3/22/2004 8:28:20 PM}		double yMin = this->m_YUpperLeft - (cy - 1) * this->m_YDataSpacing;
// COMMENT: {3/22/2004 8:28:20 PM}		this->m_ImageReader2->SetDataOrigin(this->m_XUpperLeft, yMin, 0.0);

// COMMENT: {3/22/2004 7:57:09 PM}		//{{
// COMMENT: {3/22/2004 7:57:09 PM}		this->m_PlaneSource->SetOrigin(this->m_XUpperLeft, yMin, 0.0);
// COMMENT: {3/22/2004 7:57:09 PM}		this->m_PlaneSource->SetPoint1(xMax, yMin, 0.0);
// COMMENT: {3/22/2004 7:57:09 PM}		this->m_PlaneSource->SetPoint2(this->m_XUpperLeft, this->m_YUpperLeft, 0.0);
// COMMENT: {3/22/2004 7:57:09 PM}		//}}
		//{{
		int *dataExtent = this->m_ImageReader2->GetDataExtent();
		int cx = dataExtent[1] - dataExtent[0];
		int cy = dataExtent[3] - dataExtent[2];
		int cz = dataExtent[5] - dataExtent[4];
		ASSERT(cx > 0);
		ASSERT(cy > 0);
		ASSERT(cz == 0);

		float xMin = this->m_XUpperLeft;
		float xMax = cx * this->m_XDataSpacing + this->m_XUpperLeft;

		float yMin = this->m_YUpperLeft - cy * this->m_YDataSpacing;
		float yMax = this->m_YUpperLeft;

		this->m_PlaneSource->SetOrigin(xMin, yMin, 0.0);
		this->m_PlaneSource->SetPoint1(xMax, yMin, 0.0);
		this->m_PlaneSource->SetPoint2(xMin, yMax, 0.0);
		//}}
	}
	else
	{
		::AfxMessageBox("SetFileName must be called before SetWorldFileName");
		return 0; // failure
	}

	return 1; // success
}


#ifdef _DEBUG
CDumpContext& operator<< (CDumpContext &dc, vtkObject &o);
CDumpContext& operator<< (CDumpContext &dc, vtkObject &o)
{
	ostrstream oss;
	o.PrintSelf(oss, 4);
	oss << ends;
	dc << oss.str() << "\n";
	oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
	return dc;
}
#endif // _DEBUG

int CMapActor::PlaceMap(float xPos, float yPos, float zPos, float angle)
{
	vtkTransform* transform = vtkTransform::New();

	////{{{{
	float* center = this->m_PlaneSource->GetCenter();
	//transform->Translate(center[0],center[1],center[2]);
	transform->RotateZ(-angle);
	//transform->Translate(-center[0],-center[1],-center[2]);
	////}}}}

	// translate
	///transform->Translate(-xPos, -yPos, -zPos);
	transform->Translate(-xPos, -yPos, zPos);
	float *o = this->m_PlaneSource->GetOrigin();
	float *pt1 = this->m_PlaneSource->GetPoint1();
	float *pt2 = this->m_PlaneSource->GetPoint2();

	float oNew[3], pt1New[3], pt2New[3];
	transform->TransformPoint(o,oNew);
	transform->TransformPoint(pt1,pt1New);
	transform->TransformPoint(pt2,pt2New);

	this->m_PlaneSource->SetOrigin(oNew);
	this->m_PlaneSource->SetPoint1(pt1New);
	this->m_PlaneSource->SetPoint2(pt2New);
	this->m_PlaneSource->Update();

	/***
	// Now rotate
	transform->Identity();
	float* center = this->m_PlaneSource->GetCenter();
	transform->Translate(center[0],center[1],center[2]);
	transform->RotateZ(-angle);
	transform->Translate(-center[0],-center[1],-center[2]);

	transform->TransformPoint(o,oNew);
	transform->TransformPoint(pt1,pt1New);
	transform->TransformPoint(pt2,pt2New);

	this->m_PlaneSource->SetOrigin(oNew);
	this->m_PlaneSource->SetPoint1(pt1New);
	this->m_PlaneSource->SetPoint2(pt2New);
	this->m_PlaneSource->Update();
	***/
	
	transform->Delete();
	return 1;

// COMMENT: {4/14/2004 3:20:24 PM}	//{{
// COMMENT: {4/14/2004 3:20:24 PM}	float *o = this->GetOrigin();
// COMMENT: {4/14/2004 3:20:24 PM}	float *center = this->GetCenter();
// COMMENT: {4/14/2004 3:20:24 PM}
// COMMENT: {4/14/2004 3:20:24 PM}#ifdef _DEBUG
// COMMENT: {4/14/2004 3:20:24 PM}	afxDump << *this << "\n";
// COMMENT: {4/14/2004 3:20:24 PM}	afxDump << "Center: (" << center[0] << ", " << center[1] << ", " << center[2] << ")\n";
// COMMENT: {4/14/2004 3:20:24 PM}	afxDump << "Origin: (" << o[0] << ", " << o[1] << ", " << o[2] << ")\n";
// COMMENT: {4/14/2004 3:20:24 PM}#endif
// COMMENT: {4/14/2004 3:20:24 PM}
// COMMENT: {4/14/2004 3:20:24 PM}	this->SetOrigin(center);
// COMMENT: {4/14/2004 3:20:24 PM}	////this->RotateZ(-angle);
// COMMENT: {4/14/2004 3:20:24 PM}	this->AddPosition(-xPos, -yPos, -zPos);
// COMMENT: {4/14/2004 3:20:24 PM}
// COMMENT: {4/14/2004 3:20:24 PM}#ifdef _DEBUG
// COMMENT: {4/14/2004 3:20:24 PM}	afxDump << *this << "\n";
// COMMENT: {4/14/2004 3:20:24 PM}	afxDump << "Center: (" << center[0] << ", " << center[1] << ", " << center[2] << ")\n";
// COMMENT: {4/14/2004 3:20:24 PM}	afxDump << "Origin: (" << o[0] << ", " << o[1] << ", " << o[2] << ")\n";
// COMMENT: {4/14/2004 3:20:24 PM}#endif
// COMMENT: {4/14/2004 3:20:24 PM}
// COMMENT: {4/14/2004 3:20:24 PM}	//}}
// COMMENT: {4/14/2004 3:20:24 PM}
// COMMENT: {4/14/2004 3:20:24 PM}	// this->SetPosition(-center[0], -center[1], -center[2]);
// COMMENT: {4/14/2004 3:20:24 PM}
// COMMENT: {4/14/2004 3:20:24 PM}	// this->AddPosition(xPos, yPos, zPos);
// COMMENT: {4/14/2004 3:20:24 PM}	// this->RotateZ(-angle);
// COMMENT: {4/14/2004 3:20:24 PM}	// this->AddPosition(-xPos, -yPos, -zPos);
// COMMENT: {4/14/2004 3:20:24 PM}	return 1;
}

void CMapActor::SetSiteMap(const CSiteMap &siteMap)
{
	if (this->SetFileName(siteMap.m_fileName.c_str()) != 1)
	{
		std::string err("Bad file name: ");
		err += siteMap.m_fileName.c_str();
		throw err;
	}

// COMMENT: {5/20/2004 4:35:36 PM}	if (!siteMap.WorldFileName.empty())
// COMMENT: {5/20/2004 4:35:36 PM}	{
// COMMENT: {5/20/2004 4:35:36 PM}		CWorldTransform wtrans;
// COMMENT: {5/20/2004 4:35:36 PM}		if (CGlobal::LoadWorldFile(siteMap.WorldFileName.c_str(), wtrans) != 0)
// COMMENT: {5/20/2004 4:35:36 PM}		{
// COMMENT: {5/20/2004 4:35:36 PM}			std::string err("Bad world file: ");
// COMMENT: {5/20/2004 4:35:36 PM}			err += siteMap.WorldFileName.c_str();
// COMMENT: {5/20/2004 4:35:36 PM}			throw err;
// COMMENT: {5/20/2004 4:35:36 PM}		}
// COMMENT: {5/20/2004 4:35:36 PM}		this->SetWorldTransform(wtrans);
// COMMENT: {5/20/2004 4:35:36 PM}	}
	this->SetWorldTransform(siteMap.GetWorldTransform());

	if (this->PlaceMap(siteMap.m_placement[0], siteMap.m_placement[1], siteMap.m_placement[2],
		siteMap.m_angle) != 1)
	{
		std::string err("Unable to place map.");
		throw err;
	}

	// OK if here
	this->m_siteMap = siteMap;
}

void CMapActor::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSiteMap[] = "SiteMap";

	ASSERT(loc_id > 0);
	if (loc_id <= 0) return;

	hid_t sitemap_id = 0;
	herr_t status;

	if (bStoring)
	{
		// Create the szSiteMap group
		sitemap_id = ::H5Gcreate(loc_id, szSiteMap, 0);
		ASSERT(sitemap_id > 0);
	}
	else
	{
		// Open the szSiteMap group
		sitemap_id = ::H5Gopen(loc_id, szSiteMap);
		TRACE("CMapActor::Serialize -- No SiteMap\n");
	}

	if (sitemap_id > 0)
	{
		status = CGlobal::HDFSerializeBinaryFile(bStoring, sitemap_id, "Image", this->m_szTempFileName);
		ASSERT(status >= 0);

		this->m_siteMap.Serialize(bStoring, sitemap_id);

		// close the szSiteMap group
		status = ::H5Gclose(sitemap_id);
		ASSERT(status >= 0);

		if (!bStoring) {
			if (this->SetFileName(this->m_szTempFileName) == 1) {
				this->SetWorldTransform(this->m_siteMap.GetWorldTransform());
				this->PlaceMap(
					this->m_siteMap.m_placement[0],
					this->m_siteMap.m_placement[1],
					this->m_siteMap.m_placement[2],
					this->m_siteMap.m_angle);
			}
		}
	}
}
