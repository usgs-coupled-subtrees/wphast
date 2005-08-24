#pragma once
#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
}

#include <vector>
#include <iosfwd>  // std::ostream
#include "tree.h"  // CTreeCtrlNode

class CGrid : public grid
{
public:
	// ctor
	CGrid();
	CGrid(double minimum, double maximum, int nodes); // NOTE: variable names min and max confuse Intellisense
	// dtor
	~CGrid();
	// copy ctor
	CGrid(const CGrid& p);
	CGrid(const struct grid& p);
	// copy assignment
	CGrid& operator=(const CGrid& rhs); 
	// helper functions
	void Setup();
	void SetUniformRange(double minimum, double maximum, int count_coord);
	void Serialize(bool bStoring, hid_t loc_id);
	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid);
	void Insert(CTreeCtrlNode node);
	void Resize(size_t count);
	double& At(size_t pos);
	template<class InputIterator>
		void insert(InputIterator First, InputIterator Last)
		{	// insert [First, Last), arbitrary iterators
			this->m_coordinates.clear();
            for (; First != Last; ++First)
			{
				this->m_coordinates.push_back(*First);
			}
			this->uniform = FALSE;
			this->count_coord = (int)this->m_coordinates.size();
			this->coord = &this->m_coordinates[0];
		}
	void SubDivide(int nStart, int nEnd, int nParts);
	void Refine(int nStart, int nEnd, int nParts);
	int Closest(double value)const;
	int Element(double value)const;
	int lower_bound(double value)const;
	int upper_bound(double value)const;

	static void Fill(std::vector<double>& vec, double x1, double x2, int count);
#ifdef _DEBUG
	void Dump(CDumpContext& dc)const;
#endif

	friend std::ostream& operator<< (std::ostream &os, const CGrid &a);
protected:
	std::vector<double> m_coordinates;

private:
	void InternalCopy(const struct grid& src);
	void InternalDelete(void);
	void InternalInit(void);
};
