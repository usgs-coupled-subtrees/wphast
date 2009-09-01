#include "StdAfx.h"
#include "TestHDFSerializePolyData.h"

#include <fstream>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#include <vtkIOInstantiator.h> /* req'd for vtkZLibDataCompressor to be 
                                  registered for vtkInstantiator used by
								  vtkXMLPolyDataReader/vtkXMLReader */

#include "Global.h"

void TestHDFSerializePolyData::testPolyDataReaderWriter(void)
{
	vtkXMLPolyDataReader *reader = 0;
	vtkXMLPolyDataWriter *writer = 0;
	try
	{
		reader = vtkXMLPolyDataReader::New();
		reader->SetFileName("Test/polydata.Binary.vtp");
		reader->Update(); // force read

		// verify 
		CPPUNIT_ASSERT(reader->GetNumberOfPoints() == 792);
		CPPUNIT_ASSERT(reader->GetNumberOfCells()  == 1440);
		CPPUNIT_ASSERT(reader->GetNumberOfVerts()  == 0);
		CPPUNIT_ASSERT(reader->GetNumberOfLines()  == 0);
		CPPUNIT_ASSERT(reader->GetNumberOfStrips() == 0);
		CPPUNIT_ASSERT(reader->GetNumberOfPolys()  == 1440);

		vtkPolyData *output = reader->GetOutput();
		CPPUNIT_ASSERT(output);

		writer = vtkXMLPolyDataWriter::New();
		writer->SetInput(output);
		writer->SetDataModeToBinary();
		writer->SetFileName("Test/actual.Binary.vtp");
		CPPUNIT_ASSERT(writer->Write() == 1);

		// clean-up
		reader->Delete();
		reader = 0;

		writer->Delete();
		writer = 0;
	}
	catch (...)
	{
		if (reader) reader->Delete();
		if (writer) writer->Delete();
		throw;
	}

	std::ifstream expected("Test/polydata.Binary.vtp");
	CPPUNIT_ASSERT(expected.is_open());

	std::ifstream actual("Test/actual.Binary.vtp");
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
	CPPUNIT_ASSERT( ::remove("Test/actual.Binary.vtp") == 0 );
}

void TestHDFSerializePolyData::testHDF(void)
{
	vtkXMLPolyDataReader *reader = 0;
	vtkXMLPolyDataWriter *writer = 0;
	vtkPolyData *deserialized = 0;

	try
	{
		reader = vtkXMLPolyDataReader::New();
		reader->SetFileName("Test/polydata.Binary.vtp");
		reader->Update(); // force read

		// verify 
		CPPUNIT_ASSERT(reader->GetNumberOfPoints() == 792);
		CPPUNIT_ASSERT(reader->GetNumberOfCells()  == 1440);
		CPPUNIT_ASSERT(reader->GetNumberOfVerts()  == 0);
		CPPUNIT_ASSERT(reader->GetNumberOfLines()  == 0);
		CPPUNIT_ASSERT(reader->GetNumberOfStrips() == 0);
		CPPUNIT_ASSERT(reader->GetNumberOfPolys()  == 1440);

		vtkPolyData *output = reader->GetOutput();
		CPPUNIT_ASSERT(output);

		hid_t file_id = H5Fcreate("Test/polydata.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		CPPUNIT_ASSERT(file_id > 0);

		herr_t e = CGlobal::HDFSerializePolyData(true, file_id, "PolyData", output);
		CPPUNIT_ASSERT(e >= 0);

		herr_t s = H5Fclose(file_id);
		CPPUNIT_ASSERT(e >= 0);


		/* Create a new file using default properties. */
		file_id = H5Fopen("Test/polydata.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
		CPPUNIT_ASSERT(file_id > 0);

		e = CGlobal::HDFSerializePolyData(false, file_id, "PolyData", deserialized);
		CPPUNIT_ASSERT(e >= 0);

		CPPUNIT_ASSERT(deserialized);

		writer = vtkXMLPolyDataWriter::New();
		writer->SetInput(deserialized);
		writer->SetDataModeToBinary();
		writer->SetFileName("Test/deserialized.Binary.vtp");
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

	std::ifstream expected("Test/polydata.Binary.vtp");
	CPPUNIT_ASSERT(expected.is_open());

	std::ifstream actual("Test/deserialized.Binary.vtp");
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
	CPPUNIT_ASSERT( ::remove("Test/deserialized.Binary.vtp") == 0 );
}
