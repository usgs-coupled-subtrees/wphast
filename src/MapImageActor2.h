#pragma once

#include <vtkOpenGLImageActor.h>

class vtkImageReader2;
class vtkImageShiftScale;
class CWorldTransform;

class CMapImageActor2 : public vtkOpenGLImageActor
{
public:
	vtkTypeRevisionMacro(CMapImageActor2,vtkOpenGLImageActor);
	static CMapImageActor2 *New();

	int SetFileName(const char *filename);
	int SetWorldTransform(const CWorldTransform &wtrans);
	int SetWorldFileName(const char *filename);
	int* GetDataExtent(void)const;

	int PlaceMap(double xPos, double yPos, double zPos, double angle);

	void SetScale(double xScale, double yScale, double zScale);
	vtkImageReader2* GetImageReader2(void) { return m_MapImageReader2; }

protected:
	CMapImageActor2(void);
	virtual ~CMapImageActor2(void);

	void UpdateImageReader2(void);


protected:
	vtkImageReader2    *m_MapImageReader2;
	vtkImageShiftScale *m_MapShiftScale;

public:
	double m_Scale[3];
	double m_DataSpacing[3];
	double m_UpperLeft[3];

private:
	CMapImageActor2(const CMapImageActor2&);  // Not implemented.
	void operator=(const CMapImageActor2&);  // Not implemented.
};
