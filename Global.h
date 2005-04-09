#pragma once

class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkProp3D;

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#include <list>
#include <string>
// #include "BC.h"
#include "TimeSeries.h"
class CBC;
class Cproperty;
class CCheckTreeCtrl;
class CWPhastView;

class vtkTransform;
class CWorldTransform;

// user defined messages
#define UM_CHECKSTATECHANGE (WM_USER + 100)
#define UM_DDX_FAILURE (WM_USER + 101)

class CGlobal
{
public:
	static void PickProp(CWPhastView* pView, vtkProp3D* pProp3D);
	static void PickProp(vtkProp3D* pProp3D, vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor);
	static void UnPickProp(vtkProp3D* pProp3D, vtkRenderer* pRenderer, vtkRenderWindowInteractor* pRenderWindowInteractor);
	static void LoadRTFString(std::string& str, UINT nID);
	static void ReadList(hid_t loc_id, const char *name, std::list<std::string> &rlist);
	static void WriteList(hid_t loc_id, const char *name, std::list<LPCTSTR> &rlist);
	static herr_t HDFSerialize(bool bStoring, hid_t loc_id, const char* szName, hid_t mem_type_id, hsize_t count, void* buf);
	static herr_t HDFSerializeWithSize(bool bStoring, hid_t loc_id, const char* szName, hid_t mem_type_id, hsize_t& count, void* buf);
	static bool IsValidTransform(vtkTransform* pTransform);
	static void EnableFace(CDataExchange* pDX, BOOL bEnable);
	static void DDX_Face(CDataExchange* pDX, CBC& r_bc);
	static void DDX_Property(CDataExchange* pDX, CCheckTreeCtrl* pTree, HTREEITEM hti, Cproperty* value, Cproperty* fixed, Cproperty* linear);
	static void EnableFixed(CWnd* pDlgWnd, BOOL bEnable);
	static void EnableLinearInterpolation(CWnd* pDlgWnd, BOOL bEnable);
	static void DDX_AssocFixed(CDataExchange* pDX, CTreeCtrl* pTree, HTREEITEM hti, CBC& value);

	static herr_t HDFSerializeStringOrNull(bool bStoring, hid_t loc_id, const char* szName, char** szString);
	static herr_t HDFSerializeString(bool bStoring, hid_t loc_id, const char* szName, std::string& str);

	static herr_t HDFSerializeBinaryFile(bool bStoring, hid_t loc_id, const char* szName, const char* szFileName);

	static herr_t HDFSerializeBool(bool bStoring, hid_t loc_id, const char* szName, bool& bValue);
	static void MinimizeLengthUnits(CString& strUnits);

	static bool GetEditValue(CWnd *pDlg, int nID, double& d);
	static bool SimpleFloatParse(LPCTSTR lpszText, double& d);

	static int LoadWorldFile(const char *filename, CWorldTransform &wtrans);

public:
	static herr_t HDFSerializeAllocate(bool bStoring, hid_t loc_id, const char* szName, hsize_t& count, double** buffer);
	static herr_t HDFSerializeAllocate(bool bStoring, hid_t loc_id, const char* szName, hsize_t& count, int** buffer);

public:
	static void DDX_GridTimeSeries(CDataExchange* pDX, int nIDC, CTimeSeries<Cproperty> &r_ts);

private:
	template<typename T> static herr_t HDFSerializeAllocate(bool bStoring, hid_t loc_id, const char* szName, hid_t mem_type_id, hsize_t& count, T** buffer);

private:
	CGlobal(void);
	~CGlobal(void);
public:
	static int AddLengthUnits(CComboBox* pCombo);
	static std::string GetStdLengthUnits(const char* unit);
	
	static int AddTimeUnits(CComboBox* pCombo);
	static std::string GetStdTimeUnits(const char* unit);

	static int AddVolumeUnits(CComboBox* pCombo);
	static std::string GetStdVolumeUnits(const char* unit);

	static int AddTimeUnitsDenom(CComboBox* pCombo);
	static std::string GetStdTimeUnitsDenom(const char* unit);

	static int AddLengthUnitsDenom(CComboBox* pCombo);
	static std::string GetStdLengthUnitsDenom(const char* unit);

};

template<typename T>
herr_t CGlobal::HDFSerializeAllocate(bool bStoring, hid_t loc_id, const char* szName, hid_t mem_type_id, hsize_t& count, T** buffer)
{
	hsize_t dims[1], maxdims[1];
	hid_t dspace_id = 0;
	hid_t dset_id = 0;
	herr_t status;
	herr_t return_val;

	if (bStoring)
	{
		ASSERT(*buffer != NULL && count > 0); // bad data
		if (*buffer == NULL || count <= 0) return -1;

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
				status = ::H5Dwrite(dset_id, mem_type_id, dspace_id, H5S_ALL, H5P_DEFAULT, *buffer);
				if (status < 0)
				{
					ASSERT(FALSE); // unable to write dataset
					return_val = status;
					goto HDFSerializeAllocateError;
				}
				// Close the szName dataset
				status = ::H5Dclose(dset_id);
				if (status < 0)
				{
					ASSERT(FALSE); // unable to close dataset
					return_val = status;
					goto HDFSerializeAllocateError;
				}
			}
			else
			{
				ASSERT(FALSE); // unable to create dataspace
				return_val = dset_id;
				goto HDFSerializeAllocateError;
			}
			// Close the szName dataspace
			status = ::H5Sclose(dspace_id);
			if (status < 0)
			{
				ASSERT(FALSE); // unable to close dataspace
				return_val = status;
				goto HDFSerializeAllocateError;
			}
		}
		else
		{
			ASSERT(FALSE); // unable to create dataspace
			return_val = dspace_id;
			goto HDFSerializeAllocateError;
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

					if (*buffer) delete[] *buffer;
					*buffer = new T[dims[0]];
					ASSERT(*buffer);

					// read the szName dataset
					status = ::H5Dread(dset_id, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, *buffer);
					if (status < 0)
					{
						ASSERT(FALSE); // unable to read dataset
						return_val = status;
						goto HDFSerializeAllocateError;
					}
					// Close the szName dataspace
					status = ::H5Sclose(dspace_id);
					ASSERT(status >= 0); // unable to close dataspace
				}
				else
				{
					ASSERT(FALSE); // unable to get_simple_extent_dims
					return_val = n_dims;
					goto HDFSerializeAllocateError;
				}
				// Close the szName dataset
				status = ::H5Dclose(dset_id);
				ASSERT(status >= 0); // unable to close dataset
			}
			else
			{
				ASSERT(FALSE); // unable to open dataspace
				return_val = dspace_id;
				goto HDFSerializeAllocateError;
			}
		}
		else
		{
			// assume zero sized
			count = 0;
			if (*buffer) delete[] *buffer;
			*buffer = 0;

			return_val = dset_id;
			goto HDFSerializeAllocateError;
		}
	}

	// ok if here
	return 0; // no errors

HDFSerializeAllocateError:

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
