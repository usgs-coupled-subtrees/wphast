#include "StdAfx.h"
#include "BC.h"

#include "Zone.h"
#include "property.h"
#include "Global.h"
#include "srcinput/Cube.h"
#include "srcinput/BC_info.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLIPFORMAT CBC::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CBC:2"));

CBC::CBC() // ctor
{
	this->InternalInit();
}

CBC::~CBC() // dtor
{
	this->InternalDelete();
}

void CBC::InternalCopy(const struct BC& src)
{
	this->polyh = 0;
	if (src.polyh)
	{
		ASSERT(::AfxIsValidAddress(src.polyh, sizeof(Polyhedron)));
		this->polyh = src.polyh->clone();
	}

	// mask
	this->mask = 0;

	// bc_type
	this->bc_type = src.bc_type;

	// bc_head
	this->bc_head = 0;

	// bc_flux
	this->bc_flux = 0;

	// current_bc_flux
	// this->current_bc_flux = 0;

	// bc_k
	this->bc_k = 0;
	Cproperty::CopyProperty(&this->bc_k, src.bc_k);

	// bc_thick
	this->bc_thick = 0;
	Cproperty::CopyProperty(&this->bc_thick, src.bc_thick);

	// face
	this->face = src.face;

	// cell_face
	this->cell_face = src.cell_face;

	// face_defined
	this->face_defined = src.face_defined;

	// bc_solution_type
	this->bc_solution_type = src.bc_solution_type;

	// bc_solution
	this->bc_solution = 0;

	// current_bc_solution
	//this->current_bc_solution = 0;

	// current_bc_head
	//this->current_bc_head = 0;
}

void CBC::InternalDelete(void)
{
	delete this->polyh;
	delete static_cast<Cproperty*>(this->mask);
	this->m_bc_head.clear();
	this->m_bc_flux.clear();
	delete static_cast<Cproperty*>(this->bc_k);
	delete static_cast<Cproperty*>(this->bc_thick);
	this->m_bc_solution.clear();
}

void CBC::InternalInit(void)
{
	this->polyh            = 0;
	this->mask             = 0;

	// boundary conditions
	this->bc_type          = BC_info::BC_UNDEFINED;
	this->bc_head          = 0;
	this->bc_flux          = 0;
	this->current_bc_flux  = 0;
	this->bc_k             = 0;
	this->bc_thick         = 0;
	this->face             = -1;
	this->cell_face        = CF_UNKNOWN;
	this->face_defined     = FALSE;
	this->bc_solution_type = ST_UNDEFINED;
	this->bc_solution      = 0;
	this->current_bc_solution = 0;
	this->current_bc_head     = 0;
	ASSERT(this->m_bc_head.empty());
	ASSERT(this->m_bc_flux.empty());
	ASSERT(this->m_bc_solution.empty());
}

CBC::CBC(const struct BC& src) // copy ctor
{
	this->InternalCopy(src);

	//
	// copy time series
	//

	ASSERT(this->bc_head == NULL);
	ASSERT(this->bc_flux == NULL);
	ASSERT(this->bc_solution == NULL);

	ASSERT(this->m_bc_head.empty());
	ASSERT(this->m_bc_flux.empty());
	ASSERT(this->m_bc_solution.empty());

	if (src.bc_head)
	{
		this->m_bc_head = (*src.bc_head);
	}
	if (src.bc_flux)
	{
		this->m_bc_flux = (*src.bc_flux);
	}
	if (src.bc_solution)
	{
		this->m_bc_solution = (*src.bc_solution);
	}
}

CBC::CBC(const CBC& src) // copy ctor
{
	this->InternalCopy(src);

	//
	// copy time series
	//
	ASSERT(this->bc_head == NULL);
	ASSERT(this->bc_flux == NULL);
	ASSERT(this->bc_solution == NULL);

	ASSERT(this->m_bc_head.empty());
	ASSERT(this->m_bc_flux.empty());
	ASSERT(this->m_bc_solution.empty());

	this->m_bc_head     = src.m_bc_head;
	this->m_bc_flux     = src.m_bc_flux;
	this->m_bc_solution = src.m_bc_solution;
}

CBC& CBC::operator=(const CBC& rhs) // copy assignment
{
	if (this != &rhs) {
		this->InternalDelete();
		this->InternalCopy(rhs);
		//
		// copy time series
		//
		ASSERT(this->bc_head     == NULL);
		ASSERT(this->bc_flux     == NULL);
		ASSERT(this->bc_solution == NULL);

		ASSERT(this->m_bc_head.empty());
		ASSERT(this->m_bc_flux.empty());
		ASSERT(this->m_bc_solution.empty());

		this->m_bc_head     = rhs.m_bc_head;
		this->m_bc_flux     = rhs.m_bc_flux;
		this->m_bc_solution = rhs.m_bc_solution;
	}
	return *this;
}

#ifdef _DEBUG
void CBC::AssertValid(int nSim)const
{
	ASSERT(nSim > 0);
	ASSERT(this);

	ASSERT(this->bc_flux     == NULL);
	ASSERT(this->bc_head     == NULL);
	ASSERT(this->bc_solution == NULL);

	// face_defined face

	switch (this->bc_type)
	{
	case BC_info::BC_FLUX: // bc_solution bc_flux
		ASSERT(this->face_defined == TRUE);
		ASSERT(this->face == 0 || this->face == 1 || this->face == 2);
		ASSERT(this->bc_head == 0);
		ASSERT(this->bc_k == 0);
		ASSERT(this->bc_thick == 0);
		ASSERT(this->bc_solution == NULL);
		if (this->m_bc_solution.size())
		{
			ASSERT(this->bc_solution_type == ST_ASSOCIATED);
			CTimeSeries<Cproperty>::const_iterator iter = this->m_bc_solution.begin();
			for (; iter != this->m_bc_solution.end(); ++iter)
			{
				(*iter).second.AssertValid();
			}
		}
		else
		{
			if (nSim == 1)
			{
				ASSERT(this->bc_solution_type == ST_UNDEFINED);
			}
		}
		ASSERT(this->bc_flux == NULL);
		if (this->m_bc_flux.size())
		{
			CTimeSeries<Cproperty>::const_iterator iter = this->m_bc_flux.begin();
			for (; iter != this->m_bc_flux.end(); ++iter)
			{
				(*iter).second.AssertValid();
			}
		}
		break;
	case BC_info::BC_LEAKY: // bc_thick bc_head bc_k bc_solution
		ASSERT(this->face_defined == TRUE);
		ASSERT(this->face == 0 || this->face == 1 || this->face == 2);
		ASSERT(this->bc_flux == 0);
		if (this->bc_thick)
		{
			static_cast<Cproperty*>(this->bc_thick)->AssertValid();
		}
		ASSERT(this->bc_flux == NULL);
		if (this->m_bc_head.size())
		{
			CTimeSeries<Cproperty>::const_iterator iter = this->m_bc_flux.begin();
			for (; iter != this->m_bc_flux.end(); ++iter)
			{
				(*iter).second.AssertValid();
			}
		}
		if (this->bc_k)
		{
			static_cast<Cproperty*>(this->bc_k)->AssertValid();
		}

		ASSERT(this->bc_solution == NULL);
		if (this->m_bc_solution.size())
		{
			ASSERT(this->bc_solution_type == ST_ASSOCIATED);
			CTimeSeries<Cproperty>::const_iterator iter = this->m_bc_solution.begin();
			for (; iter != this->m_bc_solution.end(); ++iter)
			{
				(*iter).second.AssertValid();
			}
		}
		else
		{
			if (nSim == 1)
			{
				ASSERT(this->bc_solution_type == ST_UNDEFINED);
			}
		}
		break;

	case BC_info::BC_SPECIFIED: // bc_head bc_solution
		if (this->face_defined)
		{
			ASSERT(this->cell_face == CF_X || this->cell_face == CF_Y || this->cell_face == CF_Z || this->cell_face == CF_ALL);
			ASSERT(this->face == 0 || this->face == 1 || this->face == 2 || this->face == 11);
		}
		else
		{
			ASSERT(this->cell_face == CF_UNKNOWN);
			ASSERT(this->face == -1);
		}
		ASSERT(this->bc_flux == 0);
		ASSERT(this->bc_thick == 0);
		ASSERT(this->bc_k == 0);
		if (this->m_bc_head.size())
		{
			CTimeSeries<Cproperty>::const_iterator iter = this->m_bc_head.begin();
			for (; iter != this->m_bc_head.end(); ++iter)
			{
				(*iter).second.AssertValid();
			}
		}

		if (this->m_bc_solution.size())
		{
			ASSERT(this->bc_solution_type == ST_ASSOCIATED || this->bc_solution_type == ST_FIXED);
			CTimeSeries<Cproperty>::const_iterator iter = this->m_bc_solution.begin();
			for (; iter != this->m_bc_solution.end(); ++iter)
			{
				(*iter).second.AssertValid();
			}
		}
		else
		{
			ASSERT(this->bc_solution_type == ST_ASSOCIATED || this->bc_solution_type == ST_FIXED || this->bc_solution_type == ST_UNDEFINED || nSim > 1 );
		}

		break;

	case BC_info::BC_UNDEFINED:
		break;

	default:

		ASSERT(FALSE); // invalid type
	}
}

void CBC::Dump(CDumpContext& dc)const
{
	dc << "<CBC>\n";

	// bc_type (UNDEFINED, SPECIFIED, FLUX, LEAKY)
	//
	dc << "<bc_type>\n";
	switch (this->bc_type)
	{
	case BC_info::BC_UNDEFINED:
		dc << "UNDEFINED\n";
		break;
	case BC_info::BC_SPECIFIED:
		dc << "SPECIFIED\n";
		break;
	case BC_info::BC_FLUX:
		dc << "FLUX\n";
		break;
	case BC_info::BC_LEAKY:
		dc << "LEAKY\n";
		break;
	default:
		dc << "(bad type)\n";
		break;
	}
	dc << "</bc_type>\n";

	// face
	//
	if (this->face_defined)
	{
		dc << "<face>";
		dc << this->face;
		dc << "</face>\n";
	}
	else
	{
		dc << "<face>UNDEFINED</face>\n";
	}

	// bc_flux
	//
	dc << "<bc_flux>\n";
	if (this->bc_flux)
	{
		///static_cast<Cproperty*>(this->bc_flux)->Dump(dc);
	}
	else
	{
		dc << "(NULL)\n";
	}
	dc << "</bc_flux>\n";

	// bc_head
	//
	dc << "<bc_head>\n";
	if (this->bc_head)
	{
		///static_cast<Cproperty*>(this->bc_head)->Dump(dc);
	}
	else
	{
		dc << "(NULL)\n";
	}
	dc << "</bc_head>\n";

	// bc_k
	//
	dc << "<bc_k>\n";
	if (this->bc_k)
	{
		static_cast<Cproperty*>(this->bc_k)->Dump(dc);
	}
	else
	{
		dc << "(NULL)\n";
	}
	dc << "</bc_k>\n";

	// bc_solution_type (UNDEFINED, FIXED, ASSOCIATED)
	//
	dc << "<bc_solution_type>\n";
	switch (this->bc_solution_type)
	{
		case ST_UNDEFINED:
			dc << "UNDEFINED\n";
			break;
		case ST_FIXED:
			dc << "FIXED\n";
			break;
		case ST_ASSOCIATED:
			dc << "ASSOCIATED\n";
			break;
		default:
			dc << "(bad type)\n";
			break;
	}
	dc << "</bc_solution_type>\n";

	// bc_solution
	//
	dc << "<bc_solution>\n";
	if (this->bc_solution)
	{
		///static_cast<Cproperty*>(this->bc_solution)->Dump(dc);
	}
	else
	{
		dc << "(NULL)\n";
	}
	dc << "</bc_solution>\n";

	// bc_thick
	//
	dc << "<bc_thick>\n";
	if (this->bc_thick)
	{
		static_cast<Cproperty*>(this->bc_thick)->Dump(dc);
	}
	else
	{
		dc << "(NULL)\n";
	}
	dc << "</bc_thick>\n";

	// bc_thick
	//
	dc << "<bc_k>\n";
	if (this->bc_k)
	{
		static_cast<Cproperty*>(this->bc_k)->Dump(dc);
	}
	else
	{
		dc << "(NULL)\n";
	}
	dc << "</bc_k>\n";

	dc << "</CBC>\n";
}
#endif // _DEBUG

void CBC::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szFace[]        = "face";
	static const char szType[]        = "bc_type";
	static const char szFlux[]        = "bc_flux";
	static const char szHead[]        = "bc_head";
	static const char szK[]           = "bc_k";
	static const char szSolution[]    = "bc_solution";
	static const char szThick[]       = "bc_thick";
	static const char szSolType[]     = "bc_solution_type";	
	static const char szCellFace[]    = "cell_face";
	static const char szFaceDefined[] = "face_defined";
	
	herr_t status;

	if (bStoring)
	{
		// bc_type
		status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->bc_type);

		// face_defined
		status = CGlobal::HDFSerialize(bStoring, loc_id, szFaceDefined, H5T_NATIVE_INT, 1, &this->face_defined);

		// face
		status = CGlobal::HDFSerialize(bStoring, loc_id, szFace, H5T_NATIVE_INT, 1, &this->face);

		// cell_face
		hid_t cellface = CGlobal::HDFCreateCellFace();
		status = CGlobal::HDFSerialize(bStoring, loc_id, szCellFace, cellface, 1, &this->cell_face);
		ASSERT(status >= 0);
		status = H5Tclose(cellface);
		ASSERT(status >= 0);

		// bc_solution_type
		status = CGlobal::HDFSerialize(bStoring, loc_id, szSolType, H5T_NATIVE_INT, 1, &this->bc_solution_type);

		// properties
		Cproperty::SerializeCreate(szK,        static_cast<Cproperty*>(this->bc_k),        loc_id);
		Cproperty::SerializeCreate(szThick,    static_cast<Cproperty*>(this->bc_thick),    loc_id);

		// CTimeSeries properties
		CTimeSeries<Cproperty>::SerializeCreate(szFlux,     this->m_bc_flux,     loc_id);
		CTimeSeries<Cproperty>::SerializeCreate(szHead,     this->m_bc_head,     loc_id);
		CTimeSeries<Cproperty>::SerializeCreate(szSolution, this->m_bc_solution, loc_id);

	}
	else
	{
		// bc_type
		status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->bc_type);

		// face_defined
		status = CGlobal::HDFSerialize(bStoring, loc_id, szFaceDefined, H5T_NATIVE_INT, 1, &this->face_defined);

		// face
		status = CGlobal::HDFSerialize(bStoring, loc_id, szFace, H5T_NATIVE_INT, 1, &this->face);

		// cell_face
		hid_t cellface = CGlobal::HDFCreateCellFace();
		status = CGlobal::HDFSerialize(bStoring, loc_id, szCellFace, cellface, 1, &this->cell_face);
		ASSERT(status >= 0);
		status = H5Tclose(cellface);
		ASSERT(status >= 0);

		// bc_solution_type
		status = CGlobal::HDFSerialize(bStoring, loc_id, szSolType, H5T_NATIVE_INT, 1, &this->bc_solution_type);

		// properties
		Cproperty::SerializeOpen(szK,        (Cproperty**)&this->bc_k,        loc_id);
		Cproperty::SerializeOpen(szThick,    (Cproperty**)&this->bc_thick,    loc_id);

		// CTimeSeries properties
		CTimeSeries<Cproperty>::SerializeOpen(szFlux,     this->m_bc_flux,     loc_id);
		CTimeSeries<Cproperty>::SerializeOpen(szHead,     this->m_bc_head,     loc_id);
		CTimeSeries<Cproperty>::SerializeOpen(szSolution, this->m_bc_solution, loc_id);
	}
}

void CBC::Serialize(CArchive& ar)
{
	static const char szCBC[] = "CBC";
	static int version = 2;

	CString type;
	int ver = version;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCBC;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCBC) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}

	CGlobal::Serialize(&(this->polyh), ar);

	// properties
	static const char szType[]     = "bc_type";
	static const char szFace[]     = "face";
	static const char szSolType[]  = "bc_solution_type";
	static const char szK[]        = "bc_k";
	static const char szThick[]    = "bc_thick";
	static const char szFlux[]     = "bc_flux";
	static const char szHead[]     = "bc_head";
	static const char szSolution[] = "bc_solution";


	// bc_type
	if (ar.IsStoring())
	{
		int n = this->bc_type;
		ar << n;
	}
	else
	{
		int n;
		ar >> n;
		this->bc_type = static_cast<BC_info::BC_TYPE>(n);
	}

	if (ver == 2)
	{
		// face_defined
		if (ar.IsStoring())
		{
			ar << this->face_defined;
		}
		else
		{
			ar >> this->face_defined;
		}
	}

	// face
	if (ar.IsStoring())
	{
		ar << this->face;
	}
	else
	{
		ar >> this->face;
	}

	if (ver == 2)
	{
		// cell_face
		if (ar.IsStoring())
		{
			int n = this->cell_face;
			ar << n;
		}
		else
		{
			int n;
			ar >> n;
			this->cell_face = static_cast<Cell_Face>(n);
		}
	}

	// bc_solution_type
	if (ar.IsStoring())
	{
		ar << this->bc_solution_type;
	}
	else
	{
		ar >> this->bc_solution_type;
	}

	// properties
	Cproperty::Serial(ar, szK,     &this->bc_k);
	Cproperty::Serial(ar, szThick, &this->bc_thick);

	// CTimeSeries<> properties
	this->m_bc_flux.Serialize(ar);
	this->m_bc_head.Serialize(ar);
	this->m_bc_solution.Serialize(ar);

	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCBC;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCBC) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}
}

std::ostream& operator<< (std::ostream &os, const CBC &a)
{
	if (!a.ContainsProperties())
	{
#if defined(_DEBUG)
		switch (a.bc_type)
		{
		case BC_info::BC_SPECIFIED:
			os << "#SPECIFIED_VALUE_BC\n";
			break;
		case BC_info::BC_FLUX:
			os << "#FLUX_BC\n";
			break;
		case BC_info::BC_LEAKY:
			os << "#LEAKY_BC\n";
			break;
		default:
			os << "#BAD BC\n";
			return os;
			break;
		}
		os << "#";
		ASSERT(a.polyh && ::AfxIsValidAddress(a.polyh, sizeof(Polyhedron)));
		os << (*a.polyh);
		os << "#(EMPTY)\n";
#endif
		return os;
	}

	switch (a.bc_type)
	{
	case BC_info::BC_SPECIFIED:
		os << "SPECIFIED_HEAD_BC\n";
		break;
	case BC_info::BC_FLUX:
		os << "FLUX_BC\n";
		break;
	case BC_info::BC_LEAKY:
		os << "LEAKY_BC\n";
		break;
	default:
		os << "#BAD BC Type\n";
		return os;
		break;
	}

	ASSERT(a.polyh && ::AfxIsValidAddress(a.polyh, sizeof(Polyhedron)));
	os << (*a.polyh);

	switch (a.bc_type)
	{
	case BC_info::BC_SPECIFIED:
		// exterior_cells_only
		if (a.face_defined)
		{
			switch(a.cell_face)
			{
			case CF_X:
				os << "\t\t-exterior_cells_only    X\n";
				break;
			case CF_Y:
				os << "\t\t-exterior_cells_only    Y\n";
				break;
			case CF_Z:
				os << "\t\t-exterior_cells_only    Z\n";
				break;
			case CF_ALL:
				os << "\t\t-exterior_cells_only    ALL\n";
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}

		// head
		if (a.m_bc_head.size())
		{
			os << "\t\t-head\n";
			CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_head.begin();
			for (; iter != a.m_bc_head.end(); ++iter)
			{
				ASSERT((*iter).second.type != PROP_UNDEFINED);
				if ((*iter).second.type == PROP_UNDEFINED) continue;

				os << "\t\t\t";
				if ((*iter).first.input)
				{
					os << (*iter).first.value << " " << (*iter).first.input;
				}
				else
				{
					os << (*iter).first.value;
				}
				os << "\t";
				os << (*iter).second;
			}
		}

		// associated_solution
		if (a.m_bc_solution.size() && a.bc_solution_type == ST_ASSOCIATED)
		{
			os << "\t\t-associated_solution\n";
			CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
			for (; iter != a.m_bc_solution.end(); ++iter)
			{
				ASSERT((*iter).second.type != PROP_UNDEFINED);
				if ((*iter).second.type == PROP_UNDEFINED) continue;

				os << "\t\t\t";
				if ((*iter).first.input)
				{
					os << (*iter).first.value << " " << (*iter).first.input;
				}
				else
				{
					os << (*iter).first.value;
				}
				os << "\t";
				os << (*iter).second;
			}
		}

		// fixed_solution
		if (a.m_bc_solution.size() && a.bc_solution_type == ST_FIXED)
		{
			os << "\t\t-fixed_solution\n";
			CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
			for (; iter != a.m_bc_solution.end(); ++iter)
			{
				ASSERT((*iter).second.type != PROP_UNDEFINED);
				if ((*iter).second.type == PROP_UNDEFINED) continue;

				os << "\t\t\t";
				if ((*iter).first.input)
				{
					os << (*iter).first.value << " " << (*iter).first.input;
				}
				else
				{
					os << (*iter).first.value;
				}
				os << "\t";
				os << (*iter).second;
			}
		}

		break;

	case BC_info::BC_FLUX:
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

		// associated_solution
		if (a.m_bc_solution.size() && a.bc_solution_type == ST_ASSOCIATED)
		{
			os << "\t\t-associated_solution\n";
			CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
			for (; iter != a.m_bc_solution.end(); ++iter)
			{
				ASSERT((*iter).second.type != PROP_UNDEFINED);
				if ((*iter).second.type == PROP_UNDEFINED) continue;

				os << "\t\t\t";
				if ((*iter).first.input)
				{
					os << (*iter).first.value << " " << (*iter).first.input;
				}
				else
				{
					os << (*iter).first.value;
				}
				os << "\t";
				os << (*iter).second;
			}
		}

		// flux
		if (a.m_bc_flux.size())
		{
			os << "\t\t-flux\n";
			CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_flux.begin();
			for (; iter != a.m_bc_flux.end(); ++iter)
			{
				ASSERT((*iter).second.type != PROP_UNDEFINED);
				if ((*iter).second.type == PROP_UNDEFINED) continue;

				os << "\t\t\t";
				if ((*iter).first.input)
				{
					os << (*iter).first.value << " " << (*iter).first.input;
				}
				else
				{
					os << (*iter).first.value;
				}
				os << "\t";
				os << (*iter).second;
			}
		}

		break;

	case BC_info::BC_LEAKY:
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

		// associated_solution
		if (a.m_bc_solution.size() && a.bc_solution_type == ST_ASSOCIATED)
		{
			os << "\t\t-associated_solution\n";
			CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
			for (; iter != a.m_bc_solution.end(); ++iter)
			{
				ASSERT((*iter).second.type != PROP_UNDEFINED);
				if ((*iter).second.type == PROP_UNDEFINED) continue;

				os << "\t\t\t";
				if ((*iter).first.input)
				{
					os << (*iter).first.value << " " << (*iter).first.input;
				}
				else
				{
					os << (*iter).first.value;
				}
				os << "\t";
				os << (*iter).second;
			}
		}

		// head
		if (a.m_bc_head.size())
		{
			os << "\t\t-head\n";
			CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_head.begin();
			for (; iter != a.m_bc_head.end(); ++iter)
			{
				ASSERT((*iter).second.type != PROP_UNDEFINED);
				if ((*iter).second.type == PROP_UNDEFINED) continue;

				os << "\t\t\t";
				if ((*iter).first.input)
				{
					os << (*iter).first.value << " " << (*iter).first.input;
				}
				else
				{
					os << (*iter).first.value;
				}
				os << "\t";
				os << (*iter).second;
			}
		}

		// hydraulic_conductivity
		if (a.bc_k && a.bc_k->type != PROP_UNDEFINED)
		{
			os << "\t\t-hydraulic_conductivity  " << static_cast<Cproperty>(*a.bc_k);
		}

		// thickness
		if (a.bc_thick && a.bc_thick->type != PROP_UNDEFINED)
		{
			os << "\t\t-thickness               " << static_cast<Cproperty>(*a.bc_thick);
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	return os;
}

void CBC::Merge(const CBC& src)
{
	// TODO 3/28/2005 Is this method still needed???
	// If so, the time series members might need to be
	// changed (m_bc_head, m_bc_flux, m_bc_solution)
	//
	if (this != &src)
	{
		// bc_type
		ASSERT(this->bc_type == src.bc_type);
		if (this->bc_type != src.bc_type) return;

		// bc_head
		if (src.m_bc_head.size())
		{
			this->m_bc_head = src.m_bc_head;
		}

		// bc_flux
		if (src.m_bc_flux.size())
		{
			this->m_bc_flux = src.m_bc_flux;
		}

		// bc_k
		if (src.bc_k)
		{
			ASSERT(FALSE); // unexpected for stress periods > 1
			// CopyProperty deletes if already exists
			Cproperty::CopyProperty(&this->bc_k, src.bc_k);
		}

		// bc_thick
		if (src.bc_thick)
		{
			ASSERT(FALSE); // unexpected for stress periods > 1
			// CopyProperty deletes if already exists
			Cproperty::CopyProperty(&this->bc_thick, src.bc_thick);
		}

		// bc_solution_type (NO CHANGE)
		// this->bc_solution_type = src.bc_solution_type;

		// bc_solution
		if (src.m_bc_solution.size())
		{
			this->m_bc_solution = src.m_bc_solution;
		}

		// face
		ASSERT(this->face == src.face);
		// TODO: CHECK THIS this->face = src.face;

		// face_defined
		ASSERT(this->face_defined == src.face_defined);
		// TODO: CHECK THIS this->face_defined = src.face_defined;
	}
}

void CBC::ClearProperties(void)
{
	delete static_cast<Cproperty*>(this->mask);        this->mask        = 0;
	this->m_bc_head.clear();
	this->m_bc_flux.clear();
	delete static_cast<Cproperty*>(this->bc_k);        this->bc_k        = 0;
	delete static_cast<Cproperty*>(this->bc_thick);    this->bc_thick    = 0;
	this->m_bc_solution.clear();
}

bool CBC::ContainsProperties(void)const
{
	switch (this->bc_type)
	{
		case BC_info::BC_FLUX: // bc_solution bc_flux
			if (!this->m_bc_solution.empty() || !this->m_bc_flux.empty())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		case BC_info::BC_LEAKY: // bc_thick bc_head bc_k bc_solution
			if (this->bc_thick || !this->m_bc_head.empty() || this->bc_k || !this->m_bc_solution.empty())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		case BC_info::BC_SPECIFIED: // bc_head bc_solution
			if (!this->m_bc_head.empty() || !this->m_bc_solution.empty())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		default:
			ASSERT(FALSE); // invalid type
	}
	return false;
}

void CBC::RemoveMutableProperties(void)
{
	// TODO for now this is the same as ClearProperties
	// (ie for LEAKY may not want to remove bc_k and bc_thick)
	delete static_cast<Cproperty*>(this->mask);        this->mask        = 0;
// COMMENT: {2/23/2005 1:24:17 PM}	delete static_cast<Cproperty*>(this->bc_head);     this->bc_head     = 0;
// COMMENT: {2/23/2005 1:24:20 PM}	delete static_cast<Cproperty*>(this->bc_flux);     this->bc_flux     = 0;
	delete static_cast<Cproperty*>(this->bc_k);        this->bc_k        = 0;
	delete static_cast<Cproperty*>(this->bc_thick);    this->bc_thick    = 0;
// COMMENT: {2/23/2005 1:24:25 PM}	delete static_cast<Cproperty*>(this->bc_solution); this->bc_solution = 0;
}
