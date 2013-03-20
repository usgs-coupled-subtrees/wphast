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
#include <vtkImageReader2.h>

#include "Global.h"
#include "WorldTransform.h"
#include "SiteMap.h"
#include "SiteMap2.h"
#include "SiteMap3.h"

vtkCxxRevisionMacro(CMapActor, "$Revision$");
vtkStandardNewMacro(CMapActor);


CMapActor::CMapActor(void)
: m_Texture(0)
, m_PlaneSource(0)
, m_PolyDataMapper(0)
, ImageData(0)
{
	this->m_Texture = vtkTexture::New();
	this->m_Texture->InterpolateOff();
	this->m_Texture->SetQualityTo32Bit();

	this->m_PlaneSource = vtkPlaneSource::New();
	this->m_PolyDataMapper = vtkPolyDataMapper::New();
	this->m_PolyDataMapper->SetInput(this->m_PlaneSource->GetOutput());
}

CMapActor::~CMapActor(void)
{
	// SetTexture is req'd so that m_ImageReader's dtor
	// is called within Delete and we can delete m_szTempFileName
	this->SetTexture(NULL);

	if (this->m_Texture)        this->m_Texture->Delete();
	if (this->m_PlaneSource)    this->m_PlaneSource->Delete();
	if (this->m_PolyDataMapper) this->m_PolyDataMapper->Delete();
	if (this->ImageData)        this->ImageData->Delete();
}

int CMapActor::SetFileName(const char *filename)
{
	vtkImageReader2Factory *factory = vtkImageReader2Factory::New();
	vtkImageReader2 *pImageReader2 = factory->CreateImageReader2(filename);
	factory->Delete();

	if (pImageReader2)
	{
		pImageReader2->SetFileName( filename );
		pImageReader2->Update( );

		ASSERT(this->ImageData == 0);
		if (this->ImageData = vtkImageData::New())
		{
			this->ImageData->DeepCopy( pImageReader2->GetOutput() );
			this->m_Texture->SetInput( this->ImageData );
		}

#if USE_FILTER
		std::ostrstream oss1;
		oss1 << "m_ImageReader2\n";
		this->m_ImageReader2->GetOutput()->PrintSelf(oss1, vtkIndent(4));
		oss1 << std::ends;
		TRACE("%s", oss1.str());

		vtkSimpleImageFilterExample2 *filter = vtkSimpleImageFilterExample2::New();
		filter->SetInput(this->m_ImageReader2->GetOutput());
		filter->Update();
		this->m_Texture->SetInput(filter->GetOutput());
		filter->Delete();
#endif

		this->SetMapper( this->m_PolyDataMapper );
		this->SetTexture( this->m_Texture );

		pImageReader2->Delete();

		return 1; // success
	}
	return 0; // failure
}

int CMapActor::SetWorldTransform(const CWorldTransform &wtrans)
{
	if (this->ImageData)
	{
		const double *dataSpacing = wtrans.GetDataSpacing();
		const double *upperLeft = wtrans.GetUpperLeft();

		this->m_XDataSpacing = dataSpacing[0];
		this->m_YDataSpacing = dataSpacing[1];
		this->m_XUpperLeft   = upperLeft[0];
		this->m_YUpperLeft   = upperLeft[1];

		this->ImageData->Update();
		this->ImageData->SetSpacing(this->m_XDataSpacing, this->m_YDataSpacing, 1);

		int *dataExtent = this->ImageData->GetExtent();
		int cx = dataExtent[1] - dataExtent[0];
		int cy = dataExtent[3] - dataExtent[2];
		int cz = dataExtent[5] - dataExtent[4];
		ASSERT(cx > 0);
		ASSERT(cy > 0);
		ASSERT(cz == 0);

		this->xMin = this->m_XUpperLeft - this->m_XDataSpacing / 2.;
		this->xMax = (cx+1) * this->m_XDataSpacing + xMin;

		this->yMax = this->m_YUpperLeft + this->m_YDataSpacing / 2.;
		this->yMin = yMax - (cy+1) * this->m_YDataSpacing;

		this->m_PlaneSource->SetOrigin(this->xMin, this->yMin, 0.0);
		this->m_PlaneSource->SetPoint1(this->xMax, this->yMin, 0.0);
		this->m_PlaneSource->SetPoint2(this->xMin, this->yMax, 0.0);
	}
	else
	{
		::AfxMessageBox("SetFileName must be called before SetWorldTransform");
		return 0; // failure
	}

	return 1; // success
}

#ifdef _DEBUG
CDumpContext& operator<< (CDumpContext &dc, vtkObject &o);
CDumpContext& operator<< (CDumpContext &dc, vtkObject &o)
{
	ostrstream oss;
	o.PrintSelf(oss, vtkIndent(4));
	oss << ends;
	dc << oss.str() << "\n";
	oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
	return dc;
}
#endif // _DEBUG

// COMMENT: {8/10/2009 7:51:21 PM}int CMapActor::PlaceMap(double xPos, double yPos, double zPos, double angle)
int CMapActor::PlaceMap(double xPos, double yPos, double zPos, double angle)
{
	vtkTransform* transform = vtkTransform::New();

	////{{{{
	double* center = this->m_PlaneSource->GetCenter();
	//transform->Translate(center[0],center[1],center[2]);
	transform->RotateZ(-angle);
	//transform->Translate(-center[0],-center[1],-center[2]);
	////}}}}

	// translate
	///transform->Translate(-xPos, -yPos, -zPos);
	transform->Translate(-xPos, -yPos, zPos);
	double *o = this->m_PlaneSource->GetOrigin();
	double *pt1 = this->m_PlaneSource->GetPoint1();
	double *pt2 = this->m_PlaneSource->GetPoint2();

	//{{
	/*
	this->m_PlaneSource->SetOrigin(this->xMin, this->yMin, 0.0);
	this->m_PlaneSource->SetPoint1(this->xMax, this->yMin, 0.0);
	this->m_PlaneSource->SetPoint2(this->xMin, this->yMax, 0.0);
	*/
	//}}

	double oNew[3], pt1New[3], pt2New[3];
	transform->TransformPoint(o,oNew);
	transform->TransformPoint(pt1,pt1New);
	transform->TransformPoint(pt2,pt2New);

	this->m_PlaneSource->SetOrigin(oNew);
	this->m_PlaneSource->SetPoint1(pt1New);
	this->m_PlaneSource->SetPoint2(pt2New);
	this->m_PlaneSource->Update();

	transform->Delete();
	return 1;
}

void CMapActor::SetSiteMap3(const CSiteMap3 &siteMap3)
{
	if (this->SetFileName(siteMap3.FileName.c_str()) != 1)
	{
		std::string err("Bad file name: ");
		err += siteMap3.FileName.c_str();
		throw err;
	}

	this->SetWorldTransform(siteMap3.GetWorldTransform());

	if (this->PlaceMap(siteMap3.Origin[0], siteMap3.Origin[1], siteMap3.Origin[2],
		siteMap3.Angle) != 1)
	{
		std::string err("Unable to place map.");
		throw err;
	}

	// OK if here
	this->SiteMap3 = siteMap3;
}

bool CMapActor::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSiteMap[]  = "SiteMap";
	static const char szSiteMap2[] = "SiteMap2";
	static const char szSiteMap3[] = "SiteMap3";

	static const char szImageData[] = "ImageData";

	bool bHaveImageData = false;

	ASSERT(loc_id > 0);
	if (loc_id <= 0) return bHaveImageData;

	hid_t sitemap_id = 0;
	herr_t status;
	int version = 0;

	if (bStoring)
	{
		// Create the szSiteMap group
		sitemap_id = ::H5Gcreate(loc_id, szSiteMap3, 0);
		ASSERT(sitemap_id > 0);
	}
	else
	{
		// Open the szSiteMap3 group
		sitemap_id = ::H5Gopen(loc_id, szSiteMap3);
		if (sitemap_id < 0)
		{
			// Open the szSiteMap2 group
			sitemap_id = ::H5Gopen(loc_id, szSiteMap2);
			if (sitemap_id < 0)
			{
				version = 1;
				// Open the szSiteMap group
				sitemap_id = ::H5Gopen(loc_id, szSiteMap);
			}
			else
			{
				version = 2;
			}
		}
		else
		{
			version = 3;
		}
	}

	if (sitemap_id > 0)
	{
		if (bStoring)
		{
			this->SiteMap3.Serialize(bStoring, sitemap_id);

			herr_t e = CGlobal::HDFSerializeImageData(bStoring, sitemap_id, szImageData, this->ImageData);
			ASSERT(e >= 0);
			if (e >= 0)
			{
				bHaveImageData = true;
			}
		}
		else
		{
			switch (version)
			{
			case 1:
				{
					CSiteMap sitemap;
					sitemap.Serialize(bStoring, sitemap_id);

					this->SiteMap3.Angle    = sitemap.m_angle;
					this->SiteMap3.FileName = sitemap.m_fileName;
					for (int i = 0; i < 3; ++i)
					{
						this->SiteMap3.Origin[i] = sitemap.m_placement[i];
					}
					this->SiteMap3.SetWorldTransform(sitemap.GetWorldTransform());
				}
				break;
			case 2:
				{
					CSiteMap2 sitemap2;
					sitemap2.Serialize(bStoring, sitemap_id);

					this->SiteMap3.Angle    = sitemap2.Angle;
					this->SiteMap3.FileName = sitemap2.FileName;
					for (int i = 0; i < 3; ++i)
					{
						this->SiteMap3.Origin[i] = sitemap2.Origin[i];
					}
					this->SiteMap3.SetWorldTransform(sitemap2.GetWorldTransform());
				}
				break;
			case 3:
				{
					this->SiteMap3.Serialize(bStoring, sitemap_id);
					ASSERT(this->ImageData == 0);
					herr_t e = CGlobal::HDFSerializeImageData(bStoring, sitemap_id, szImageData, this->ImageData);
					if (e >= 0)
					{
						bHaveImageData = true;

						this->m_Texture->SetInput( this->ImageData );
						this->SetMapper( this->m_PolyDataMapper );
						this->SetTexture( this->m_Texture );

						this->SetWorldTransform(this->SiteMap3.GetWorldTransform());
						this->PlaceMap(this->SiteMap3.Origin[0], this->SiteMap3.Origin[1], this->SiteMap3.Origin[2], this->SiteMap3.Angle);
					}
				}
				break;
			default:
				ASSERT(FALSE);
			}
		}			

		// close the szSiteMap group
		status = ::H5Gclose(sitemap_id);
		ASSERT(status >= 0);
	}
	return bHaveImageData;
}

int* CMapActor::GetDataExtent(void)const
{
	static const int extent[6];
	if (this->ImageData)
	{
		this->ImageData->Update();
		return this->ImageData->GetExtent();
	}
	return (int*)extent;
}

double* CMapActor::GetDataSpacing(void)const
{
	static const double spacing[3];
	if (this->ImageData)
	{
		this->ImageData->Update();
		return this->ImageData->GetSpacing();
	}
	return (double*)spacing;
}

double* CMapActor::GetDataOrigin(void)const
{
	static const double origin[3];
	if (this->ImageData)
	{
		this->ImageData->Update();
		return this->ImageData->GetOrigin();
	}
	return (double*)origin;
}

void CMapActor::SetDataOrigin(double x, double y, double z)
{
	if (this->ImageData)
	{
		this->ImageData->Update();
		return this->ImageData->SetOrigin(x, y, z);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMapActor::SetDataSpacing(double x, double y, double z)
{
	if (this->ImageData)
	{
		this->ImageData->Update();
		return this->ImageData->SetSpacing(x, y, z);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMapActor::UpdateFileName(const char *filename)
{
	this->SiteMap3.FileName = filename;
}

void CMapActor::UpdateWorldFileName(const char *filename)
{
	this->SiteMap3.WorldFileName = filename;
}
