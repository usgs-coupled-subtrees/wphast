#if _MSC_VER > 1000
#pragma once
#endif

#include "zone.h"
#include "property.h"
#include "TimeSeries.h"

class CBCZone : public CZone
{
public:
	CBCZone(void);
	~CBCZone(void);

	// copy ctor
	CBCZone(const struct bc& src);
	CBCZone(const CBCZone& src);

	// copy assignment
	CBCZone& operator=(const struct bc& rhs);
	CBCZone& operator=(const CBCZone& rhs);

	// helper functions
#ifdef _DEBUG
	void AssertValid(int nSim/* = 1*/) const;
#endif
	void ClearProperties(void);
	bool ContainsProperties(void)const;

	friend std::ostream& operator<< (std::ostream &os, const CBCZone &a);

public:
	// boundary condition information
	int bc_type;      // UNDEFINED, SPECIFIED, FLUX, LEAKY
	int new_def;

	// head for SPECIFIED and LEAKY
	CTimeSeries<Cproperty> *bc_head;

	// flux for FLUX BC
	CTimeSeries<Cproperty> *bc_flux;

	// other parameters for LEAKY
	Cproperty *bc_k;
	Cproperty *bc_thick;
	
	// flux face for FLUX and LEAKY, 0, 1, or 2
	int face;
	int face_defined;

	// solution information for bc
	int bc_solution_type; // UNDEFINED, FIXED, ASSOCIATED
	CTimeSeries<Cproperty> *bc_solution;

protected:
	template<typename T> void InternalCopy(const T& src);
	void InternalDelete(void);
	void InternalInit(void);
};

