#include "StdAfx.h"
#include "FakeFiledata.h"

std::list<Filedata*> FakeFiledata::fake_file_data_list;

FakeFiledata::FakeFiledata(void)
{
}

FakeFiledata::FakeFiledata(std::string filename, PHAST_Transform::COORDINATE_SYSTEM cs)
{
	this->filename = filename;

	std::vector<Point> new_pts;
	this->coordinate_system = cs;
	this->Add_data_source(-1, new_pts, 3, this->coordinate_system);

// COMMENT: {4/29/2009 3:57:12 PM}	std::map<std::string, Filedata*>::iterator it = Filedata::file_data_map.find(this->filename);
// COMMENT: {4/29/2009 3:57:12 PM}	if (it != Filedata::file_data_map.end())
// COMMENT: {4/29/2009 3:57:12 PM}	{
// COMMENT: {4/29/2009 3:57:12 PM}		ASSERT(FALSE);
// COMMENT: {4/29/2009 3:57:12 PM}	}
// COMMENT: {4/29/2009 3:57:12 PM}	Filedata::file_data_map[this->filename] = this;
}

Filedata* FakeFiledata::New(std::string filename, PHAST_Transform::COORDINATE_SYSTEM cs)
{
	FakeFiledata* fd = new FakeFiledata(filename, cs);
	FakeFiledata::fake_file_data_list.push_back(fd);
	return fd;
}


FakeFiledata::~FakeFiledata(void)
{
}

bool FakeFiledata::Make_polygons(int field, PHAST_polygon& polygons)
{
	ASSERT(FALSE);
	return false;
}

void FakeFiledata::Clear_fake_file_data_list(void)
{
	std::list<Filedata*>::iterator it = FakeFiledata::fake_file_data_list.begin();
	for (; it != FakeFiledata::fake_file_data_list.end(); ++it)
	{
		delete (*it);
	}
	FakeFiledata::fake_file_data_list.clear();
}
