#pragma once

#include "srcinput/Drain.h"
#include "River.h"

#include <list>

class CDrain : public Drain
{
public:
	CDrain(void);
	~CDrain(void);

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);

	static CLIPFORMAT clipFormat;

	CDrain(const Drain &drain);
	std::list<CRiverPoint> m_listPoints;
protected:
	friend std::ostream& operator<< (std::ostream &os, const CDrain &a);
};
