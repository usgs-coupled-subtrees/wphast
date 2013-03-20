#pragma once
#include <vtkXMLImageDataReader.h>

class srcXMLImageDataReader : public vtkXMLImageDataReader
{
public:
	static srcXMLImageDataReader *New();
	vtkTypeRevisionMacro(srcXMLImageDataReader, vtkXMLImageDataReader);
	void SetIstream(istream* os);

protected:
	srcXMLImageDataReader(void);
	~srcXMLImageDataReader(void);
};
