#pragma once

class CDisplayColors
{
public:
	CDisplayColors(void);
	~CDisplayColors(void);

public:
	COLORREF crMedia;     // H5T_NATIVE_ULONG  H5T_STD_U32LE
	COLORREF crICHead;
    COLORREF crICChem;
	COLORREF crFlux;
    COLORREF crLeaky;
    COLORREF crSpecHead;
    COLORREF crRiver;
    COLORREF crWell;

	virtual void Serialize(bool bStoring, hid_t loc_id);

};
