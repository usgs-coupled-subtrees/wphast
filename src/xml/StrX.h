#ifndef STRX_INCLUDED
#define STRX_INCLUDED


// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
	// -----------------------------------------------------------------------
	//  Constructors and Destructor
	// -----------------------------------------------------------------------
	StrX(const XMLCh* const toTranscode)
	{
		// Call the private transcoding method
		fLocalForm = xercesc_2_7::XMLString::transcode(toTranscode);
	}

	~StrX()
	{
		xercesc_2_7::XMLString::release(&fLocalForm);
	}


	// -----------------------------------------------------------------------
	//  Getter methods
	// -----------------------------------------------------------------------
	const char* localForm() const
	{
		return fLocalForm;
	}

private :
	// -----------------------------------------------------------------------
	//  Private data members
	//
	//  fLocalForm
	//      This is the local code page form of the string.
	// -----------------------------------------------------------------------
	char*   fLocalForm;
};

inline std::ostream& operator<<(std::ostream& target, const StrX& toDump)
{
	target << toDump.localForm();
	return target;
}

#endif // STRX_INCLUDED