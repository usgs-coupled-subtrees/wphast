#pragma once

#include <vtkOpenGLImageActor.h>

class vtkImageReader2;
class vtkImageShiftScale;
class CWorldTransform;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CMapImageActor : public vtkOpenGLImageActor
{
public:
	vtkTypeRevisionMacro(CMapImageActor,vtkOpenGLImageActor);
	static CMapImageActor *New();

	int SetFileName(const char *filename);
	int SetWorldTransform(const CWorldTransform &wtrans);
	int SetWorldFileName(const char *filename);
	int* GetDataExtent(void)const;

	int PlaceMap(vtkFloatingPointType xPos, vtkFloatingPointType yPos, vtkFloatingPointType zPos, vtkFloatingPointType angle);

	void SetScale(vtkFloatingPointType xScale, vtkFloatingPointType yScale, vtkFloatingPointType zScale);
	vtkImageReader2* GetImageReader2(void) { return m_MapImageReader2; }

protected:
	CMapImageActor(void);
	virtual ~CMapImageActor(void);

	void UpdateImageReader2(void);


protected:
	vtkImageReader2    *m_MapImageReader2;
	vtkImageShiftScale *m_MapShiftScale;

public:
	//double m_XDataSpacing;
	//double m_YDataSpacing;
	//double m_XUpperLeft;
	//double m_YUpperLeft;

//protected:
public:
    
	vtkFloatingPointType m_Scale[3];
	vtkFloatingPointType m_DataSpacing[3];
	vtkFloatingPointType m_UpperLeft[3];

private:
	CMapImageActor(const CMapImageActor&);  // Not implemented.
	void operator=(const CMapImageActor&);  // Not implemented.
};
