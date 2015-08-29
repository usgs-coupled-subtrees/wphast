#pragma once

#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/dom/DOM.hpp>

class CWPhastDoc;
class CMapActor;
class CNewModel;
class CSiteMap;

#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/sax/InputSource.hpp>

class std_input_stream : public xercesc::BinInputStream
{
public :
	std_input_stream (std::istream& is)
		: m_is(is)
	{
	}

	virtual unsigned int curPos(void) const
	{
		return static_cast<unsigned int> (m_is.tellg());
	}

	virtual unsigned int readBytes(XMLByte* const buf, const unsigned int size)
	{
		m_is.read (reinterpret_cast<char*>(buf), size);
		return static_cast<unsigned int>(m_is.gcount());
	}

private :
	std::istream& m_is;
};


class std_input_source : public xercesc::InputSource
{
public :
	std_input_source (std::istream& is)
		: pis(&is)
	{
	}

	struct copy {};

	virtual xercesc::BinInputStream* makeStream(void) const
	{
		if (this->pis == 0)
		{
			throw copy();
		}

		std::istream& is(*this->pis);

		this->pis = 0;

		return new std_input_stream(is);
	}

private :
	mutable std::istream* pis;
};

class CXMLSerializer
{
public:
	CXMLSerializer(void);
	~CXMLSerializer(void);
	
	const char* writeDOM(CWPhastDoc* doc, const char* prefix, LPCTSTR lpszPathName);
	
	int load(std::istream& is, CNewModel& model);

protected:
	void AddCommentNode(xercesc::DOMDocument* doc, CWPhastDoc* wphastDoc, LPCTSTR lpszPathName);
	int AddSiteMapNode(CMapActor* actor, const char* prefix, xercesc::DOMElement* element);
	void AddSiteMapNode(xercesc::DOMDocument* doc, CWPhastDoc* wphastDoc, const char* prefix);

	int LoadSiteMap(xercesc::DOMDocument* doc, CSiteMap& siteMap);

protected:
	bool bInitialized;
	xercesc::MemBufFormatTarget* formatTarget;
};
