#include "StdAfx.h"
#include "NullPolyhedron.h"

NullPolyhedron::NullPolyhedron(void)
{
	this->type = Polyhedron::NONE;
	this->box.x1 = 0;
	this->box.y1 = 0;
	this->box.z1 = 0;
	this->box.x2 = 0;
	this->box.y2 = 0;
	this->box.z2 = 0;
	this->box.zone_defined = true;
}

NullPolyhedron::~NullPolyhedron(void)
{
}

struct zone *
NullPolyhedron::Set_bounding_box()
{
	return &(this->box);
}

void
NullPolyhedron::Points_in_polyhedron(std::list < int > &list_of_numbers, std::vector < Point > &node_xyz)
{
	list_of_numbers.clear();
}

gpc_polygon *
NullPolyhedron::Slice(Cell_Face face, double coord)
{
	return (NULL);
}

void
NullPolyhedron::printOn(std::ostream & os) const
{
	// nothing to do
}

NullPolyhedron *
NullPolyhedron::clone() const
{
	return new NullPolyhedron(*this);
}

NullPolyhedron *
NullPolyhedron::create() const
{
	return new NullPolyhedron();
}
