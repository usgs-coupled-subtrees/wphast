#include "StdAfx.h"
#include "XMLSerializer.h"

#include <sstream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/DOMBuilderImpl.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include "xml/StrX.h"
#include "xml/XStr.h"

#include "WPhastDoc.h"
#include "NewModel.h"
#include "SiteMap.h" // no longer used - kept only for backward compatibility
#include "MapActor.h"

// Note: these could be replaced by either of the following if using VS2005
// const XMLCh gLS[] = XMLStrL("LS");
// const XMLCh gLS[] = L"LS";
//

const XMLCh gLS[]          = {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};

const XMLCh gWPhast[]      = {xercesc::chLatin_W, xercesc::chLatin_P, xercesc::chLatin_h,
                              xercesc::chLatin_a, xercesc::chLatin_s, xercesc::chLatin_t,
                              xercesc::chNull};

const XMLCh gSiteMap[]     = {xercesc::chLatin_S, xercesc::chLatin_i, xercesc::chLatin_t,
                              xercesc::chLatin_e, xercesc::chLatin_M, xercesc::chLatin_a,
                              xercesc::chLatin_p, xercesc::chNull};

const XMLCh gSrc[]         = {xercesc::chLatin_s, xercesc::chLatin_r, xercesc::chLatin_c,
                              xercesc::chNull};

const XMLCh gWorldfile[]   = {xercesc::chLatin_w, xercesc::chLatin_o, xercesc::chLatin_r,
                              xercesc::chLatin_l, xercesc::chLatin_d, xercesc::chLatin_f,
                              xercesc::chLatin_i, xercesc::chLatin_l, xercesc::chLatin_e,
                              xercesc::chNull};

const XMLCh gAngle[]       = {xercesc::chLatin_a, xercesc::chLatin_n, xercesc::chLatin_g,
                              xercesc::chLatin_l, xercesc::chLatin_e, xercesc::chNull};

const XMLCh gPlacement_x[] = {xercesc::chLatin_p, xercesc::chLatin_l, xercesc::chLatin_a,
                              xercesc::chLatin_c, xercesc::chLatin_e, xercesc::chLatin_m,
                              xercesc::chLatin_e, xercesc::chLatin_n, xercesc::chLatin_t,
                              xercesc::chUnderscore, xercesc::chLatin_x, xercesc::chNull};

const XMLCh gPlacement_y[] = {xercesc::chLatin_p, xercesc::chLatin_l, xercesc::chLatin_a,
                              xercesc::chLatin_c, xercesc::chLatin_e, xercesc::chLatin_m,
                              xercesc::chLatin_e, xercesc::chLatin_n, xercesc::chLatin_t,
                              xercesc::chUnderscore, xercesc::chLatin_y, xercesc::chNull};

const XMLCh gPlacement_z[] = {xercesc::chLatin_p, xercesc::chLatin_l, xercesc::chLatin_a,
                              xercesc::chLatin_c, xercesc::chLatin_e, xercesc::chLatin_m,
                              xercesc::chLatin_e, xercesc::chLatin_n, xercesc::chLatin_t,
                              xercesc::chUnderscore, xercesc::chLatin_z, xercesc::chNull};

CXMLSerializer::CXMLSerializer(void)
: bInitialized(true)
, formatTarget(0)
{
	// Initialize the XML4C2 system
	try
	{
		xercesc::XMLPlatformUtils::Initialize();
		formatTarget = new xercesc::MemBufFormatTarget();
	}
	catch(const xercesc::XMLException&)
	{
		bInitialized = false;
	}
}

CXMLSerializer::~CXMLSerializer(void)
{
	delete formatTarget;

	// cleanup the XML4C2 system
	xercesc::XMLPlatformUtils::Terminate();
}

const char* CXMLSerializer::writeDOM(CWPhastDoc* wphastDoc, const char* prefix, LPCTSTR lpszPathName)
{
	if (!bInitialized || formatTarget == 0)
	{
		return 0;
	}

	// get impl
	//
	if (xercesc::DOMImplementation *impl = xercesc::DOMImplementationRegistry::getDOMImplementation(gLS))
	{
		// Create the DOM document
		// ie
		// <WPhast>
		//     <image src="bitmap.bmp" worldfile="bitmap.bpw"/>
		// </WPhast>
		//
		xercesc::DOMDocument* doc = impl->createDocument(0, gWPhast, 0);

		// Add comment node
		//
		this->AddCommentNode(doc, wphastDoc, lpszPathName);

		// TODO check for writeability of current directory
		//

		// Add SiteMap node
		//
		this->AddSiteMapNode(doc, wphastDoc, prefix);

		// Create the DOM writer
		//
		xercesc::DOMWriter *writer = ((xercesc::DOMImplementationLS*)impl)->createDOMWriter();

		// hardcode the encoding to "UTF-8"
		//
		writer->setEncoding(X("UTF-8"));

		// set pretty output (add whitespace)
		//
		if (writer->canSetFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true))
		{
			writer->setFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
		}

		// write the doc
		//
		writer->writeNode(formatTarget, *doc);

		// cleanup
		//
		doc->release();
	}

	return (char*) formatTarget->getRawBuffer();
}

void CXMLSerializer::AddCommentNode(xercesc::DOMDocument* doc, CWPhastDoc* wphastDoc, LPCTSTR lpszPathName)
{
	// Create comment node
	//
	std::string comment(_T("Exported from "));
	std::string comment2(_T("Exported to   "));
	comment2 += lpszPathName;
	CString path = wphastDoc->GetPathName();
	if (path.IsEmpty())
	{
		comment += _T("an unsaved file");
	}
	else
	{
		comment += path;
	}
	xercesc::DOMElement* rootElem = doc->getDocumentElement();
	xercesc::DOMComment* com = doc->createComment(X(comment.c_str()));
	rootElem->appendChild(com);
	xercesc::DOMComment* com2 = doc->createComment(X(comment2.c_str()));
	rootElem->appendChild(com2);
}

void CXMLSerializer::AddSiteMapNode(xercesc::DOMDocument* doc, CWPhastDoc* wphastDoc, const char* prefix)
{
	CMapActor* mapActor = wphastDoc->m_pMapActor;
	if (!mapActor)
	{
		// nothing to do
		return;
	}

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	VERIFY(::_tsplitpath_s(mapActor->SiteMap2.FileName.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

	CString imageFileName(prefix);
	imageFileName += _T(".sitemap");
	imageFileName += szExt;
	if (!::CopyFile(mapActor->m_szTempFileName, imageFileName, FALSE))
	{
		::AfxMessageBox(_T("Unable to write site map."));
	}

	CString strWorldFileName(prefix);
	strWorldFileName += _T(".sitemap");
	CString strExt(szExt);
	if (strExt.CompareNoCase(_T(".bmp")) == 0)
	{
		strWorldFileName += _T(".bpw");
	}
	else if (strExt.CompareNoCase(_T(".jpg")) == 0 || strExt.CompareNoCase(_T(".jpeg")) == 0)
	{
		strWorldFileName += _T(".jgw");
	}
	else if (strExt.CompareNoCase(_T(".png")) == 0)
	{
		strWorldFileName += _T(".pgw");
	}
	else if (strExt.CompareNoCase(_T(".tif")) == 0 || strExt.CompareNoCase(_T(".tiff")) == 0)
	{
		strWorldFileName += _T(".tfw");
	}
	else
	{
		ASSERT(FALSE);
		strWorldFileName += _T(".wf");
	}

	try 
	{
		CGlobal::WriteWorldFile(strWorldFileName, mapActor->SiteMap2.GetWorldTransform());
	}
	catch(...)
	{
		::AfxMessageBox(_T("Unable to write worldfile."));
	}

	std::ostringstream oss;
	oss.precision(DBL_DIG);
	oss << mapActor->SiteMap2.Angle;

	// SiteMap
	//
	xercesc::DOMElement* element = doc->createElement(gSiteMap);

	// src
	//
	element->setAttribute(gSrc, X(imageFileName));

	// worldfile
	//
	element->setAttribute(gWorldfile, X(strWorldFileName));

	ASSERT(doc->getDocumentElement() != (xercesc::DOMElement*)doc);
	ASSERT(doc->getDocumentElement()->getOwnerDocument() == doc);

// COMMENT: {11/15/2010 4:24:30 PM}	// angle
// COMMENT: {11/15/2010 4:24:30 PM}	//
// COMMENT: {11/15/2010 4:24:30 PM}	{
// COMMENT: {11/15/2010 4:24:30 PM}		std::ostringstream oss;
// COMMENT: {11/15/2010 4:24:30 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {11/15/2010 4:24:30 PM}		oss << mapActor->SiteMap2.Angle;
// COMMENT: {11/15/2010 4:24:30 PM}		oss.precision(p);
// COMMENT: {11/15/2010 4:24:30 PM}		element->setAttribute(gAngle, X(oss.str().c_str()));
// COMMENT: {11/15/2010 4:24:30 PM}	}

// COMMENT: {11/15/2010 4:24:34 PM}	// placement_x
// COMMENT: {11/15/2010 4:24:34 PM}	//
// COMMENT: {11/15/2010 4:24:34 PM}	{
// COMMENT: {11/15/2010 4:24:34 PM}		std::ostringstream oss;
// COMMENT: {11/15/2010 4:24:34 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {11/15/2010 4:24:34 PM}		oss << mapActor->SiteMap2.Origin[0];
// COMMENT: {11/15/2010 4:24:34 PM}		oss.precision(p);
// COMMENT: {11/15/2010 4:24:34 PM}		element->setAttribute(gPlacement_x, X(oss.str().c_str()));
// COMMENT: {11/15/2010 4:24:34 PM}	}

// COMMENT: {11/15/2010 4:24:39 PM}	// placement_y
// COMMENT: {11/15/2010 4:24:39 PM}	//
// COMMENT: {11/15/2010 4:24:39 PM}	{
// COMMENT: {11/15/2010 4:24:39 PM}		std::ostringstream oss;
// COMMENT: {11/15/2010 4:24:39 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {11/15/2010 4:24:39 PM}		oss << mapActor->SiteMap2.Origin[1];
// COMMENT: {11/15/2010 4:24:39 PM}		oss.precision(p);
// COMMENT: {11/15/2010 4:24:39 PM}		element->setAttribute(gPlacement_y, X(oss.str().c_str()));
// COMMENT: {11/15/2010 4:24:39 PM}	}

// COMMENT: {11/15/2010 4:24:44 PM}	// placement_z
// COMMENT: {11/15/2010 4:24:44 PM}	//
// COMMENT: {11/15/2010 4:24:44 PM}	{
// COMMENT: {11/15/2010 4:24:44 PM}		std::ostringstream oss;
// COMMENT: {11/15/2010 4:24:44 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {11/15/2010 4:24:44 PM}		oss << mapActor->SiteMap2.Origin[2];
// COMMENT: {11/15/2010 4:24:44 PM}		oss.precision(p);
// COMMENT: {11/15/2010 4:24:44 PM}		element->setAttribute(gPlacement_z, X(oss.str().c_str()));
// COMMENT: {11/15/2010 4:24:44 PM}	}

	xercesc::DOMElement* rootElem = doc->getDocumentElement();
	rootElem->appendChild(element);
}

int CXMLSerializer::AddSiteMapNode(CMapActor* mapActor, const char* prefix, xercesc::DOMElement* element)
{
// COMMENT: {8/10/2009 8:23:28 PM}	if (!mapActor) return 0;
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	TCHAR szDrive[_MAX_DRIVE];
// COMMENT: {8/10/2009 8:23:28 PM}	TCHAR szDir[_MAX_DIR];
// COMMENT: {8/10/2009 8:23:28 PM}	TCHAR szFName[_MAX_FNAME];
// COMMENT: {8/10/2009 8:23:28 PM}	TCHAR szExt[_MAX_EXT];
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	::_tsplitpath(mapActor->m_siteMap.m_fileName.c_str(), szDrive, szDir, szFName, szExt);
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	CString imageFileName(prefix);
// COMMENT: {8/10/2009 8:23:28 PM}	imageFileName += ".sitemap";
// COMMENT: {8/10/2009 8:23:28 PM}	imageFileName += szExt;
// COMMENT: {8/10/2009 8:23:28 PM}	if (::CopyFile(mapActor->m_szTempFileName, imageFileName, FALSE) == 0)
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		::AfxMessageBox("Unable to write sitemap image file.");
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	CString strWorldFileName(prefix);
// COMMENT: {8/10/2009 8:23:28 PM}	strWorldFileName += ".sitemap";
// COMMENT: {8/10/2009 8:23:28 PM}	CString strExt(szExt);
// COMMENT: {8/10/2009 8:23:28 PM}	if (strExt.CompareNoCase(_T(".bmp")) == 0)
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		strWorldFileName += _T(".bpw");
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}	else if (strExt.CompareNoCase(_T(".jpg")) == 0 || strExt.CompareNoCase(_T(".jpeg")) == 0)
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		strWorldFileName += _T(".jgw");
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}	else if (strExt.CompareNoCase(_T(".png")) == 0)
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		strWorldFileName += _T(".pgw");
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}	else if (strExt.CompareNoCase(_T(".tif")) == 0 || strExt.CompareNoCase(_T(".tiff")) == 0)
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		strWorldFileName += _T(".tfw");
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}	else
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		ASSERT(FALSE);
// COMMENT: {8/10/2009 8:23:28 PM}		strWorldFileName += _T(".wf");
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	try 
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		CGlobal::WriteWorldFile(strWorldFileName, mapActor->m_siteMap.GetWorldTransform());
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}	catch(...)
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		::AfxMessageBox("Unable to write worldfile.");
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	element->setAttribute(gSrc, X(imageFileName));
// COMMENT: {8/10/2009 8:23:28 PM}	element->setAttribute(gWorldfile, X(strWorldFileName));
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	// angle
// COMMENT: {8/10/2009 8:23:28 PM}	//
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		std::ostringstream oss;
// COMMENT: {8/10/2009 8:23:28 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {8/10/2009 8:23:28 PM}		oss << mapActor->m_siteMap.m_angle;
// COMMENT: {8/10/2009 8:23:28 PM}		oss.precision(p);
// COMMENT: {8/10/2009 8:23:28 PM}		element->setAttribute(gAngle, X(oss.str().c_str()));
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	// placement_x
// COMMENT: {8/10/2009 8:23:28 PM}	//
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		std::ostringstream oss;
// COMMENT: {8/10/2009 8:23:28 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {8/10/2009 8:23:28 PM}		oss << mapActor->m_siteMap.m_placement[0];
// COMMENT: {8/10/2009 8:23:28 PM}		oss.precision(p);
// COMMENT: {8/10/2009 8:23:28 PM}		element->setAttribute(gPlacement_x, X(oss.str().c_str()));
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	// placement_y
// COMMENT: {8/10/2009 8:23:28 PM}	//
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		std::ostringstream oss;
// COMMENT: {8/10/2009 8:23:28 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {8/10/2009 8:23:28 PM}		oss << mapActor->m_siteMap.m_placement[1];
// COMMENT: {8/10/2009 8:23:28 PM}		oss.precision(p);
// COMMENT: {8/10/2009 8:23:28 PM}		element->setAttribute(gPlacement_y, X(oss.str().c_str()));
// COMMENT: {8/10/2009 8:23:28 PM}	}
// COMMENT: {8/10/2009 8:23:28 PM}
// COMMENT: {8/10/2009 8:23:28 PM}	// placement_z
// COMMENT: {8/10/2009 8:23:28 PM}	//
// COMMENT: {8/10/2009 8:23:28 PM}	{
// COMMENT: {8/10/2009 8:23:28 PM}		std::ostringstream oss;
// COMMENT: {8/10/2009 8:23:28 PM}		std::streamsize p(oss.precision(DBL_DIG));
// COMMENT: {8/10/2009 8:23:28 PM}		oss << mapActor->m_siteMap.m_placement[2];
// COMMENT: {8/10/2009 8:23:28 PM}		oss.precision(p);
// COMMENT: {8/10/2009 8:23:28 PM}		element->setAttribute(gPlacement_z, X(oss.str().c_str()));
// COMMENT: {8/10/2009 8:23:28 PM}	}


	return 0;
}

/**
  Prereqs: valid model.m_units and model.m_gridKeyword.m_grid[2]
*/
int CXMLSerializer::load(std::istream& is, CNewModel& model)
{
	// Instantiate the DOM parser.
	//
	const XMLCh gLS[] = {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};

	// Get an implementation of the Load-Store (LS) interface.
	//
	xercesc::DOMImplementationLS* impl (
		static_cast<xercesc::DOMImplementationLS*>(
		xercesc::DOMImplementationRegistry::getDOMImplementation(gLS)));

	// Create a DOMBuilder.
	//
	xercesc::DOMBuilder* parser = impl->createDOMBuilder(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);

	// Create InputSource
	//
	std_input_source isrc(is);
	xercesc::Wrapper4InputSource wrap(&isrc, false);
	xercesc::DOMDocument* doc = parser->parse(wrap);

	// get minimum z value for sitemap
	//
	double pt[3] = {0.0, 0.0, 0.0};
	CGrid z(model.m_gridKeyword.m_grid[2]);
	z.Setup();
	pt[2] = z.coord[0] - .01 *(z.coord[z.count_coord - 1] - z.coord[0]);

	// Load SiteMap
	//
	CSiteMap siteMap;
	if (this->LoadSiteMap(doc, siteMap) == 0)
	{
		CSiteMap2 siteMap2;
		siteMap2.Angle    = model.m_gridKeyword.m_grid_angle;
		siteMap2.FileName = siteMap.m_fileName;
		for (int i = 0; i < 3; ++i)
		{
			siteMap2.Origin[i] = model.m_gridKeyword.m_grid_origin[i];
		}

		vtkTransform *trans = vtkTransform::New();
		const CUnits& units = model.m_units;
		trans->Scale(
			units.horizontal.input_to_si,
			units.horizontal.input_to_si,
			units.vertical.input_to_si);
		trans->TransformPoint(pt, pt);
		siteMap2.Origin[2] = pt[2];
		trans->Delete();

		siteMap2.Origin[2] = pt[2];
		siteMap2.SetWorldTransform(siteMap.GetWorldTransform());
		model.SetSiteMap2(siteMap2);
	}

	return 0;
}

int CXMLSerializer::LoadSiteMap(xercesc::DOMDocument* doc, CSiteMap& siteMap)
{
	xercesc::DOMElement* e = doc->getDocumentElement();
	if (!e) return 1;

	if (xercesc::XMLString::compareString(e->getTagName(), gWPhast) != 0) return 2; 

	xercesc::DOMNodeList* list = e->getElementsByTagName(gSiteMap);
	if (list->getLength() == 0) return 3;

	xercesc::DOMNode* node = list->item(0);
	if (node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE)
	{
		xercesc::DOMElement* siteElement = static_cast<xercesc::DOMElement*>(node);

		TCHAR szFullPathName[MAX_PATH];
		LPTSTR lpFilePart;

		// src
		//
		StrX src(siteElement->getAttribute(gSrc));
		if (::strlen(src.localForm()) == 0) return 4;
		::GetFullPathName(src.localForm(), MAX_PATH, szFullPathName, &lpFilePart);
		siteMap.m_fileName = szFullPathName;

		// worldfile
		//
		StrX worldfile(siteElement->getAttribute(gWorldfile));
		if (::strlen(worldfile.localForm()) == 0) return 5;
		::GetFullPathName(worldfile.localForm(), MAX_PATH, szFullPathName, &lpFilePart);
		CWorldTransform wtrans;
		wtrans.LoadWorldFile(szFullPathName);
		siteMap.SetWorldTransform(wtrans);

		// angle
		//
		{
			StrX angle(siteElement->getAttribute(gAngle));
			if (::strlen(angle.localForm()) != 0)
			{
				std::istringstream iss(angle.localForm());
				iss >> siteMap.m_angle;
			}
		}

		// placement_x
		//
		{
			StrX placement_x(siteElement->getAttribute(gPlacement_x));
			if (::strlen(placement_x.localForm()) != 0)
			{
				std::istringstream iss(placement_x.localForm());
				iss >> siteMap.m_placement[0];
			}
		}

		// placement_y
		//
		{
			StrX placement_y(siteElement->getAttribute(gPlacement_y));
			if (::strlen(placement_y.localForm()) != 0)
			{
				std::istringstream iss(placement_y.localForm());
				iss >> siteMap.m_placement[1];
			}
		}

		// placement_z
		//
		{
			StrX placement_z(siteElement->getAttribute(gPlacement_z));
			if (::strlen(placement_z.localForm()) != 0)
			{
				std::istringstream iss(placement_z.localForm());
				iss >> siteMap.m_placement[2];
			}
		}
	}

	return 0;
}
