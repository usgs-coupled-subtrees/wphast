#pragma once

#include "srcinput/Filedata.h"
#include <list>

class FakeFiledata : public Filedata
{
public:
	static Filedata* New(std::string filename, PHAST_Transform::COORDINATE_SYSTEM cs, int attribute);
	static void Clear_fake_file_data_list(void);

	virtual ~FakeFiledata(void);

	virtual bool Make_polygons(int field, PHAST_polygon & polygons);

	static std::list<Filedata*> fake_file_data_list;

private:
	FakeFiledata(void);
	FakeFiledata(std::string filename, PHAST_Transform::COORDINATE_SYSTEM cs, int attribute);
	FakeFiledata(const FakeFiledata& a);

	FakeFiledata& operator=(const FakeFiledata& rhs);
};
