#pragma once
#include "srcinput/Polyhedron.h"

class NullPolyhedron : public Polyhedron
{
public:
	NullPolyhedron(void);
	virtual ~NullPolyhedron(void);

	virtual void Points_in_polyhedron(std::list < int >&list, std::vector < Point > &point_xyz);

	virtual NullPolyhedron *clone() const;
	virtual NullPolyhedron *create() const;
	virtual gpc_polygon *Slice(Cell_Face face, double coord);

protected:
	virtual struct zone *Set_bounding_box();
	virtual void printOn(std::ostream & os) const;
};
