#include "StdAfx.h"
#include "srcXMLImageDataReader.h"

vtkCxxRevisionMacro(srcXMLImageDataReader, "$Revision$");
vtkStandardNewMacro(srcXMLImageDataReader);


srcXMLImageDataReader::srcXMLImageDataReader(void)
{
}

srcXMLImageDataReader::~srcXMLImageDataReader(void)
{
}

void srcXMLImageDataReader::SetIstream(istream* is)
{
	this->Stream = is;
	this->Modified();
}

