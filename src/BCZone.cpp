#include "StdAfx.h"
#include "BCZone.h"

CBCZone::CBCZone(void)
{
	this->InternalInit();
}

CBCZone::~CBCZone(void)
{
	this->InternalDelete();
}

void CBCZone::InternalInit(void)
{
	this->bc_type          = UNDEFINED;
	this->new_def          = TRUE;

	this->bc_head          = 0;

	this->bc_flux          = 0;

	this->bc_k             = 0;
	this->bc_thick         = 0;

	this->face             = -1;
	this->face_defined     = FALSE;

	this->bc_solution_type = UNDEFINED;
	this->bc_solution      = 0;
}

template<typename T> 
void CBCZone::InternalCopy(const T& src)
{
	// bc_type
	this->bc_type = src.bc_type;

	// new_def
	this->new_def = src.new_def;

	// bc_head time_series
	if (src.bc_head)
	{
		this->bc_head = new CTimeSeries<Cproperty>();
		(*this->bc_head) = (*src.bc_head);
	}
	else
	{
		this->bc_head = 0;
	}

	// bc_flux time_series
	if (src.bc_flux)
	{
		this->bc_flux = new CTimeSeries<Cproperty>();
		(*this->bc_flux) = (*src.bc_flux);
	}
	else
	{
		this->bc_flux = 0;
	}

	// bc_k property
	if (src.bc_k)
	{
		this->bc_k = new Cproperty;
		(*this->bc_k) = (*src.bc_k);
	}
	else
	{
		this->bc_k = 0;
	}

	// bc_thick property
	if (src.bc_thick)
	{
		this->bc_thick = new Cproperty;
		(*this->bc_thick) = (*src.bc_thick);
	}
	else
	{
		this->bc_thick = 0;
	}

	// face
	this->face = src.face;

	// face_defined
	this->face_defined = src.face_defined;

	// bc_solution_type
	this->bc_solution_type = src.bc_solution_type;

	// bc_solution time_series
	if (src.bc_solution)
	{
		this->bc_solution = new CTimeSeries<Cproperty>();
		(*this->bc_solution) = (*src.bc_solution);
	}
	else
	{
		this->bc_solution = 0;
	}
}

void CBCZone::InternalDelete(void)
{
	delete this->bc_head;
	delete this->bc_flux;
	delete this->bc_k;
	delete this->bc_thick;
	delete this->bc_solution;
}

CBCZone::CBCZone(const struct bc& src) // copy ctor
{
	// zone
	assert(src.zone);
	if (src.zone)
	{
		this->CZone::InternalCopy(*src.zone);
	}

	this->InternalCopy(src);
}

CBCZone::CBCZone(const CBCZone& src) // copy ctor
{
	// zone
	this->CZone::InternalCopy(src);

	this->InternalCopy(src);
}

CBCZone& CBCZone::operator=(const struct bc& rhs) // copy assignment
{
	this->InternalDelete();
	this->InternalCopy(rhs);
	return *this;
}

CBCZone& CBCZone::operator=(const CBCZone& rhs) // copy assignment
{
	if (this != &rhs)
	{
		this->InternalDelete();
		this->InternalCopy(rhs);
	}
	return *this;
}

#ifdef _DEBUG
void CBCZone::AssertValid(int nSim)const
{
	ASSERT(nSim > 0);
	ASSERT(this);

	// zone
	if (this->zone_defined == TRUE)
	{
		this->CZone::AssertValid();
	}

	// face_defined face

	switch (this->bc_type)
	{
	case FLUX: // bc_solution bc_flux
		ASSERT(this->face_defined == TRUE);
		ASSERT(this->face == 0 || this->face == 1 || this->face == 2);
		ASSERT(this->bc_head == 0);
		ASSERT(this->bc_k == 0);
		ASSERT(this->bc_thick == 0);
		if (this->bc_solution)
		{
			ASSERT(this->bc_solution_type == ASSOCIATED);
			this->bc_solution->AssertValid();
		}
		else
		{
			if (nSim == 1)
			{
				ASSERT(this->bc_solution_type == UNDEFINED);
			}
		}
		if (this->bc_flux)
		{
			this->bc_flux->AssertValid();
		}
		break;

	case LEAKY: // bc_thick bc_head bc_k bc_solution
		ASSERT(this->face_defined == TRUE);
		ASSERT(this->face == 0 || this->face == 1 || this->face == 2);
		ASSERT(this->bc_flux == 0);
		if (this->bc_thick)
		{
			this->bc_thick->AssertValid();
		}
		if (this->bc_head)
		{
			this->bc_head->AssertValid();
		}
		if (this->bc_k)
		{
			static_cast<Cproperty*>(this->bc_k)->AssertValid();
		}
		if (this->bc_solution)
		{
			ASSERT(this->bc_solution_type == ASSOCIATED);
			this->bc_solution->AssertValid();
		}
		else
		{
			if (nSim == 1)
			{
				ASSERT(this->bc_solution_type == UNDEFINED);
			}
		}
		break;

	case SPECIFIED: // bc_head bc_solution
		ASSERT(this->face_defined == FALSE);
		ASSERT(this->bc_flux == 0);
		ASSERT(this->bc_thick == 0);
		ASSERT(this->bc_k == 0);
		if (this->bc_head)
		{
			this->bc_head->AssertValid();
		}
		if (this->bc_solution)
		{
			ASSERT(this->bc_solution_type == ASSOCIATED || this->bc_solution_type == FIXED);
			this->bc_solution->AssertValid();
		}
		else
		{
			ASSERT(this->bc_solution_type == ASSOCIATED || this->bc_solution_type == FIXED || this->bc_solution_type == UNDEFINED || nSim > 1 );
		}
		break;

	case UNDEFINED:
		break;

	default:
		ASSERT(FALSE); // invalid type
	}
}
#endif

void CBCZone::ClearProperties(void)
{
	this->InternalDelete();

	this->bc_head     = 0;
	this->bc_flux     = 0;
	this->bc_k        = 0;
	this->bc_thick    = 0;
	this->bc_solution = 0;
}

bool CBCZone::ContainsProperties(void)const
{
	switch (this->bc_type) {
		case FLUX: // bc_solution bc_flux
			if (this->bc_solution || this->bc_flux) {
				return true;
			}
			else {
				return false;
			}
			break;

		case LEAKY: // bc_thick bc_head bc_k bc_solution
			if (this->bc_thick || this->bc_head || this->bc_k || this->bc_solution) {
				return true;
			}
			else {
				return false;
			}
			break;

		case SPECIFIED: // bc_head bc_solution
			if (this->bc_head || this->bc_solution) {
				return true;
			}
			else {
				return false;
			}
			break;
		default:
			ASSERT(FALSE); // invalid type
	}
	return false;
}

std::ostream& operator<< (std::ostream &os, const CBCZone &a)
{
	if (!a.ContainsProperties()) {
#if defined(_DEBUG)
		switch (a.bc_type) {
			case SPECIFIED:
				os << "#SPECIFIED_VALUE_BC\n";
				break;
			case FLUX:
				os << "#FLUX_BC\n";
				break;
			case LEAKY:
				os << "#LEAKY_BC\n";
				break;
			default:
				os << "#BAD BC\n";
				return os;
				break;
		}
		os << "#";
		os << static_cast<CZone>(a);
		os << "#(EMPTY)\n";
#endif
		return os;
	}

	switch (a.bc_type)
	{
		case SPECIFIED:
			os << "SPECIFIED_HEAD_BC\n";
			break;
		case FLUX:
			os << "FLUX_BC\n";
			break;
		case LEAKY:
			os << "LEAKY_BC\n";
			break;
		default:
			os << "#BAD BC\n";
			return os;
			break;
	}

	os << static_cast<CZone>(a);

	std::map<Ctime, Cproperty>::const_iterator iter;

	switch (a.bc_type)
	{
		case SPECIFIED:
			// head
			if (a.bc_head && !a.bc_head->empty())
			{
				os << "\t\t-head\n";
				for (iter = a.bc_head->begin(); iter != a.bc_head->end(); ++iter)
				{
					os << "\t\t\t";
					os << iter->first.value;
					if (iter->first.input)
					{
						os << " " << iter->first.input;
					}
					os << "     " << iter->second;
				}				
			}
			// associated_solution
			if (a.bc_solution && !a.bc_solution->empty() && a.bc_solution_type == ASSOCIATED)
			{
				os << "\t\t-associated_solution\n";
				for (iter = a.bc_solution->begin(); iter != a.bc_solution->end(); ++iter)
				{
					os << "\t\t\t";
					os << iter->first.value;
					if (iter->first.input)
					{
						os << " " << iter->first.input;
					}
					os << "     " << iter->second;
				}				
			}
			// fixed_solution
			if (a.bc_solution && !a.bc_solution->empty() && a.bc_solution_type == FIXED)
			{
				os << "\t\t-fixed_solution\n";
				for (iter = a.bc_solution->begin(); iter != a.bc_solution->end(); ++iter)
				{
					os << "\t\t\t";
					os << iter->first.value;
					if (iter->first.input)
					{
						os << " " << iter->first.input;
					}
					os << "     " << iter->second;
				}				
			}
			break;

		case FLUX:
			// flux
			if (a.bc_flux && !a.bc_flux->empty())
			{
				os << "\t\t-flux\n";
				for (iter = a.bc_flux->begin(); iter != a.bc_flux->end(); ++iter)
				{
					os << "\t\t\t";
					os << iter->first.value;
					if (iter->first.input)
					{
						os << " " << iter->first.input;
					}
					os << "     " << iter->second;
				}				
			}
			// associated_solution
			if (a.bc_solution && !a.bc_solution->empty() && a.bc_solution_type == ASSOCIATED)
			{
				os << "\t\t-associated_solution\n";
				for (iter = a.bc_solution->begin(); iter != a.bc_solution->end(); ++iter)
				{
					os << "\t\t\t";
					os << iter->first.value;
					if (iter->first.input)
					{
						os << " " << iter->first.input;
					}
					os << "     " << iter->second;
				}				
			}
			// face
			if (a.face_defined == TRUE) {
				switch (a.face) {
					case 0:
						os << "\t\t-face X\n";
						break;
					case 1:
						os << "\t\t-face Y\n";
						break;
					case 2:
						os << "\t\t-face Z\n";
						break;
					default:
						ASSERT(FALSE);
						break;
				}
			}
			break;

		case LEAKY:
			// head
			if (a.bc_head && !a.bc_head->empty())
			{
				os << "\t\t-head\n";
				for (iter = a.bc_head->begin(); iter != a.bc_head->end(); ++iter)
				{
					os << "\t\t\t";
					os << iter->first.value;
					if (iter->first.input)
					{
						os << " " << iter->first.input;
					}
					os << "     " << iter->second;
				}				
			}
			// associated_solution
			if (a.bc_solution && !a.bc_solution->empty())
			{
				os << "\t\t-associated_solution\n";
				for (iter = a.bc_solution->begin(); iter != a.bc_solution->end(); ++iter)
				{
					os << "\t\t\t";
					os << iter->first.value;
					if (iter->first.input)
					{
						os << " " << iter->first.input;
					}
					os << "     " << iter->second;
				}				
			}
			// thickness
			if (a.bc_thick && a.bc_thick->type != UNDEFINED)
			{
				os << "\t\t-thickness  " << (*a.bc_thick);
			}
			// hydraulic_conductivity
			if (a.bc_k && a.bc_k->type != UNDEFINED)
			{
				os << "\t\t-hydraulic_conductivity  " << (*a.bc_k);
			}
			// face
			if (a.face_defined == TRUE)
			{
				switch (a.face)
				{
					case 0:
						os << "\t\t-face X\n";
						break;
					case 1:
						os << "\t\t-face Y\n";
						break;
					case 2:
						os << "\t\t-face Z\n";
						break;
					default:
						ASSERT(FALSE);
						break;
				}
			}
			break;

		default:
			ASSERT(FALSE);
			break;
	}

	return os;
}
