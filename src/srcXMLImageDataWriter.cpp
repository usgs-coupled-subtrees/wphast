#include "StdAfx.h"
#include "srcXMLImageDataWriter.h"
#include <vtkOutputStream.h>

vtkCxxRevisionMacro(srcXMLImageDataWriter, "$Revision$");
vtkStandardNewMacro(srcXMLImageDataWriter);

srcXMLImageDataWriter::srcXMLImageDataWriter(void)
{
}

srcXMLImageDataWriter::~srcXMLImageDataWriter(void)
{
}

void srcXMLImageDataWriter::SetOstream(ostream* os)
{
	this->Stream = os;
	this->Modified();
}

