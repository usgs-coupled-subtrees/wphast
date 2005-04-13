#include "StdAfx.h"
#include "Global.h"
#include "resource.h"

#include <fstream>

#include "WPhastDoc.h"
#include "WPhastView.h"

#include "BC.h"
#include "property.h"
#include "CheckTreeCtrl.h"
#include "ZoneActor.h"

#include "gridctrl/ModGridCtrlEx.h"

#include "vtkPropPickerFixed.h"

#include <vtkProp3D.h>
#include <vtkRenderer.h>
#include <vtkWin32RenderWindowInteractor.h>

#include <vtkAbstractPropPicker.h>
#include <vtkCoordinate.h>
#include <vtkWin32OpenGLRenderWindow.h>

#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

#include "WorldTransform.h"

#include "Units.h"

#include <afxmt.h>
static CCriticalSection  critSect;


CGlobal::CGlobal(void)
{
}

CGlobal::~CGlobal(void)
{
}

void CGlobal::LoadRTFString(std::string& str, UINT nID)
{
	HRSRC hRes = ::FindResource(::AfxGetResourceHandle(), MAKEINTRESOURCE(nID), "RTF");
	if (hRes)
	{
		HGLOBAL hData = ::LoadResource(::AfxGetResourceHandle(), hRes);
		if (hData)
		{
            LPCTSTR lp = (LPCTSTR)::LockResource(hData);
			str = lp;
			::UnlockResource(hRes);
			::FreeResource(hData);
		}
	}
}

void CGlobal::PickProp(CWPhastView* pView, vtkProp3D* pProp3D)
{
	ASSERT(FALSE); // deprecated use CZoneActor->Select instead

	vtkRenderer* pRenderer = pView->GetRenderer();
	vtkRenderWindowInteractor* pRenderWindowInteractor = pView->GetRenderWindowInteractor();

	////CGlobal::PickProp(pProp3D, pView->GetRenderer(), pView->GetRenderWindowInteractor());

	if (CZoneActor* pZone = CZoneActor::SafeDownCast(pProp3D))
	{
		pZone->Select(pView);
	}
}

void CGlobal::PickProp(vtkProp3D* pProp3D, vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	ASSERT(FALSE); // deprecated use CZoneActor->Select instead
	//{{
// COMMENT: {12/30/2003 6:59:50 PM}	if (vtkPropPickerFixed *picker = vtkPropPickerFixed::SafeDownCast( pRenderWindowInteractor->GetPicker() )) {
// COMMENT: {12/30/2003 6:59:50 PM}		picker->ForcePick(pProp3D);
// COMMENT: {12/30/2003 6:59:50 PM}		picker->ForcePick(0);
// COMMENT: {12/30/2003 6:59:50 PM}		return;
// COMMENT: {12/30/2003 6:59:50 PM}	}
	//}}

	////critSect.Lock();

	// get center of prop
	//
	float *bounds = pProp3D->GetBounds();
	vtkCoordinate* pCoor = vtkCoordinate::New();
	pCoor->SetCoordinateSystemToWorld();
	pCoor->SetValue((bounds[0] + bounds[1])/2, (bounds[2] + bounds[3])/2, (bounds[4] + bounds[5])/2);

	// convert world coordinates to viewport coordinates
	//
	int* xy = pCoor->GetComputedViewportValue(pRenderer);
	TRACE("CGlobal::PickProp position(%d, %d)\n", xy[0], xy[1]);

	// perform the pick
	//
	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pRenderWindowInteractor->GetPicker() )) {
		picker->AddPickList(pProp3D);
		picker->PickFromListOn();

		pRenderWindowInteractor->SetEventPosition(xy);
		if (vtkWin32OpenGLRenderWindow *pRenderWindow = vtkWin32OpenGLRenderWindow::SafeDownCast( pRenderer->GetRenderWindow() )) {
			::SendMessage(pRenderWindow->GetWindowId(), WM_CHAR, 'p', 0);
		}

		picker->DeletePickList(pProp3D);
		picker->PickFromListOff();
	}
	pCoor->Delete();

	////critSect.Unlock();
}

void CGlobal::UnPickProp(vtkProp3D* pProp3D, vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	ASSERT(pProp3D->GetPickable());
	pProp3D->SetPickable(0);
	CGlobal::PickProp(pProp3D, pRenderer, pRenderWindowInteractor);
	ASSERT(!pProp3D->GetPickable());
	pProp3D->SetPickable(1);
}

herr_t CGlobal::HDFSerialize(bool bStoring, hid_t loc_id, const char* szName, hid_t mem_type_id, hsize_t count, void* buf)
{
	hsize_t dims[1], maxdims[1];
	hid_t dspace_id = 0;
	hid_t dset_id = 0;
	herr_t status;
	herr_t return_val;

	ASSERT(count > 0); // bad count

	if (bStoring)
	{
		// Create the szName dataspace
		dims[0] = maxdims[0] = count;
		dspace_id = ::H5Screate_simple(1, dims, maxdims);
		if (dspace_id > 0)
		{
			// Create the szName dataset
			dset_id = ::H5Dcreate(loc_id, szName, mem_type_id, dspace_id, H5P_DEFAULT);
			if (dset_id > 0)
			{
				// Write the szName dataset
				status = ::H5Dwrite(dset_id, mem_type_id, dspace_id, H5S_ALL, H5P_DEFAULT, buf);
				if (status < 0)
				{
					ASSERT(FALSE); // unable to write dataset
					return_val = status;
					goto HDFSerializeError;
				}
				// Close the szName dataset
				status = ::H5Dclose(dset_id);
				if (status < 0)
				{
					ASSERT(FALSE); // unable to close dataset
					return_val = status;
					goto HDFSerializeError;
				}
			}
			else
			{
				ASSERT(FALSE); // unable to create dataspace
				return_val = dset_id;
				goto HDFSerializeError;
			}
			// Close the szName dataspace
			status = ::H5Sclose(dspace_id);
			if (status < 0)
			{
				ASSERT(FALSE); // unable to close dataspace
				return_val = status;
				goto HDFSerializeError;
			}
		}
		else
		{
			ASSERT(FALSE); // unable to create dataspace
			return_val = dspace_id;
			goto HDFSerializeError;
		}
	}
	else
	{
		// Open the szName dataset
		dset_id = ::H5Dopen(loc_id, szName);
		if (dset_id > 0)
		{
			// get the szName dataspace
			dspace_id = ::H5Dget_space(dset_id);
			if (dspace_id > 0)
			{
				int n_dims = ::H5Sget_simple_extent_dims(dspace_id, dims, maxdims);
				if (n_dims == 1)
				{
					ASSERT(dims[0] == count); // count mismatch
					if (dims[0] == count)
					{
						// read the szName dataset
						status = ::H5Dread(dset_id, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
						if (status < 0)
						{
							ASSERT(FALSE); // unable to read dataset
							return_val = status;
							goto HDFSerializeError;
						}
						// Close the szName dataspace
						status = ::H5Sclose(dspace_id);
						ASSERT(status >= 0); // unable to close dataspace
					}
					else
					{
						ASSERT(FALSE); // count mismatch
						return_val = -1;
						goto HDFSerializeError;
					}
				}
				else
				{
					ASSERT(FALSE); // unable to get_simple_extent_dims
					return_val = n_dims;
					goto HDFSerializeError;
				}
				// Close the szName dataset
				status = ::H5Dclose(dset_id);
				ASSERT(status >= 0); // unable to close dataset
			}
			else
			{
				ASSERT(FALSE); // unable to open dataspace
				return_val = dspace_id;
				goto HDFSerializeError;
			}
		}
		else
		{
			ASSERT(FALSE); // unable to open dataset
			return_val = dset_id;
			goto HDFSerializeError;

		}
	}

	// ok if here
	return 0; // no errors

HDFSerializeError:

	if (dset_id > 0)
	{
		status = ::H5Dclose(dset_id);
		ASSERT(status >= 0); // unable to close dataset
	}
	if (dspace_id > 0)
	{
		status = ::H5Sclose(dspace_id);
		ASSERT(status >= 0); // unable to close dataspace
	}
	return return_val;
}

herr_t CGlobal::HDFSerializeWithSize(bool bStoring, hid_t loc_id, const char* szName, hid_t mem_type_id, hsize_t& count, void* buf)
{
	hsize_t dims[1], maxdims[1];
	hid_t dspace_id = 0;
	hid_t dset_id = 0;
	herr_t status;
	herr_t return_val;

	ASSERT(count > 0); // bad count

	if (bStoring)
	{
		// Create the szName dataspace
		dims[0] = maxdims[0] = count;
		dspace_id = ::H5Screate_simple(1, dims, maxdims);
		if (dspace_id > 0)
		{
			// Create the szName dataset
			dset_id = ::H5Dcreate(loc_id, szName, mem_type_id, dspace_id, H5P_DEFAULT);
			if (dset_id > 0)
			{
				// Write the szName dataset
				status = ::H5Dwrite(dset_id, mem_type_id, dspace_id, H5S_ALL, H5P_DEFAULT, buf);
				if (status < 0)
				{
					ASSERT(FALSE); // unable to write dataset
					return_val = status;
					goto HDFSerializeWithSizeError;
				}
				// Close the szName dataset
				status = ::H5Dclose(dset_id);
				if (status < 0)
				{
					ASSERT(FALSE); // unable to close dataset
					return_val = status;
					goto HDFSerializeWithSizeError;
				}
			}
			else
			{
				ASSERT(FALSE); // unable to create dataspace
				return_val = dset_id;
				goto HDFSerializeWithSizeError;
			}
			// Close the szName dataspace
			status = ::H5Sclose(dspace_id);
			if (status < 0)
			{
				ASSERT(FALSE); // unable to close dataspace
				return_val = status;
				goto HDFSerializeWithSizeError;
			}
		}
		else
		{
			ASSERT(FALSE); // unable to create dataspace
			return_val = dspace_id;
			goto HDFSerializeWithSizeError;
		}
	}
	else
	{
		// Open the szName dataset
		dset_id = ::H5Dopen(loc_id, szName);
		if (dset_id > 0)
		{
			// get the szName dataspace
			dspace_id = ::H5Dget_space(dset_id);
			if (dspace_id > 0)
			{
				int n_dims = ::H5Sget_simple_extent_dims(dspace_id, dims, maxdims);
				if (n_dims == 1)
				{
					count = dims[0];
					// read the szName dataset
					status = ::H5Dread(dset_id, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
					if (status < 0)
					{
						ASSERT(FALSE); // unable to read dataset
						return_val = status;
						goto HDFSerializeWithSizeError;
					}
					// Close the szName dataspace
					status = ::H5Sclose(dspace_id);
					ASSERT(status >= 0); // unable to close dataspace
				}
				else
				{
					ASSERT(FALSE); // unable to get_simple_extent_dims
					return_val = n_dims;
					goto HDFSerializeWithSizeError;
				}
				// Close the szName dataset
				status = ::H5Dclose(dset_id);
				ASSERT(status >= 0); // unable to close dataset
			}
			else
			{
				ASSERT(FALSE); // unable to open dataspace
				return_val = dspace_id;
				goto HDFSerializeWithSizeError;
			}
		}
		else
		{
			ASSERT(FALSE); // unable to open dataset
			return_val = dset_id;
			goto HDFSerializeWithSizeError;
		}
	}

	// ok if here
	return 0; // no errors

HDFSerializeWithSizeError:

	if (dset_id > 0)
	{
		status = ::H5Dclose(dset_id);
		ASSERT(status >= 0); // unable to close dataset
	}
	if (dspace_id > 0)
	{
		status = ::H5Sclose(dspace_id);
		ASSERT(status >= 0); // unable to close dataspace
	}
	return return_val;
}

herr_t CGlobal::HDFSerializeAllocate(bool bStoring, hid_t loc_id, const char* szName, hsize_t& count, double** buffer)
{
	return CGlobal::HDFSerializeAllocate<double>(bStoring, loc_id, szName, H5T_NATIVE_DOUBLE, count, buffer);
}

herr_t CGlobal::HDFSerializeAllocate(bool bStoring, hid_t loc_id, const char* szName, hsize_t& count, int** buffer)
{
	return CGlobal::HDFSerializeAllocate<int>(bStoring, loc_id, szName, H5T_NATIVE_INT, count, buffer);
}

void CGlobal::WriteList(hid_t loc_id, const char *name, std::list<LPCTSTR> &rlist)
{
	herr_t status;
	hsize_t dims[1];
	std::list<LPCTSTR>::iterator iter;
	size_t len;

	hid_t dspace_id = 0;
	hid_t dset_id = 0;
	hid_t dtype_id = 0;
	char* szNames = 0;
	size_t maxlen = 0;

	// determine max length string
	ASSERT(!rlist.empty());
	iter = rlist.begin();
	for (; iter != rlist.end(); ++iter) {
		len = ::strlen(*iter);
		maxlen = (len > maxlen) ? len : maxlen;
	}

    // create fixed length string type
	dtype_id = ::H5Tcopy(H5T_C_S1);
	if (dtype_id < 0) goto ErrorWriteList;

	status = ::H5Tset_strpad(dtype_id, H5T_STR_NULLTERM);
	ASSERT(status >= 0);
	if (status < 0) goto ErrorWriteList;

	status = ::H5Tset_size(dtype_id, maxlen + 1);
	ASSERT(status >= 0);
	if (status < 0) goto ErrorWriteList;

    // create the dataspace
    dims[0] = rlist.size();
    dspace_id = H5Screate_simple(1, dims, NULL);
	ASSERT(dspace_id > 0);
	if (dspace_id <= 0) goto ErrorWriteList;

    // create the dataset
    dset_id = H5Dcreate(loc_id, name, dtype_id, dspace_id, H5P_DEFAULT);
    ASSERT(dset_id > 0);
	if (dset_id <= 0) goto ErrorWriteList;

    // copy variable length time steps to fixed length strings
	////char *names = (char*)::calloc((maxlen + 1) * rlist.size(), sizeof(char));
	szNames = new char[(maxlen + 1) * rlist.size()];
	if (szNames == 0) goto ErrorWriteList;
	::memset(szNames, 0, (maxlen + 1) * rlist.size());

	iter = rlist.begin();
	for (int i = 0; iter != rlist.end(); ++iter, ++i) {
		::strcpy(szNames + i * (maxlen + 1), *iter);
	}

    // write the dataset
    status = H5Dwrite(dset_id, dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, szNames);
	ASSERT(status >= 0);

ErrorWriteList:
	if (szNames) {
		delete szNames;
	}

	if (dtype_id > 0) {
		status = ::H5Tclose(dtype_id);
		ASSERT(status >= 0);
	}

	if (dspace_id > 0) {
		status = ::H5Sclose(dspace_id);
		ASSERT(status >= 0);
	}

	if (dset_id > 0) {
		status = ::H5Dclose(dset_id);
		ASSERT(status >= 0);
	}
}

void CGlobal::ReadList(hid_t loc_id, const char *name, std::list<std::string> &rlist)
{
	hsize_t dims[1], maxdims[1];
	herr_t status;
	hid_t dspace_id = 0;
	hid_t dset_id = 0;
	hid_t dtype_id = 0;
	char* szNames = 0;

	dset_id = ::H5Dopen(loc_id, name);
	if (dset_id <= 0) goto ErrorReadList;

	dspace_id = ::H5Dget_space(dset_id);
	if (dspace_id < 0) goto ErrorReadList;
	if (::H5Sget_simple_extent_dims(dspace_id, dims, maxdims) < 0) goto ErrorReadList;

	dtype_id = ::H5Dget_type(dset_id);
	if (dtype_id < 0) goto ErrorReadList;

	if (H5Tget_strpad(dtype_id) == H5T_STR_NULLTERM)
	{
		// fixed length strings
		size_t len = ::H5Tget_size(dtype_id);
		ASSERT(len * dims[0]);
		szNames = new char[len * (int)dims[0]];
		if (szNames == 0) goto ErrorReadList;

		if (::H5Dread(dset_id, dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, szNames) < 0) goto ErrorReadList;

		for (int i = 0; i < dims[0]; ++i) {
			rlist.push_back(szNames + i * len);
		}
	}

ErrorReadList:

	if (szNames) {
		delete szNames;
	}

	if (dtype_id > 0) {
		status = ::H5Tclose(dtype_id);
		ASSERT(status >= 0);
	}

	if (dspace_id > 0) {
		status = ::H5Sclose(dspace_id);
		ASSERT(status >= 0);
	}

	if (dset_id > 0) {
		status = ::H5Dclose(dset_id);
		ASSERT(status >= 0);
	}
}

bool CGlobal::IsValidTransform(vtkTransform* pTransform)
{
	vtkMatrix4x4* pMatrix = pTransform->GetMatrix();
	if (pMatrix == 0) return false;
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 4; ++i) {
			if (pMatrix->GetElement(i, j) != pMatrix->GetElement(i, j)) // true if NaN
				return false;
		}
	}
	return true;
}

void CGlobal::EnableFace(CDataExchange* pDX, BOOL bEnable)
{
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_STATIC));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_X_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_Y_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_Z_RADIO));

	if (CWnd *pWnd = pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_STATIC)) {
		pWnd->EnableWindow(bEnable);
	}
	if (CWnd *pWnd = pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_X_RADIO)) {
		pWnd->EnableWindow(bEnable);
	}
	if (CWnd *pWnd = pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_Y_RADIO)) {
		pWnd->EnableWindow(bEnable);
	}
	if (CWnd *pWnd = pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_Z_RADIO)) {
		pWnd->EnableWindow(bEnable);
	}
}


void CGlobal::DDX_Face(CDataExchange* pDX, CBC& r_bc)
{
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_X_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_Y_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FACE_Z_RADIO));

	if (pDX->m_bSaveAndValidate)
	{
		r_bc.face_defined = TRUE;
		int nFace = pDX->m_pDlgWnd->GetCheckedRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO);
		if (nFace == IDC_FACE_X_RADIO) {
			r_bc.face = 0;
		}
		else if (nFace == IDC_FACE_Y_RADIO) {
			r_bc.face = 1;
		}
		else if (nFace == IDC_FACE_Z_RADIO) {
			r_bc.face = 2;
		}
		else {
			::AfxMessageBox(_T("A face must be defined"));
			r_bc.face_defined = 0;
			pDX->Fail();
		}
	}
	else
	{
		if (r_bc.face_defined) {
			switch (r_bc.face) {
				case 0:
					pDX->m_pDlgWnd->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
					break;
				case 1:
					pDX->m_pDlgWnd->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Y_RADIO);
					break;
				case 2:
					pDX->m_pDlgWnd->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Z_RADIO);
					break;
				default:
					ASSERT(FALSE);
					// set x as default
					pDX->m_pDlgWnd->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
					break;
			}
		}
		else {
			// check x by default
			pDX->m_pDlgWnd->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
		}
	}
}

void CGlobal::DDX_Property(CDataExchange* pDX, CCheckTreeCtrl* pTree, HTREEITEM hti, Cproperty* value, Cproperty* fixed, Cproperty* linear)
{
	/*
	IDC_SINGLE_VALUE_RADIO
	IDC_LINEAR_INTERPOLATION_RADIO
	IDC_SINGLE_VALUE_STATIC
	IDC_SINGLE_VALUE_EDIT
	IDC_X_RADIO
	IDC_Y_RADIO
	IDC_Z_RADIO
	IDC_DISTANCE1_STATIC
	IDC_DISTANCE1_EDIT
	IDC_VALUE1_STATIC
	IDC_VALUE1_EDIT
	IDC_DISTANCE2_STATIC
	IDC_DISTANCE2_EDIT
	IDC_VALUE2_STATIC
	IDC_VALUE2_EDIT
	*/
	// resource must have these values
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_SINGLE_VALUE_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_SINGLE_VALUE_EDIT));

	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_LINEAR_INTERPOLATION_RADIO));
	//{{
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_STATIC_DIRECTION));
	//}}	
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_X_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_Y_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_Z_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_VALUE1_EDIT));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_DISTANCE1_EDIT));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_VALUE1_EDIT));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_DISTANCE2_EDIT));

	// Note: pTree->GetItemData(hti) stores the last defined type
	//
	ASSERT(pTree && ::IsWindow(pTree->m_hWnd));

	if (pDX->m_bSaveAndValidate)
	{
		// get type
		//
		if (pDX->m_pDlgWnd->IsDlgButtonChecked(IDC_SINGLE_VALUE_RADIO))
		{
			value->type = FIXED;
		}
		else if (pDX->m_pDlgWnd->IsDlgButtonChecked(IDC_LINEAR_INTERPOLATION_RADIO))
		{
			value->type = LINEAR;
		}
		else
		{
			ASSERT(FALSE);
		}

		// check if defining
		//
		if (!(pTree->GetCheck(hti) == BST_CHECKED))
		{
			// save last type
			//
			pTree->SetItemData(hti, value->type);
			value->type = UNDEFINED;
			return; // nothing else to do
		}

		// validate single
		//
		if (value->type == FIXED)
		{
			DDX_Text(pDX, IDC_SINGLE_VALUE_EDIT, value->v[0]);
		}

		// validate linear
		//
		if (value->type == LINEAR)
		{
			int nDirection = pDX->m_pDlgWnd->GetCheckedRadioButton(IDC_X_RADIO, IDC_Z_RADIO);
			value->coord = 'x';
			if (nDirection == IDC_X_RADIO)
			{
				value->coord = 'x';
			}
			else if (nDirection == IDC_Y_RADIO)
			{
				value->coord = 'y';
			}
			else if (nDirection == IDC_Z_RADIO)
			{
				value->coord = 'z';
			}
			else
			{
				ASSERT(FALSE);
			}
			DDX_Text(pDX, IDC_VALUE1_EDIT, value->v[0]);
			DDX_Text(pDX, IDC_DISTANCE1_EDIT, value->dist1);
			DDX_Text(pDX, IDC_VALUE2_EDIT, value->v[1]);
			DDX_Text(pDX, IDC_DISTANCE2_EDIT, value->dist2);
		}

		// all ok so copy 
		if (value->type == FIXED)
		{
			value->count_v = 1;
			(*fixed) = (*value);
		}
		else if (value->type == LINEAR)
		{
			value->count_v = 2;
			(*linear) = (*value);
		}
#ifdef _DEBUG
		value->AssertValid();
#endif
	}
	else
	{
		// fixed
		//
		if (fixed->type == FIXED)
		{
			DDX_Text(pDX, IDC_SINGLE_VALUE_EDIT, fixed->v[0]);
		}
		else
		{
			CString strEmpty("");
			DDX_Text(pDX, IDC_SINGLE_VALUE_EDIT, strEmpty);
		}

		// linear
		//
		if (linear->type == LINEAR)
		{
			// set direction
			//
			switch (linear->coord)
			{
				case 'x':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_X_RADIO);
					break;
				case 'y':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Y_RADIO);
					break;
				case 'z':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Z_RADIO);
					break;
				default:
					ASSERT(FALSE);
					break;
			}
			// set values and distances
			//
			DDX_Text(pDX, IDC_VALUE1_EDIT, linear->v[0]);
			DDX_Text(pDX, IDC_DISTANCE1_EDIT, linear->dist1);
			DDX_Text(pDX, IDC_VALUE2_EDIT, linear->v[1]);
			DDX_Text(pDX, IDC_DISTANCE2_EDIT, linear->dist2);
		}
		else
		{
			// set direction default to x
			//
			pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_X_RADIO);

			// empty values and distances
			//
			CString strEmpty("");
			DDX_Text(pDX, IDC_VALUE1_EDIT, strEmpty);
			DDX_Text(pDX, IDC_DISTANCE1_EDIT, strEmpty);
			DDX_Text(pDX, IDC_VALUE2_EDIT, strEmpty);
			DDX_Text(pDX, IDC_DISTANCE2_EDIT, strEmpty);
		}

		int type = value->type;
		switch (type)
		{
		case UNDEFINED:
			switch (pTree->GetItemData(hti))
			{
				case FIXED:
					// check fixed value radio button
					pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_SINGLE_VALUE_RADIO);
					type = FIXED;
					break;

				case LINEAR:
					// check linear radio button
					pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_LINEAR_INTERPOLATION_RADIO);
					type = LINEAR;
					break;

				default:
					// check fixed value radio button as default
					pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_SINGLE_VALUE_RADIO);
					type = FIXED;
					break;
			}
			break;

		case FIXED:
			// check fixed value radio button
			pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_SINGLE_VALUE_RADIO);
			break;

		case LINEAR:
			// check linear radio button
			pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_LINEAR_INTERPOLATION_RADIO);
			break;
		}


		// enable/disable
		//
		if (pTree->GetCheck(hti) == BST_CHECKED)
		{
			// enable radio buttons
			//
			pDX->m_pDlgWnd->GetDlgItem(IDC_SINGLE_VALUE_RADIO)->EnableWindow(TRUE);
			pDX->m_pDlgWnd->GetDlgItem(IDC_LINEAR_INTERPOLATION_RADIO)->EnableWindow(TRUE);

			switch (type)
			{
				case FIXED:
					CGlobal::EnableFixed(pDX->m_pDlgWnd, TRUE);
					CGlobal::EnableLinearInterpolation(pDX->m_pDlgWnd, FALSE);
					break;
				case LINEAR:
					CGlobal::EnableFixed(pDX->m_pDlgWnd, FALSE);
					CGlobal::EnableLinearInterpolation(pDX->m_pDlgWnd, TRUE);
					break;
				default:
					ASSERT(FALSE);
			}
		}
		else
		{
			// disable fixed input
			//
			CGlobal::EnableFixed(pDX->m_pDlgWnd, FALSE);

			// disable linear input
			//
			CGlobal::EnableLinearInterpolation(pDX->m_pDlgWnd, FALSE);

			// disable radio buttons
			//
			pDX->m_pDlgWnd->GetDlgItem(IDC_SINGLE_VALUE_RADIO)->EnableWindow(FALSE);
			pDX->m_pDlgWnd->GetDlgItem(IDC_LINEAR_INTERPOLATION_RADIO)->EnableWindow(FALSE);
		}
	}
}

void CGlobal::EnableFixed(CWnd* pDlgWnd, BOOL bEnable)
{
	pDlgWnd->GetDlgItem(IDC_SINGLE_VALUE_STATIC)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_SINGLE_VALUE_EDIT)->EnableWindow(bEnable);
}

void CGlobal::EnableLinearInterpolation(CWnd* pDlgWnd, BOOL bEnable)
{
	//{{
	pDlgWnd->GetDlgItem(IDC_STATIC_DIRECTION)->EnableWindow(bEnable);
	//}}
	pDlgWnd->GetDlgItem(IDC_X_RADIO)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_Y_RADIO)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_Z_RADIO)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_DISTANCE1_STATIC)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_DISTANCE1_EDIT)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_VALUE1_STATIC)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_VALUE1_EDIT)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_DISTANCE2_STATIC)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_DISTANCE2_EDIT)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_VALUE2_STATIC)->EnableWindow(bEnable);
	pDlgWnd->GetDlgItem(IDC_VALUE2_EDIT)->EnableWindow(bEnable);

}

void CGlobal::DDX_AssocFixed(CDataExchange* pDX, CTreeCtrl* pTree, HTREEITEM hti, CBC& value)
{
	// resource must have these values
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_SOL_TYPE_STATIC));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_ASSOC_RADIO));
	ASSERT(pDX->m_pDlgWnd->GetDlgItem(IDC_FIXED_RADIO));

	ASSERT(pTree && ::IsWindow(pTree->m_hWnd));

	if (pDX->m_bSaveAndValidate)
	{
		// get radio button
		//
		switch (pDX->m_pDlgWnd->GetCheckedRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO))
		{
			case IDC_ASSOC_RADIO:
				value.bc_solution_type = ASSOCIATED;
				break;
			case IDC_FIXED_RADIO:
				value.bc_solution_type = FIXED;
				break;
			default:
				ASSERT(FALSE);
				value.bc_solution_type = ASSOCIATED;
				break;
		}
	}
	else
	{
		// check radio button
		//
		if (value.bc_solution_type == ASSOCIATED)
		{
			pDX->m_pDlgWnd->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_ASSOC_RADIO);
		}
		else if (value.bc_solution_type == FIXED)
		{
			pDX->m_pDlgWnd->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_FIXED_RADIO);
		}
		else
		{
			pDX->m_pDlgWnd->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_ASSOC_RADIO);
		}

		// enable/disable
		//
		if (pTree->GetCheck(hti) == BST_CHECKED)
		{
			// enable radio buttons
			//
			pDX->m_pDlgWnd->GetDlgItem(IDC_SOL_TYPE_STATIC)->EnableWindow(TRUE);
			pDX->m_pDlgWnd->GetDlgItem(IDC_ASSOC_RADIO)->EnableWindow(TRUE);
			pDX->m_pDlgWnd->GetDlgItem(IDC_FIXED_RADIO)->EnableWindow(TRUE);
		}
		else
		{
			// disable radio buttons
			//
			pDX->m_pDlgWnd->GetDlgItem(IDC_SOL_TYPE_STATIC)->EnableWindow(FALSE);
			pDX->m_pDlgWnd->GetDlgItem(IDC_ASSOC_RADIO)->EnableWindow(FALSE);
			pDX->m_pDlgWnd->GetDlgItem(IDC_FIXED_RADIO)->EnableWindow(FALSE);
		}
	}
}

herr_t CGlobal::HDFSerializeString(bool bStoring, hid_t loc_id, const char* szName, std::string& str)
{
	herr_t status = -1;
	if (bStoring)
	{
		if (!str.empty())
		{
			char *szCopy = new char[str.length() + 2];
			if (szCopy != NULL)
			{
				::strcpy(szCopy, str.c_str());
				status = HDFSerializeStringOrNull(bStoring, loc_id, szName, &szCopy);
				delete[] szCopy;
			}
		}
	}
	else
	{
		char *szCopy = NULL;
		status = HDFSerializeStringOrNull(bStoring, loc_id, szName, &szCopy);
		if (szCopy == NULL)
		{
			str.clear();
		}
		else
		{
			str = szCopy;	
			delete[] szCopy;
		}
	}
	return status;
}

herr_t CGlobal::HDFSerializeStringOrNull(bool bStoring, hid_t loc_id, const char* szName, char** szString)
{
	hsize_t dims[1];

	hid_t dspace_id = 0;
	hid_t dset_id = 0;
	hid_t dtype_id = 0;
	herr_t status = -1;
	herr_t return_status = -1;

	if (bStoring)
	{
		if (*szString == NULL)
		{
			return -1;
		}
		size_t len = ::strlen(*szString);

		// create fixed length string type
		dtype_id = ::H5Tcopy(H5T_C_S1);
		if (dtype_id < 0) goto ErrorHDFSerializeString;

		status = ::H5Tset_strpad(dtype_id, H5T_STR_NULLTERM);
		ASSERT(status >= 0);
		if (status < 0) goto ErrorHDFSerializeString;

		status = ::H5Tset_size(dtype_id, len + 1);
		ASSERT(status >= 0);
		if (status < 0) goto ErrorHDFSerializeString;

		// create the dataspace
		dims[0] = 1;
		dspace_id = H5Screate_simple(1, dims, NULL);
		ASSERT(dspace_id > 0);
		if (dspace_id <= 0) goto ErrorHDFSerializeString;

		// create the dataset
		dset_id = H5Dcreate(loc_id, szName, dtype_id, dspace_id, H5P_DEFAULT);
		ASSERT(dset_id > 0);
		if (dset_id <= 0) goto ErrorHDFSerializeString;

		// write the dataset
		status = H5Dwrite(dset_id, dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, *szString);
		ASSERT(status >= 0);
	}
	else
	{
		// loading
		dset_id = ::H5Dopen(loc_id, szName);
		if (dset_id <= 0)
		{
			if (*szString) delete *szString;
			*szString = NULL;
			return -1; // not found
		}

		dspace_id = ::H5Dget_space(dset_id);
		if (dspace_id < 0) goto ErrorHDFSerializeString;
		if (::H5Sget_simple_extent_dims(dspace_id, dims, NULL) < 0) goto ErrorHDFSerializeString;
		ASSERT(dims[0] == 1);

		dtype_id = ::H5Dget_type(dset_id);
		if (dtype_id < 0) goto ErrorHDFSerializeString;

		if (H5Tget_strpad(dtype_id) == H5T_STR_NULLTERM)
		{
			// fixed length strings
			size_t len = ::H5Tget_size(dtype_id);
			ASSERT(len * dims[0]);

			if (*szString) delete *szString;
			*szString = new char[(size_t)(len * dims[0])];
			ASSERT(*szString);

			status = ::H5Dread(dset_id, dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, *szString);
			ASSERT(status >= 0);
		}
	}

ErrorHDFSerializeString:

	if (dtype_id > 0)
	{
		status = ::H5Tclose(dtype_id);
		ASSERT(status >= 0);
	}

	if (dspace_id > 0)
	{
		status = ::H5Sclose(dspace_id);
		ASSERT(status >= 0);
	}

	if (dset_id > 0)
	{
		status = ::H5Dclose(dset_id);
		ASSERT(status >= 0);
	}

	return status;
}

void CGlobal::MinimizeLengthUnits(CString& strUnits)
{
	strUnits.MakeLower();
	strUnits.Replace("inches", "in");
	strUnits.Replace("inch", "in");
	strUnits.Replace("feet", "ft");
	strUnits.Replace("miles", "mile");
	strUnits.Replace("meters", "m");
	strUnits.Replace("kilo", "k");
	strUnits.Replace("centi", "c");
	strUnits.Replace("milli", "m");
}

static const char* s_length_units[] = {"inches", "ft", "miles", "millimeters", "centimeters", "meters", "kilometers"};

int CGlobal::AddLengthUnits(CComboBox* pCombo)
{
#ifdef _DEBUG
	CUnits units;
#endif

	for (size_t i = 0; i < sizeof(s_length_units) / sizeof(s_length_units[0]); ++i) {
#ifdef _DEBUG
		ASSERT(units.horizontal.set_input(s_length_units[i]) == OK);
#endif
		pCombo->InsertString(-1, s_length_units[i]);
	}
	return 0;
}

std::string CGlobal::GetStdLengthUnits(const char* unit)
{
	CUnits units_std;
	CUnits units_usr;

	VERIFY(units_usr.horizontal.set_input(unit) == OK);
	for (size_t i = 0; i < sizeof(s_length_units) / sizeof(s_length_units[0]); ++i)
	{
		units_std.horizontal.set_input(s_length_units[i]);
		if (units_usr.horizontal.input_to_si == units_std.horizontal.input_to_si)
		{
			return s_length_units[i];
		}
	}
	ASSERT(FALSE);
	return s_length_units[0];
}

static const char* s_time_units[] = {"seconds", "minutes", "hours", "days", "years"};

int CGlobal::AddTimeUnits(CComboBox* pCombo)
{
#ifdef _DEBUG
	CUnits units;
#endif
	for (size_t i = 0; i < sizeof(s_time_units) / sizeof(s_time_units[0]); ++i)
	{
#ifdef _DEBUG
		ASSERT(units.time.set_input(s_time_units[i]) == OK);
#endif
		pCombo->InsertString(-1, s_time_units[i]);
	}
	return 0;
}

std::string CGlobal::GetStdTimeUnits(const char* unit)
{
	CUnits units_std;
	CUnits units_usr;

	VERIFY(units_usr.time.set_input(unit) == OK);
	for (size_t i = 0; i < sizeof(s_time_units) / sizeof(s_time_units[0]); ++i)
	{
		units_std.time.set_input(s_time_units[i]);
		if (units_usr.time.input_to_si == units_std.time.input_to_si)
		{
			return s_time_units[i];
		}
	}
	ASSERT(FALSE);
	return s_time_units[0];
}


static const char* s_volume_units[] = {"gallon", "liter", "ft^3", "meters^3"};

int CGlobal::AddVolumeUnits(CComboBox* pCombo)
{
#ifdef _DEBUG
	CUnits units;
#endif
	for (size_t i = 0; i < sizeof(s_volume_units) / sizeof(s_volume_units[0]); ++i)
	{
#ifdef _DEBUG
		std::string str(s_volume_units[i]);
		str.append("/s");
		ASSERT(units.well_pumpage.set_input(str.c_str()) == OK);
#endif
		pCombo->InsertString(-1, s_volume_units[i]);
	}
	return 0;
}

std::string CGlobal::GetStdVolumeUnits(const char* unit)
{
	CUnits units_std;
	CUnits units_usr;

	std::string units_per_second(unit);
	units_per_second += "/s";

	VERIFY(units_usr.well_pumpage.set_input(units_per_second.c_str()) == OK);
	for (size_t i = 0; i < sizeof(s_time_units) / sizeof(s_time_units[0]); ++i)
	{
		units_per_second = s_volume_units[i];
		units_per_second += "/s";
		units_std.well_pumpage.set_input(units_per_second.c_str());
		if (units_usr.well_pumpage.input_to_si == units_std.well_pumpage.input_to_si)
		{
			return s_volume_units[i];
		}
	}
	ASSERT(FALSE);
	return s_volume_units[0];
}

static const char* s_time_units_denom[] = {"second", "minute", "hour", "day", "year"};

int CGlobal::AddTimeUnitsDenom(CComboBox* pCombo)
{
#ifdef _DEBUG
	CUnits units;
#endif
	for (size_t i = 0; i < sizeof(s_time_units_denom) / sizeof(s_time_units_denom[0]); ++i)
	{
#ifdef _DEBUG
		ASSERT(units.time.set_input(s_time_units_denom[i]) == OK);
#endif
		pCombo->InsertString(-1, s_time_units_denom[i]);
	}
	return 0;
}

std::string CGlobal::GetStdTimeUnitsDenom(const char* unit)
{
	CUnits units_std;
	CUnits units_usr;

	VERIFY(units_usr.time.set_input(unit) == OK);
	for (size_t i = 0; i < sizeof(s_time_units_denom) / sizeof(s_time_units_denom[0]); ++i)
	{
		units_std.time.set_input(s_time_units_denom[i]);
		if (units_usr.time.input_to_si == units_std.time.input_to_si)
		{
			return s_time_units_denom[i];
		}
	}
	ASSERT(FALSE);
	return s_time_units_denom[0];
}

static const char* s_length_units_denom[] = {"inch", "foot", "mile", "millimeter", "centimeter", "meter", "kilometer"};

int CGlobal::AddLengthUnitsDenom(CComboBox* pCombo)
{
#ifdef _DEBUG
	CUnits units;
#endif

	for (size_t i = 0; i < sizeof(s_length_units_denom) / sizeof(s_length_units_denom[0]); ++i)
	{
#ifdef _DEBUG
		ASSERT(units.horizontal.set_input(s_length_units_denom[i]) == OK);
#endif
		pCombo->InsertString(-1, s_length_units_denom[i]);
	}
	return 0;
}

std::string CGlobal::GetStdLengthUnitsDenom(const char* unit)
{
	CUnits units_std;
	CUnits units_usr;

	VERIFY(units_usr.horizontal.set_input(unit) == OK);
	for (size_t i = 0; i < sizeof(s_length_units_denom) / sizeof(s_length_units_denom[0]); ++i)
	{
		units_std.horizontal.set_input(s_length_units_denom[i]);
		if (units_usr.horizontal.input_to_si == units_std.horizontal.input_to_si)
		{
			return s_length_units_denom[i];
		}
	}
	ASSERT(FALSE);
	return s_length_units_denom[0];
}

herr_t CGlobal::HDFSerializeBool(bool bStoring, hid_t loc_id, const char* szName, bool& bValue)
{
	int nValue;

	if (bStoring)
	{
		if (bValue)
		{
			nValue = 1;
		}
		else
		{
			nValue = 0;
		}
	}

	herr_t status = CGlobal::HDFSerialize(bStoring, loc_id, szName, H5T_NATIVE_INT, 1, &nValue);
	ASSERT(status >= 0);

	if (!bStoring && status >= 0)
	{
		bValue = (nValue != 0);
	}
	return status;
}

bool CGlobal::GetEditValue(CWnd *pDlg, int nID, double& d)
{
	static TCHAR buffer[80];
	int cc = pDlg->GetDlgItemText(nID, buffer, sizeof(buffer) /* /sizeof(TCHAR) */);
	return (cc && CGlobal::SimpleFloatParse(buffer, d));
}

bool CGlobal::SimpleFloatParse(LPCTSTR lpszText, double& d)
{
	// strtod expects nptr to point to a string of the following form:
	//
	// [whitespace] [sign] [digits] [.digits] [ {d | D | e | E}[sign]digits]
	//
	// A whitespace may consist of space and tab characters, which are ignored;
	// sign is either plus (+) or minus (–); and digits are one or more decimal
	// digits. If no digits appear before the radix character, at least one must
	// appear after the radix character. The decimal digits can be followed by an
	// exponent, which consists of an introductory letter (d, D, e, or E) and an
	// optionally signed integer. If neither an exponent part nor a radix 
	// character appears, a radix character is assumed to follow the last digit
	// in the string. The first character that does not fit this form stops the
	// scan.

	ASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t') // TODO iswhite
		lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')  // TODO what about .0[000...]
		return false;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != '\0')
		return false;   // not terminated properly

	return true;
}

int CGlobal::LoadWorldFile(const char *filename, CWorldTransform &wtrans)
{
	/////////////////////////////////////////////////
	//from http://support.esri.com
	//Article ID: 17489 
	//Software:  ArcGIS - ArcInfo 8.0.1, 8.0.2 ArcInfo Workstation 7.0.4, 7.1.1, 7.1.2, 7.2.1 ArcView GIS 3.0, 3.0a, 3.0b, 3.1, 3.2, 3.2a 
	//Platforms: N/A 
	//
	//Question
	//What is the format of the world file used for georeferencing images?
	//
	//
	//Answer
	//The world file is an ASCII text file associated with an image and contains the following lines: 
	//
	//Line 1: x-dimension of a pixel in map units 
	//Line 2: rotation parameter 
	//Line 3: rotation parameter 
	//Line 4: NEGATIVE of y-dimension of a pixel in map units 
	//Line 5: x-coordinate of center of upper left pixel 
	//Line 6: y-coordinate of center of upper left pixel 
	/////////////////////////////////////////////////

	std::ifstream ifs;
	ifs.open(filename);

	if (!ifs.is_open()) {
		throw "Unable to open world file";
	};

	double dataSpacing[3];
	double rotation[2];

	ifs >> dataSpacing[0];
	ifs >> rotation[0];
	ifs >> rotation[1];

	ASSERT(rotation[0] == 0.0);
	ASSERT(rotation[1] == 0.0);
	
	ifs >> dataSpacing[1];
	ASSERT(dataSpacing[1] < 0.0);
	dataSpacing[1] = fabs(dataSpacing[1]);
	dataSpacing[2] = 1.0;

	double upperLeft[3];
	ifs >> upperLeft[0];
	ifs >> upperLeft[1];
	upperLeft[2] = 0;

	wtrans.SetUpperLeft(upperLeft);
	wtrans.SetDataSpacing(dataSpacing);

	return 0; // success
}

herr_t CGlobal::HDFSerializeBinaryFile(bool bStoring, hid_t loc_id, const char* szName, const char* szFileName)
{
	hsize_t dims[1];

	hid_t   dspace_id = 0;
	hid_t   dset_id = 0;
	herr_t  status = -1;
	herr_t  return_status = -1;

	char   *bigbuffer = 0;

	if (bStoring)
	{
		// determine size of file
		std::ifstream::pos_type b, e;
		///std::ifstream infile(szFileName, std::ios::in | std::ios::binary);
		std::ifstream infile;
		infile.open(szFileName, std::ios_base::in | std::ios_base::binary);
		if (!infile.is_open()) goto ErrorHDFSerializeBinaryFile;

		b = infile.tellg();
		infile.seekg(0, std::ios::end);
		e = infile.tellg();
		//{{
		///infile.seekg(0, ios_base::beg);
		//}}
		infile.close();

		// read file into buffer
		bigbuffer = new char[e - b];
		ASSERT(bigbuffer);
		if (!bigbuffer) goto ErrorHDFSerializeBinaryFile;

		infile.open(szFileName, std::ios_base::in | std::ios_base::binary);
		infile.read(bigbuffer, e - b);

		// create the dataspace
		dims[0] = e - b;
		dspace_id = H5Screate_simple(1, dims, NULL);
		ASSERT(dspace_id >= 0);
		if (dspace_id <= 0) goto ErrorHDFSerializeBinaryFile;

		// create the dataset
		dset_id = H5Dcreate(loc_id, szName, H5T_NATIVE_CHAR, dspace_id, H5P_DEFAULT);
		ASSERT(dset_id > 0);
		if (dset_id <= 0) goto ErrorHDFSerializeBinaryFile;

		// write the dataset
		status = H5Dwrite(dset_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, bigbuffer);
		ASSERT(status >= 0);
	}
	else
	{
		// loading
		dset_id = ::H5Dopen(loc_id, szName);
		ASSERT(dset_id > 0);
		if (dset_id <= 0) goto ErrorHDFSerializeBinaryFile;

		dspace_id = ::H5Dget_space(dset_id);
		if (dspace_id < 0) goto ErrorHDFSerializeBinaryFile;
		if (::H5Sget_simple_extent_dims(dspace_id, dims, NULL) < 0) goto ErrorHDFSerializeBinaryFile;

		bigbuffer = new char[(size_t)dims[0]];
		ASSERT(bigbuffer != 0);
		if (!bigbuffer) goto ErrorHDFSerializeBinaryFile;

		status = ::H5Dread(dset_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, bigbuffer);
		ASSERT(status >= 0);
		if (status < 0) goto ErrorHDFSerializeBinaryFile;

		std::fstream outfile;
		outfile.open(szFileName, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
		outfile.write(bigbuffer, (std::streamsize)dims[0]);
		outfile.close();
	}

ErrorHDFSerializeBinaryFile:

	if (dspace_id > 0)
	{
		status = ::H5Sclose(dspace_id);
		ASSERT(status >= 0);
	}
	if (dset_id > 0)
	{
		status = ::H5Dclose(dset_id);
		ASSERT(status >= 0);
	}
	if (bigbuffer)
	{
		delete[] bigbuffer;
	}
	return status;
}

void CGlobal::DDX_GridTimeSeries(CDataExchange* pDX, int nIDC, CTimeSeries<Cproperty> &r_ts)
{
	CModGridCtrlEx* pGrid = static_cast<CModGridCtrlEx*>(pDX->m_pDlgWnd->GetDlgItem(nIDC));
	if (!pGrid) return;

	if (pDX->m_bSaveAndValidate)
	{
		CTimeSeries<Cproperty> ts;

		for (int iRow = pGrid->GetFixedRowCount(); iRow < pGrid->GetRowCount(); ++iRow)
		{
			// time start
			//
			CString start;
			::DDX_TextGridControl(pDX, nIDC, iRow, 0, start);
			if (start.IsEmpty())
			{
				continue;
			}

			double d;
			::DDX_TextGridControl(pDX, nIDC, iRow, 0, d);
			Ctime t;
			t.type = UNITS;
			t.SetValue(d);

			Cproperty p;
			if (pGrid->GetCheck(iRow, 3) != BST_CHECKED)
			{
				if (iRow == 1)
				{
					start.Empty();
					::DDX_TextGridControl(pDX, nIDC, iRow, 2, start);
					if (start.IsEmpty())
					{
						continue;
					}
				}

				// value
				//
				::DDX_TextGridControl(pDX, nIDC, iRow, 2, p.v[0]);

				p.type    = FIXED;
				p.count_v = 1;
				ts.insert(CTimeSeries<Cproperty>::value_type(t, p));
			}
			else
			{
				// direction
				//
				if (iRow == 1)
				{
					start.Empty();
					::DDX_TextGridControl(pDX, nIDC, iRow, 4, start);
					if (start.IsEmpty())
					{
						continue;
					}
				}

				// direction
				//
				::DDX_TextGridControl(pDX, nIDC, iRow, 4, start);
				start.Trim();
				if (start.IsEmpty())
				{
					continue;
				}
				else
				{
					if (start[0] == 'X' || start[0] == 'x')
					{
						p.coord = 'x';
					}
					else if (start[0] == 'Y' || start[0] == 'y')
					{
						p.coord = 'y';
					}
					else if (start[0] == 'Z' || start[0] == 'z')
					{
						p.coord = 'z';
					}
					else
					{
						ASSERT(FALSE);
						p.coord = 'x';
					}
				}

				// value 1
				//
				::DDX_TextGridControl(pDX, nIDC, iRow, 5, p.v[0]);

				// dist 1
				//
				::DDX_TextGridControl(pDX, nIDC, iRow, 6, p.dist1);

				// value 2
				//
				::DDX_TextGridControl(pDX, nIDC, iRow, 7, p.v[1]);

				// dist 2
				//
				::DDX_TextGridControl(pDX, nIDC, iRow, 8, p.dist2);

				p.type    = LINEAR;
				p.count_v = 2;
				ts.insert(CTimeSeries<Cproperty>::value_type(t, p));
			}
		}
		r_ts = ts;
	}
	else
	{
		CTimeSeries<Cproperty>::iterator iter = r_ts.begin();
		for (int nRow = 1; iter != r_ts.end(); ++iter, ++nRow)
		{
			// time start
			Ctime t(iter->first);
			::DDX_TextGridControl(pDX, nIDC, nRow, 0, t.value);

			// head
			if (iter->second.type == FIXED)
			{
				ASSERT(iter->second.count_v == 1);
				::DDX_TextGridControl(pDX, nIDC, nRow, 2, iter->second.v[0]);
			}
			else if (iter->second.type == LINEAR)
			{
				ASSERT(iter->second.count_v == 2);

				// set checkmark
				//
				pGrid->SetCheck(nRow, 3, BST_CHECKED);
				pGrid->DisableCell(nRow, 2);
				for (int nCol = 4; nCol < 9; ++nCol)
				{
					pGrid->EnableCell(nRow, nCol);
				}
				
				// direction
				CString dir(iter->second.coord);
				dir.MakeUpper();
				::DDX_TextGridControl(pDX, nIDC, nRow, 4, dir);

				// value 1
				::DDX_TextGridControl(pDX, nIDC, nRow, 5, iter->second.v[0]);

				// dist 1
				::DDX_TextGridControl(pDX, nIDC, nRow, 6, iter->second.dist1);

				// value 2
				::DDX_TextGridControl(pDX, nIDC, nRow, 7, iter->second.v[1]);

				// dist 2
				::DDX_TextGridControl(pDX, nIDC, nRow, 8, iter->second.dist2);
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}
}
