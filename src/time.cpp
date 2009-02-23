#include "StdAfx.h"
#include "time.h"

#include <string>
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


Ctime::Ctime(void)
{
	this->type          = TT_UNDEFINED;
	this->value         = 0.0;
	this->value_defined = FALSE;
	this->input         = NULL;
	this->input_to_si   = 1.0;
	this->input_to_user = 1.0;
}

Ctime::~Ctime(void)
{
	delete[] this->input;
}

Ctime::Ctime(const struct time& t)
{
#ifdef _DEBUG
	this->input = 0;
#endif
	Ctime::Copy(*this, t);
	//{{{4/21/2005 9:52:09 PM}
	if (t.input && ::strlen(t.input))
	{
		ASSERT(t.type == TT_UNITS || t.type == TT_STEP);
		this->SetInput(t.input);
	}
	//}}{4/21/2005 9:52:09 PM}
}

Ctime::Ctime(const Ctime& t)
{
#ifdef _DEBUG
	this->input = 0;
#endif
	Ctime::Copy(*this, t);
	//{{{4/21/2005 9:52:09 PM}
	if (t.input && ::strlen(t.input))
	{
		ASSERT(t.type == TT_UNITS || t.type == TT_STEP);
		this->SetInput(t.input);
	}
	//}}{4/21/2005 9:52:09 PM}
}

void Ctime::Copy(struct time& dest, const struct time& src)
{
	dest.type          = src.type;
	dest.value         = src.value;
	dest.value_defined = src.value_defined;
	if (src.input)
	{
		if (src.type == TT_STEP)
		{
			ASSERT(dest.input == 0);
			dest.input = new char[::strlen(src.input) + 1];
			::strcpy(dest.input, src.input);
		}
		else
		{
			std::string s =  CGlobal::GetStdTimeUnits(src.input);
			ASSERT(dest.input == 0);
			dest.input = new char[s.length() + 1];
			::strcpy(dest.input, s.c_str());
		}
	}
	else
	{
		ASSERT(dest.input == 0);
		dest.input = NULL;
	}
	dest.input_to_si   = src.input_to_si;
	dest.input_to_user = src.input_to_user;
}

Ctime& Ctime::operator=(const Ctime& rhs)
{
	if (this != &rhs)
	{
		delete[] this->input;
		this->input = 0;
		Ctime::Copy(*this, rhs);
	}
	return *this;
}

Ctime& Ctime::operator=(const struct time& rhs)
{
	if (this != &rhs)
	{
		delete[] this->input;
		this->input = NULL;
		Ctime::Copy(*this, rhs);
	}
	return *this;
}

bool Ctime::operator<(const Ctime& rhs)const
{
	return ((this->value * this->input_to_si) < (rhs.value * rhs.input_to_si));
}

int Ctime::SetInput(const char* input)
{
	if (!input) return ERROR;
	int n = ERROR;

	if (::strstr(input, "step") == input)
	{
		delete[] this->input;
		this->input = new char[::strlen(input) + 1];
		::strcpy(this->input, input);
		this->type = TT_STEP;
		n = OK;
	}
	else
	{
		std::string s =  CGlobal::GetStdTimeUnits(input);
		delete[] this->input;
		this->input = new char[s.length() + 1];
		::strcpy(this->input, s.c_str());

		n = ::units_conversion(this->input, "s", &(this->input_to_si), FALSE);
		if (n == OK)
		{
			this->type = TT_UNITS;
		}
		else
		{
			delete[] this->input;
			this->input = NULL;
		}
	}
	return n;
}

void Ctime::SetValue(double dValue)
{
	this->value_defined = TRUE;
	this->value         = dValue;
}

void Ctime::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szValue[]        = "value";
	static const char szType[]         = "type";
	static const char szInput[]        = "input";
	static const char szValueDefined[] = "value_defined";

	herr_t status;

	// value_defined
	status = CGlobal::HDFSerialize(bStoring, loc_id, szValueDefined, H5T_NATIVE_INT, 1, &this->value_defined);
	ASSERT(status >= 0);

	// value
	status = CGlobal::HDFSerialize(bStoring, loc_id, szValue, H5T_NATIVE_DOUBLE, 1, &this->value);
	ASSERT(status >= 0);

	// type
	status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->type);
	ASSERT(status >= 0);
	switch (this->type)
	{
		case TT_UNITS:
			status = CGlobal::HDFSerializeStringOrNull(bStoring, loc_id, szInput, &this->input);
			ASSERT(status >= 0 || this->input == NULL);
			break;
		case TT_STEP:
			status = CGlobal::HDFSerializeStringOrNull(bStoring, loc_id, szInput, &this->input);
			ASSERT(status >= 0 || this->input == NULL);
			break;
		case TT_UNDEFINED:
			ASSERT(this->input == NULL);
			break;
		default:
			ASSERT(FALSE);
	}

	if (bStoring)
	{
	}
	else 
	{
		switch (this->type)
		{
			case TT_UNITS:
				this->SetInput(this->input);
				break;
			case TT_STEP:
				// nothing to do ???
				break;
			case TT_UNDEFINED:
				break;
			default:
				ASSERT(FALSE);
		}
	}
}

void Ctime::Serialize(CArchive& ar)
{
	static const char szValue[]        = "value";
	static const char szType[]         = "type";
	static const char szInput[]        = "input";
	static const char szValueDefined[] = "value_defined";

	// value_defined
	if (ar.IsStoring())
	{
		ar << this->value_defined;
	}
	else
	{
		ar >> this->value_defined;
	}

	// value
	if (ar.IsStoring())
	{
		ar << this->value;
	}
	else
	{
		ar >> this->value;
	}

	// type
	if (ar.IsStoring())
	{
		ar << this->type;
	}
	else
	{
#ifdef USE_TIME_TYPE_ENUM
		int i;
		ar >> i;
		this->type = static_cast<TIME_TYPE>(i);
#else
		ar >> this->type;
#endif
	}

	CString temp;
	switch (this->type)
	{
		case TT_UNITS:	case TT_STEP:
			if (ar.IsStoring())
			{
				if (this->input)
				{
					temp = this->input;
				}
				else
				{
					temp.Empty();
				}
				ar << temp;
			}
			else
			{
				ar >> temp;
				if (this->input) delete[] this->input;
				if (temp.IsEmpty())
				{
					this->input = NULL;
				}
				else
				{
					this->input = new char[temp.GetLength() + 1];
					::strcpy(this->input, temp);
				}
			}
			break;
		case TT_UNDEFINED:
			ASSERT(this->input == NULL);
			break;
		default:
			ASSERT(FALSE);
	}

	if (ar.IsStoring())
	{
		// nothing else to do
	}
	else 
	{
		switch (this->type)
		{
			case TT_UNITS:
				this->SetInput(this->input);
				break;
			case TT_STEP:
				// nothing to do ???
				break;
			case TT_UNDEFINED:
				break;
			default:
				ASSERT(FALSE);
		}
	}
}
