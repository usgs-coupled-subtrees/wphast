#pragma once
#include <vtkXMLImageDataWriter.h>

class srcXMLImageDataWriter : public vtkXMLImageDataWriter
{
public:
	static srcXMLImageDataWriter *New();
	vtkTypeRevisionMacro(srcXMLImageDataWriter, vtkXMLImageDataWriter);
	void SetOstream(ostream* os);

protected:
	srcXMLImageDataWriter(void);
	~srcXMLImageDataWriter(void);
};
