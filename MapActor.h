#pragma once
#include <vtkOpenGLActor.h>
#include "SiteMap.h"

class vtkImageReader2;
class vtkTexture;
class vtkPlaneSource;
class vtkPolyDataMapper;
class CWorldTransform;
// class CSiteMap;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CMapActor : public vtkOpenGLActor
{
public:
	vtkTypeRevisionMacro(CMapActor,vtkOpenGLActor);
	static CMapActor *New();

	void SetSiteMap(const CSiteMap &siteMap); // throws LPTCSTR
	CSiteMap GetSiteMap(void)const { return m_siteMap; }

	void Serialize(bool bStoring, hid_t loc_id);

protected:
	CMapActor(void);
	virtual ~CMapActor(void);

	int PlaceMap(vtkFloatingPointType xPos, vtkFloatingPointType yPos, vtkFloatingPointType zPos, vtkFloatingPointType angle);
	int SetFileName(const char *filename);
	int SetWorldTransform(const CWorldTransform &wtrans);
	int SetWorldFileName(const char *filename);

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

protected:
	vtkFloatingPointType m_UpperLeft[3];
	vtkFloatingPointType m_DataSpacing[3];
	CSiteMap m_siteMap;
	TCHAR m_szTempFileName[MAX_PATH];
private:
	CMapActor(const CMapActor&);  // Not implemented.
	void operator=(const CMapActor&);  // Not implemented.
};
