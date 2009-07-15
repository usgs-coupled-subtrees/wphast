#include "StdAfx.h"
#include "GridElt.h"

#include "Zone.h"
#include "property.h"
#include <ostream> // std::ostream
#include "Global.h"
#include "srcinput/Cube.h"

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLIPFORMAT CGridElt::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CGridElt:3"));


/* ---------------------------------------------------------------------- 
 *   Grid Elements
 * ---------------------------------------------------------------------- */
CGridElt::CGridElt()
{
	this->InternalInit();
}

CGridElt::~CGridElt()
{
	this->InternalDelete();
}

void CGridElt::InternalInit(void)
{
	this->polyh             = 0;
	this->mask              = 0;
	this->active            = 0;
	this->porosity          = 0;
	this->kx                = 0;
	this->ky                = 0;
	this->kz                = 0;
	this->storage           = 0;
	this->alpha_long        = 0;
	this->alpha_trans       = 0;
	this->alpha_horizontal  = 0;
	this->alpha_vertical    = 0;
	this->shell             = false;
	this->shell_width[0]    = 0;
	this->shell_width[1]    = 0;
	this->shell_width[2]    = 0;
}

void CGridElt::InternalDelete(void)
{
	delete this->polyh;
	delete static_cast<Cproperty*>(this->mask);
	delete static_cast<Cproperty*>(this->active);
	delete static_cast<Cproperty*>(this->porosity);
	delete static_cast<Cproperty*>(this->kx);
	delete static_cast<Cproperty*>(this->ky);
	delete static_cast<Cproperty*>(this->kz);
	delete static_cast<Cproperty*>(this->storage);
	delete static_cast<Cproperty*>(this->alpha_long);
	delete static_cast<Cproperty*>(this->alpha_trans);
	delete static_cast<Cproperty*>(this->alpha_horizontal);
	delete static_cast<Cproperty*>(this->alpha_vertical);
}

CGridElt CGridElt::NewDefaults(bool bFlowOnly)
{
	CGridElt elt;
	// Active
	elt.active          = new Cproperty();
	elt.active->type    = PROP_FIXED;
	elt.active->count_v = 1;
	elt.active->v[0]    = 1;
	// Kx
	elt.kx          = new Cproperty();
	elt.kx->type    = PROP_FIXED;
	elt.kx->count_v = 1;
	elt.kx->v[0]    = 1e-4;
	// Ky
	elt.ky          = new Cproperty();
	elt.ky->type    = PROP_FIXED;
	elt.ky->count_v = 1;
	elt.ky->v[0]    = 1e-4;
	// Kz
	elt.kz          = new Cproperty();
	elt.kz->type    = PROP_FIXED;
	elt.kz->count_v = 1;
	elt.kz->v[0]    = 1e-5;
	// porosity
	elt.porosity          = new Cproperty();
	elt.porosity->type    = PROP_FIXED;
	elt.porosity->count_v = 1;
	elt.porosity->v[0]    = 0.2;
	// storage
	elt.storage          = new Cproperty();
	elt.storage->type    = PROP_FIXED;
	elt.storage->count_v = 1;
	elt.storage->v[0]    = 1e-4;

	if (!bFlowOnly)
	{
		elt.alpha_long                = new Cproperty();
		elt.alpha_long->type          = PROP_FIXED;
		elt.alpha_long->count_v       = 1;
		elt.alpha_long->v[0]          = 1.0;

		elt.alpha_horizontal          = new Cproperty();
		elt.alpha_horizontal->type    = PROP_FIXED;
		elt.alpha_horizontal->count_v = 1;
		elt.alpha_horizontal->v[0]    = 1.0;

		elt.alpha_vertical            = new Cproperty();
		elt.alpha_vertical->type      = PROP_FIXED;
		elt.alpha_vertical->count_v   = 1;
		elt.alpha_vertical->v[0]      = 1.0;
	}
	return elt;
}

CGridElt CGridElt::Full(void)
{
	CGridElt elt;
	elt.mask              = new Cproperty;
	elt.active            = new Cproperty;
	elt.porosity          = new Cproperty;
	elt.kx                = new Cproperty;
	elt.ky                = new Cproperty;
	elt.kz                = new Cproperty;
	elt.storage           = new Cproperty;
	elt.alpha_long        = new Cproperty;
	elt.alpha_trans       = new Cproperty;
	elt.alpha_horizontal  = new Cproperty;
	elt.alpha_vertical    = new Cproperty;
	return elt;
}

void CGridElt::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szActive[]          = "active";
	static const char szMask[]            = "mask";
	static const char szPorosity[]        = "porosity";
	static const char szKx[]              = "kx";
	static const char szKy[]              = "ky";
	static const char szKz[]              = "kz";
	static const char szStorage[]         = "storage";
	static const char szAlphaLong[]       = "alpha_long";
	static const char szAlphaTrans[]      = "alpha_trans";
	static const char szAlphaHorizontal[] = "alpha_horizontal";
	static const char szAlphaVertical[]   = "alpha_vertical";
	static const char szShell[]           = "shell";
	static const char szShellWidth[]      = "shell_width";

	if (bStoring)
	{
		Cproperty::SerializeCreate(szActive,          static_cast<Cproperty*>(this->active),           loc_id);
		//Cproperty::SerializeCreate(szMask,            static_cast<Cproperty*>(this->mask),             loc_id);
		Cproperty::SerializeCreate(szPorosity,        static_cast<Cproperty*>(this->porosity),         loc_id);
		Cproperty::SerializeCreate(szKx,              static_cast<Cproperty*>(this->kx),               loc_id);
		Cproperty::SerializeCreate(szKy,              static_cast<Cproperty*>(this->ky),               loc_id);
		Cproperty::SerializeCreate(szKz,              static_cast<Cproperty*>(this->kz),               loc_id);
		Cproperty::SerializeCreate(szStorage,         static_cast<Cproperty*>(this->storage),          loc_id);
		Cproperty::SerializeCreate(szAlphaLong,       static_cast<Cproperty*>(this->alpha_long),       loc_id);
		//Cproperty::SerializeCreate(szAlphaTrans,      static_cast<Cproperty*>(this->alpha_trans),      loc_id);
		Cproperty::SerializeCreate(szAlphaHorizontal, static_cast<Cproperty*>(this->alpha_horizontal), loc_id);
		Cproperty::SerializeCreate(szAlphaVertical,   static_cast<Cproperty*>(this->alpha_vertical),   loc_id);
	}
	else
	{
		Cproperty::SerializeOpen(szActive,          (Cproperty**)&this->active,           loc_id);
		//Cproperty::SerializeOpen(szMask,            (Cproperty**)&this->mask,             loc_id);
		Cproperty::SerializeOpen(szPorosity,        (Cproperty**)&this->porosity,         loc_id);
		Cproperty::SerializeOpen(szKx,              (Cproperty**)&this->kx,               loc_id);
		Cproperty::SerializeOpen(szKy,              (Cproperty**)&this->ky,               loc_id);
		Cproperty::SerializeOpen(szKz,              (Cproperty**)&this->kz,               loc_id);
		Cproperty::SerializeOpen(szStorage,         (Cproperty**)&this->storage,          loc_id);
		Cproperty::SerializeOpen(szAlphaLong,       (Cproperty**)&this->alpha_long,       loc_id);
		//Cproperty::SerializeOpen(szAlphaTrans,      (Cproperty**)&this->alpha_trans,      loc_id);
		Cproperty::SerializeOpen(szAlphaHorizontal, (Cproperty**)&this->alpha_horizontal, loc_id);
		Cproperty::SerializeOpen(szAlphaVertical,   (Cproperty**)&this->alpha_vertical,   loc_id);
	}

	CGlobal::HDFSerializeBool(bStoring, loc_id, szShell, this->shell);
	herr_t status = CGlobal::HDFSerializeSafe(bStoring, loc_id, szShellWidth, H5T_NATIVE_DOUBLE, 3, this->shell_width);
	if (!bStoring && status < 0)
	{
		this->shell_width[0] = 0;
		this->shell_width[1] = 0;
		this->shell_width[2] = 0;
	}
}

#ifdef _DEBUG
void CGridElt::Dump(CDumpContext& dc)const
{
	dc << "<CGridElt>\n";

	ASSERT(this->polyh && ::AfxIsValidAddress(this->polyh, sizeof(Polyhedron)));
	CGlobal::Dump(dc, *this->polyh);

	dc << "<mask>\n";
	if (this->mask)
		static_cast<Cproperty*>(this->mask)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</mask>\n";


	dc << "<active>\n";
	if (this->active)
		static_cast<Cproperty*>(this->active)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</active>\n";

	dc << "<porosity>\n";
	if (this->porosity)
		static_cast<Cproperty*>(this->porosity)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</porosity>\n";

	dc << "<kx>\n";
	if (this->kx)
		static_cast<Cproperty*>(this->kx)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</kx>\n";

	dc << "<ky>\n";
	if (this->ky)
		static_cast<Cproperty*>(this->ky)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</ky>\n";

	dc << "<kz>\n";
	if (this->kz)
		static_cast<Cproperty*>(this->kz)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</kz>\n";

	dc << "<storage>\n";
	if (this->storage)
		static_cast<Cproperty*>(this->storage)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</storage>\n";

	dc << "<alpha_long>\n";
	if (this->alpha_long)
		static_cast<Cproperty*>(this->alpha_long)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</alpha_long>\n";

	dc << "<alpha_trans>\n";
	if (this->alpha_trans)
		static_cast<Cproperty*>(this->alpha_trans)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</alpha_trans>\n";

	dc << "<alpha_horizontal>\n";
	if (this->alpha_horizontal)
		static_cast<Cproperty*>(this->alpha_horizontal)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</alpha_horizontal>\n";

	dc << "<alpha_vertical>\n";
	if (this->alpha_vertical)
		static_cast<Cproperty*>(this->alpha_vertical)->Dump(dc);
	else
		dc << "(NULL)\n";
	dc << "</alpha_vertical>\n";

	dc << "<shell>\n";
	dc << this->shell << "\n";
	dc << this->shell_width[0] << "\n";
	dc << this->shell_width[1] << "\n";
	dc << this->shell_width[2] << "\n";
	dc << "</shell>\n";

	dc << "</CGridElt>\n";

}
#endif

CGridElt::CGridElt(const CGridElt& src) // copy ctor
{
	this->InternalCopy(src);
}

CGridElt::CGridElt(const grid_elt& src) // copy ctor
{
	ASSERT(src.polyh && ::AfxIsValidAddress(src.polyh, sizeof(Polyhedron)));
	this->InternalCopy(src);
}

void CGridElt::InternalCopy(const grid_elt& src)
{
	// polyh
	this->polyh = 0;
	if (src.polyh)
	{
		this->polyh = src.polyh->clone();
	}

	// mask
	this->mask = 0;
	Cproperty::CopyProperty(&this->mask, src.mask);

	// active
	this->active = 0;
	Cproperty::CopyProperty(&this->active, src.active);

	// porosity
	this->porosity = 0;
	Cproperty::CopyProperty(&this->porosity, src.porosity);

	// kx
	this->kx = 0;
	Cproperty::CopyProperty(&this->kx, src.kx);

	// ky
	this->ky = 0;
	Cproperty::CopyProperty(&this->ky, src.ky);

	// kz
	this->kz = 0;
	Cproperty::CopyProperty(&this->kz, src.kz);

	// storage
	this->storage = 0;
	Cproperty::CopyProperty(&this->storage, src.storage);
	
	// alpha_long
	this->alpha_long = 0;
	Cproperty::CopyProperty(&this->alpha_long, src.alpha_long);

	// alpha_trans
	this->alpha_trans = 0;
	Cproperty::CopyProperty(&this->alpha_trans, src.alpha_trans);

	// alpha_horizontal
	this->alpha_horizontal = 0;
	Cproperty::CopyProperty(&this->alpha_horizontal, src.alpha_horizontal);

	// alpha_vertical
	this->alpha_vertical = 0;
	Cproperty::CopyProperty(&this->alpha_vertical, src.alpha_vertical);

	// shell
	this->shell             = src.shell;
	this->shell_width[0]    = src.shell_width[0];
	this->shell_width[1]    = src.shell_width[1];
	this->shell_width[2]    = src.shell_width[2];
}

CGridElt& CGridElt::operator=(const CGridElt& rhs) // copy assignment
{
	if (this != &rhs)
	{
		this->InternalDelete();
		this->InternalInit();
		this->InternalCopy(rhs);
	}
	return *this;
}

std::ostream& operator<< (std::ostream &os, const CGridElt &a)
{
	ASSERT(a.polyh && ::AfxIsValidAddress(a.polyh, sizeof(Polyhedron)));
	os << (*a.polyh);

	Cproperty* property_ptr;

	property_ptr = static_cast<Cproperty*>(a.mask);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-mask                     " << (*property_ptr);
	}
    property_ptr = static_cast<Cproperty*>(a.active);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-active                   " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.kx);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-Kx                       " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.ky);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-Ky                       " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.kz);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-Kz                       " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.porosity);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-porosity                 " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.storage);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-specific_storage         " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.alpha_long);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-long_dispersivity        " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.alpha_trans);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-trans_dispersivity       " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.alpha_horizontal);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-horizontal_dispersivity  " << (*property_ptr);
	}
	property_ptr = static_cast<Cproperty*>(a.alpha_vertical);
	if (property_ptr && property_ptr->type != PROP_UNDEFINED)
	{
		os << "\t\t-vertical_dispersivity    " << (*property_ptr);
	}
	if (a.shell)
	{
		os << "\t\t-shell " << a.shell_width[0] << " " <<  a.shell_width[1] << " " << a.shell_width[2] << "\n";
	}
	return os;
}

void CGridElt::Serialize(CArchive& ar)
{
	static const char szCGridElt[] = "CGridElt";
	static int version = 3;

	CString type;
	int ver = version;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCGridElt;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCGridElt) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}

	if (ver == 1)
	{
		CZone z;
		z.Serialize(ar);
		if (ar.IsStoring())
		{
			ASSERT(FALSE); // can't happen
		}
		else
		{
			ASSERT(this->polyh == 0);
			this->polyh = new Cube(&z);
		}		
	}
	else if(ver == 2)
	{
		CGlobal::Serialize(&(this->polyh), ar);
	}
	else if(ver == 3)
	{
		CGlobal::Serialize(&(this->polyh), ar);

		if (ar.IsStoring())
		{
			ar << this->shell;
			ar << this->shell_width[0];
			ar << this->shell_width[1];
			ar << this->shell_width[2];
		}
		else
		{
			ar >> this->shell;
			ar >> this->shell_width[0];
			ar >> this->shell_width[1];
			ar >> this->shell_width[2];
		}
	}
	else
	{
		ASSERT(FALSE);
	}


	static const char szActive[]          = "active";
	static const char szPorosity[]        = "porosity";
	static const char szKx[]              = "kx";
	static const char szKy[]              = "ky";
	static const char szKz[]              = "kz";
	static const char szStorage[]         = "storage";
	static const char szAlphaLong[]       = "alpha_long";
	static const char szAlphaHorizontal[] = "alpha_horizontal";
	static const char szAlphaVertical[]   = "alpha_vertical";

	Cproperty::Serial(ar, szActive,          &this->active);
	Cproperty::Serial(ar, szPorosity,        &this->porosity);
	Cproperty::Serial(ar, szKx,              &this->kx);
	Cproperty::Serial(ar, szKy,              &this->ky);
	Cproperty::Serial(ar, szKz,              &this->kz);
	Cproperty::Serial(ar, szStorage,         &this->storage);
	Cproperty::Serial(ar, szAlphaLong,       &this->alpha_long);
	Cproperty::Serial(ar, szAlphaHorizontal, &this->alpha_horizontal);
	Cproperty::Serial(ar, szAlphaVertical,   &this->alpha_vertical);

	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szCGridElt;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szCGridElt) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}
}

bool CGridElt::operator==(const grid_elt& rhs)const
{
	size_t s = sizeof(grid_elt);
	ASSERT(s == 80); // need to modify if this changes

	COMPARE_PROPERTY_MACRO(mask);
	COMPARE_PROPERTY_MACRO(active);
	COMPARE_PROPERTY_MACRO(porosity);
	COMPARE_PROPERTY_MACRO(kx);
	COMPARE_PROPERTY_MACRO(ky);
	COMPARE_PROPERTY_MACRO(kz);
	COMPARE_PROPERTY_MACRO(storage);
	COMPARE_PROPERTY_MACRO(alpha_long);
	COMPARE_PROPERTY_MACRO(alpha_trans);
	COMPARE_PROPERTY_MACRO(alpha_horizontal);
	COMPARE_PROPERTY_MACRO(alpha_vertical);

	if (this->shell          != rhs.shell)          return false;
	if (this->shell_width[0] != rhs.shell_width[0]) return false;
	if (this->shell_width[1] != rhs.shell_width[1]) return false;
	if (this->shell_width[2] != rhs.shell_width[2]) return false;

	return true;
}

#define COPY_PROP_MACRO(S, P) \
	do { \
		p.title = S; \
		if (this->P) { \
			p.prop = *this->P; \
		} else { \
			p.prop = Cproperty(); \
		} \
		v.push_back(p); \
	} while (0)			

void CGridElt::Fill(std::vector<CPropStruct>& v)const
{
	CPropStruct p;
	COPY_PROP_MACRO("Active",                    active);
	COPY_PROP_MACRO("Kx",                        kx);
	COPY_PROP_MACRO("Ky",                        ky);
	COPY_PROP_MACRO("Kz",                        kz);
	COPY_PROP_MACRO("Porosity",                  porosity);
	COPY_PROP_MACRO("Specific storage",          storage);
	COPY_PROP_MACRO("Longitudinal dispersivity", alpha_long);
	COPY_PROP_MACRO("Horizontal dispersivity",   alpha_horizontal);
	COPY_PROP_MACRO("Vertical dispersivity",     alpha_vertical);
}
