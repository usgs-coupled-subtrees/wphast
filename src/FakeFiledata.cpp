#include "StdAfx.h"
#include "FakeFiledata.h"

#if defined(_WIN32) && defined(_DEBUG)
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

std::list<Filedata*> FakeFiledata::fake_file_data_list;

FakeFiledata::FakeFiledata(void)
{
}

FakeFiledata::FakeFiledata(std::string filename, PHAST_Transform::COORDINATE_SYSTEM cs, int attribute)
{
	this->filename = filename;

	std::vector<Point> new_pts;
	this->coordinate_system = cs;
// COMMENT: {7/20/2009 4:29:41 PM}	this->Add_data_source(-1, new_pts, 3, this->coordinate_system);
	this->Add_data_source(attribute, new_pts, 3, this->coordinate_system);
}

// COMMENT: {7/20/2009 4:29:03 PM}Filedata* FakeFiledata::New(std::string filename, PHAST_Transform::COORDINATE_SYSTEM cs)
Filedata* FakeFiledata::New(std::string filename, PHAST_Transform::COORDINATE_SYSTEM cs, int attribute)
{
	FakeFiledata* fd = new FakeFiledata(filename, cs, attribute);
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
		std::map < std::string, Filedata * >::iterator fi = Filedata::file_data_map.find((*it)->Get_filename());
		if (fi != Filedata::file_data_map.end())
		{
			if ((*fi).second != (*it))
			{
				delete (*it);
			}
		}
		else
		{
			delete (*it);
		}
	}
	FakeFiledata::fake_file_data_list.clear();
}
