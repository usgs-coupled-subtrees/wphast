#pragma once

#include <vtkOpenGLImageActor.h>

class vtkImageReader2;
class vtkImageShiftScale;
class CWorldTransform;

class CMapImageActor : public vtkOpenGLImageActor
{
public:
	vtkTypeRevisionMacro(CMapImageActor,vtkOpenGLImageActor);
	static CMapImageActor *New();

	int SetFileName(const char *filename);
	int SetWorldTransform(const CWorldTransform &wtrans);
	int SetWorldFileName(const char *filename);
	int* GetDataExtent(void)const;

	int PlaceMap(float xPos, float yPos, float zPos, float angle);

	void SetScale(float xScale, float yScale, float zScale);
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
    
	float m_Scale[3];
	float m_DataSpacing[3];
	float m_UpperLeft[3];

private:
	CMapImageActor(const CMapImageActor&);  // Not implemented.
	void operator=(const CMapImageActor&);  // Not implemented.
};
