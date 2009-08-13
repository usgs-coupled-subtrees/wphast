#include "StdAfx.h"
#include "property.h"

#include "Global.h"
#include "FakeFiledata.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Cproperty::Cproperty()
{
	this->InternalInit();

	/**
	modified from phastinput[struct property *property_alloc(void)]
	**/
	this->count_alloc = 2;
	this->v = new double[this->count_alloc];
	ASSERT(this->v);
}

Cproperty::Cproperty(int value)
{
	this->InternalInit();

	this->count_alloc = 2;
	this->v = new double[this->count_alloc];
	ASSERT(this->v);
	this->v[0] = value;
	this->type = PROP_FIXED;
	this->count_v = 1;
}

Cproperty::Cproperty(double value)
{
	this->InternalInit();

	this->count_alloc = 2;
	this->v = new double[this->count_alloc];
	ASSERT(this->v);
	this->v[0] = value;
	this->type = PROP_FIXED;
	this->count_v = 1;
}

//{{
Cproperty::Cproperty(enum PROP_TYPE pt)
{
	const TCHAR PSZ_UNDEFINED[] = _T("UNDEFINED");

	this->InternalInit();

	if (pt == PROP_FIXED)
	{
		this->type        = PROP_FIXED;
		this->count_alloc = 2;
		this->v           = new double[this->count_alloc];
		ASSERT(this->v);
		this->v[0]        = std::numeric_limits<double>::signaling_NaN();
		this->count_v     = 1;
		this->mix1        = std::numeric_limits<double>::signaling_NaN();
		this->mix2        = std::numeric_limits<double>::signaling_NaN();
	}
	else if (pt == PROP_LINEAR)
	{
		this->type        = PROP_LINEAR;
		this->count_alloc = 2;
		this->v           = new double[this->count_alloc];
		ASSERT(this->v);
		this->coord       = 'x';
		this->v[0]        = std::numeric_limits<double>::signaling_NaN();
		this->v[1]        = std::numeric_limits<double>::signaling_NaN();
		this->dist1       = std::numeric_limits<double>::signaling_NaN();
		this->dist2       = std::numeric_limits<double>::signaling_NaN();
		this->count_v     = 2;
		this->mix1        = std::numeric_limits<double>::signaling_NaN();
		this->mix2        = std::numeric_limits<double>::signaling_NaN();
	}
	else if (pt == PROP_POINTS)
	{
		this->type = PROP_POINTS;
		this->data_source = new Data_source;
		this->data_source->Set_defined(true);
		this->data_source->Set_source_type(Data_source::POINTS);
		this->data_source->Set_user_source_type(Data_source::POINTS);
		this->mix1        = std::numeric_limits<double>::signaling_NaN();
		this->mix2        = std::numeric_limits<double>::signaling_NaN();
	}
	else if (pt == PROP_XYZ)
	{
		this->type = PROP_XYZ;
		this->data_source = new Data_source;
		this->data_source->Set_defined(true);
		this->data_source->Set_source_type(Data_source::XYZ);
		this->data_source->Set_user_source_type(Data_source::XYZ);
		this->data_source->Set_file_name(PSZ_UNDEFINED);
		this->data_source->Set_filedata(FakeFiledata::New(PSZ_UNDEFINED, PHAST_Transform::GRID, -1));
		this->mix1        = std::numeric_limits<double>::signaling_NaN();
		this->mix2        = std::numeric_limits<double>::signaling_NaN();
	}
	else if (pt == PROP_XYZT)
	{
		this->type = PROP_XYZT;
		this->data_source = new Data_source;
		this->data_source->Set_defined(true);
		this->data_source->Set_source_type(Data_source::XYZT);
		this->data_source->Set_user_source_type(Data_source::XYZT);
		this->data_source->Set_file_name(PSZ_UNDEFINED);
		this->data_source->Set_filedata(FakeFiledata::New(PSZ_UNDEFINED, PHAST_Transform::GRID, -1));
		this->mix1        = std::numeric_limits<double>::signaling_NaN();
		this->mix2        = std::numeric_limits<double>::signaling_NaN();
	}
	else
	{
		ASSERT(FALSE);
	}
}
//}}

Cproperty::~Cproperty()
{
	this->InternalDelete();
}

void Cproperty::InternalDelete(void)
{
	/**
	modified from phastinput[int property_free(struct property *property_ptr)]
	**/
	delete[] this->v;
	delete this->data_source;
	this->data_source = 0;
}

void Cproperty::InternalInit(void)
{
	/**
	modified from phastinput[struct property *property_alloc(void)]
	**/
	this->count_alloc = 0;
	this->v           = 0;
	this->count_v     = 0;
	this->type        = PROP_UNDEFINED;
	this->coord       = '\000';
	this->icoord      = -1;
	this->dist1       = -1;
	this->dist2       = -1;
	this->mix         = false;
	this->mix1        = -1;
	this->mix2        = -1;
	this->data_source = 0;
}

void Cproperty::InternalCopy(const property& src)
{
	if (src.type == PROP_FIXED) ASSERT(src.count_v == 1);
	if (src.type == PROP_LINEAR) ASSERT(src.count_v == 2);

	ASSERT(this->v == 0);
	this->v = new double[this->count_alloc = src.count_alloc];
	ASSERT(this->v);
	for (int i = 0; i < src.count_v; ++i)
	{
		this->v[i] = src.v[i];
	}
	this->count_v = src.count_v;
	this->type    = src.type;
	this->coord   = src.coord;
	this->icoord  = src.icoord;
	this->dist1   = src.dist1;
	this->dist2   = src.dist2;
	this->mix     = src.mix;
	this->mix1    = src.mix1;
	this->mix2    = src.mix2;

	if (src.data_source)
	{
		if (this->data_source)
		{
			*this->data_source = *src.data_source;
		}
		else
		{
			this->data_source = new Data_source(*src.data_source);
		}
	}
	else
	{
		if (this->data_source)
		{
			delete this->data_source;
			this->data_source = 0;
		}
	}
}

Cproperty::Cproperty(const Cproperty& src) // copy ctor
{
#ifdef _DEBUG
	this->v = 0;
#endif
	this->data_source = 0; // reqd for all ctor that don't call InternalInit
	this->InternalCopy(src);
}

Cproperty::Cproperty(const struct property& src) // copy ctor
{
#ifdef _DEBUG
	this->v = 0;
#endif
	this->data_source = 0; // reqd for all ctor that don't call InternalInit
	this->InternalCopy(src);
}

Cproperty& Cproperty::operator=(const Cproperty& rhs) // copy assignment
{
	if (this != &rhs)
	{
		this->InternalDelete();
		this->InternalInit();
		this->InternalCopy(rhs);
	}
	return *this;

	//// check for assignment to self
	//if (this == &rhs) return *this;

	//if (this->count_alloc != rhs.count_alloc) {
	//	delete[] this->v;
	//	this->v = new double[this->count_alloc = rhs.count_alloc];
	//	ASSERT(this->v);
	//}
	//for (int i = 0; i < rhs.count_v; ++i) {
	//	this->v[i] = rhs.v[i];
	//}
	//this->count_v = rhs.count_v;
	//this->type    = rhs.type;
	//this->coord   = rhs.coord;
	//this->icoord  = rhs.icoord;
	//this->dist1   = rhs.dist1;
	//this->dist2   = rhs.dist2;
	//return *this;
}

#ifdef _DEBUG
void Cproperty::AssertValid() const
{
	ASSERT(this);
	switch (this->type)
	{
	case PROP_UNDEFINED:
		break;
	case PROP_FIXED:
		ASSERT(this->count_v == 1);
		break;
	case PROP_LINEAR:
		ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');
		ASSERT(this->count_v == 2);
		break;
	case PROP_ZONE:
		ASSERT(FALSE);
		break;
	case PROP_POINTS:
		ASSERT(this->data_source);
		ASSERT(this->data_source->Get_defined());
		ASSERT(this->data_source->Get_points().size());
		break;
	case PROP_XYZ:
		ASSERT(this->count_v == 0);
		ASSERT(this->data_source);
		ASSERT(this->data_source->Get_defined());
		ASSERT(this->data_source->Get_file_name().size());
		ASSERT(this->data_source->Get_source_type() == Data_source::XYZ);
		ASSERT(this->data_source->Get_user_source_type() == Data_source::XYZ);
		break;
	case PROP_XYZT:
		ASSERT(this->count_v == 0);
		ASSERT(this->data_source);
		ASSERT(this->data_source->Get_defined());
		ASSERT(this->data_source->Get_file_name().size());
		ASSERT(this->data_source->Get_source_type() == Data_source::XYZT);
		ASSERT(this->data_source->Get_user_source_type() == Data_source::XYZT);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void Cproperty::Dump(CDumpContext& dc)const
{
	char buffer[10];
	if (this->type == PROP_LINEAR)
	{
		sprintf(buffer, "%c", this->coord);
	}

	dc << "<Cproperty>\n";
	dc << "\t<type>\n";
	switch (this->type)
	{
		case PROP_UNDEFINED:
			dc << "\t\t<UNDEFINED>\n";
			break;
		case PROP_FIXED:
			dc << "\t\t<FIXED value=\"" << this->v[0] << "\">\n";
			break;
		case PROP_LINEAR:
			dc << "\t\t<LINEAR"
				<< " direction=\"" << buffer << "\""
				<< " value1=\"" << this->v[0] << "\""
				<< " dist1=\"" << this->dist1 << "\""
				<< " value2=\"" << this->v[1] << "\""
				<< " dist2=\"" << this->dist2 << "\""
                << ">\n";
			break;
		case PROP_POINTS:
			{
				dc << "\t\t<POINTS>";
				std::vector<Point> &pts = this->data_source->Get_user_points();
				for (size_t i = 0; i < pts.size(); ++i)
				{
					dc << "\t\t\t<pt \'x\'=" << pts[i].x() << " \'y\'=" << pts[i].y() << " \'z\'=" << pts[i].z() << " \'v\'=" << pts[i].get_v() << "/>\n";
				}
				dc << "\t\t<\\POINTS>\n";
			}
			break;
		case PROP_XYZ:
			dc << "\t\t<XYZ \'filename\'=\"" << this->data_source->Get_file_name().c_str() << "\"\\>\n";
			break;
		case PROP_XYZT:
			dc << "\t\t<XYZT \'filename\'=\"" << this->data_source->Get_file_name().c_str() << "\"\\>\n";
			break;
		default:
			ASSERT(FALSE);
	}
	dc << "\t</type>\n";
	dc << "</Cproperty>\n";
}
#endif

void Cproperty::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szPropType[]   = "prop_type";
	static const char szType[]       = "type";
	static const char szV[]          = "v";
	static const char szCoord[]      = "coord";
	static const char szDist[]       = "dist";
	static const char szDataSource[] = "data_source";
	static const char szMixture[]    = "mixture";
	static const char szMix1[]       = "mix1";
	static const char szMix2[]       = "mix2";

	const int _LINEAR = 11;
	const int _FIXED  = 12;

	herr_t status;

	double dist[2];

  	ASSERT(this);

	if (bStoring)
	{
		// prop_type
		//
  		ASSERT(this->type == PROP_FIXED || this->type == PROP_LINEAR || this->type == PROP_POINTS
			|| this->type == PROP_XYZ || this->type == PROP_XYZT);

		hid_t proptype = CGlobal::HDFCreatePropType();
		status = CGlobal::HDFSerialize(bStoring, loc_id, szPropType, proptype, 1, &this->type);
		ASSERT(status >= 0);
		status = H5Tclose(proptype);
		ASSERT(status >= 0);

		if (this->type == PROP_FIXED || this->type == PROP_LINEAR)
		{
			// v and count_v
			//
			hsize_t count = this->count_v;
			status = CGlobal::HDFSerializeWithSize(bStoring, loc_id, szV, H5T_NATIVE_DOUBLE, count, this->v);
			ASSERT(status >= 0);
		}

		// mixture
		status = CGlobal::HDFSerializeBool(bStoring, loc_id, szMixture, this->mix);
		if (this->mix)
		{
			status = CGlobal::HDFSerialize(bStoring, loc_id, szMix1, H5T_NATIVE_DOUBLE, 1, &this->mix1);
			status = CGlobal::HDFSerialize(bStoring, loc_id, szMix2, H5T_NATIVE_DOUBLE, 1, &this->mix2);
		}

		switch (this->type)
		{
			case PROP_FIXED:   // was FIXED (12)
				ASSERT(this->count_v == 1);
				break;
			case PROP_LINEAR:  // was LINEAR (11)
				ASSERT(this->count_v == 2);
				ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');

				// coord
				//
				status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_CHAR, 1, &this->coord);
				ASSERT(status >= 0);

				// dist1 and dist2
				//
				dist[0] = this->dist1;
				dist[1] = this->dist2;
				status = CGlobal::HDFSerialize(bStoring, loc_id, szDist, H5T_NATIVE_DOUBLE, 2, dist);
				ASSERT(status >= 0);
				break;
			case PROP_POINTS:
				ASSERT(this->data_source);

				// Datasource
				//
				if (this->data_source)
				{
					ASSERT(this->data_source->Get_attribute() == -1);
					status = CGlobal::HDFSerializeData_source(bStoring, loc_id, szDataSource, *this->data_source);
					ASSERT(status >= 0);
				}
				break;
			case PROP_XYZ:
			case PROP_XYZT:
				ASSERT(this->data_source);

				// Datasource
				//
				if (this->data_source)
				{
					ASSERT(this->data_source->Get_attribute() == -1);
					status = CGlobal::HDFSerializeData_source(bStoring, loc_id, szDataSource, *this->data_source);
					ASSERT(status >= 0);
				}
				break;
			default:
				ASSERT(FALSE);
				break;
		}
	}
	else
	{

		// prop_type
		//
		hid_t proptype = CGlobal::HDFCreatePropType();
		if (CGlobal::HDFSerializeSafe(bStoring, loc_id, szPropType, proptype, 1, &this->type) >= 0)
		{
  			ASSERT(this->type == PROP_FIXED || this->type == PROP_LINEAR || this->type == PROP_POINTS
				|| this->type == PROP_XYZ || this->type == PROP_XYZT);
		}
		else
		{
			// type (svn versions <= 3040)
			//
			int n;
			status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &n);
			ASSERT(status >= 0);
			switch(n)
			{
			case _LINEAR:
				this->type = PROP_LINEAR;
				break;
			case _FIXED:
				this->type = PROP_FIXED;
				break;
			default:
				ASSERT(FALSE);
				this->type = PROP_FIXED;
				break;
			}
		}
		status = H5Tclose(proptype);
		ASSERT(status >= 0);

		if (this->type == PROP_FIXED || this->type == PROP_LINEAR)
		{
			// v and count_v
			//
			hsize_t count;
			status = CGlobal::HDFSerializeWithSize(bStoring, loc_id, szV, H5T_NATIVE_DOUBLE, count, this->v);
			this->count_v = (int)count;
			ASSERT(status >= 0);
		}

		// mixture
		status = CGlobal::HDFSerializeBool(bStoring, loc_id, szMixture, this->mix);
		if (status >= 0)
		{
			if (this->mix)
			{
				status = CGlobal::HDFSerialize(bStoring, loc_id, szMix1, H5T_NATIVE_DOUBLE, 1, &this->mix1);
				ASSERT(status >= 0);
				status = CGlobal::HDFSerialize(bStoring, loc_id, szMix2, H5T_NATIVE_DOUBLE, 1, &this->mix2);
				ASSERT(status >= 0);
			}
		}
		else
		{
			this->mix = false;
			this->mix1 = -1;
			this->mix2 = -1;
		}

		switch (this->type)
		{
			case PROP_FIXED:
				ASSERT(this->count_v == 1);
				break;
			case PROP_LINEAR:
				ASSERT(this->count_v == 2);

				// coord
				//
				status = CGlobal::HDFSerialize(bStoring, loc_id, szCoord, H5T_NATIVE_CHAR, 1, &this->coord);
				ASSERT(status >= 0);
				ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');

				// dist1 and dist2
				//
				dist[0] = this->dist1;
				dist[1] = this->dist2;
				status = CGlobal::HDFSerialize(bStoring, loc_id, szDist, H5T_NATIVE_DOUBLE, 2, dist);
				this->dist1 = dist[0];
				this->dist2 = dist[1];
				ASSERT(status >= 0);
				break;
			case PROP_POINTS:
				if (this->data_source)
				{
					delete this->data_source;
				}
				this->data_source = new Data_source();

				// Datasource
				//
				if (this->data_source)
				{
					ASSERT(this->data_source->Get_attribute() == -1);
					status = CGlobal::HDFSerializeData_source(bStoring, loc_id, szDataSource, *this->data_source);
					ASSERT(status >= 0);
					//{{ TODO CHECK
					this->data_source->Set_user_points(this->data_source->Get_points());
					//}}
				}
				break;
			case PROP_XYZ:
			case PROP_XYZT:
				if (this->data_source)
				{
					delete this->data_source;
				}
				this->data_source = new Data_source();

				// Datasource
				//
				if (this->data_source)
				{
					ASSERT(this->data_source->Get_attribute() == -1);
					status = CGlobal::HDFSerializeData_source(bStoring, loc_id, szDataSource, *this->data_source);
					ASSERT(status >= 0);
				}
				break;
			default:
				ASSERT(FALSE);
				break;
		}
	}
}

void Cproperty::Serialize(CArchive& ar)
{
	static const char szCproperty[] = "Cproperty";
	static int version = 2;

	CString typeName;
	int ver;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		typeName = szCproperty;
		ar << typeName;

		// store version in case changes need to be made
		ar << version;
		ver = version;
	}
	else
	{
		// read type as string
		//
		ar >> typeName;
		ASSERT(typeName.Compare(szCproperty) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}

	switch (ver)
	{
	case 1:
		{
			// type
			//
			if (ar.IsStoring())
			{
  				ASSERT(this->type == PROP_FIXED || this->type == PROP_LINEAR);
				ar << this->type;
			}
			else
			{
				int i;
				ar >> i;
				this->type = static_cast<PROP_TYPE>(i);
  				ASSERT(this->type == PROP_FIXED || this->type == PROP_LINEAR);
			}

			// count_v
			//
			if (ar.IsStoring())
			{
				ar << this->count_v;
			}
			else
			{
				ar >> this->count_v;
			}

			// v
			//
			if (ar.IsStoring())
			{
				for (int i = 0; i < this->count_v; ++i)
				{
					ar << this->v[i];
				}
			}
			else
			{
				for (int i = 0; i < this->count_v; ++i)
				{
					ar >> this->v[i];
				}
			}

			// coord
			//
			if (ar.IsStoring())
			{
				if (this->type == PROP_LINEAR)
				{
					ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');
					ar << this->coord;
				}
			}
			else
			{
				if (this->type == PROP_LINEAR)
				{
					ar >> this->coord;
					ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');
				}
			}

			// dist1 and dist2
			//
			if (ar.IsStoring())
			{
				if (this->type == PROP_LINEAR)
				{
					ar << this->dist1;
					ar << this->dist2;
				}
			}
			else
			{
				if (this->type == PROP_LINEAR)
				{
					ar >> this->dist1;
					ar >> this->dist2;
				}
			}

			break;
		}
	case 2:
		{
			// Version 2 adds support for PROP_POINTS, PROP_XYZ, PROP_XYZT and MIXTURES

			// type
			//
			if (ar.IsStoring())
			{
				ASSERT(this->type == PROP_FIXED || this->type == PROP_LINEAR || this->type == PROP_POINTS || this->type == PROP_XYZ || this->type == PROP_XYZT);
				ar << this->type;
			}
			else
			{
				int i;
				ar >> i;
				this->type = static_cast<PROP_TYPE>(i);
				ASSERT(this->type == PROP_FIXED || this->type == PROP_LINEAR || this->type == PROP_POINTS || this->type == PROP_XYZ || this->type == PROP_XYZT);
			}

			// count_v
			//
			if (ar.IsStoring())
			{
				ar << this->count_v;
			}
			else
			{
				ar >> this->count_v;
			}

			// v
			//
			if (ar.IsStoring())
			{
				for (int i = 0; i < this->count_v; ++i)
				{
					ar << this->v[i];
				}
			}
			else
			{
				delete [] this->v;
				this->v = new double[this->count_alloc = this->count_v];
				for (int i = 0; i < this->count_v; ++i)
				{
					ar >> this->v[i];
				}
			}

			// coord
			//
			if (ar.IsStoring())
			{
				if (this->type == PROP_LINEAR)
				{
					ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');
					ar << this->coord;
				}
			}
			else
			{
				if (this->type == PROP_LINEAR)
				{
					ar >> this->coord;
					ASSERT(this->coord == 'x' || this->coord == 'y' || this->coord == 'z');
				}
			}

			// icoord (Is this used?)
			//
			if (ar.IsStoring())
			{
				ar << this->icoord;
			}
			else
			{
				ar >> this->icoord;
			}

			// dist1 and dist2
			//
			if (ar.IsStoring())
			{
				if (this->type == PROP_LINEAR)
				{
					ar << this->dist1;
					ar << this->dist2;
				}
			}
			else
			{
				if (this->type == PROP_LINEAR)
				{
					ar >> this->dist1;
					ar >> this->dist2;
				}
			}

			// mix
			//
			if (ar.IsStoring())
			{
				ar << this->mix;
				if (this->mix)
				{
					ar << this->mix1;
					ar << this->mix2;
				}
			}
			else
			{
				ar >> this->mix;
				if (this->mix)
				{
					ar >> this->mix1;
					ar >> this->mix2;
				}
			}

			// data_source (POINTS)
			//
			if (this->type == PROP_POINTS)
			{
				// points
				if (ar.IsStoring())
				{
					ASSERT(this->data_source);
					size_t count = this->data_source->Get_user_points().size();
					ASSERT(count < UINT_MAX);
					ar << (unsigned int) count;
					for (unsigned int i = 0; i < count; ++i)
					{
						Point pt = this->data_source->Get_user_points()[i];
						ar << pt.x();
						ar << pt.y();
						ar << pt.z();
						ar << pt.get_v();
					}
					ASSERT(this->data_source->Get_defined());
					ASSERT(this->data_source->Get_source_type() == Data_source::POINTS);
					ASSERT(this->data_source->Get_user_source_type() == Data_source::POINTS);
				}
				else
				{
					ASSERT(!this->data_source);
					this->data_source = new Data_source;
					std::vector<Point> pts;
					unsigned int count;
					ar >> count;
					pts.resize(count);
					double x, y, z, v;
					for (unsigned int i = 0; i < count; ++i)
					{
						ar >> x;
						ar >> y;
						ar >> z;
						ar >> v;
						pts[i].set_x(x);
						pts[i].set_y(y);
						pts[i].set_z(z);
						pts[i].set_v(v);
					}
					this->data_source->Set_points(pts);
					this->data_source->Set_source_type(Data_source::POINTS);
					this->data_source->Set_user_source_type(Data_source::POINTS);
					this->data_source->Set_defined(true);
				}

				// map or grid
				if (ar.IsStoring())
				{
					PHAST_Transform::COORDINATE_SYSTEM cs = this->data_source->Get_user_coordinate_system();
  					ASSERT(cs == PHAST_Transform::MAP || cs == PHAST_Transform::GRID);
					ar << cs;
				}
				else
				{
					int i;
					ar >> i;
					PHAST_Transform::COORDINATE_SYSTEM cs = static_cast<PHAST_Transform::COORDINATE_SYSTEM>(i);
  					ASSERT(cs == PHAST_Transform::MAP || cs == PHAST_Transform::GRID);
					this->data_source->Set_user_coordinate_system(cs);
				}
			}

			// data_source (XYZ or XYZT)
			//
			if (this->type == PROP_XYZ || this->type == PROP_XYZT)
			{
				// TODO
				// Data_source::DATA_SOURCE_TYPE
				Data_source::DATA_SOURCE_TYPE dt = Data_source::XYZ;
				if (this->type == PROP_XYZT)
				{
					dt = Data_source::XYZT;
				}

				// filename
				std::string file_name;
				if (ar.IsStoring())
				{
					ASSERT(this->data_source);
					ASSERT(this->data_source->Get_defined());
					ASSERT(this->data_source->Get_source_type() == dt);
					ASSERT(this->data_source->Get_user_source_type() == dt);
					ASSERT(this->data_source->Get_file_name().size() > 0);
					CString str(this->data_source->Get_file_name().c_str());
					ar << str;
				}
				else
				{
					ASSERT(!this->data_source);
					this->data_source = new Data_source;
					this->data_source->Set_defined(true);
					this->data_source->Set_source_type(dt);
					this->data_source->Set_user_source_type(dt);
					CString str;
					ar >> str;
					file_name = str;
					this->data_source->Set_file_name(file_name);
				}

				// map or grid
				if (ar.IsStoring())
				{
					PHAST_Transform::COORDINATE_SYSTEM cs = this->data_source->Get_user_coordinate_system();
  					ASSERT(cs == PHAST_Transform::MAP || cs == PHAST_Transform::GRID);
					ar << cs;
				}
				else
				{
					int i;
					ar >> i;
					PHAST_Transform::COORDINATE_SYSTEM cs = static_cast<PHAST_Transform::COORDINATE_SYSTEM>(i);
  					ASSERT(cs == PHAST_Transform::MAP || cs == PHAST_Transform::GRID);
					this->data_source->Set_user_coordinate_system(cs);
					ASSERT(!this->data_source->Get_filedata());
					ASSERT(this->data_source->Get_attribute() == -1); // should be -1 for XYZ and XYZT
					this->data_source->Set_filedata(FakeFiledata::New(file_name, cs, this->data_source->Get_attribute()));
					ASSERT(this->data_source->Get_filedata());
					this->data_source->Get_filedata()->Set_coordinate_system(cs);
				}
			}
		}
	}

	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		typeName = szCproperty;
		ar << typeName;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> typeName;
		ASSERT(typeName.Compare(szCproperty) == 0);

		// read version in case changes need to be made
		ar >> ver;
		ASSERT(ver == version);
	}
}


void Cproperty::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, LPCTSTR heading)const
{
	CString format;
	const char *coor_name[] = { "MAP", "GRID", "NONE" };
	size_t cs;

	switch (this->type)
	{
		case PROP_UNDEFINED:
			TRACE("Warning attempted to insert undefined property\n");
			return;
			break;
		case PROP_FIXED:
			if (this->mix)
			{
				format.Format("%s MIXTURE %g %g %g", heading, this->mix1, this->mix2, this->v[0]);
			}
			else
			{
				format.Format("%s %g", heading, this->v[0]);
			}
			break;
		case PROP_LINEAR:
			if (this->mix)
			{
				format.Format("%s MIXTURE %g %g %c %g %g %g %g",
					heading,
					this->mix1,
					this->mix2,
					::toupper(this->coord),
					this->v[0],
					this->dist1,
					this->v[1],
					this->dist2
					);
			}
			else
			{
				format.Format("%s %c %g %g %g %g",
					heading,
					::toupper(this->coord),
					this->v[0],
					this->dist1,
					this->v[1],
					this->dist2
					);
			}
			break;
		case PROP_POINTS:
			ASSERT(this->data_source);
			cs = this->data_source->Get_user_coordinate_system();
			ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);
			if (this->mix)
			{
				format.Format("%s MIXTURE %g %g POINTS %s", heading, this->mix1, this->mix2, coor_name[cs]);
			}
			else
			{
				format.Format("%s POINTS %s", heading, coor_name[cs]);
			}
			break;
		case PROP_XYZ:
			ASSERT(this->data_source);
			cs = this->data_source->Get_user_coordinate_system();
			ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);
			if (this->mix)
			{
				format.Format("%s MIXTURE %g %g XYZ %s %s", heading, this->mix1, this->mix2, coor_name[cs], this->data_source->Get_file_name().c_str());
			}
			else
			{
				format.Format("%s XYZ %s %s", heading, coor_name[cs], this->data_source->Get_file_name().c_str());
			}
			break;
		case PROP_XYZT:
			ASSERT(this->data_source);
			cs = this->data_source->Get_user_coordinate_system();
			ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);
			if (this->mix)
			{
				format.Format("%s MIXTURE %g %g XYZT %s %s", heading, this->mix1, this->mix2, coor_name[cs], this->data_source->Get_file_name().c_str());
			}
			else
			{
				format.Format("%s XYZT %s %s", heading, coor_name[cs], this->data_source->Get_file_name().c_str());
			}
			break;
		default:
			ASSERT(FALSE);
	}
	if (!format.IsEmpty())
	{
		HTREEITEM item = pTreeCtrl->InsertItem(format, htiParent);
		if (this->type == PROP_POINTS)
		{
			std::vector<Point>::const_iterator citer = this->data_source->Get_user_points().begin();
			for (; citer != this->data_source->Get_user_points().end(); ++citer)
			{
				format.Format("%g %g %g %g", citer->x(), citer->y(), citer->z(), citer->get_v());
				pTreeCtrl->InsertItem(format, item);
			}
		}
	}
}

std::ostream& operator<< (std::ostream &os, const Cproperty &a)
{
	const char *coor_name[] = { " MAP ", " GRID ", " NONE " };
	size_t cs;

	if (a.mix && (a.type != PROP_UNDEFINED))
	{
		os << "MIXTURE  " << a.mix1 << "  " << a.mix2 << "  ";
	}
	switch (a.type)
	{
		case PROP_UNDEFINED:
			ASSERT(FALSE);
			os << "#UNDEFINED property\n";
			break;
		case PROP_FIXED:
			os << a.v[0] << "\n";
			break;
		case PROP_LINEAR:
			os << (char)::toupper(a.coord)
				<< " " << a.v[0]
				<< " " << a.dist1
				<< " " << a.v[1]
				<< " " << a.dist2
				<< "\n";
			break;
		case PROP_POINTS:
			ASSERT(a.data_source);
			cs = a.data_source->Get_user_coordinate_system();
			ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);
			os << "POINTS" << coor_name[cs] << std::endl;
			{
				std::vector < Point >::const_iterator citer = a.data_source->Get_user_points().begin();
				for (; citer != a.data_source->Get_user_points().end(); ++citer)
				{
					os << "\t\t\t\t" << citer->x() << " " << citer->y()
						<< " " << citer->z() << "    " << citer->get_v() << std::endl;
				}
				os << "\t\t\t\tend_points" << std::endl;
			}
			break;
		case PROP_XYZ:
			ASSERT(a.data_source);
			cs = a.data_source->Get_user_coordinate_system();
			ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);
			os << "XYZ" << coor_name[cs] << a.data_source->Get_file_name().c_str() << std::endl;
			break;
		case PROP_XYZT:
			ASSERT(a.data_source);
			cs = a.data_source->Get_user_coordinate_system();
			ASSERT(cs >= PHAST_Transform::MAP && cs <= PHAST_Transform::NONE);
			os << "XYZ" << coor_name[cs] << a.data_source->Get_file_name().c_str() << std::endl;
			break;
		default:
			ASSERT(FALSE);
			os << "#UNSUPPORTED property type\n";
			break;
	}
	return os;
}

bool operator==(const property& lhs, const property& rhs)
{
	if (lhs.type == rhs.type)
	{
		// mixture
		if (lhs.mix != rhs.mix) return false;

		if (lhs.mix)
		{
			if (lhs.mix1 != rhs.mix1) return false;
			if (lhs.mix2 != rhs.mix2) return false;
		}

		switch (lhs.type)
		{
		case PROP_UNDEFINED:
			break;
		case PROP_FIXED:
			if (lhs.v[0] != rhs.v[0])
			{
				return false;
			}
			break;
		case PROP_LINEAR:
			if (lhs.coord != rhs.coord) return false;
			if (lhs.v[0]  != rhs.v[0])  return false;
			if (lhs.dist1 != rhs.dist1) return false;
			if (lhs.v[1]  != rhs.v[1])  return false;
			if (lhs.dist2 != rhs.dist2) return false;
			break;
		case PROP_POINTS:
			if (lhs.data_source->Get_points().size() == rhs.data_source->Get_points().size())
			{
				for (size_t i = 0; i < lhs.data_source->Get_points().size(); ++i)
				{
					if (lhs.data_source->Get_points()[i] != rhs.data_source->Get_points()[i])
					{
						return false;
					}
				}
				if (lhs.data_source->Get_user_coordinate_system() != rhs.data_source->Get_user_coordinate_system())
				{
					return false;
				}
			}
			break;
		case PROP_XYZ:
			if (lhs.data_source->Get_file_name() != rhs.data_source->Get_file_name())
			{
				return false;
			}
			if (lhs.data_source->Get_user_coordinate_system() != rhs.data_source->Get_user_coordinate_system())
			{
				return false;
			}
			break;
		case PROP_XYZT:
			if (lhs.data_source->Get_file_name() != rhs.data_source->Get_file_name())
			{
				return false;
			}
			if (lhs.data_source->Get_user_coordinate_system() != rhs.data_source->Get_user_coordinate_system())
			{
				return false;
			}
			break;
		default:
			ASSERT(FALSE);
			return false;
			break;
		}
		return true;
	}
	return false;
}