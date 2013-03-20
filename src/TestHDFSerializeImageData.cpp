#include "StdAfx.h"
#include "TestHDFSerializeImageData.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include <vtkPolyData.h>
#include <vtkXMLImageDataReader.h>
#include "srcXMLImageDataWriter.h"

#include <vtkIOInstantiator.h> /* req'd for vtkZLibDataCompressor to be
                                  registered for vtkInstantiator used by
								  vtkXMLPolyDataReader/vtkXMLReader */
#include "Global.h"

void TestHDFSerializeImageData::testImageDataWriter(void)
{
	const char png_filename[] = "Test/sitemap.png";

	srcXMLImageDataWriter *writer = 0;
	vtkImageReader2Factory *factory = 0;
	vtkImageReader2 *pImageReader2 = 0;

	try
	{
		// load png and write xml
		if (factory = vtkImageReader2Factory::New())
		{
			if (pImageReader2 = factory->CreateImageReader2(png_filename))
			{
				pImageReader2->SetFileName(png_filename);
				pImageReader2->Update();				
				if (vtkImageData* id = pImageReader2->GetOutput())
				{
					if (writer = srcXMLImageDataWriter::New())
					{
						// set file name
						std::ostringstream oss;
						oss << "Test/actual.sitemap." << writer->GetDefaultFileExtension();

						std::ofstream ofs(oss.str().c_str());
						writer->SetOstream(&ofs);

						writer->SetInput(id);
						writer->SetDataModeToBinary();
						CPPUNIT_ASSERT(writer->Write() == 1);

						writer->Delete();
						writer = 0;
					}
				}
				pImageReader2->Delete();
				pImageReader2 = 0;
			}
			factory->Delete();
			factory = 0;
		}
	}
	catch (...)
	{
		if (writer)        writer->Delete();
		if (pImageReader2) pImageReader2->Delete();
		if (factory)       factory->Delete();
		throw;
	}

	vtkXMLImageDataReader *reader = 0;

	std::ifstream expected("Test/expected.sitemap.vti");
	CPPUNIT_ASSERT(expected.is_open());

	std::ifstream actual("Test/actual.sitemap.vti");
	CPPUNIT_ASSERT(actual.is_open());

	std::string str_expected;
	std::string str_actual;
	while(!(expected.eof()) && !(actual.eof()))
	{
		std::getline(expected, str_expected, '\n');
		std::getline(actual,   str_actual,   '\n');

#if defined(SKIP)
		::OutputDebugString("Expected:\n");
		::OutputDebugString(str_expected.c_str());
		::OutputDebugString("\n");

		::OutputDebugString("Actual:\n");
		::OutputDebugString(str_actual.c_str());
		::OutputDebugString("\n");
#endif
		CPPUNIT_ASSERT( str_expected == str_actual );
	}
	CPPUNIT_ASSERT( expected.eof() && actual.eof() );
	actual.close();
	CPPUNIT_ASSERT( ::remove("Test/actual.sitemap.vti") == 0 );
}

void TestHDFSerializeImageData::testHDF(void)
{
	vtkXMLImageDataReader *reader = 0;
	vtkXMLImageDataWriter *writer = 0;
	vtkImageData *deserialized = 0;

	try
	{
		reader = vtkXMLImageDataReader::New();
		reader->SetFileName("Test/expected.sitemap.vti");
		reader->Update(); // force read

		vtkImageData *output = reader->GetOutput();

		CPPUNIT_ASSERT(output);

		hid_t file_id = H5Fcreate("Test/imagedata.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		CPPUNIT_ASSERT(file_id > 0);

		herr_t e = CGlobal::HDFSerializeImageData(true, file_id, "ImageData", output);
		CPPUNIT_ASSERT(e >= 0);

		herr_t s = H5Fclose(file_id);
		CPPUNIT_ASSERT(s >= 0);

		// now open and read into deserialized
		file_id = H5Fopen("Test/imagedata.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
		CPPUNIT_ASSERT(file_id > 0);

		e = CGlobal::HDFSerializeImageData(false, file_id, "ImageData", deserialized);
		CPPUNIT_ASSERT(e >= 0);

		CPPUNIT_ASSERT(deserialized);

		writer = vtkXMLImageDataWriter::New();
		writer->SetInput(deserialized);
		writer->SetDataModeToBinary();
		writer->SetFileName("Test/deserialized.sitemap.vti");
		CPPUNIT_ASSERT(writer->Write() == 1);

		// clean-up
		reader->Delete();
		reader = 0;

		writer->Delete();
		writer = 0;

		deserialized->Delete();
		deserialized = 0;

	}
	catch (...)
	{
		if (reader) reader->Delete();
		if (writer) writer->Delete();
		if (deserialized) deserialized->Delete();
		throw;
	}

	std::ifstream expected("Test/expected.sitemap.vti");
	CPPUNIT_ASSERT(expected.is_open());

	std::ifstream actual("Test/deserialized.sitemap.vti");
	CPPUNIT_ASSERT(actual.is_open());

	std::string str_expected;
	std::string str_actual;
	while(!(expected.eof()) && !(actual.eof()))
	{
		std::getline(expected, str_expected, '\n');
		std::getline(actual,   str_actual,   '\n');

#if defined(SKIP)
		::OutputDebugString("Expected:\n");
		::OutputDebugString(str_expected.c_str());
		::OutputDebugString("\n");

		::OutputDebugString("Actual:\n");
		::OutputDebugString(str_actual.c_str());
		::OutputDebugString("\n");
#endif
		CPPUNIT_ASSERT( str_expected == str_actual );
	}
	CPPUNIT_ASSERT( expected.eof() && actual.eof() );
	actual.close();
	CPPUNIT_ASSERT( ::remove("Test/deserialized.sitemap.vti") == 0 );
}
