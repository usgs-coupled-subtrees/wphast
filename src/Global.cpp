#include "StdAfx.h"
#include "Global.h"
#include "resource.h"

#include <fstream>

#include "WPhastDoc.h"
#include "WPhastView.h"

#include "BC.h"
#include "ChemIC.h"
#include "HeadIC.h"

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
#include "srcinput/Data_source.h"
#include "srcinput/Prism.h"
#include "TreePropSheetExSRC.h"
#include "MediaPropsPage.h"
#include "FluxPropsPage.h"
#include "FakeFiledata.h"
#include "GridKeyword.h"


#include <afxmt.h>
#include <atlpath.h> // ATL::ATLPath::FileExists
static CCriticalSection  critSect;

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
// COMMENT: {9/8/2009 9:04:42 PM}	ASSERT(FALSE); // deprecated use CZoneActor->Select instead
// COMMENT: {9/8/2009 9:04:42 PM}
// COMMENT: {9/8/2009 9:04:42 PM}	vtkRenderer* pRenderer = pView->GetRenderer();
// COMMENT: {9/8/2009 9:04:42 PM}	vtkRenderWindowInteractor* pRenderWindowInteractor = pView->GetRenderWindowInteractor();
// COMMENT: {9/8/2009 9:04:42 PM}
// COMMENT: {9/8/2009 9:04:42 PM}	////CGlobal::PickProp(pProp3D, pView->GetRenderer(), pView->GetRenderWindowInteractor());
// COMMENT: {9/8/2009 9:04:42 PM}
// COMMENT: {9/8/2009 9:04:42 PM}	if (CZoneActor* pZone = CZoneActor::SafeDownCast(pProp3D))
// COMMENT: {9/8/2009 9:04:42 PM}	{
// COMMENT: {9/8/2009 9:04:42 PM}		pZone->Select(pView);
// COMMENT: {9/8/2009 9:04:42 PM}	}
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
	double *bounds = pProp3D->GetBounds();
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

herr_t CGlobal::HDFSerializeAllocate(bool bStoring, hid_t loc_id, const char* szName, hsize_t& count, float** buffer)
{
	return CGlobal::HDFSerializeAllocate<float>(bStoring, loc_id, szName, H5T_NATIVE_FLOAT, count, buffer);
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
			value->type = PROP_FIXED;
		}
		else if (pDX->m_pDlgWnd->IsDlgButtonChecked(IDC_LINEAR_INTERPOLATION_RADIO))
		{
			value->type = PROP_LINEAR;
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
			value->type = PROP_UNDEFINED;
			return; // nothing else to do
		}

		// validate single
		//
		if (value->type == PROP_FIXED)
		{
			DDX_Text(pDX, IDC_SINGLE_VALUE_EDIT, value->v[0]);
		}

		// validate linear
		//
		if (value->type == PROP_LINEAR)
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
		if (value->type == PROP_FIXED)
		{
			value->count_v = 1;
			(*fixed) = (*value);
		}
		else if (value->type == PROP_LINEAR)
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
		if (fixed->type == PROP_FIXED)
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
		if (linear->type == PROP_LINEAR)
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

// COMMENT: {8/13/2008 9:37:00 PM}		int type = value->type;
		switch (value->type)
		{
		case PROP_UNDEFINED:
			switch (pTree->GetItemData(hti))
			{
				case PROP_FIXED:
					// check fixed value radio button
					pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_SINGLE_VALUE_RADIO);
// COMMENT: {8/13/2008 9:44:07 PM}					type = PROP_FIXED;
					break;

				case PROP_LINEAR:
					// check linear radio button
					pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_LINEAR_INTERPOLATION_RADIO);
// COMMENT: {8/13/2008 9:44:11 PM}					type = PROP_LINEAR;
					break;

				default:
					// check fixed value radio button as default
					pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_SINGLE_VALUE_RADIO);
// COMMENT: {8/13/2008 9:44:15 PM}					type = PROP_FIXED;
					break;
			}
			break;

		case PROP_FIXED:
			// check fixed value radio button
			pDX->m_pDlgWnd->CheckRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO, IDC_SINGLE_VALUE_RADIO);
			break;

		case PROP_LINEAR:
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

			switch (value->type)
			{
				case PROP_FIXED:
					CGlobal::EnableFixed(pDX->m_pDlgWnd, TRUE);
					CGlobal::EnableLinearInterpolation(pDX->m_pDlgWnd, FALSE);
					break;
				case PROP_LINEAR:
					CGlobal::EnableFixed(pDX->m_pDlgWnd, FALSE);
					CGlobal::EnableLinearInterpolation(pDX->m_pDlgWnd, TRUE);
					break;
				case PROP_UNDEFINED:
					CGlobal::EnableFixed(pDX->m_pDlgWnd, TRUE);
					CGlobal::EnableLinearInterpolation(pDX->m_pDlgWnd, FALSE);
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

void CGlobal::DDX_Property(CDataExchange* pDX, TreePropSheet::CTreePropSheetEx* pTreeProp, HTREEITEM hti, std::vector<Cproperty*> &props, std::vector<CPropertyPage*> &pages)
{
	const int PAGE_NONE   = 0;
	const int PAGE_FIXED  = 1;
	const int PAGE_LINEAR = 2;
	const int PAGE_POINTS = 3;
	const int PAGE_XYZ    = 4;

	Cproperty *value = props[0];
	ASSERT(value);
	ASSERT(props.size() == 5);

	// Cproperty* prop  => std::vector<Cproperty*> &props
	if (pDX->m_bSaveAndValidate)
	{
		switch (pTreeProp->GetActiveIndex())
		{
		case PAGE_NONE:
			{
				value->type = PROP_UNDEFINED;
			}
			break;
		case PAGE_FIXED:
			{
				if (!pages[PAGE_FIXED]->UpdateData(TRUE))
				{
					pDX->Fail();
				}
				CPropConstant *pConstant = dynamic_cast<CPropConstant*>(pages[PAGE_FIXED]);
				(*value) = pConstant->GetProperty();
				ASSERT(value->type == PROP_FIXED);
			}
			break;
		case PAGE_LINEAR:
			{
				if (!pages[PAGE_LINEAR]->UpdateData(TRUE))
				{
					pDX->Fail();
				}
				CPropLinear *pLinear = dynamic_cast<CPropLinear*>(pages[PAGE_LINEAR]);
				(*value) = pLinear->GetProperty();
				ASSERT(value->type == PROP_LINEAR);
			}
			break;
		case PAGE_POINTS:
			{
				if (!pages[PAGE_POINTS]->UpdateData(TRUE))
				{
					pDX->Fail();
				}
				CPropPoints *pPoints = dynamic_cast<CPropPoints*>(pages[PAGE_POINTS]);
				(*value) = pPoints->GetProperty();
				ASSERT(value->type == PROP_POINTS);
			}
			break;
		case PAGE_XYZ:
			{
				if (!pages[PAGE_XYZ]->UpdateData(TRUE))
				{
					pDX->Fail();
				}
				CPropXYZ *pXYZ = dynamic_cast<CPropXYZ*>(pages[PAGE_XYZ]);
				(*value) = pXYZ->GetProperty();
				ASSERT(value->type == PROP_XYZ);
			}
			break;
		default:
			ASSERT(FALSE);
		}

		// all ok so copy
		if (value->type == PROP_FIXED)
		{
			ASSERT(value->count_v == 1);
			(*props[PAGE_FIXED]) = (*value);
		}
		else if (value->type == PROP_LINEAR)
		{
			ASSERT(value->count_v == 2);
			(*props[PAGE_LINEAR]) = (*value);
		}
		else if (value->type == PROP_POINTS)
		{
			(*props[PAGE_POINTS]) = (*value);
		}
		else if (value->type == PROP_XYZ)
		{
			(*props[PAGE_XYZ]) = (*value);
		}

#if defined(_DEBUG)
		value->AssertValid();
#endif

	}
	else
	{
		//
		// always set properties
		//

		CPropConstant *pConstant = dynamic_cast<CPropConstant*>(pages[PAGE_FIXED]);
		pConstant->SetProperty(*props[PAGE_FIXED]);

		CPropLinear *pLinear = dynamic_cast<CPropLinear*>(pages[PAGE_LINEAR]);
		pLinear->SetProperty(*props[PAGE_LINEAR]);

		CPropPoints *pPoints = dynamic_cast<CPropPoints*>(pages[PAGE_POINTS]);
		pPoints->SetProperty(*props[PAGE_POINTS]);

		CPropXYZ *pXYZ = dynamic_cast<CPropXYZ*>(pages[PAGE_XYZ]);
		pXYZ->SetProperty(*props[PAGE_XYZ]);

		bool bSetActivePage = false;

		if (value->type == PROP_UNDEFINED)
		{
			if (pTreeProp->GetActiveIndex() != PAGE_NONE)
			{
				pTreeProp->SetActivePage(PAGE_NONE);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_NONE]->UpdateData(FALSE);
			}
		}
		else if (value->type == PROP_FIXED)
		{
			ASSERT(*props[PAGE_FIXED] == *value);
			if (pTreeProp->GetActiveIndex() != PAGE_FIXED)
			{
				pTreeProp->SetActivePage(PAGE_FIXED);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_FIXED]->UpdateData(FALSE);
			}
		}
		else if (value->type == PROP_LINEAR)
		{
			ASSERT(*props[PAGE_LINEAR] == *value);
			if (pTreeProp->GetActiveIndex() != PAGE_LINEAR)
			{
				pTreeProp->SetActivePage(PAGE_LINEAR);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_LINEAR]->UpdateData(FALSE);
			}
		}
		else if (value->type == PROP_POINTS)
		{
			ASSERT(*props[PAGE_POINTS] == *value);
			if (pTreeProp->GetActiveIndex() != PAGE_POINTS)
			{
				pTreeProp->SetActivePage(PAGE_POINTS);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_POINTS]->UpdateData(FALSE);
			}
		}
		else if (value->type == PROP_XYZ)
		{
			ASSERT(*props[PAGE_XYZ] == *value);
			if (pTreeProp->GetActiveIndex() != PAGE_XYZ)
			{
				pTreeProp->SetActivePage(PAGE_XYZ);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_XYZ]->UpdateData(FALSE);
			}
		}

		if (bSetActivePage)
		{
			//
			// Note: The above SetActivePage call causes a UpdateData(TRUE) and
			// subsequently a DoDataExchange(bSaveAndValidate := TRUE) which causes
			// the previous selected property to overwrite the newly selected property
			// so we must re-overwrite the newly selected property with the correct
			// value.
			//

			CPropConstant *pConstant = dynamic_cast<CPropConstant*>(pages[PAGE_FIXED]);
			pConstant->SetProperty(*props[PAGE_FIXED]);

			CPropLinear *pLinear = dynamic_cast<CPropLinear*>(pages[PAGE_LINEAR]);
			pLinear->SetProperty(*props[PAGE_LINEAR]);

			CPropPoints *pPoints = dynamic_cast<CPropPoints*>(pages[PAGE_POINTS]);
			pPoints->SetProperty(*props[PAGE_POINTS]);

			CPropXYZ *pXYZ = dynamic_cast<CPropXYZ*>(pages[PAGE_XYZ]);
			pXYZ->SetProperty(*props[PAGE_XYZ]);
		}
	}
}
void CGlobal::DDX_PropertyM(CDataExchange* pDX, TreePropSheet::CTreePropSheetEx* pTreeProp, HTREEITEM hti, std::vector<Cproperty*> &props, std::vector<CPropertyPage*> &pages)
{
	const int PAGE_NONE   = 0;
	const int PAGE_FIXED  = 1;
	const int PAGE_LINEAR = 2;
	const int PAGE_POINTS = 3;
	const int PAGE_XYZ    = 4;

	Cproperty *value = props[0];
	ASSERT(value);
// COMMENT: {6/3/2009 4:10:12 PM}	ASSERT(props.size() == 5);

	// Cproperty* prop  => std::vector<Cproperty*> &props
	if (pDX->m_bSaveAndValidate)
	{
		switch (pTreeProp->GetActiveIndex())
		{
		case PAGE_NONE:
			{
				value->type = PROP_UNDEFINED;
			}
			break;
		case PAGE_FIXED:
			{
				if (!pages[PAGE_FIXED]->UpdateData(TRUE))
				{
					pDX->Fail();
				}
				CPropConstantM *pConstantM = dynamic_cast<CPropConstantM*>(pages[PAGE_FIXED]);
				(*value) = pConstantM->GetProperty();
				ASSERT(value->type == PROP_FIXED);
			}
			break;
		case PAGE_LINEAR:
			{
				if (!pages[PAGE_LINEAR]->UpdateData(TRUE))
				{
					pDX->Fail();
				}
				CPropLinearM *pLinearM = dynamic_cast<CPropLinearM*>(pages[PAGE_LINEAR]);
				(*value) = pLinearM->GetProperty();
				ASSERT(value->type == PROP_LINEAR);
			}
			break;
// COMMENT: {6/3/2009 4:15:26 PM}		case PAGE_POINTS:
// COMMENT: {6/3/2009 4:15:26 PM}			{
// COMMENT: {6/3/2009 4:15:26 PM}				if (!pages[PAGE_POINTS]->UpdateData(TRUE))
// COMMENT: {6/3/2009 4:15:26 PM}				{
// COMMENT: {6/3/2009 4:15:26 PM}					pDX->Fail();
// COMMENT: {6/3/2009 4:15:26 PM}				}
// COMMENT: {6/3/2009 4:15:26 PM}				CPropPoints *pPoints = dynamic_cast<CPropPoints*>(pages[PAGE_POINTS]);
// COMMENT: {6/3/2009 4:15:26 PM}				(*value) = pPoints->GetProperty();
// COMMENT: {6/3/2009 4:15:26 PM}				ASSERT(value->type == PROP_POINTS);
// COMMENT: {6/3/2009 4:15:26 PM}			}
// COMMENT: {6/3/2009 4:15:26 PM}			break;
// COMMENT: {6/3/2009 4:15:26 PM}		case PAGE_XYZ:
// COMMENT: {6/3/2009 4:15:26 PM}			{
// COMMENT: {6/3/2009 4:15:26 PM}				if (!pages[PAGE_XYZ]->UpdateData(TRUE))
// COMMENT: {6/3/2009 4:15:26 PM}				{
// COMMENT: {6/3/2009 4:15:26 PM}					pDX->Fail();
// COMMENT: {6/3/2009 4:15:26 PM}				}
// COMMENT: {6/3/2009 4:15:26 PM}				CPropXYZ *pXYZ = dynamic_cast<CPropXYZ*>(pages[PAGE_XYZ]);
// COMMENT: {6/3/2009 4:15:26 PM}				(*value) = pXYZ->GetProperty();
// COMMENT: {6/3/2009 4:15:26 PM}				ASSERT(value->type == PROP_XYZ);
// COMMENT: {6/3/2009 4:15:26 PM}			}
// COMMENT: {6/3/2009 4:15:26 PM}			break;
		default:
			ASSERT(FALSE);
		}

		// all ok so copy
		if (value->type == PROP_FIXED)
		{
			ASSERT(value->count_v == 1);
			(*props[PAGE_FIXED]) = (*value);
		}
		else if (value->type == PROP_LINEAR)
		{
			ASSERT(value->count_v == 2);
			(*props[PAGE_LINEAR]) = (*value);
		}
// COMMENT: {6/3/2009 4:16:40 PM}		else if (value->type == PROP_POINTS)
// COMMENT: {6/3/2009 4:16:40 PM}		{
// COMMENT: {6/3/2009 4:16:40 PM}			(*props[PAGE_POINTS]) = (*value);
// COMMENT: {6/3/2009 4:16:40 PM}		}
// COMMENT: {6/3/2009 4:16:40 PM}		else if (value->type == PROP_XYZ)
// COMMENT: {6/3/2009 4:16:40 PM}		{
// COMMENT: {6/3/2009 4:16:40 PM}			(*props[PAGE_XYZ]) = (*value);
// COMMENT: {6/3/2009 4:16:40 PM}		}

#if defined(_DEBUG)
		value->AssertValid();
#endif

	}
	else
	{
		//
		// always set properties
		//

		CPropConstantM *pConstantM = dynamic_cast<CPropConstantM*>(pages[PAGE_FIXED]);
		pConstantM->SetProperty(*props[PAGE_FIXED]);

		CPropLinearM *pLinearM = dynamic_cast<CPropLinearM*>(pages[PAGE_LINEAR]);
		pLinearM->SetProperty(*props[PAGE_LINEAR]);

// COMMENT: {6/3/2009 4:13:25 PM}		CPropPoints *pPoints = dynamic_cast<CPropPoints*>(pages[PAGE_POINTS]);
// COMMENT: {6/3/2009 4:13:25 PM}		pPoints->SetProperty(*props[PAGE_POINTS]);
// COMMENT: {6/3/2009 4:13:25 PM}
// COMMENT: {6/3/2009 4:13:25 PM}		CPropXYZ *pXYZ = dynamic_cast<CPropXYZ*>(pages[PAGE_XYZ]);
// COMMENT: {6/3/2009 4:13:25 PM}		pXYZ->SetProperty(*props[PAGE_XYZ]);

		bool bSetActivePage = false;

		if (value->type == PROP_UNDEFINED)
		{
			if (pTreeProp->GetActiveIndex() != PAGE_NONE)
			{
				pTreeProp->SetActivePage(PAGE_NONE);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_NONE]->UpdateData(FALSE);
			}
		}
		else if (value->type == PROP_FIXED)
		{
			ASSERT(*props[PAGE_FIXED] == *value);
			if (pTreeProp->GetActiveIndex() != PAGE_FIXED)
			{
				pTreeProp->SetActivePage(PAGE_FIXED);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_FIXED]->UpdateData(FALSE);
			}
		}
		else if (value->type == PROP_LINEAR)
		{
			ASSERT(*props[PAGE_LINEAR] == *value);
			if (pTreeProp->GetActiveIndex() != PAGE_LINEAR)
			{
				pTreeProp->SetActivePage(PAGE_LINEAR);
				bSetActivePage = true;
			}
			else
			{
				pages[PAGE_LINEAR]->UpdateData(FALSE);
			}
		}
// COMMENT: {6/3/2009 4:14:15 PM}		else if (value->type == PROP_POINTS)
// COMMENT: {6/3/2009 4:14:15 PM}		{
// COMMENT: {6/3/2009 4:14:15 PM}			ASSERT(*props[PAGE_POINTS] == *value);
// COMMENT: {6/3/2009 4:14:15 PM}			if (pTreeProp->GetActiveIndex() != PAGE_POINTS)
// COMMENT: {6/3/2009 4:14:15 PM}			{
// COMMENT: {6/3/2009 4:14:15 PM}				pTreeProp->SetActivePage(PAGE_POINTS);
// COMMENT: {6/3/2009 4:14:15 PM}				bSetActivePage = true;
// COMMENT: {6/3/2009 4:14:15 PM}			}
// COMMENT: {6/3/2009 4:14:15 PM}			else
// COMMENT: {6/3/2009 4:14:15 PM}			{
// COMMENT: {6/3/2009 4:14:15 PM}				pages[PAGE_POINTS]->UpdateData(FALSE);
// COMMENT: {6/3/2009 4:14:15 PM}			}
// COMMENT: {6/3/2009 4:14:15 PM}		}
// COMMENT: {6/3/2009 4:14:15 PM}		else if (value->type == PROP_XYZ)
// COMMENT: {6/3/2009 4:14:15 PM}		{
// COMMENT: {6/3/2009 4:14:15 PM}			ASSERT(*props[PAGE_XYZ] == *value);
// COMMENT: {6/3/2009 4:14:15 PM}			if (pTreeProp->GetActiveIndex() != PAGE_XYZ)
// COMMENT: {6/3/2009 4:14:15 PM}			{
// COMMENT: {6/3/2009 4:14:15 PM}				pTreeProp->SetActivePage(PAGE_XYZ);
// COMMENT: {6/3/2009 4:14:15 PM}				bSetActivePage = true;
// COMMENT: {6/3/2009 4:14:15 PM}			}
// COMMENT: {6/3/2009 4:14:15 PM}			else
// COMMENT: {6/3/2009 4:14:15 PM}			{
// COMMENT: {6/3/2009 4:14:15 PM}				pages[PAGE_XYZ]->UpdateData(FALSE);
// COMMENT: {6/3/2009 4:14:15 PM}			}
// COMMENT: {6/3/2009 4:14:15 PM}		}

		if (bSetActivePage)
		{
			//
			// Note: The above SetActivePage call causes a UpdateData(TRUE) and
			// subsequently a DoDataExchange(bSaveAndValidate := TRUE) which causes
			// the previous selected property to overwrite the newly selected property
			// so we must re-overwrite the newly selected property with the correct
			// value.
			//

			CPropConstantM *pConstantM = dynamic_cast<CPropConstantM*>(pages[PAGE_FIXED]);
			pConstantM->SetProperty(*props[PAGE_FIXED]);

			CPropLinearM *pLinearM = dynamic_cast<CPropLinearM*>(pages[PAGE_LINEAR]);
			pLinearM->SetProperty(*props[PAGE_LINEAR]);

// COMMENT: {6/3/2009 4:14:20 PM}			CPropPoints *pPoints = dynamic_cast<CPropPoints*>(pages[PAGE_POINTS]);
// COMMENT: {6/3/2009 4:14:20 PM}			pPoints->SetProperty(*props[PAGE_POINTS]);
// COMMENT: {6/3/2009 4:14:20 PM}
// COMMENT: {6/3/2009 4:14:20 PM}			CPropXYZ *pXYZ = dynamic_cast<CPropXYZ*>(pages[PAGE_XYZ]);
// COMMENT: {6/3/2009 4:14:20 PM}			pXYZ->SetProperty(*props[PAGE_XYZ]);
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
				value.bc_solution_type = ST_ASSOCIATED;
				break;
			case IDC_FIXED_RADIO:
				value.bc_solution_type = ST_FIXED;
				break;
			default:
				ASSERT(FALSE);
				value.bc_solution_type = ST_ASSOCIATED;
				break;
		}
	}
	else
	{
		// check radio button
		//
		if (value.bc_solution_type == ST_ASSOCIATED)
		{
			pDX->m_pDlgWnd->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_ASSOC_RADIO);
		}
		else if (value.bc_solution_type == ST_FIXED)
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

void CGlobal::MinimizeTimeUnits(CString& strUnits)
{
	strUnits.MakeLower();
	strUnits.Replace("second", "s");
	strUnits.Replace("seconds", "s");
	strUnits.Replace("minute", "min");
	strUnits.Replace("minutes", "min");
	strUnits.Replace("hours", "hr");
	strUnits.Replace("hour", "hr");
	strUnits.Replace("day", "d");
	strUnits.Replace("days", "d");
	strUnits.Replace("year", "yr");
	strUnits.Replace("years", "yr");
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
	static CUnits units_std;
	static CUnits units_usr;

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

std::string CGlobal::GetStdTimeUnits(const char* sz_unit)
{
	cunit std("s");
	/*static*/ CUnits units_usr;

	VERIFY(units_usr.time.set_input(sz_unit) == OK);
	for (size_t i = 0; i < sizeof(s_time_units) / sizeof(s_time_units[0]); ++i)
	{
		std.set_input(s_time_units[i]);
		if (units_usr.time.input_to_si == std.input_to_si)
		{
			return s_time_units[i];
		}
	}
	ASSERT(FALSE);
	return s_time_units[0];
}

std::string CGlobal::GetStdTimeUnitsSafe(const char* sz_unit)
{
	cunit std("s");
	/*static*/ CUnits units_usr;

	if (units_usr.time.set_input(sz_unit) != OK) return "";
	for (size_t i = 0; i < sizeof(s_time_units) / sizeof(s_time_units[0]); ++i)
	{
		std.set_input(s_time_units[i]);
		if (units_usr.time.input_to_si == std.input_to_si)
		{
			return s_time_units[i];
		}
	}
	ASSERT(FALSE);
	return "";
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
	static CUnits units_std;
	static CUnits units_usr;

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
	static CUnits units_std;
	static CUnits units_usr;

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

static const char* s_solid_units[] = {"UNKNOWN", "WATER", "ROCK"};

int CGlobal::AddSolidUnits(CComboBox* pCombo)
{
	if (pCombo->GetCount() == 0)
	{
		for (size_t i = 1; i < sizeof(s_solid_units) / sizeof(s_solid_units[0]); ++i)
		{
			pCombo->InsertString(-1, s_solid_units[i]);
		}
	}
	return 0;
}

SOLID_UNITS CGlobal::GetSolidUnits(const CComboBox* pCombo)
{
	SOLID_UNITS su = WATER;
	switch (pCombo->GetCurSel())
	{
	case 0:
		su = WATER;
		break;
	case 1:
		su = ROCK;
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return su;
}

void CGlobal::SetSolidUnits(CComboBox* pCombo, SOLID_UNITS su)
{
	ASSERT(pCombo->GetCount() == 2);
	switch (su)
	{
	case WATER:
		pCombo->SetCurSel(0);
		break;
	case ROCK:
		pCombo->SetCurSel(1);
		break;
	default:
		ASSERT(FALSE);
		pCombo->SetCurSel(0);
		break;
	}
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

	herr_t status = CGlobal::HDFSerializeSafe(bStoring, loc_id, szName, H5T_NATIVE_INT, 1, &nValue);

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

int CGlobal::WriteWorldFile(const char *filename, const CWorldTransform &wtrans)
{
	std::ofstream ofs;
	ofs.open(filename);

	if (!ofs.is_open())
	{
		throw "Unable to open world file";
	};

	const double *dataSpacing = wtrans.GetDataSpacing();
	const double *upperLeft = wtrans.GetUpperLeft();

	ofs.precision(DBL_DIG);
	ofs << dataSpacing[0] << std::endl;
	ofs << "0.000000" << std::endl;
	ofs << "0.000000" << std::endl;
	ofs << -dataSpacing[1] << std::endl;
	ofs << upperLeft[0] << std::endl;
	ofs << upperLeft[1] << std::endl;

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

void CGlobal::DDX_GridTimeSeries(CDataExchange* pDX, int nIDC, CTimeSeries<Cproperty> &r_ts, bool bTimeZeroRequired)
{
	///const bool bTimeZeroRequired = true;
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
			t.type = TT_UNITS;
			t.SetValue(d);

			// time units
			//
			::DDX_TextGridControl(pDX, nIDC, iRow, 1, start);
			if (!start.IsEmpty())
			{
				t.SetInput(start);
			}

			Cproperty p;
			if (pGrid->GetCheck(iRow, 3) != BST_CHECKED)
			{
				if (!bTimeZeroRequired)
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
				}

				// value
				//
				::DDX_TextGridControl(pDX, nIDC, iRow, 2, p.v[0]);

				p.type    = PROP_FIXED;
				p.count_v = 1;
				ts[t] = p;
			}
			else
			{
				// direction
				//
				if (!bTimeZeroRequired)
				{
					if (iRow == 1)
					{
						start.Empty();
						::DDX_TextGridControl(pDX, nIDC, iRow, 4, start);
						if (start.IsEmpty())
						{
							continue;
						}
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

				p.type    = PROP_LINEAR;
				p.count_v = 2;
				ts[t] = p;
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
			//
			Ctime t(iter->first);
			::DDX_TextGridControl(pDX, nIDC, nRow, 0, t.value);

			// time units
			//
			if (t.input && ::strlen(t.input))
			{
				CString str(t.input);
				::DDX_TextGridControl(pDX, nIDC, nRow, 1, str);
			}

			if (iter->second.type == PROP_FIXED)
			{
				ASSERT(iter->second.count_v == 1);
				::DDX_TextGridControl(pDX, nIDC, nRow, 2, iter->second.v[0]);
			}
			else if (iter->second.type == PROP_LINEAR)
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
				//
				CString dir(iter->second.coord);
				dir.MakeUpper();
				::DDX_TextGridControl(pDX, nIDC, nRow, 4, dir);

				// value 1
				//
				::DDX_TextGridControl(pDX, nIDC, nRow, 5, iter->second.v[0]);

				// dist 1
				//
				::DDX_TextGridControl(pDX, nIDC, nRow, 6, iter->second.dist1);

				// value 2
				//
				::DDX_TextGridControl(pDX, nIDC, nRow, 7, iter->second.v[1]);

				// dist 2
				//
				::DDX_TextGridControl(pDX, nIDC, nRow, 8, iter->second.dist2);
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}
}

void CGlobal::DDX_Property(CDataExchange* pDX, int nIDC, int nRow, struct property* pProperty, bool bRequired)
{
	static const int COL_VALUE       = 1;
	static const int COL_INTERPOLATE = 2;
	static const int COL_DIRECTION   = 3;
	static const int COL_VALUE_1     = 4;
	static const int COL_DISTANCE_1  = 5;
	static const int COL_VALUE_2     = 6;
	static const int COL_DISTANCE_2  = 7;

	CModGridCtrlEx* pGrid = static_cast<CModGridCtrlEx*>(pDX->m_pDlgWnd->GetDlgItem(nIDC));
	if (!pGrid || !pProperty) return;


	if (pDX->m_bSaveAndValidate)
	{
		// assume failure
		pProperty->type = PROP_UNDEFINED;

		Cproperty p;

		ASSERT(pGrid->IsCheckMarkCell(nRow, COL_INTERPOLATE));

		if (pGrid->GetCheck(nRow, COL_INTERPOLATE) != BST_CHECKED)
		{
			// value
			//
			CString value;
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE, value);
			value.Trim();
			if (value.IsEmpty())
			{
				if (bRequired)
				{
					::DDX_GridControlFail(pDX, nIDC, nRow, COL_VALUE, "No value specified.");
				}
				return;
			}

			// value
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE, p.v[0]);

			p.type    = PROP_FIXED;
			p.count_v = 1;
		}
		else
		{
			// direction
			//
			CString direction;
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_DIRECTION, direction);
			direction.Trim();
			if (direction.IsEmpty())
			{
				if (bRequired)
				{
					::DDX_GridControlFail(pDX, nIDC, nRow, COL_DIRECTION, "Please choose X, Y, or Z");
				}
				return;
			}
			else
			{
				if (direction[0] == 'X' || direction[0] == 'x')
				{
					p.coord = 'x';
				}
				else if (direction[0] == 'Y' || direction[0] == 'y')
				{
					p.coord = 'y';
				}
				else if (direction[0] == 'Z' || direction[0] == 'z')
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
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE_1, p.v[0]);

			// dist 1
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_DISTANCE_1, p.dist1);

			// value 2
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE_2, p.v[1]);

			// dist 2
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_DISTANCE_2, p.dist2);

			p.type    = PROP_LINEAR;
			p.count_v = 2;
		}
		// (*pProperty) = p;
		Cproperty::CopyProperty(&pProperty, &p);
	}
	else
	{
		if (pProperty->type == PROP_FIXED)
		{
			// value
			//
			ASSERT(pProperty->count_v == 1);
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE, pProperty->v[0]);

			// set checkmark
			//
			pGrid->SetCheck(nRow, COL_INTERPOLATE, BST_UNCHECKED);

			// enable/disable cells
			//
			pGrid->EnableCell(nRow, COL_VALUE);
			pGrid->DisableCell(nRow, COL_DIRECTION);
			pGrid->DisableCell(nRow, COL_VALUE_1);
			pGrid->DisableCell(nRow, COL_DISTANCE_1);
			pGrid->DisableCell(nRow, COL_VALUE_2);
			pGrid->DisableCell(nRow, COL_DISTANCE_2);
		}
		else if (pProperty->type == PROP_LINEAR)
		{
			ASSERT(pProperty->count_v == 2);

			// set checkmark
			//
			pGrid->SetCheck(nRow, COL_INTERPOLATE, BST_CHECKED);

			// enable/disable cells
			//
			pGrid->DisableCell(nRow, COL_VALUE);
			pGrid->EnableCell(nRow, COL_DIRECTION);
			pGrid->EnableCell(nRow, COL_VALUE_1);
			pGrid->EnableCell(nRow, COL_DISTANCE_1);
			pGrid->EnableCell(nRow, COL_VALUE_2);
			pGrid->EnableCell(nRow, COL_DISTANCE_2);

			// direction
			//
			CString dir(pProperty->coord);
			dir.MakeUpper();
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_DIRECTION, dir);

			// value 1
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE_1, pProperty->v[0]);

			// dist 1
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_DISTANCE_1, pProperty->dist1);

			// value 2
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE_2, pProperty->v[1]);

			// dist 2
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_DISTANCE_2, pProperty->dist2);
		}
		else if (pProperty->type == PROP_UNDEFINED)
		{
			// value
			//
			::DDX_TextGridControl(pDX, nIDC, nRow, COL_VALUE, CString(""));

			// set checkmark
			//
			pGrid->SetCheck(nRow, COL_INTERPOLATE, BST_UNCHECKED);

			// enable/disable cells
			//
			pGrid->EnableCell(nRow, COL_VALUE);
			pGrid->DisableCell(nRow, COL_DIRECTION);
			pGrid->DisableCell(nRow, COL_VALUE_1);
			pGrid->DisableCell(nRow, COL_DISTANCE_1);
			pGrid->DisableCell(nRow, COL_VALUE_2);
			pGrid->DisableCell(nRow, COL_DISTANCE_2);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

herr_t CGlobal::HDFSerializeSetOfTimes(bool bStoring, hid_t loc_id, std::set<Ctime>& setOfTimes)
{
	static const char szSteps[]       = "Steps";
	static const char szStepsFormat[] = "Step %d";
	static const char szCtime[]       = "Ctime";


	hid_t  step_id;
	hid_t  time_id;

	herr_t status = 0;

	if (bStoring)
	{
		if (!setOfTimes.empty())
		{
			std::list<LPCTSTR> listNames;
			CString* arrName = new CString[setOfTimes.size()];

			std::set<Ctime>::iterator iter = setOfTimes.begin();
			for (size_t i = 0; iter != setOfTimes.end(); ++iter, ++i)
			{
				arrName[i].Format(szStepsFormat, i);

				// Create the "Step %d" group
				step_id = ::H5Gcreate(loc_id, arrName[i], 0);
				ASSERT(step_id > 0);
				if (step_id > 0)
				{
					// Create the szCtime group
					time_id = ::H5Gcreate(step_id, szCtime, 0);
					ASSERT(time_id > 0);
					if (time_id > 0)
					{
						// first is const
						Ctime t(*iter);
						t.Serialize(bStoring, time_id);
						status = ::H5Gclose(time_id);
						ASSERT(status >= 0);
						listNames.push_back(arrName[i]);
					}
					status = ::H5Gclose(step_id);
					ASSERT(status >= 0);
				}
			}
			CGlobal::WriteList(loc_id, szSteps, listNames);
			delete[] arrName;
		}
	}
	else
	{
		std::list<std::string> listNames;
		CGlobal::ReadList(loc_id, szSteps, listNames);
		std::list<std::string>::iterator iter = listNames.begin();
		for (; iter != listNames.end(); ++iter)
		{
			// Open the "Step %d" group
			step_id = ::H5Gopen(loc_id, (*iter).c_str());
			ASSERT(step_id > 0);
			if (step_id > 0)
			{
				// Open the szCtime group
				time_id = ::H5Gopen(step_id, szCtime);
				ASSERT(time_id > 0);
				if (time_id > 0)
				{
					Ctime t;
					t.Serialize(bStoring, time_id);
					status = ::H5Gclose(time_id);
					ASSERT(status >= 0);
					setOfTimes.insert(t);
				}
				status = ::H5Gclose(step_id);
				ASSERT(status >= 0);
			}
		}
	}
	return status;
}

void CGlobal::DDX_Check(CDataExchange* pDX, int nIDC, bool& value)
{
	pDX->PrepareCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	if (pDX->m_bSaveAndValidate)
	{
		value = ((int)::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L) == BST_CHECKED);
	}
	else
	{
		if (value)
		{
			::SendMessage(hWndCtrl, BM_SETCHECK, (WPARAM)BST_CHECKED, 0L);
		}
		else
		{
			::SendMessage(hWndCtrl, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0L);
		}
	}
}

void CGlobal::WriteBool(std::ostream &os, int width, int value, const char *heading)
{
	CGlobal::WriteBool(os, width, (value != 0), heading);
}

void CGlobal::WriteBool(std::ostream &os, int width, bool value, const char *heading)
{
	// save original format
	std::ios_base::fmtflags format;

	os << "\t";
	os.width(width - 1);
	format = os.setf(std::ios_base::left, std::ios_base::adjustfield);
	os << heading;

	if (value)
	{
		os << " true\n";
	}
	else
	{
		os << " false\n";
	}

	// restore original format
	os.setf(format);
}

herr_t CGlobal::HDFSerializeCOLORREF(bool bStoring, hid_t loc_id, const char* szName, COLORREF& clr)
{
	ASSERT(sizeof(COLORREF) <= sizeof(unsigned long));
	herr_t status = CGlobal::HDFSerializeSafe(bStoring, loc_id, szName, H5T_NATIVE_ULONG, 1, &clr);
	return status;
}

int CGlobal::ExtractXMLStream(std::istream &is, std::iostream &ios)
{
	std::string line;
	while(is)
	{
		std::getline(is, line);
		std::string::size_type f = line.find('#');
		if (f == std::string::npos)
		{
			break;
		}
		else
		{
			// skip whitespace
			std::string::size_type l = line.size();
			for(f = f+1; f < l; ++f)
			{
				if (!::isspace(line[f])) break;
			}
			std::string comment = line.substr(f);
			ios << comment.c_str();
		}
	}
	return 0;
}

void CGlobal::Serialize(Polyhedron **p, CArchive &ar)
{
	static const char szPolyhedron[] = "Polyhedron";
	static int version = 1;

	CString type;
	int ver;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szPolyhedron;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szPolyhedron) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}

	int t;
	int ncs;
	if (ar.IsStoring())
	{
		ASSERT(*p && ::AfxIsValidAddress(*p, sizeof(Polyhedron *)));

		t = (*p)->get_type();
		ar << t;

		if (Cube *c = dynamic_cast<Cube*>(*p))
		{
			ncs = c->Get_user_coordinate_system();
			ar << ncs;
		}
		else
		{
			ASSERT(FALSE);
		}

		CZone z(*(*p)->Get_bounding_box());
		z.Serialize(ar);

		if (t == Polyhedron::WEDGE)
		{
			Wedge *w = static_cast<Wedge*>(*p);
			CString c(srcWedgeSource::GetWedgeOrientationString(w->orientation).c_str());
			ar << c;
		}

	}
	else
	{
		ASSERT(*p == 0);

		ar >> t;

		ar >> ncs;
		PHAST_Transform::COORDINATE_SYSTEM cs = static_cast<PHAST_Transform::COORDINATE_SYSTEM>(ncs);

		CZone z;
		z.Serialize(ar);

		CString c;
		if (t == Polyhedron::WEDGE)
		{
			ar >> c;
		}

		switch (t)
		{
		case Polyhedron::CUBE:
			(*p) = new Cube(&z, cs);
			break;
		case Polyhedron::WEDGE:
			(*p) = new Wedge(&z, std::string(c), cs);
			break;
		}
	}


	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szPolyhedron;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szPolyhedron) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}

}

#ifdef _DEBUG
void CGlobal::Dump(CDumpContext& dc, Polyhedron& p)
{
	dc << "<Polyhedron>\n";

	Polyhedron::POLYHEDRON_TYPE t = p.get_type();
	switch (t)
	{
	case Polyhedron::CUBE:
		dc << "<CUBE>\n";
		break;
	case Polyhedron::WEDGE:
		dc << "<WEDGE>\n";
		break;
	case Polyhedron::PRISM:
		dc << "<PRISM>\n";
		break;
	default:
		dc << "<UNKNOWN>\n";
		break;
	}

	CZone z(*p.Get_bounding_box());
	z.Dump(dc);

	switch (t)
	{
	case Polyhedron::CUBE:
		dc << "</CUBE>\n";
		break;
	case Polyhedron::WEDGE:
		dc << "</WEDGE>\n";
		break;
	case Polyhedron::PRISM:
		dc << "</PRISM>\n";
		break;
	default:
		dc << "</UNKNOWN>\n";
		break;
	}

	dc << "</Polyhedron>\n";
}
#endif // _DEBUG

herr_t CGlobal::HDFSerializeSafe(bool bStoring, hid_t loc_id, const char* szName, hid_t mem_type_id, hsize_t count, void* buf)
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
							return_val = status;
							goto HDFSerializeError;
						}
						// Close the szName dataspace
						status = ::H5Sclose(dspace_id);
						ASSERT(status >= 0); // unable to close dataspace
					}
					else
					{
						return_val = -1;
						goto HDFSerializeError;
					}
				}
				else
				{
					return_val = n_dims;
					goto HDFSerializeError;
				}
				// Close the szName dataset
				status = ::H5Dclose(dset_id);
				ASSERT(status >= 0); // unable to close dataset
			}
			else
			{
				return_val = dspace_id;
				goto HDFSerializeError;
			}
		}
		else
		{
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

herr_t CGlobal::HDFSerializePolyData(bool bStoring, hid_t loc_id, const char* szName, vtkPolyData *&pPolyData)
{
	// based on vtkXMLPolyDataReader and vtkXMLPolyDataWriter

	hid_t group_id;
	herr_t return_val = 0;

	static const char szVerts[]         = "Verts";
	static const char szLines[]         = "Lines";
	static const char szStrips[]        = "Strips";
	static const char szPolys[]         = "Polys";

	if (bStoring)
	{
		ASSERT(pPolyData != 0);
		group_id = ::H5Gcreate(loc_id, szName, 0);
		if (group_id > 0)
		{
			// points
			vtkPoints *p = pPolyData->GetPoints();
			HDFSerializePoints(bStoring, group_id, p);

			// cells
			//

			// verts
			vtkCellArray *verts = pPolyData->GetVerts();
			HDFSerializeCells(bStoring, group_id, szVerts, verts);

			// lines
			vtkCellArray *lines = pPolyData->GetLines();
			HDFSerializeCells(bStoring, group_id, szLines, lines);

			// strips
			vtkCellArray *strips = pPolyData->GetStrips();
			HDFSerializeCells(bStoring, group_id, szStrips, strips);

			// polys
			vtkCellArray *polys = pPolyData->GetPolys();
			HDFSerializeCells(bStoring, group_id, szPolys, polys);

			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}
	else
	{
		ASSERT(pPolyData == 0);
		pPolyData = vtkPolyData::New();

		group_id = ::H5Gopen(loc_id, szName);
		if (group_id > 0)
		{
			// points
// COMMENT: {11/24/2008 11:30:45 PM}			vtkPoints *points = vtkPoints::New();
			vtkPoints *points = 0;
			HDFSerializePoints(bStoring, group_id, points);
			pPolyData->SetPoints(points);
			points->Delete();

			// verts
			vtkCellArray *verts = 0;
			HDFSerializeCells(bStoring, group_id, szVerts,  verts);
			if (verts)
			{
				pPolyData->SetVerts(verts);
				verts->Delete();
			}

			// lines
			vtkCellArray *lines = 0;
			HDFSerializeCells(bStoring, group_id, szLines,  lines);
			if (lines)
			{
				pPolyData->SetLines(lines);
				verts->Delete();
			}

			// strips
			vtkCellArray *strips = 0;
			HDFSerializeCells(bStoring, group_id, szStrips, strips);
			if (strips)
			{
				pPolyData->SetStrips(strips);
				strips->Delete();
			}

			// polys
			vtkCellArray *polys = 0;
			HDFSerializeCells(bStoring, group_id, szPolys, polys);
			if (polys)
			{
				pPolyData->SetPolys(polys);
				polys->Delete();
			}

			return_val = H5Gclose(group_id);
		}

	}
	return return_val;
}

herr_t CGlobal::HDFSerializePoints(bool bStoring, hid_t loc_id, vtkPoints *&pPoints)
{
	static const char szPoints[] = "Points";
	static const char szData[]   = "Data";

	herr_t return_val = 0;
	hid_t group_id = 0;
	hid_t mem_type_id = 0;
	hsize_t size = 0;

	if (bStoring)
	{
		ASSERT(pPoints != 0);

		group_id = ::H5Gcreate(loc_id, szPoints, 0);
		ASSERT(group_id >= 0);
		if (group_id > 0)
		{
			vtkDataArray* data = pPoints->GetData();
			int numberOfComponents = data->GetNumberOfComponents();
			ASSERT(numberOfComponents == 3);
			vtkIdType numberOfTuples = data->GetNumberOfTuples();

			mem_type_id = 0;
			if (data->GetDataType() == VTK_FLOAT)
			{
				mem_type_id = H5T_NATIVE_FLOAT;
			}
			else if (data->GetDataType() == VTK_DOUBLE)
			{
				mem_type_id = H5T_NATIVE_DOUBLE;
			}
			else
			{
				ASSERT(FALSE);
				return H5Gclose(group_id);
			}
			if (numberOfTuples*numberOfComponents > 0)
			{
				return_val = CGlobal::HDFSerializeSafe(bStoring, group_id, szData, mem_type_id, numberOfTuples*numberOfComponents, data->GetVoidPointer(0));
			}
			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}
	else
	{
		ASSERT(pPoints == 0);

		group_id = ::H5Gopen(loc_id, szPoints);
		ASSERT(group_id >= 0);
		if (group_id > 0)
		{
			// Open the szName dataset
			hid_t dset_id = ::H5Dopen(group_id, szData);
			if (dset_id > 0)
			{
				hid_t type_id = ::H5Dget_type(dset_id);
				if (type_id)
				{

					if (::H5Tequal(type_id, H5T_NATIVE_FLOAT) > 0)
					{
						mem_type_id = H5T_NATIVE_FLOAT;
					}
					else if (::H5Tequal(type_id, H5T_NATIVE_DOUBLE) > 0)
					{
						mem_type_id = H5T_NATIVE_DOUBLE;
					}
					else
					{
						ASSERT(FALSE);
					}
					return_val = H5Tclose(type_id);
					ASSERT(return_val >= 0); // unable to close type
				}

				// Close the szName dataset
				return_val = ::H5Dclose(dset_id);
				ASSERT(return_val >= 0); // unable to close dataset
			}
			if (mem_type_id == H5T_NATIVE_FLOAT)
			{
				pPoints = vtkPoints::New();
				ASSERT(pPoints->GetData()->GetDataType() == VTK_FLOAT);
				hsize_t size = HDFGetSize(group_id, szData, H5T_NATIVE_FLOAT);
				ASSERT(pPoints->GetData()->GetNumberOfComponents() == 3);
				int npts = size/pPoints->GetData()->GetNumberOfComponents();
				pPoints->GetData()->SetNumberOfTuples(npts);
				return_val = CGlobal::HDFSerializeWithSize(bStoring, group_id, szData, H5T_NATIVE_FLOAT, size, pPoints->GetData()->GetVoidPointer(0));
			}
			else if (mem_type_id == H5T_NATIVE_DOUBLE)
			{
				pPoints = vtkPoints::New(VTK_DOUBLE);
				ASSERT(pPoints->GetData()->GetDataType() == VTK_DOUBLE);
				hsize_t size = HDFGetSize(group_id, szData, H5T_NATIVE_DOUBLE);
				ASSERT(pPoints->GetData()->GetNumberOfComponents() == 3);
				int npts = size/pPoints->GetData()->GetNumberOfComponents();
				pPoints->GetData()->SetNumberOfTuples(npts);
				return_val = CGlobal::HDFSerializeWithSize(bStoring, group_id, szData, H5T_NATIVE_DOUBLE, size, pPoints->GetData()->GetVoidPointer(0));
			}
			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}
	return return_val;
}

herr_t CGlobal::HDFSerializeCells(bool bStoring, hid_t loc_id, const char* szName, vtkCellArray *&pCells)
{
	herr_t return_val = 0;

	static const char szConnectivity[]  = "connectivity";
	static const char szOffsets[]       = "offsets";

	if (bStoring)
	{
		ASSERT(pCells);

		// based on vtkXMLUnstructuredDataWriter::ConvertCells
		//
		vtkIdTypeArray* connectivity = pCells->GetData();
		vtkIdType numberOfCells = pCells->GetNumberOfCells();
		vtkIdType numberOfTuples = connectivity->GetNumberOfTuples();

		if (numberOfTuples - numberOfCells == 0) return return_val;

		hid_t group_id = ::H5Gcreate(loc_id, szName, 0);
		if (group_id > 0)
		{
			vtkIdTypeArray *cellPoints = vtkIdTypeArray::New();
			vtkIdTypeArray *cellOffsets = vtkIdTypeArray::New();

			cellPoints->SetNumberOfTuples(numberOfTuples - numberOfCells);
			cellOffsets->SetNumberOfTuples(numberOfCells);

			vtkIdType* inCell = connectivity->GetPointer(0);
			vtkIdType* outCellPointsBase = cellPoints->GetPointer(0);
			vtkIdType* outCellPoints = outCellPointsBase;
			vtkIdType* outCellOffset = cellOffsets->GetPointer(0);

			vtkIdType i;
			for(i = 0; i < numberOfCells; ++i)
			{
				vtkIdType numberOfPoints = *inCell++;
				::memcpy(outCellPoints, inCell, sizeof(vtkIdType)*numberOfPoints);
				outCellPoints += numberOfPoints;
				inCell += numberOfPoints;
				*outCellOffset++ = outCellPoints - outCellPointsBase;
			}

			return_val = CGlobal::HDFSerializeSafe(bStoring, group_id, szConnectivity, H5T_NATIVE_INT, cellPoints->GetNumberOfTuples(), cellPoints->GetPointer(0));
			return_val = CGlobal::HDFSerializeSafe(bStoring, group_id, szOffsets, H5T_NATIVE_INT, cellOffsets->GetNumberOfTuples(), cellOffsets->GetPointer(0));

			cellPoints->Delete();
			cellOffsets->Delete();

			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}
	else
	{
		ASSERT(pCells == 0);
		hid_t group_id = ::H5Gopen(loc_id, szName);
		if (group_id > 0)
		{
			vtkIdTypeArray *cellPoints = vtkIdTypeArray::New();
			ASSERT(cellPoints);

			vtkIdTypeArray *cellOffsets = vtkIdTypeArray::New();
			ASSERT(cellOffsets);

			hsize_t sizec = HDFGetSize(group_id, szConnectivity, H5T_NATIVE_INT);
			cellPoints->SetNumberOfTuples(sizec);

			hsize_t sizeo = HDFGetSize(group_id, szOffsets, H5T_NATIVE_INT);
			cellOffsets->SetNumberOfTuples(sizec);

			if (sizec*sizeo)
			{
				return_val = CGlobal::HDFSerializeSafe(bStoring, group_id, szConnectivity, H5T_NATIVE_INT, sizec, cellPoints->GetPointer(0));
				return_val = CGlobal::HDFSerializeSafe(bStoring, group_id, szOffsets, H5T_NATIVE_INT, sizeo, cellOffsets->GetPointer(0));

				pCells = vtkCellArray::New();
				if (pCells)
				{
					// based on vtkXMLUnstructuredDataReader::ReadCellArray
					//
					vtkIdType numberOfCells = sizeo;
					vtkIdType newSize = sizec + sizeo;
					vtkIdType* cptr = pCells->WritePointer(sizeo, newSize);

					pCells->WritePointer(sizeo, sizec+sizeo);

					// Copy the connectivity data.
					vtkIdType i;
					vtkIdType previousOffset = 0;
					for(i=0; i < numberOfCells; ++i)
					{
						vtkIdType length = cellOffsets->GetValue(i)-previousOffset;
						*cptr++ = length;
						vtkIdType* sptr = cellPoints->GetPointer(previousOffset);
						// Copy the point indices, but increment them for the appended
						// version's index.
						vtkIdType j;
						for(j=0;j < length; ++j)
						{
							cptr[j] = sptr[j];
						}
						cptr += length;
						previousOffset += length;
					}
				}
			}
			cellPoints->Delete();
			cellOffsets->Delete();

			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}

	return return_val;
}

hsize_t CGlobal::HDFGetSize(hid_t loc_id, const char* szName, hid_t mem_type_id)
{
	herr_t status;
	hsize_t dims[1], maxdims[1];
	hid_t dspace_id = 0;
	hid_t dset_id = 0;

	hsize_t return_val = 0;

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
				return_val = dims[0];
			}
			else
			{
				ASSERT(FALSE); // unable to get_simple_extent_dims
				goto HDFSerializeWithSizeError;
			}
			// Close the szName dataset
			status = ::H5Dclose(dset_id);
			ASSERT(status >= 0); // unable to close dataset
		}
		else
		{
			ASSERT(FALSE); // unable to open dataspace
			goto HDFSerializeWithSizeError;
		}
	}
	else
	{
		ASSERT(FALSE); // unable to open dataset
		goto HDFSerializeWithSizeError;
	}

	// ok if here
	return return_val; // no errors

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

herr_t CGlobal::HDFSerializePrism(bool bStoring, hid_t loc_id, Prism &rPrism)
{
	static const char szPrism[]     = "Prism";
	static const char szBox[]       = "box";
	static const char szTop[]       = "top";
	static const char szBottom[]    = "bottom";
	static const char szPerimeter[] = "perimeter";

	static const char szOrig_perimeter_datum[] = "orig_perimeter_datum";


	/*
	<polyh>
		<Prism>
			<box/>
			<perimeter/>
			<top/>
			<bottom/>
		</Prism>
	</polyh>
	*/
	herr_t return_val = 0;
	double xyz[6];

	if (bStoring)
	{
		hid_t group_id = ::H5Gcreate(loc_id, szPrism, 0);
		if (group_id > 0)
		{
			// box
			//
			struct zone* pzone = rPrism.Get_bounding_box();
			ASSERT(pzone->zone_defined);
			xyz[0] = pzone->x1;
			xyz[1] = pzone->y1;
			xyz[2] = pzone->z1;
			xyz[3] = pzone->x2;
			xyz[4] = pzone->y2;
			xyz[5] = pzone->z2;
			return_val = CGlobal::HDFSerialize(bStoring, group_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
			ASSERT(return_val >= 0);

			// perimeter, top, bottom
			CGlobal::HDFSerializeData_source(bStoring, group_id, szPerimeter, rPrism.perimeter);
			CGlobal::HDFSerializeData_source(bStoring, group_id, szTop, rPrism.top);
			CGlobal::HDFSerializeData_source(bStoring, group_id, szBottom, rPrism.bottom);

// COMMENT: {10/21/2008 3:23:05 PM}			// perimeter_option
// COMMENT: {10/21/2008 3:23:05 PM}			CGlobal::HDFSerializePerimeterOption(bStoring, group_id, rPrism.perimeter_option);

// COMMENT: {10/21/2008 3:23:19 PM}			switch(rPrism.perimeter_option)
// COMMENT: {10/21/2008 3:23:19 PM}			{
// COMMENT: {10/21/2008 3:23:19 PM}			case Prism::CONSTANT:
// COMMENT: {10/21/2008 3:23:19 PM}				CGlobal::HDFSerialize(bStoring, group_id, szOrig_perimeter_datum, H5T_NATIVE_DOUBLE, 1, &rPrism.orig_perimeter_datum);
// COMMENT: {10/21/2008 3:23:19 PM}				break;
// COMMENT: {10/21/2008 3:23:19 PM}			case Prism::ATTRIBUTE:
// COMMENT: {10/21/2008 3:23:19 PM}				// do nothing
// COMMENT: {10/21/2008 3:23:19 PM}				break;
// COMMENT: {10/21/2008 3:23:19 PM}			case Prism::USE_Z:
// COMMENT: {10/21/2008 3:23:19 PM}				// do nothing
// COMMENT: {10/21/2008 3:23:19 PM}				break;
// COMMENT: {10/21/2008 3:23:19 PM}			case Prism::DEFAULT:
// COMMENT: {10/21/2008 3:23:19 PM}				// do nothing
// COMMENT: {10/21/2008 3:23:19 PM}				break;
// COMMENT: {10/21/2008 3:23:19 PM}			default:
// COMMENT: {10/21/2008 3:23:19 PM}				ASSERT(FALSE);
// COMMENT: {10/21/2008 3:23:19 PM}			}

			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}
	else
	{
		hid_t group_id = ::H5Gopen(loc_id, szPrism);
		if (group_id > 0)
		{
			// box
			return_val = CGlobal::HDFSerialize(bStoring, group_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
			ASSERT(return_val >= 0);
			struct zone* pzone = rPrism.Get_bounding_box();
			pzone->zone_defined = 1;
			pzone->x1 = xyz[0];
			pzone->y1 = xyz[1];
			pzone->z1 = xyz[2];
			pzone->x2 = xyz[3];
			pzone->y2 = xyz[4];
			pzone->z2 = xyz[5];

			CGlobal::HDFSerializeData_source(bStoring, group_id, szPerimeter, rPrism.perimeter);
			CGlobal::HDFSerializeData_source(bStoring, group_id, szTop, rPrism.top);
			CGlobal::HDFSerializeData_source(bStoring, group_id, szBottom, rPrism.bottom);

// COMMENT: {10/21/2008 3:23:28 PM}			// perimeter_option
// COMMENT: {10/21/2008 3:23:28 PM}			CGlobal::HDFSerializePerimeterOption(bStoring, group_id, rPrism.perimeter_option);
// COMMENT: {10/21/2008 3:23:28 PM}
// COMMENT: {10/21/2008 3:23:28 PM}			switch(rPrism.perimeter_option)
// COMMENT: {10/21/2008 3:23:28 PM}			{
// COMMENT: {10/21/2008 3:23:28 PM}			case Prism::CONSTANT:
// COMMENT: {10/21/2008 3:23:28 PM}				CGlobal::HDFSerialize(bStoring, group_id, szOrig_perimeter_datum, H5T_NATIVE_DOUBLE, 1, &rPrism.orig_perimeter_datum);
// COMMENT: {10/21/2008 3:23:28 PM}				break;
// COMMENT: {10/21/2008 3:23:28 PM}			case Prism::ATTRIBUTE:
// COMMENT: {10/21/2008 3:23:28 PM}				// do nothing
// COMMENT: {10/21/2008 3:23:28 PM}				break;
// COMMENT: {10/21/2008 3:23:28 PM}			case Prism::USE_Z:
// COMMENT: {10/21/2008 3:23:28 PM}				// do nothing
// COMMENT: {10/21/2008 3:23:28 PM}				break;
// COMMENT: {10/21/2008 3:23:28 PM}			case Prism::DEFAULT:
// COMMENT: {10/21/2008 3:23:28 PM}				// do nothing
// COMMENT: {10/21/2008 3:23:28 PM}				break;
// COMMENT: {10/21/2008 3:23:28 PM}			default:
// COMMENT: {10/21/2008 3:23:28 PM}				ASSERT(FALSE);
// COMMENT: {10/21/2008 3:23:28 PM}			}

			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}
	return return_val;
}

void CGlobal::UpgradePrism(Prism &rPrism, const CGridKeyword &rGridKeyword, const CUnits &rUnits)
{
	// perimeter, top, bottom
	CGlobal::UpgradeDataSourcePoints(rPrism.perimeter, rGridKeyword, rUnits);
	CGlobal::UpgradeDataSourcePoints(rPrism.top,       rGridKeyword, rUnits);
	CGlobal::UpgradeDataSourcePoints(rPrism.bottom,    rGridKeyword, rUnits);
}

void CGlobal::UpgradeDataSourcePoints(Data_source &rData_source, const CGridKeyword &rGridKeyword, const CUnits &rUnits)
{
	if (rData_source.Get_source_type() == Data_source::POINTS)
	{
		std::vector<Point> &upts = rData_source.Get_user_points();
		if (upts.size() != 0)
		{
			ASSERT(upts.size() == rData_source.Get_points().size());
			return;
		}

		upts = rData_source.Get_points();

		double scale_h = rUnits.map_horizontal.input_to_si / rUnits.horizontal.input_to_si;
		double scale_v = rUnits.map_vertical.input_to_si / rUnits.vertical.input_to_si;
		PHAST_Transform map2grid = PHAST_Transform(
			rGridKeyword.m_grid_origin[0],
			rGridKeyword.m_grid_origin[1],
			rGridKeyword.m_grid_origin[2],
			rGridKeyword.m_grid_angle,
			scale_h,
			scale_h,
			scale_v
			);

		switch (rData_source.Get_coordinate_system())
		{
		case PHAST_Transform::GRID:
			if (rData_source.Get_user_coordinate_system() == PHAST_Transform::MAP)
			{
				map2grid.Inverse_transform(upts);
			}
			break;
		case PHAST_Transform::MAP:
			if (rData_source.Get_user_coordinate_system() == PHAST_Transform::GRID)
			{
				map2grid.Transform(upts);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
}

herr_t CGlobal::HDFSerializeData_source(bool bStoring, hid_t loc_id, const char* szName, Data_source &rData_source)
{
	static const char szData_source[] = "Data_source";
	static const char szType[]        = "type";
	static const char szUserType[]    = "user_type";
	static const char szBox[]         = "box";
	static const char szfile_name[]   = "file_name";
	static const char szPoints[]      = "Points";
	static const char szPoints4[]     = "Points4";
	static const char szUPoints4[]    = "UserPoints4";
	static const char szAttribute[]   = "attribute";

	/*
	<polyh>
		<Prism>
			<szName> (ie top)
				<Data_source>
					<type>Data_source::DATA_SOURCE_TYPE</type>
					<box/>
					<file_name>std::string</file_name>
					<coordinate_system>PHAST_Transform::COORDINATE_SYSTEM</coordinate_system>
					<coordinate_system_user>PHAST_Transform::COORDINATE_SYSTEM</coordinate_system_user>
				</Data_source>
			</szName>
		</Prism>
	</polyh>
	*/

	herr_t status;
	double xyz[6];
	herr_t return_val = 0;

	if (bStoring)
	{
		hid_t group_id = ::H5Gcreate(loc_id, szName, 0);
		if (group_id > 0)
		{
			hid_t ds_gr_id = ::H5Gcreate(group_id, szData_source, 0);
			if (ds_gr_id > 0)
			{
				// source_type
				hid_t sourcetype = CGlobal::HDFCreateDataSourceType();
				if (sourcetype > 0)
				{
					// source_type
					Data_source::DATA_SOURCE_TYPE nValue = rData_source.Get_source_type();
					status = CGlobal::HDFSerialize(bStoring, ds_gr_id, szType, sourcetype, 1, &nValue);
					ASSERT(status >= 0);

					// source_type_user
					Data_source::DATA_SOURCE_TYPE nUserValue = rData_source.Get_user_source_type();
					status = CGlobal::HDFSerialize(bStoring, ds_gr_id, szUserType, sourcetype, 1, &nUserValue);
					ASSERT(status >= 0);

					switch(nValue)
					{
					case Data_source::SHAPE:
						{
							std::string filename(rData_source.Get_file_name());
							status = HDFSerializeString(bStoring, ds_gr_id, szfile_name, filename);
							ASSERT(status >= 0);

							int attrib = rData_source.Get_attribute();
							status = HDFSerialize(bStoring, ds_gr_id, szAttribute, H5T_NATIVE_INT, 1, &attrib);
							ASSERT(status >= 0);

							rData_source.Set_attribute(attrib);
						}
						break;
					case Data_source::ARCRASTER:
					case Data_source::XYZ:
						{
							std::string filename(rData_source.Get_file_name());
							status = HDFSerializeString(bStoring, ds_gr_id, szfile_name, filename);
							ASSERT(status >= 0);
						}
						break;
					case Data_source::XYZT:
						{
							// TODO this may need additional work
							std::string filename(rData_source.Get_file_name());
							status = HDFSerializeString(bStoring, ds_gr_id, szfile_name, filename);
							ASSERT(status >= 0);
						}
						break;
					case Data_source::CONSTANT:
						{
							std::vector<Point> &pts = rData_source.Get_points();
							ASSERT(pts.size() == 1);
							hsize_t count = 3;							
							//double *points = new double[count];
							pts.front().get_coord();
							status = CGlobal::HDFSerializeWithSize(bStoring, ds_gr_id, szPoints, H5T_NATIVE_DOUBLE, count, pts.front().get_coord());
						}
						break;
					case Data_source::POINTS:
						{
							std::vector<Point> &pts = rData_source.Get_points();
							std::vector<Point> &upts = rData_source.Get_user_points();
							ASSERT(pts.size() == upts.size());

							hsize_t count = 4 * pts.size();							
							double *points = new double[count];
							double *upoints = new double[count];
							for (size_t i = 0; i < pts.size(); ++i)
							{
								double *d = pts[i].get_coord();
								double *u = upts[i].get_coord();
								for (size_t n = 0; n < 3; ++n)
								{
									points[i*4+n] = d[n];
									upoints[i*4+n] = u[n];
								}
								points[i*4+3] = pts[i].get_v();
								upoints[i*4+3] = upts[i].get_v();
							}
							status = CGlobal::HDFSerializeWithSize(bStoring, ds_gr_id, szPoints4, H5T_NATIVE_DOUBLE, count, points);
							ASSERT(status >= 0);
							status = CGlobal::HDFSerializeWithSize(bStoring, ds_gr_id, szUPoints4, H5T_NATIVE_DOUBLE, count, upoints);
							ASSERT(status >= 0);
							delete[] points;
							delete[] upoints;
						}
						break;
					case Data_source::NONE:
						// do nothing
						break;
					default:
						ASSERT(FALSE);
					}
					status = H5Tclose(sourcetype);
					ASSERT(status >= 0);
				}
				// box
				//
				struct zone* pzone = rData_source.Get_bounding_box();
				//ASSERT(pzone->zone_defined);
				if (pzone->zone_defined)
				{
					xyz[0] = pzone->x1;
					xyz[1] = pzone->y1;
					xyz[2] = pzone->z1;
					xyz[3] = pzone->x2;
					xyz[4] = pzone->y2;
					xyz[5] = pzone->z2;
				}
				else
				{
					for (int i = 0; i < 6; ++i)
					{
						xyz[i] = 0;
					}
				}
				return_val = CGlobal::HDFSerialize(bStoring, ds_gr_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
				ASSERT(return_val >= 0);

				// coor_sys_type
				PHAST_Transform::COORDINATE_SYSTEM nValue = rData_source.Get_coordinate_system();
				return_val = CGlobal::HDFSerializeCoordinateSystem(bStoring, ds_gr_id, nValue);
				ASSERT(return_val >= 0);

				// coor_sys_type
				nValue = rData_source.Get_user_coordinate_system();
				return_val = CGlobal::HDFSerializeCoordinateSystemUser(bStoring, ds_gr_id, nValue);
				ASSERT(return_val >= 0);

				return_val = H5Gclose(ds_gr_id);
				ASSERT(return_val >= 0);
			}

			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}

	}
	else
	{
		hid_t group_id = ::H5Gopen(loc_id, szName);
		if (group_id > 0)
		{
			hid_t ds_gr_id = ::H5Gopen(group_id, szData_source);
			if (ds_gr_id > 0)
			{
				// source_type
				hid_t sourcetype = CGlobal::HDFCreateDataSourceType();
				if (sourcetype > 0)
				{
					// source_type
					Data_source::DATA_SOURCE_TYPE nValue;
					status = CGlobal::HDFSerialize(bStoring, ds_gr_id, szType, sourcetype, 1, &nValue);
					rData_source.Set_source_type(nValue);

					// source_type_user
					Data_source::DATA_SOURCE_TYPE nUserValue;
					status = CGlobal::HDFSerializeSafe(bStoring, ds_gr_id, szUserType, sourcetype, 1, &nUserValue);
					if (status < 0)
					{
						nUserValue = nValue;
					}
					rData_source.Set_user_source_type(nUserValue);

					switch(nValue)
					{
					case Data_source::SHAPE:
						{
							std::string filename;
							status = HDFSerializeString(bStoring, ds_gr_id, szfile_name, filename);
							ASSERT(status >= 0);
							rData_source.Set_file_name(filename);

							int attrib;
							status = HDFSerialize(bStoring, ds_gr_id, szAttribute, H5T_NATIVE_INT, 1, &attrib);
							ASSERT(status >= 0);
							rData_source.Set_attribute(attrib);
						}
						break;
					case Data_source::ARCRASTER:
					case Data_source::XYZ:
						{
							std::string filename;
							status = HDFSerializeString(bStoring, ds_gr_id, szfile_name, filename);
							ASSERT(status >= 0);
							rData_source.Set_file_name(filename);
						}
						break;
					case Data_source::XYZT:
						{
							// TODO this may need additional work
							std::string filename;
							status = HDFSerializeString(bStoring, ds_gr_id, szfile_name, filename);
							ASSERT(status >= 0);
							rData_source.Set_file_name(filename);
						}
						break;
					case Data_source::CONSTANT:
						{
							std::vector<Point> &pts = rData_source.Get_points();
							ASSERT(pts.size() == 0);
							hsize_t count;							
							double *data = NULL;
							status = CGlobal::HDFSerializeAllocate(bStoring, ds_gr_id, szPoints, count, &data);
							ASSERT(status >= 0);
							ASSERT(count == 3);
							pts.push_back(Point(data[0], data[1], data[2], data[2]));
							delete[] data;
						}
						break;
					case Data_source::POINTS:
						{
							std::vector<Point> &pts = rData_source.Get_points();
							ASSERT(pts.size() == 0);
							double *data = NULL;
							hsize_t count;
							status = CGlobal::HDFSerializeAllocate(bStoring, ds_gr_id, szPoints4, count, &data);
							if (status >= 0)
							{
								ASSERT((count%4) == 0);

								Point p;
								double *coord = p.get_coord();
								for (size_t i = 0; i < count; ++i)
								{
									if (i%4 != 3)
									{
										coord[i%4] = data[i];
									}
									else
									{
										p.set_v(data[i]);
										pts.push_back(p);
									}
								}
								ASSERT(pts.size() == count/4);
								delete[] data;
							}
							else
							{
								status = CGlobal::HDFSerializeAllocate(bStoring, ds_gr_id, szPoints, count, &data);
								ASSERT(status >= 0);
								ASSERT((count%3) == 0);

								Point p;
								double *coord = p.get_coord();
								for (size_t i = 0; i < count; ++i)
								{
									coord[i%3] = data[i];
									if (i%3 == 2)
									{
										p.set_v(data[i]);
										pts.push_back(p);
									}
								}
								ASSERT(pts.size() == count/3);
								delete[] data;
							}

							std::vector<Point> &upts = rData_source.Get_user_points();
							ASSERT(upts.size() == 0);
							data = NULL;
							status = CGlobal::HDFSerializeAllocate(bStoring, ds_gr_id, szUPoints4, count, &data);
							if (status >= 0)
							{
								ASSERT((count%4) == 0);

								Point p;
								double *coord = p.get_coord();
								for (size_t i = 0; i < count; ++i)
								{
									if (i%4 != 3)
									{
										coord[i%4] = data[i];
									}
									else
									{
										p.set_v(data[i]);
										upts.push_back(p);
									}
								}
								ASSERT(upts.size() == count/4);
								ASSERT(pts.size() == upts.size());
								delete[] data;
							}
						}
						break;
					case Data_source::NONE:
						// do nothing
						break;
					default:
						ASSERT(FALSE);
					}

					rData_source.Set_defined(true);

					status = H5Tclose(sourcetype);
					ASSERT(status >= 0);
				}

				// box
				return_val = CGlobal::HDFSerialize(bStoring, ds_gr_id, szBox, H5T_NATIVE_DOUBLE, 6, xyz);
				ASSERT(return_val >= 0);
				struct zone* pzone = rData_source.Get_bounding_box();
				pzone->zone_defined = 1;
				pzone->x1 = xyz[0];
				pzone->y1 = xyz[1];
				pzone->z1 = xyz[2];
				pzone->x2 = xyz[3];
				pzone->y2 = xyz[4];
				pzone->z2 = xyz[5];

				// coor_sys_type
				PHAST_Transform::COORDINATE_SYSTEM nValue;
				status = CGlobal::HDFSerializeCoordinateSystem(bStoring, ds_gr_id, nValue);
				rData_source.Set_coordinate_system(nValue);

				status = CGlobal::HDFSerializeCoordinateSystemUser(bStoring, ds_gr_id, nValue);
				rData_source.Set_user_coordinate_system(nValue);

				Data_source::DATA_SOURCE_TYPE ds = rData_source.Get_source_type();
				if (ds == Data_source::XYZT) TRACE("WARNING: ds == Data_source::XYZT FakeFiledata may be needed\n");
				if (ds == Data_source::SHAPE || ds == Data_source::ARCRASTER || ds == Data_source::XYZ)
				{
					if (ds != Data_source::SHAPE) ASSERT(rData_source.Get_attribute() == -1);
					rData_source.Set_filedata(FakeFiledata::New(rData_source.Get_file_name(), nValue, rData_source.Get_attribute()));
				}

				return_val = H5Gclose(ds_gr_id);
				ASSERT(return_val >= 0);

			}
			return_val = H5Gclose(group_id);
			ASSERT(return_val >= 0);
		}
	}
	return return_val;
}

hid_t CGlobal::HDFCreatePolyhedronDataType(void)
{
	herr_t status;

	// Create the datatype
	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(Polyhedron::POLYHEDRON_TYPE));
	ASSERT(enum_datatype >= 0);

	Polyhedron::POLYHEDRON_TYPE nValue;

	// Insert the enumerated data
	nValue = Polyhedron::CUBE;
	status = H5Tenum_insert(enum_datatype, "CUBE", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Polyhedron::WEDGE;
	status = H5Tenum_insert(enum_datatype, "WEDGE", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Polyhedron::PRISM;
	status = H5Tenum_insert(enum_datatype, "PRISM", &nValue);
	ASSERT(status >= 0);

	return enum_datatype;
}

hid_t CGlobal::HDFCreateWidgetOrientationDataType(void)
{
	herr_t status;

	// Create the datatype
	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(Wedge::WEDGE_ORIENTATION));
	ASSERT(enum_datatype >= 0);

	Wedge::WEDGE_ORIENTATION nValue;

	// Insert the enumerated data
	nValue = Wedge::X1;
	status = H5Tenum_insert(enum_datatype, "X1", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::X2;
	status = H5Tenum_insert(enum_datatype, "X2", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::X3;
	status = H5Tenum_insert(enum_datatype, "X3", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::X4;
	status = H5Tenum_insert(enum_datatype, "X4", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Y1;
	status = H5Tenum_insert(enum_datatype, "Y1", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Y2;
	status = H5Tenum_insert(enum_datatype, "Y2", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Y3;
	status = H5Tenum_insert(enum_datatype, "Y3", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Y4;
	status = H5Tenum_insert(enum_datatype, "Y4", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Z1;
	status = H5Tenum_insert(enum_datatype, "Z1", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Z2;
	status = H5Tenum_insert(enum_datatype, "Z2", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Z3;
	status = H5Tenum_insert(enum_datatype, "Z3", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::Z4;
	status = H5Tenum_insert(enum_datatype, "Z4", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Wedge::WEDGE_ERROR;
	status = H5Tenum_insert(enum_datatype, "WEDGE_ERROR", &nValue);
	ASSERT(status >= 0);

	return enum_datatype;
}

hid_t CGlobal::HDFCreateDataSourceType(void)
{
	herr_t status;

	// Create the datatype
	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(Data_source::DATA_SOURCE_TYPE));
	ASSERT(enum_datatype > 0);

	Data_source::DATA_SOURCE_TYPE nValue;

	// Insert the enumerated data
	nValue = Data_source::SHAPE;
	status = H5Tenum_insert(enum_datatype, "SHAPE", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Data_source::ARCRASTER;
	status = H5Tenum_insert(enum_datatype, "ARCRASTER", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Data_source::XYZ;
	status = H5Tenum_insert(enum_datatype, "XYZ", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Data_source::CONSTANT;
	status = H5Tenum_insert(enum_datatype, "CONSTANT", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Data_source::POINTS;
	status = H5Tenum_insert(enum_datatype, "POINTS", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Data_source::NONE;
	status = H5Tenum_insert(enum_datatype, "NONE", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = Data_source::XYZT;
	status = H5Tenum_insert(enum_datatype, "XYZT", &nValue);
	ASSERT(status >= 0);

	return enum_datatype;
}

hid_t CGlobal::HDFCreateCoordinateSystemType(void)
{
	herr_t status;

	// Create the datatype
	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(PHAST_Transform::COORDINATE_SYSTEM));
	ASSERT(enum_datatype > 0);

	PHAST_Transform::COORDINATE_SYSTEM nValue;

	// Insert the enumerated data
	nValue = PHAST_Transform::MAP;
	status = H5Tenum_insert(enum_datatype, "MAP", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PHAST_Transform::GRID;
	status = H5Tenum_insert(enum_datatype, "GRID", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PHAST_Transform::NONE;
	status = H5Tenum_insert(enum_datatype, "NONE", &nValue);
	ASSERT(status >= 0);

	return enum_datatype;
}

hid_t CGlobal::HDFCreateSolidUnitsType(void)
{
	herr_t status;

	// Create the datatype
	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(SOLID_UNITS));
	ASSERT(enum_datatype >= 0);

	SOLID_UNITS nValue;

	// Insert the enumerated data
	nValue = WATER;
	status = H5Tenum_insert(enum_datatype, "WATER", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = ROCK;
	status = H5Tenum_insert(enum_datatype, "ROCK", &nValue);
	ASSERT(status >= 0);

	return enum_datatype;
}

herr_t CGlobal::HDFSerializeCoordinateSystem(bool bStoring, hid_t loc_id, PHAST_Transform::COORDINATE_SYSTEM &coor_sys)
{
	static const char szCoorSys[] = "coordinate_system";

	herr_t status = -1;

	// coor_sys_type
	hid_t coor_sys_type = CGlobal::HDFCreateCoordinateSystemType();
	if (coor_sys_type > 0)
	{
		if (!bStoring) coor_sys = PHAST_Transform::MAP;
		status = CGlobal::HDFSerialize(bStoring, loc_id, szCoorSys, coor_sys_type, 1, &coor_sys);
		ASSERT(!bStoring || status >= 0); // if storing assert status is valid

		VERIFY(H5Tclose(coor_sys_type) >= 0);
	}
	return status;
}

herr_t CGlobal::HDFSerializeCoordinateSystemUser(bool bStoring, hid_t loc_id, PHAST_Transform::COORDINATE_SYSTEM &coor_sys_user)
{
	static const char szCoorSysUser[] = "coordinate_system_user";

	herr_t status = -1;

	// coor_sys_type
	hid_t coor_sys_type = CGlobal::HDFCreateCoordinateSystemType();
	if (coor_sys_type > 0)
	{
		status = CGlobal::HDFSerialize(bStoring, loc_id, szCoorSysUser, coor_sys_type, 1, &coor_sys_user);
		ASSERT(!bStoring || status >= 0); // if storing assert status is valid

		VERIFY(H5Tclose(coor_sys_type) >= 0);
	}
	return status;
}

herr_t CGlobal::HDFSerializeXYCoordinateSystem(bool bStoring, hid_t loc_id, PHAST_Transform::COORDINATE_SYSTEM &coor_sys)
{
	static const char szCoorSys[] = "xy_coordinate_system";

	herr_t status = -1;

	// coor_sys_type
	hid_t coor_sys_type = CGlobal::HDFCreateCoordinateSystemType();
	if (coor_sys_type > 0)
	{
		if (!bStoring) coor_sys = PHAST_Transform::MAP;
		status = CGlobal::HDFSerializeSafe(bStoring, loc_id, szCoorSys, coor_sys_type, 1, &coor_sys);
		ASSERT(!bStoring || status >= 0); // if storing assert status is valid

		VERIFY(H5Tclose(coor_sys_type) >= 0);
	}
	return status;
}

herr_t CGlobal::HDFSerializeZCoordinateSystem(bool bStoring, hid_t loc_id, PHAST_Transform::COORDINATE_SYSTEM &coor_sys)
{
	static const char szCoorSys[] = "z_coordinate_system";

	herr_t status = -1;

	// coor_sys_type
	hid_t coor_sys_type = CGlobal::HDFCreateCoordinateSystemType();
	if (coor_sys_type > 0)
	{
		if (!bStoring) coor_sys = PHAST_Transform::MAP;
		status = CGlobal::HDFSerializeSafe(bStoring, loc_id, szCoorSys, coor_sys_type, 1, &coor_sys);
		ASSERT(!bStoring || status >= 0); // if storing assert status is valid

		VERIFY(H5Tclose(coor_sys_type) >= 0);
	}
	return status;
}

// COMMENT: {10/21/2008 3:15:41 PM}herr_t CGlobal::HDFSerializePerimeterOption(bool bStoring, hid_t loc_id, Prism::PERIMETER_OPTION &perimeter_option)
// COMMENT: {10/21/2008 3:15:41 PM}{
// COMMENT: {10/21/2008 3:15:41 PM}	static const char szPerimeterOption[] = "perimeter_option";
// COMMENT: {10/21/2008 3:15:41 PM}	
// COMMENT: {10/21/2008 3:15:41 PM}	herr_t status = -1;
// COMMENT: {10/21/2008 3:15:41 PM}
// COMMENT: {10/21/2008 3:15:41 PM}	// source_type
// COMMENT: {10/21/2008 3:15:41 PM}	hid_t po_type = CGlobal::HDFCreatePerimeterOptionType();
// COMMENT: {10/21/2008 3:15:41 PM}	if (po_type > 0)
// COMMENT: {10/21/2008 3:15:41 PM}	{
// COMMENT: {10/21/2008 3:15:41 PM}		status = CGlobal::HDFSerialize(bStoring, loc_id, szPerimeterOption, po_type, 1, &perimeter_option);
// COMMENT: {10/21/2008 3:15:41 PM}	}
// COMMENT: {10/21/2008 3:15:41 PM}	return status;
// COMMENT: {10/21/2008 3:15:41 PM}}

// COMMENT: {10/21/2008 3:15:31 PM}hid_t CGlobal::HDFCreatePerimeterOptionType(void)
// COMMENT: {10/21/2008 3:15:31 PM}{
// COMMENT: {10/21/2008 3:15:31 PM}	herr_t status;
// COMMENT: {10/21/2008 3:15:31 PM}
// COMMENT: {10/21/2008 3:15:31 PM}	// Create the datatype
// COMMENT: {10/21/2008 3:15:31 PM}	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(Prism::PERIMETER_OPTION));
// COMMENT: {10/21/2008 3:15:31 PM}	ASSERT(enum_datatype > 0);
// COMMENT: {10/21/2008 3:15:31 PM}
// COMMENT: {10/21/2008 3:15:31 PM}	Prism::PERIMETER_OPTION nValue;
// COMMENT: {10/21/2008 3:15:31 PM}
// COMMENT: {10/21/2008 3:15:31 PM}	// Insert the enumerated data
// COMMENT: {10/21/2008 3:15:31 PM}	nValue = Prism::CONSTANT;
// COMMENT: {10/21/2008 3:15:31 PM}	status = H5Tenum_insert(enum_datatype, "CONSTANT", &nValue);
// COMMENT: {10/21/2008 3:15:31 PM}	ASSERT(status >= 0);
// COMMENT: {10/21/2008 3:15:31 PM}
// COMMENT: {10/21/2008 3:15:31 PM}	// Insert the enumerated data
// COMMENT: {10/21/2008 3:15:31 PM}	nValue = Prism::ATTRIBUTE;
// COMMENT: {10/21/2008 3:15:31 PM}	status = H5Tenum_insert(enum_datatype, "ATTRIBUTE", &nValue);
// COMMENT: {10/21/2008 3:15:31 PM}	ASSERT(status >= 0);
// COMMENT: {10/21/2008 3:15:31 PM}
// COMMENT: {10/21/2008 3:15:31 PM}	// Insert the enumerated data
// COMMENT: {10/21/2008 3:15:31 PM}	nValue = Prism::USE_Z;
// COMMENT: {10/21/2008 3:15:31 PM}	status = H5Tenum_insert(enum_datatype, "USE_Z", &nValue);
// COMMENT: {10/21/2008 3:15:31 PM}	ASSERT(status >= 0);
// COMMENT: {10/21/2008 3:15:31 PM}
// COMMENT: {10/21/2008 3:15:31 PM}	// Insert the enumerated data
// COMMENT: {10/21/2008 3:15:31 PM}	nValue = Prism::DEFAULT;
// COMMENT: {10/21/2008 3:15:31 PM}	status = H5Tenum_insert(enum_datatype, "DEFAULT", &nValue);
// COMMENT: {10/21/2008 3:15:31 PM}	ASSERT(status >= 0);
// COMMENT: {10/21/2008 3:15:31 PM}
// COMMENT: {10/21/2008 3:15:31 PM}	return enum_datatype;
// COMMENT: {10/21/2008 3:15:31 PM}}

std::string CGlobal::FullPath(std::string filename)
{
	TCHAR   fullPath[4096] = TEXT(""); 
	LPTSTR  lpszFilePart   = NULL;							    
	DWORD retval = GetFullPathName(
		filename.c_str(),
		4096,
		fullPath,
		&lpszFilePart);
	if (retval == 0)
	{
		std::string("");
	}
	return std::string(fullPath);
}

void CGlobal::InsertHeadings(CComboBox &combo, std::vector< std::string > headings)
{
	CDC* pDC = combo.GetDC();
	CSize size(0, 0);
	long nWidest = 0;

	CString strNone("None");
	combo.InsertString(0, strNone);
	size = pDC->GetTextExtent(strNone);
	nWidest = size.cx;

	CString str;

	for (size_t i = 0; i < headings.size(); ++i)
	{
		str.Format("%d - %s", (int)i, headings[i].c_str());
		combo.InsertString((int)i+1, str);

		size = pDC->GetTextExtent(str);
		if (nWidest < size.cx )
		{
			nWidest = size.cx;
		}
	}
	combo.SetDroppedWidth(nWidest);
}

BOOL CGlobal::IsValidShapefile(CString filename, CDataExchange* pDX /* = NULL */)
{
	filename.Trim();
	if (!filename.IsEmpty())
	{
		TCHAR path[MAX_PATH];
		::strcpy(path, filename);
		ATL::ATLPath::RenameExtension(path, ".shp");
		if (ATL::ATLPath::FileExists(path))
		{
			ATL::ATLPath::RenameExtension(path, ".shx");
			if (ATL::ATLPath::FileExists(path))
			{
				ATL::ATLPath::RenameExtension(path, ".dbf");
				if (ATL::ATLPath::FileExists(path))
				{
					return TRUE;
				}
				else
				{
					if (pDX)
					{
						CString str;
						str.Format("Shape file component \"%s\" not found.\n", path);
						::AfxMessageBox(str);
						pDX->Fail(); // throws
					}
				}
			}
			else
			{
				if (pDX)
				{
					CString str;
					str.Format("Shape file component \"%s\" not found.\n", path);
					::AfxMessageBox(str);
					pDX->Fail(); // throws
				}
			}
		}
		else
		{
			if (pDX)
			{
				CString str;
				str.Format("Shape file \"%s\" not found.\n", path);
				::AfxMessageBox(str);
				pDX->Fail(); // throws
			}
		}
	}
	if (pDX)
	{
		::AfxMessageBox("Please select a shape file.");
		pDX->Fail(); // throws
	}
	return FALSE;
}

BOOL CGlobal::IsValidArcraster(CString filename)
{
	if (filename.GetLength())
	{
		if (ATL::ATLPath::FileExists(filename))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGlobal::IsValidXYZFile(CString filename, CDataExchange* pDX /* = NULL */)
{
	filename.Trim();
	if (!filename.IsEmpty())
	{
		TCHAR path[MAX_PATH];
		::strcpy(path, filename);
		if (ATL::ATLPath::FileExists(path))
		{
			return TRUE;
		}
		else
		{
			if (pDX)
			{
				CString str;
				str.Format("XYZ file \"%s\" not found.\n", path);
				::AfxMessageBox(str);
				pDX->Fail(); // throws
			}
		}
	}
	if (pDX)
	{
		::AfxMessageBox("Please select an XYZ file.");
		pDX->Fail(); // throws
	}
	return FALSE;
}

BOOL CGlobal::IsValidXYZTFile(CString filename, CDataExchange* pDX /* = NULL */)
{
	filename.Trim();
	if (!filename.IsEmpty())
	{
		TCHAR path[MAX_PATH];
		::strcpy(path, filename);
		if (ATL::ATLPath::FileExists(path))
		{
			return TRUE;
		}
		else
		{
			if (pDX)
			{
				CString str;
				str.Format("XYZT file \"%s\" not found.\n", path);
				::AfxMessageBox(str);
				pDX->Fail(); // throws
			}
		}
	}
	if (pDX)
	{
		::AfxMessageBox("Please select an XYZT file.");
		pDX->Fail(); // throws
	}
	return FALSE;
}

BOOL CGlobal::FileExists(CString filename)
{
	return ATL::ATLPath::FileExists(filename);
}

hid_t CGlobal::HDFCreatePropType(void)
{
	ASSERT(PROP_UNDEFINED == 100);
	ASSERT(PROP_FIXED     == 101);
	ASSERT(PROP_LINEAR    == 102);
	ASSERT(PROP_ZONE      == 103);
	ASSERT(PROP_POINTS    == 105);
	ASSERT(PROP_XYZ       == 106);
	ASSERT(PROP_XYZT      == 110);

	herr_t status;

	// Create the datatype
	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(PROP_TYPE));
	ASSERT(enum_datatype > 0);

	PROP_TYPE nValue;

	// Insert the enumerated data
	nValue = PROP_UNDEFINED;
	status = H5Tenum_insert(enum_datatype, "PROP_UNDEFINED", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PROP_FIXED;
	status = H5Tenum_insert(enum_datatype, "PROP_FIXED", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PROP_LINEAR;
	status = H5Tenum_insert(enum_datatype, "PROP_LINEAR", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PROP_ZONE;
	status = H5Tenum_insert(enum_datatype, "PROP_ZONE", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	// nValue = PROP_MIXTURE;
	// status = H5Tenum_insert(enum_datatype, "PROP_MIXTURE", &nValue);
	//
	// Note: This must be inserted in order to be backward compatible
	//
	int n = 104;
	status = H5Tenum_insert(enum_datatype, "PROP_MIXTURE", &n);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PROP_POINTS;
	status = H5Tenum_insert(enum_datatype, "PROP_POINTS", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PROP_XYZ;
	status = H5Tenum_insert(enum_datatype, "PROP_XYZ", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = PROP_XYZT;
	status = H5Tenum_insert(enum_datatype, "PROP_XYZT", &nValue);
	ASSERT(status >= 0);

	return enum_datatype;
}

hid_t CGlobal::HDFCreateCellFace(void)
{
	herr_t status;

	// Create the datatype
	hid_t enum_datatype = H5Tcreate(H5T_ENUM, sizeof(Cell_Face));
	ASSERT(enum_datatype > 0);

	Cell_Face nValue;

	// Insert the enumerated data
	nValue = CF_X;
	status = H5Tenum_insert(enum_datatype, "CF_X", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_Y;
	status = H5Tenum_insert(enum_datatype, "CF_Y", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_Z;
	status = H5Tenum_insert(enum_datatype, "CF_Z", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_XN;
	status = H5Tenum_insert(enum_datatype, "CF_XN", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_XP;
	status = H5Tenum_insert(enum_datatype, "CF_XP", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_YN;
	status = H5Tenum_insert(enum_datatype, "CF_YN", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_YP;
	status = H5Tenum_insert(enum_datatype, "CF_YP", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_ZN;
	status = H5Tenum_insert(enum_datatype, "CF_ZN", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_ZP;
	status = H5Tenum_insert(enum_datatype, "CF_ZP", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_NONE;
	status = H5Tenum_insert(enum_datatype, "CF_NONE", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_UNKNOWN;
	status = H5Tenum_insert(enum_datatype, "CF_UNKNOWN", &nValue);
	ASSERT(status >= 0);

	// Insert the enumerated data
	nValue = CF_ALL;
	status = H5Tenum_insert(enum_datatype, "CF_ALL", &nValue);
	ASSERT(status >= 0);

	return enum_datatype;
}

void CGlobal::ComputeDisplayToWorld(vtkRenderer *renderer, double x, double y, double z, double worldPt[4])
{
	if (!renderer)
	{
		return;
	}

	renderer->SetDisplayPoint(x, y, z);
	renderer->DisplayToWorld();
	renderer->GetWorldPoint(worldPt);
	if (worldPt[3])
	{
		worldPt[0] /= worldPt[3];
		worldPt[1] /= worldPt[3];
		worldPt[2] /= worldPt[3];
		worldPt[3] = 1.0;
	}
}

void CGlobal::ComputeWorldToDisplay(vtkRenderer *renderer, double x, double y, double z, double displayPt[4])
{
	if (!renderer)
	{
		return;
	}

	renderer->SetWorldPoint(x, y, z, 1.0);
	renderer->WorldToDisplay();
	renderer->GetDisplayPoint(displayPt);
}

double CGlobal::ComputeRadius(vtkRenderer *renderer)
{
	double radius = 1.0;
	if (renderer)
	{
		int i;
		double z;
		double windowLowerLeft[4], windowUpperRight[4];
		double *viewport = renderer->GetViewport();
		int *winSize = renderer->GetRenderWindow()->GetSize();
		double focalPoint[4];

		// get the focal point in world coordinates
		//
		vtkCamera *camera = renderer->GetActiveCamera();	
		double cameraFP[4];
		camera->GetFocalPoint((double*)cameraFP); cameraFP[3] = 1.0;

		CGlobal::ComputeWorldToDisplay(renderer, cameraFP[0], cameraFP[1], cameraFP[2], focalPoint);

		z = focalPoint[2];

		double x = winSize[0] * viewport[0];
		double y = winSize[1] * viewport[1];
		CGlobal::ComputeDisplayToWorld(renderer, x, y, z, windowLowerLeft);

		x = winSize[0] * viewport[2];
		y = winSize[1] * viewport[3];
		CGlobal::ComputeDisplayToWorld(renderer, x, y, z, windowUpperRight);

		for (radius=0.0, i=0; i<3; ++i)
		{
			radius += (windowUpperRight[i] - windowLowerLeft[i]) * (windowUpperRight[i] - windowLowerLeft[i]);
		}
		radius = ::sqrt(radius);
	}
	else
	{
		TRACE("WARNING renderer is NULL in CGlobal::ComputeRadius\n");
	}
	return radius;
}

double CGlobal::RadiusFactor = 0.008;

void CGlobal::SetRadiusFactor(double factor)
{
	if (0 <= factor)
	{
		CGlobal::RadiusFactor = factor;
	}
}

double CGlobal::GetRadiusFactor(void)
{
	return CGlobal::RadiusFactor;
}

double CGlobal::ComputeHandleRadius(vtkRenderer *renderer)
{
	return CGlobal::ComputeRadius(renderer) * CGlobal::GetRadiusFactor();
}

#define GET_ABS_PATH_MACRO(D, C, P) \
do { \
	if (C.P && (C.P->type == PROP_XYZ || C.P->type == PROP_XYZT)) { \
		if (C.P->data_source) { \
			std::string filename = C.P->data_source->Get_file_name(); \
			if (filename.length() > 0) { \
				C.P->data_source->Set_file_name(D->GetAbsolutePath(lpszPathName, filename)); \
			} \
		} \
	} \
} while(0)

void CGlobal::PathsRelativeToAbsolute(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CGridElt& elt)
{
	GET_ABS_PATH_MACRO(pDoc, elt, active);
	GET_ABS_PATH_MACRO(pDoc, elt, porosity);
	GET_ABS_PATH_MACRO(pDoc, elt, kx);
	GET_ABS_PATH_MACRO(pDoc, elt, ky);
	GET_ABS_PATH_MACRO(pDoc, elt, kz);
	GET_ABS_PATH_MACRO(pDoc, elt, storage);
	GET_ABS_PATH_MACRO(pDoc, elt, alpha_long);
	GET_ABS_PATH_MACRO(pDoc, elt, alpha_trans);
	GET_ABS_PATH_MACRO(pDoc, elt, alpha_horizontal);
	GET_ABS_PATH_MACRO(pDoc, elt, alpha_vertical);
}

#define GET_ABS_PATH_TIMESERIES_MACRO(D, C, TS) \
do { \
	CTimeSeries<Cproperty>::iterator it = C.TS.begin(); \
	for (; it != C.TS.end(); ++it) { \
		if ((*it).second.type == PROP_XYZ || (*it).second.type == PROP_XYZT) { \
			if ((*it).second.data_source) { \
				std::string filename = (*it).second.data_source->Get_file_name(); \
				if (filename.length() > 0) { \
					(*it).second.data_source->Set_file_name(D->GetAbsolutePath(lpszPathName, filename)); \
				} \
			} \
		} \
	} \
} while(0)

void CGlobal::PathsRelativeToAbsolute(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CBC& bc)
{	
	// Cproperty
	GET_ABS_PATH_MACRO(pDoc, bc, bc_k);
	GET_ABS_PATH_MACRO(pDoc, bc, bc_thick);

	// CTimeSeries<Cproperty>
	GET_ABS_PATH_TIMESERIES_MACRO(pDoc, bc, m_bc_head);
	GET_ABS_PATH_TIMESERIES_MACRO(pDoc, bc, m_bc_flux);
	GET_ABS_PATH_TIMESERIES_MACRO(pDoc, bc, m_bc_solution);
}

void CGlobal::PathsRelativeToAbsolute(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CChemIC& chemIC)
{	
	// Cproperty
	GET_ABS_PATH_MACRO(pDoc, chemIC, equilibrium_phases);
	GET_ABS_PATH_MACRO(pDoc, chemIC, exchange);
	GET_ABS_PATH_MACRO(pDoc, chemIC, gas_phase);
	GET_ABS_PATH_MACRO(pDoc, chemIC, kinetics);
	GET_ABS_PATH_MACRO(pDoc, chemIC, solid_solutions);
	GET_ABS_PATH_MACRO(pDoc, chemIC, solution);
	GET_ABS_PATH_MACRO(pDoc, chemIC, surface);
}

void CGlobal::PathsRelativeToAbsolute(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CHeadIC& headIC)
{
	// Cproperty
	GET_ABS_PATH_MACRO(pDoc, headIC, head);
}

#define GET_REL_PATH_MACRO(D, C, P) \
do { \
	if (C.P && (C.P->type == PROP_XYZ || C.P->type == PROP_XYZT)) { \
		if (C.P->data_source) { \
			std::string filename = C.P->data_source->Get_file_name(); \
			if (filename.length() > 0) { \
				C.P->data_source->Set_file_name(D->GetRelativePath(lpszPathName, filename)); \
			} \
		} \
	} \
} while(0)

void CGlobal::PathsAbsoluteToRelative(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CGridElt& elt)
{
	GET_REL_PATH_MACRO(pDoc, elt, active);
	GET_REL_PATH_MACRO(pDoc, elt, porosity);
	GET_REL_PATH_MACRO(pDoc, elt, kx);
	GET_REL_PATH_MACRO(pDoc, elt, ky);
	GET_REL_PATH_MACRO(pDoc, elt, kz);
	GET_REL_PATH_MACRO(pDoc, elt, storage);
	GET_REL_PATH_MACRO(pDoc, elt, alpha_long);
	GET_REL_PATH_MACRO(pDoc, elt, alpha_trans);
	GET_REL_PATH_MACRO(pDoc, elt, alpha_horizontal);
	GET_REL_PATH_MACRO(pDoc, elt, alpha_vertical);
}

#define GET_REL_PATH_TIMESERIES_MACRO(D, C, TS) \
do { \
	CTimeSeries<Cproperty>::iterator it = C.TS.begin(); \
	for (; it != C.TS.end(); ++it) { \
		if ((*it).second.type == PROP_XYZ || (*it).second.type == PROP_XYZT) { \
			if ((*it).second.data_source) { \
				std::string filename = (*it).second.data_source->Get_file_name(); \
				if (filename.length() > 0) { \
					(*it).second.data_source->Set_file_name(D->GetRelativePath(lpszPathName, filename)); \
				} \
			} \
		} \
	} \
} while(0)

void CGlobal::PathsAbsoluteToRelative(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CBC& bc)
{
	// Cproperty
	GET_REL_PATH_MACRO(pDoc, bc, bc_k);
	GET_REL_PATH_MACRO(pDoc, bc, bc_thick);

	// CTimeSeries<Cproperty>
	GET_REL_PATH_TIMESERIES_MACRO(pDoc, bc, m_bc_head);
	GET_REL_PATH_TIMESERIES_MACRO(pDoc, bc, m_bc_flux);
	GET_REL_PATH_TIMESERIES_MACRO(pDoc, bc, m_bc_solution);
}

void CGlobal::PathsAbsoluteToRelative(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CChemIC& chemIC)
{
	// Cproperty
	GET_REL_PATH_MACRO(pDoc, chemIC, equilibrium_phases);
	GET_REL_PATH_MACRO(pDoc, chemIC, exchange);
	GET_REL_PATH_MACRO(pDoc, chemIC, gas_phase);
	GET_REL_PATH_MACRO(pDoc, chemIC, kinetics);
	GET_REL_PATH_MACRO(pDoc, chemIC, solid_solutions);
	GET_REL_PATH_MACRO(pDoc, chemIC, solution);
	GET_REL_PATH_MACRO(pDoc, chemIC, surface);
}

void CGlobal::PathsAbsoluteToRelative(LPCTSTR lpszPathName, CWPhastDoc* pDoc, CHeadIC& headIC)
{
	// Cproperty
	GET_REL_PATH_MACRO(pDoc, headIC, head);
}

// based on DDX_Text(CDataExchange* pDX, int nIDC, float& value)
void CGlobal::DDX_Text_Safe(CDataExchange* pDX, int nIDC, float& value)
{
	CGlobal::TextFloatFormat(pDX, nIDC, &value, value, FLT_DIG);
}

// based on DDX_Text(CDataExchange* pDX, int nIDC, double& value)
void CGlobal::DDX_Text_Safe(CDataExchange *pDX, int nIDC, double &value)
{
	CGlobal::TextFloatFormat(pDX, nIDC, &value, value, DBL_DIG);
}

// based on ::AfxTextFloatFormat
void CGlobal::TextFloatFormat(CDataExchange* pDX, int nIDC, void* pData, double value, int nSizeGcvt)
{
	ASSERT(pData != NULL);

	pDX->PrepareEditCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	
	const int TEXT_BUFFER_SIZE = 400;
	TCHAR szBuffer[TEXT_BUFFER_SIZE];
	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
		double d;
		if (_sntscanf_s(szBuffer, _countof(szBuffer), _T("%lf"), &d) != 1)
		{
			//AfxMessageBox(AFX_IDP_PARSE_REAL);
			//pDX->Fail();            // throws exception
			if (nSizeGcvt == FLT_DIG)
			{
				*((float*)pData) = std::numeric_limits<float>::signaling_NaN();
			}
			else
			{
				*((double*)pData) = std::numeric_limits<double>::signaling_NaN();
			}
		}
		else
		{
			if (nSizeGcvt == FLT_DIG)
			{
				*((float*)pData) = (float)d;
			}
			else
			{
				*((double*)pData) = d;
			}
		}
	}
	else
	{
		if (value == value)
		{
			ATL_CRT_ERRORCHECK_SPRINTF(_sntprintf_s(szBuffer, _countof(szBuffer), _countof(szBuffer) -1, _T("%.*g"), nSizeGcvt, value));
			AfxSetWindowText(hWndCtrl, szBuffer);
		}
		else
		{
			AfxSetWindowText(hWndCtrl, _T(""));
		}
	}
}

BOOL CGlobal::DDX_Text_Safe(CDataExchange* pDX, int nIDC, int& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		try 
		{
			int i;
			CGlobal::TextWithFormat(pDX, nIDC, _T("%d"), &i);
			value = i;
		}
		catch(...)
		{
			value = -1;
			return FALSE;
		}
	}
	else
	{
		if (value < 0)
		{
			pDX->PrepareEditCtrl(nIDC);
			HWND hWndCtrl;
			pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
			AfxSetWindowText(hWndCtrl, _T(""));
		}
		else
		{
			CGlobal::TextWithFormat(pDX, nIDC, _T("%d"), value);
		}
	}
	return TRUE;
}

// based on AFX_STATIC void AFX_CDECL _Afx_DDX_TextWithFormat(CDataExchange* pDX, int nIDC,	LPCTSTR lpszFormat, UINT nIDPrompt, ...)
// only supports windows output formats - no floating point
void CGlobal::TextWithFormat(CDataExchange* pDX, int nIDC, LPCTSTR lpszFormat, ...)
{
	va_list pData;
	va_start(pData, lpszFormat);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	ASSERT( hWndCtrl != NULL );

	const int SZT_SIZE = 64;
	TCHAR szT[SZT_SIZE];
	if (pDX->m_bSaveAndValidate)
	{
		void* pResult;

		pResult = va_arg( pData, void* );
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, _countof(szT));
		if (_sntscanf_s(szT, _countof(szT), lpszFormat, pResult) != 1)
		{
			//AfxMessageBox(nIDPrompt);
			pDX->Fail();        // throws exception
		}
	}
	else
	{
		ATL_CRT_ERRORCHECK_SPRINTF(_vsntprintf_s(szT, _countof(szT), _countof(szT) - 1, lpszFormat, pData));
			// does not support floating point numbers - see dlgfloat.cpp
		AfxSetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
}

void CGlobal::DumpAndLoadPrism(const Prism &src, Prism &dest)
{
	// TOP
	if (src.top.Get_user_source_type() != Data_source::NONE)
	{
		std::ostringstream oss;
		oss.precision(DBL_DIG);
		oss << "\t\t-top       " << src.top;

		TRACE(oss.str().c_str());
		std::istringstream iss(oss.str());

		// read top
		while(dest.Read(iss))
		{
			if (iss.rdstate() & std::ios::eofbit) break;
			iss.clear();
		}
		ASSERT(dest.top.Get_defined());
	}

	// BOTTOM
	if (src.bottom.Get_user_source_type() != Data_source::NONE)
	{
		std::ostringstream oss;
		oss.precision(DBL_DIG);
		oss << "\t\t-bottom       " << src.bottom;

		TRACE(oss.str().c_str());
		std::istringstream iss(oss.str());

		// read bottom
		while(dest.Read(iss))
		{
			if (iss.rdstate() & std::ios::eofbit) break;
			iss.clear();
		}
		ASSERT(dest.bottom.Get_defined());
	}

	// PERIMETER
	if (src.perimeter.Get_user_source_type() != Data_source::NONE)
	{
		std::ostringstream oss;
		oss.precision(DBL_DIG);
		oss << "\t\t-perimeter       " << src.perimeter;

		TRACE(oss.str().c_str());
		std::istringstream iss(oss.str());

		// read perimeter
		while(dest.Read(iss))
		{
			if (iss.rdstate() & std::ios::eofbit) break;
			iss.clear();
		}
		ASSERT(dest.perimeter.Get_defined());
	}
}

bool CGlobal::PolygonIntersectsSelf(std::vector<Point> &vect)
{
	// TODO check for 3 coincident points
	//

	double *a, *b, *c, *d;
	double rn, sn, den, r, s;
	a = vect[0].get_coord();
	for (size_t i = 0; i < vect.size(); ++i)
	{
		b = vect[(i + 1) % vect.size()].get_coord();
		c = vect[(i + 2) % vect.size()].get_coord();
		for (size_t j = i+2; j <= i+vect.size()-2; ++j)
		{
			d   = vect[(j + 1) % vect.size()].get_coord();
			rn  = (a[1]-c[1])*(d[0]-c[0])-(a[0]-c[0])*(d[1]-c[1]);
			sn  = (a[1]-c[1])*(b[0]-a[0])-(a[0]-c[0])*(b[1]-a[1]);
			den = (b[0]-a[0])*(d[1]-c[1])-(b[1]-a[1])*(d[0]-c[0]);
			if (den != 0)
			{
				r = rn/den;
				s = sn/den;
				if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
				{
					return true;
				}
			}
			else if (rn == 0)
			{
				// both AB and CD are collinear (coincident)
				// project values to each axis to check for overlap
				for (size_t k = 0; k < 2; ++k)
				{
					double minab = (a[k] < b[k]) ? a[k] : b[k];
					double maxab = (a[k] > b[k]) ? a[k] : b[k];
					if (minab <= c[k] && c[k] <= maxab)
					{
						return true;
					}
					if (minab <= d[k] && d[k] <= maxab)
					{
						return true;
					}
				}
			}
			c = d;
		}
		a = b;
	}
	return false;
}
