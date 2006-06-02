#pragma once

class vtkLineSource;
class vtkAppendPolyData;
class vtkPolyDataMapper;

#include <vtkOpenGLActor.h>

class CMarker : public vtkOpenGLActor
{
public:
	vtkTypeRevisionMacro(CMarker,vtkOpenGLActor);
	static CMarker *New();

	vtkSetVector3Macro(Point,double);
	vtkGetVector3Macro(Point,double);

	virtual void Modified();

protected:
	vtkLineSource      *m_horzLine;
	vtkLineSource      *m_vertLine;
	vtkAppendPolyData  *m_append;
	vtkPolyDataMapper  *m_mapper;
	double              Point[3];

protected:
	CMarker(void);
	virtual ~CMarker(void);

private:
	CMarker(const CMarker&);  // Not implemented.
	void operator=(const CMarker&);  // Not implemented.
};
