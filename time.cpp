#include "StdAfx.h"
#include "time.h"

#include <string>
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


Ctime::Ctime(void)
{
	this->type          = UNDEFINED;
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
		ASSERT(t.type == UNITS);
		ASSERT(this->type == UNITS);
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
		ASSERT(t.type == UNITS);
		ASSERT(this->type == UNITS);
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
		if (src.type == STEP)
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

	std::string s =  CGlobal::GetStdTimeUnits(input);
	delete[] this->input;
	this->input = new char[s.length() + 1];
	::strcpy(this->input, s.c_str());

	int n = ::units_conversion(this->input, "s", &(this->input_to_si), FALSE);
	if (n == OK)
	{
		this->type = UNITS;
	}
	else
	{
		delete[] this->input;
		this->input = NULL;
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
	static const char szValue[] = "value";
	static const char szType[]  = "type";
	static const char szInput[] = "input";

	herr_t status;

	// value
	status = CGlobal::HDFSerialize(bStoring, loc_id, szValue, H5T_NATIVE_DOUBLE, 1, &this->value);
	ASSERT(status >= 0);

	// type
	status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->type);
	ASSERT(status >= 0);
	switch (this->type)
	{
		case UNITS:
			status = CGlobal::HDFSerializeStringOrNull(bStoring, loc_id, szInput, &this->input);
			ASSERT(status >= 0 || this->input == NULL);
			break;
		case STEP:
			// nothing to do ???
			break;
		case UNDEFINED:
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
		this->SetValue(this->value);
		switch (this->type)
		{
			case UNITS:
				this->SetInput(this->input);
				break;
			case STEP:
				// nothing to do ???
				break;
			case UNDEFINED:
				break;
			default:
				ASSERT(FALSE);
		}
	}
}
