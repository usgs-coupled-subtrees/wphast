#include "StdAfx.h"
#include "TestHDFFile.h"

#include <atlcrypt.h>
#include "Global.h"

void TestHDFFile::TestMD5(void)
{
	CCryptProv prov;
	HRESULT hr = prov.Initialize(PROV_RSA_FULL, NULL, MS_DEF_PROV , CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	if (hr == NTE_KEYSET_NOT_DEF)
	{
		hr = prov.Initialize(PROV_RSA_FULL, NULL, MS_DEF_PROV, CRYPT_VERIFYCONTEXT | CRYPT_SILENT | CRYPT_NEWKEYSET);
	}
	CPPUNIT_ASSERT(SUCCEEDED(hr));

	CCryptMD5Hash hash0;
	hr = hash0.Initialize(prov);
	CPPUNIT_ASSERT(SUCCEEDED(hr));

	hr = hash0.AddString("abc");
	CPPUNIT_ASSERT(SUCCEEDED(hr));

	DWORD dw = 32;
	BYTE val[32];

	hr = hash0.GetValue(val, &dw);
	CPPUNIT_ASSERT(SUCCEEDED(hr));

    CString temp;
	CString result;
    for(int i = 0; i < 16; ++i)
    {
		temp.Format(_T("%02x"), val[i]);
		result += temp;
    }
	CPPUNIT_ASSERT_EQUAL(std::string("900150983cd24fb0d6963f7d28e17f72"), std::string(result));

	// Note: can't reuse a hash
	CCryptMD5Hash hash1;
	hr = hash1.Initialize(prov);
	CPPUNIT_ASSERT(SUCCEEDED(hr));

	hr = hash1.AddString("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
	CPPUNIT_ASSERT(SUCCEEDED(hr));

	hr = hash1.GetValue(val, &dw);
	CPPUNIT_ASSERT(SUCCEEDED(hr));

	result.Empty();
    for(int i = 0; i < 16; ++i)
    {
		temp.Format(_T("%02x"), val[i]);
		result += temp;
    }
	CPPUNIT_ASSERT_EQUAL(std::string("8215ef0796a20bcaaae116d3876c664a"), std::string(result));
}

void TestHDFFile::TestImage(void)
{
	const char hdf_file[] = "Test/article.h5";

	::remove("Test/article.jpg");

	CPPUNIT_ASSERT( ::rename("Test/article.jpg.orig", "Test/article.jpg") == 0 );

	// create hdf file
	hid_t file_id = ::H5Fcreate(hdf_file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	CPPUNIT_ASSERT(file_id > 0);

	// store in hdf
	std::string in_path("Test/article.jpg");
	std::string in_md5;
	FILETIME ftWriteIn;
	CGlobal::HDFFile(true, file_id, "file0", in_path, in_md5, ftWriteIn);
	CPPUNIT_ASSERT_EQUAL(std::string("c2c21ce573987fe68ccee59e4a01a0fa"), std::string(in_md5));

	// close hdf file
	herr_t status = ::H5Fclose(file_id);
	ASSERT(status >= 0);

	CPPUNIT_ASSERT( ::remove("Test/article.jpg") == 0 );

	file_id = ::H5Fopen(hdf_file, H5F_ACC_RDONLY, H5P_DEFAULT);
	ASSERT(file_id > 0);

	// create from hdf
	std::string out_path;
	std::string out_md5;
	FILETIME ftWriteOut;
	CGlobal::HDFFile(false, file_id, "file0", out_path, out_md5, ftWriteOut);

	CPPUNIT_ASSERT_EQUAL(std::string("Test/article.jpg"), std::string(out_path));
	CPPUNIT_ASSERT_EQUAL(std::string("c2c21ce573987fe68ccee59e4a01a0fa"), std::string(out_md5));

	// close hdf file
	status = ::H5Fclose(file_id);
	ASSERT(status >= 0);

	CPPUNIT_ASSERT( ::rename("Test/article.jpg", "Test/article.jpg.orig") == 0 );
}

#include "srcinput/BC_info.h"

void TestHDFFile::TestSizes(void)
{
	CPPUNIT_ASSERT_EQUAL(H5Tget_size(H5T_NATIVE_INT),   sizeof(int));

	CPPUNIT_ASSERT_EQUAL(H5Tget_size(H5T_NATIVE_INT),   sizeof(BC_info::BC_TYPE));

#ifdef _WIN64
	CPPUNIT_ASSERT_EQUAL(H5Tget_size(H5T_NATIVE_LLONG), sizeof(size_t));
#else
	CPPUNIT_ASSERT_EQUAL(H5Tget_size(H5T_NATIVE_INT), sizeof(size_t));
#endif
}