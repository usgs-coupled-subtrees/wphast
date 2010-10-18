#pragma once
#include <vtkOpenGLActor.h>
#include <vtkLODActor.h>

class vtkAxes;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkLookupTable;

struct AxesActorProperties {
};

// 6/20/05 Why is this inherited from vtkOpenGLActor and not vtkActor?
// It isn't visible if vtkActor is used

class CAxesActor : public vtkOpenGLActor
{
public:
	vtkTypeRevisionMacro(CAxesActor,vtkOpenGLActor);
	static CAxesActor *New();

	void SetDefaultPositions(const double p[6]);
	void SetDefaultSize(float s);
	void SetDefaultTubeDiameter(float d);

	float GetNormalizedSize(void)const;
	float GetNormalizedTubeDiameter(void) const;
	const float *GetNormalizedPosition(void)const;

	int GetRepresentation(void)const;

protected:
	CAxesActor(void);
	virtual ~CAxesActor(void);

protected:
	vtkAxes           *m_pAxes;
	vtkTubeFilter     *m_pTubeFilter;
	vtkPolyDataMapper *m_pPolyDataMapper;
	vtkLookupTable    *m_LookupTable;

	float m_DefaultPositions[6];
	float m_NormalizedPosition[3];
	float m_DefaultTubeDiameter;
	float m_DefaultSize;

private:
	CAxesActor(const CAxesActor&);  // Not implemented.
	void operator=(const CAxesActor&);  // Not implemented.
};
