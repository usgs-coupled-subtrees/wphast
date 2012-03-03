#pragma once
#include <vtkOpenGLActor.h>
#include "SiteMap3.h"

class vtkImageReader2;
class vtkTexture;
class vtkPlaneSource;
class vtkPolyDataMapper;
class CWorldTransform;
// class CSiteMap;

class CMapActor : public vtkOpenGLActor
{
public:
	vtkTypeRevisionMacro(CMapActor,vtkOpenGLActor);
	static CMapActor *New();

	void SetSiteMap3(const CSiteMap3 &siteMap3); // throws LPTCSTR
	CSiteMap3 GetSiteMap3(void)const { return SiteMap3; }

	void Serialize(bool bStoring, hid_t loc_id);
	friend class CXMLSerializer;

	int SetFileName(const char *filename);
	int SetWorldTransform(const CWorldTransform &wtrans);

	void UpdateFileName(const char *filename);
	void UpdateWorldFileName(const char *filename);

	double *GetDataSpacing()const;
	double *GetDataOrigin()const;
	int *GetDataExtent()const;

	void SetDataOrigin(double x, double y, double z);
	void SetDataSpacing(double x, double y, double z);

protected:
	CMapActor(void);
	virtual ~CMapActor(void);

	int PlaceMap(double xPos, double yPos, double zPos, double angle);
// COMMENT: {8/11/2009 2:52:54 PM}	int SetFileName(const char *filename);
// COMMENT: {8/11/2009 2:56:55 PM}	int SetWorldTransform(const CWorldTransform &wtrans);
	int SetWorldFileName(const char *filename);

	// Description:
	// Turn on/off linear interpolation of the texture map when rendering.
	int GetInterpolate() { return this->m_Texture->GetInterpolate(); };
	void SetInterpolate(int interpolate) { this->m_Texture->SetInterpolate(interpolate); };
	void InterpolateOn() { this->SetInterpolate((int)1); };
	void InterpolateOff() { this->SetInterpolate((int)0); };

protected:
	vtkImageReader2    *m_ImageReader2;
	vtkTexture         *m_Texture;
	vtkPlaneSource     *m_PlaneSource;
	vtkPolyDataMapper  *m_PolyDataMapper;

public:
	double m_XDataSpacing;
	double m_YDataSpacing;
	double m_XUpperLeft;
	double m_YUpperLeft;

	double xMin;
	double xMax;

	double yMax;
	double yMin;

	double Z;

protected:
	double m_UpperLeft[3];
	double m_DataSpacing[3];
	CSiteMap3 SiteMap3;
private:
	CMapActor(const CMapActor&);  // Not implemented.
	void operator=(const CMapActor&);  // Not implemented.
};
