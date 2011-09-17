#pragma once

class CTitle
{
public:
	CTitle(void);
	CTitle(const char* t);
	~CTitle(void);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiTitle)const;
	void Serialize(bool bStoring, hid_t loc_id);
	void Edit(CTreeCtrl* pTreeCtrl);

	friend std::ostream& operator<< (std::ostream &os, const CTitle &a);

public:
	std::string text;
};
