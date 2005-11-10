#include "StdAfx.h"
#include "BC.h"

#include "Zone.h"
#include "property.h"
#include "Global.h"
#include <ostream> // std::ostream

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLIPFORMAT CBC::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CBC"));

CBC::CBC() // ctor
{
	this->InternalInit();
}

CBC::~CBC() // dtor
{
	this->InternalDelete();
}

void CBC::InternalCopy(const struct bc& src)
{
	// zone
	if (src.zone) {
		this->zone = new CZone(*src.zone);
	}
	else {
		this->zone = 0;
	}

	// mask
	this->mask = 0;

	// bc_type
	this->bc_type = src.bc_type;

	// bc_head
	this->bc_head = 0;

	// bc_flux
	this->bc_flux = 0;

	// bc_k
	this->bc_k = 0;
	Cproperty::CopyProperty(&this->bc_k, src.bc_k);

	// bc_thick
	this->bc_thick = 0;
	Cproperty::CopyProperty(&this->bc_thick, src.bc_thick);

	// bc_solution_type
	this->bc_solution_type = src.bc_solution_type;

	// bc_solution
	this->bc_solution = 0;

	// face
	this->face = src.face;

	// face_defined
	this->face_defined = src.face_defined;
}

void CBC::InternalDelete(void)
{
	delete static_cast<CZone*>(this->zone);
	delete static_cast<Cproperty*>(this->mask);
	this->m_bc_head.clear();
	this->m_bc_flux.clear();
	delete static_cast<Cproperty*>(this->bc_k);
	delete static_cast<Cproperty*>(this->bc_thick);
	this->m_bc_solution.clear();
}

void CBC::InternalInit(void)
{
	this->zone             = 0;

	// boundary conditions
	this->mask             = 0;
	this->bc_type          = UNDEFINED;
	this->bc_head          = 0;
	this->bc_flux          = 0;
	this->bc_k             = 0;
	this->bc_thick         = 0;
	this->bc_solution_type = UNDEFINED;
	this->bc_solution      = 0;
	this->face             = -1;
	this->face_defined     = FALSE;
	ASSERT(this->m_bc_head.empty());
	ASSERT(this->m_bc_flux.empty());
	ASSERT(this->m_bc_solution.empty());
}

CBC::CBC(const struct bc& src) // copy ctor
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
// COMMENT: {8/12/2004 7:45:57 PM}		this->InternalInit();
		this->InternalCopy(rhs);
		//{{
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
		//}}
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


	// zone
	if (this->zone)
	{
		static_cast<CZone*>(this->zone)->AssertValid();
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
		ASSERT(this->bc_solution == NULL);
		if (this->m_bc_solution.size())
		{
			ASSERT(this->bc_solution_type == ASSOCIATED);
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
				ASSERT(this->bc_solution_type == UNDEFINED);
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
	case LEAKY: // bc_thick bc_head bc_k bc_solution
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
			ASSERT(this->bc_solution_type == ASSOCIATED);
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
				ASSERT(this->bc_solution_type == UNDEFINED);
			}
		}
		break;

	case SPECIFIED: // bc_head bc_solution
		ASSERT(this->face_defined == FALSE);
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
			ASSERT(this->bc_solution_type == ASSOCIATED || this->bc_solution_type == FIXED);
			CTimeSeries<Cproperty>::const_iterator iter = this->m_bc_solution.begin();
			for (; iter != this->m_bc_solution.end(); ++iter)
			{
				(*iter).second.AssertValid();
			}
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

void CBC::Dump(CDumpContext& dc)const
{
	dc << "<CBC>\n";

	// zone
	//
	if (this->zone) {
		static_cast<CZone*>(this->zone)->Dump(dc);
	}
	else {
		dc << "<zone>NULL</zone>\n";
	}

	// bc_type (UNDEFINED, SPECIFIED, FLUX, LEAKY)
	//
	dc << "<bc_type>\n";
	switch (this->bc_type) {
		case UNDEFINED:
			dc << "UNDEFINED\n";
			break;
		case SPECIFIED:
			dc << "SPECIFIED\n";
			break;
		case FLUX:
			dc << "FLUX\n";
			break;
		case LEAKY:
			dc << "LEAKY\n";
			break;
		default:
			dc << "(bad type)\n";
			break;
	}
	dc << "</bc_type>\n";

	// face
	//
	if (this->face_defined) {
		dc << "<face>";
		dc << this->face;
		dc << "</face>\n";
	}
	else {
		dc << "<face>UNDEFINED</face>\n";
	}

	// bc_flux
	//
	dc << "<bc_flux>\n";
	if (this->bc_flux) {
// COMMENT: {2/23/2005 1:22:06 PM}		static_cast<Cproperty*>(this->bc_flux)->Dump(dc);
	}
	else {
		dc << "(NULL)\n";
	}
	dc << "</bc_flux>\n";

	// bc_head
	//
	dc << "<bc_head>\n";
	if (this->bc_head) {
// COMMENT: {2/23/2005 1:22:12 PM}		static_cast<Cproperty*>(this->bc_head)->Dump(dc);
	}
	else {
		dc << "(NULL)\n";
	}
	dc << "</bc_head>\n";

	// bc_k
	//
	dc << "<bc_k>\n";
	if (this->bc_k) {
		static_cast<Cproperty*>(this->bc_k)->Dump(dc);
	}
	else {
		dc << "(NULL)\n";
	}
	dc << "</bc_k>\n";

	// bc_solution_type (UNDEFINED, FIXED, ASSOCIATED)
	//
	dc << "<bc_solution_type>\n";
	switch (this->bc_solution_type) {
		case UNDEFINED:
			dc << "UNDEFINED\n";
			break;
		case FIXED:
			dc << "FIXED\n";
			break;
		case ASSOCIATED:
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
	if (this->bc_solution) {
// COMMENT: {2/23/2005 1:22:17 PM}		static_cast<Cproperty*>(this->bc_solution)->Dump(dc);
	}
	else {
		dc << "(NULL)\n";
	}
	dc << "</bc_solution>\n";

	// bc_thick
	//
	dc << "<bc_thick>\n";
	if (this->bc_thick) {
		static_cast<Cproperty*>(this->bc_thick)->Dump(dc);
	}
	else {
		dc << "(NULL)\n";
	}
	dc << "</bc_thick>\n";

	// bc_thick
	//
	dc << "<bc_k>\n";
	if (this->bc_k) {
		static_cast<Cproperty*>(this->bc_k)->Dump(dc);
	}
	else {
		dc << "(NULL)\n";
	}
	dc << "</bc_k>\n";

	dc << "</CBC>\n";
}
#endif // _DEBUG

void CBC::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szFace[]     = "face";
	static const char szType[]     = "bc_type";
	static const char szFlux[]     = "bc_flux";
	static const char szHead[]     = "bc_head";
	static const char szK[]        = "bc_k";
	static const char szSolution[] = "bc_solution";
	static const char szThick[]    = "bc_thick";
	static const char szSolType[]  = "bc_solution_type";
	
	herr_t status;

	if (bStoring)
	{
#ifdef _DEBUG
		static_cast<CZone*>(this->zone)->AssertValid();
		// this->AssertValid();
#endif

		// zone
		static_cast<CZone*>(this->zone)->Serialize(bStoring, loc_id);

		// bc_type
		status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->bc_type);

		// face
		status = CGlobal::HDFSerialize(bStoring, loc_id, szFace, H5T_NATIVE_INT, 1, &this->face);

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
		// zone
		if (!this->zone) {
			this->zone = new struct zone();
		}
		static_cast<CZone*>(this->zone)->Serialize(bStoring, loc_id);

		// bc_type
		status = CGlobal::HDFSerialize(bStoring, loc_id, szType, H5T_NATIVE_INT, 1, &this->bc_type);

		// face
		this->face_defined = FALSE;
		status = CGlobal::HDFSerialize(bStoring, loc_id, szFace, H5T_NATIVE_INT, 1, &this->face);
		if (this->face != -1) {
			this->face_defined = TRUE;
		}

		// bc_solution_type
		status = CGlobal::HDFSerialize(bStoring, loc_id, szSolType, H5T_NATIVE_INT, 1, &this->bc_solution_type);

		// properties
		Cproperty::SerializeOpen(szK,        (Cproperty**)&this->bc_k,        loc_id);
		Cproperty::SerializeOpen(szThick,    (Cproperty**)&this->bc_thick,    loc_id);

		// CTimeSeries properties
		CTimeSeries<Cproperty>::SerializeOpen(szFlux,     this->m_bc_flux,     loc_id);
		CTimeSeries<Cproperty>::SerializeOpen(szHead,     this->m_bc_head,     loc_id);
		CTimeSeries<Cproperty>::SerializeOpen(szSolution, this->m_bc_solution, loc_id);

#ifdef _DEBUG
		static_cast<CZone*>(this->zone)->AssertValid();
		// this->AssertValid();
#endif
	}
}

void CBC::Serialize(CArchive& ar)
{
	static const char szCBC[] = "CBC";
	static int version = 1;

	CString type;
	int ver;

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
		ASSERT(ver == version);
	}

	// zone
	if (!this->zone)
	{
		this->zone = new CZone();
	}
	static_cast<CZone*>(this->zone)->Serialize(ar);

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
		ar << this->bc_type;
	}
	else
	{
		ar >> this->bc_type;
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
		ASSERT(ver == version);
	}
}

std::ostream& operator<< (std::ostream &os, const CBC &a)
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
		os << static_cast<CZone>(*a.zone);
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

	os << static_cast<CZone>(*a.zone);

	//{{
	switch (a.bc_type)
	{
		case SPECIFIED:
			// head
			if (a.m_bc_head.size())
			{
				os << "\t\t-head\n";
				CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_head.begin();
				for (; iter != a.m_bc_head.end(); ++iter)
				{
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

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
			if (a.m_bc_solution.size() && a.bc_solution_type == ASSOCIATED)
			{
				os << "\t\t-associated_solution\n";
				CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
				for (; iter != a.m_bc_solution.end(); ++iter)
				{
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

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
			if (a.m_bc_solution.size() && a.bc_solution_type == FIXED)
			{
				os << "\t\t-fixed_solution\n";
				CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
				for (; iter != a.m_bc_solution.end(); ++iter)
				{
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

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

		case FLUX:
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
			// associated_solution
			if (a.m_bc_solution.size() && a.bc_solution_type == ASSOCIATED)
			{
				os << "\t\t-associated_solution\n";
				CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
				for (; iter != a.m_bc_solution.end(); ++iter)
				{
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

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
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

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

		case LEAKY:
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
			// associated_solution
			if (a.m_bc_solution.size() && a.bc_solution_type == ASSOCIATED)
			{
				os << "\t\t-associated_solution\n";
				CTimeSeries<Cproperty>::const_iterator iter = a.m_bc_solution.begin();
				for (; iter != a.m_bc_solution.end(); ++iter)
				{
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

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
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

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
			if (a.bc_k && a.bc_k->type != UNDEFINED) {
				os << "\t\t-hydraulic_conductivity  " << static_cast<Cproperty>(*a.bc_k);
			}
			// thickness
			if (a.bc_thick && a.bc_thick->type != UNDEFINED) {
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
	switch (this->bc_type) {
		case FLUX: // bc_solution bc_flux
			if (!this->m_bc_solution.empty() || !this->m_bc_flux.empty())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		case LEAKY: // bc_thick bc_head bc_k bc_solution
			if (this->bc_thick || !this->m_bc_head.empty() || this->bc_k || !this->m_bc_solution.empty())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		case SPECIFIED: // bc_head bc_solution
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
