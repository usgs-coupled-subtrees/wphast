#pragma once

#include <iosfwd> // std::ostream

class CFreeSurface
{
public:
	CFreeSurface(void);
	CFreeSurface(bool bFreeSurface);
	~CFreeSurface(void);

	operator bool()const throw()              { return m_bFreeSurface; }

	void SetFreeSurface(bool bFreeSurface)    { m_bFreeSurface = bFreeSurface; }
	bool GetFreeSurface(void)const throw()    { return m_bFreeSurface; }

	CFreeSurface& operator=(const bool& rhs);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiFreeSurface)const;
	void Serialize(bool bStoring, hid_t loc_id);
	void Edit(CTreeCtrl* pTreeCtrl);

	friend std::ostream& operator<< (std::ostream &os, const CFreeSurface &a);

protected:
	bool m_bFreeSurface;
};
