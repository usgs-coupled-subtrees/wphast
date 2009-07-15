#include "StdAfx.h"
#include "FakeFiledata.h"

#if defined(_WIN32) && defined(_DEBUG)
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

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
