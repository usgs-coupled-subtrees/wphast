#include "StdAfx.h"
#include "TestPrism.h"

#include <sstream>
#include "WPhastDoc.h"
#include "Global.h"
#include "srcinput/NNInterpolator/NNInterpolator.h"
#include "srcinput/Filedata.h"
extern int input_error;

#include <vtkDebugLeaks.h>
#include <vtkAssemblyPaths.h>
#include <vtkAssemblyPath.h>
#include <vtkAssemblyNode.h>
#include <vtkCellTypes.h>
#include <vtkIntArray.h>
#include <vtkCellLinks.h>
#include <vtkMergePoints.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPropAssembly.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkAxes.h>
#include <vtkGeometryFilter.h>
#include <vtkFeatureEdges.h>
#include <vtkPlaneSource.h>
#include <vtkLODActor.h>
#include <vtkStructuredGrid.h>
#include <vtkVertex.h>
#include <vtkLine.h>
#include <vtkQuad.h>
#include <vtkHexahedron.h>
#include <vtkPlane.h>
#include <vtkImageData.h>
#include <vtkPixel.h>
#include <vtkVoxel.h>
#include <vtkOutlineFilter.h>
#include <vtkOutlineSource.h>
#include <vtkCutter.h>
#include <vtkContourValues.h>
#include <vtkConeSource.h>

#include "AxesActor.h"
#include "MediaZoneActor.h"
#include "srcWedgeSource.h"
#include "ICHeadZoneActor.h"
#include "ICChemZoneActor.h"
#include "BCZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"
#include "GridActor.h"
#include "GridLineWidget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestPrism::setUp(void)
{
// COMMENT: {12/6/2008 8:50:34 PM}	CGridLineWidget *glw = CGridLineWidget::New();
// COMMENT: {12/6/2008 8:50:34 PM}	glw->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkStructuredGrid *sg = vtkStructuredGrid::New();
// COMMENT: {12/6/2008 8:50:34 PM}	sg->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkHexahedron *h = vtkHexahedron::New();
// COMMENT: {12/6/2008 8:50:34 PM}	h->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkPlane *p = vtkPlane::New();
// COMMENT: {12/6/2008 8:50:34 PM}	p->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkImageData *id = vtkImageData::New();
// COMMENT: {12/6/2008 8:50:34 PM}	id->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkPixel *px = vtkPixel::New();
// COMMENT: {12/6/2008 8:50:34 PM}	px->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkVoxel *v = vtkVoxel::New();
// COMMENT: {12/6/2008 8:50:34 PM}	v->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkOutlineFilter *of = vtkOutlineFilter::New();
// COMMENT: {12/6/2008 8:50:34 PM}	of->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkOutlineSource *os = vtkOutlineSource::New();
// COMMENT: {12/6/2008 8:50:34 PM}	os->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkCutter *c = vtkCutter::New();
// COMMENT: {12/6/2008 8:50:34 PM}	c->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkContourValues *cv = vtkContourValues::New();
// COMMENT: {12/6/2008 8:50:34 PM}	cv->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkConeSource *cs = vtkConeSource::New();
// COMMENT: {12/6/2008 8:50:34 PM}	cs->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkLODActor *la = vtkLODActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	la->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkPlaneSource *ps = vtkPlaneSource::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ps->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkFeatureEdges *fe = vtkFeatureEdges::New();
// COMMENT: {12/6/2008 8:50:34 PM}	fe->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkGeometryFilter *gf = vtkGeometryFilter::New();
// COMMENT: {12/6/2008 8:50:34 PM}	gf->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CGridActor *ga = CGridActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ga->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkLookupTable *lt = vtkLookupTable::New();
// COMMENT: {12/6/2008 8:50:34 PM}	lt->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkPropAssembly *pa = vtkPropAssembly::New();
// COMMENT: {12/6/2008 8:50:34 PM}	pa->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkAxes *a = vtkAxes::New();
// COMMENT: {12/6/2008 8:50:34 PM}	a->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkAssemblyPaths *aps = vtkAssemblyPaths::New();
// COMMENT: {12/6/2008 8:50:34 PM}	aps->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkAssemblyPath *ap = vtkAssemblyPath::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ap->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkAssemblyNode *an = vtkAssemblyNode::New();
// COMMENT: {12/6/2008 8:50:34 PM}	an->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkCellTypes *ct = vtkCellTypes::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ct->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkIntArray *ia = vtkIntArray::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ia->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkCellLinks *cl = vtkCellLinks::New();
// COMMENT: {12/6/2008 8:50:34 PM}	cl->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkMergePoints *mp = vtkMergePoints::New();
// COMMENT: {12/6/2008 8:50:34 PM}	mp->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	vtkTransformPolyDataFilter *tpdf = vtkTransformPolyDataFilter::New();
// COMMENT: {12/6/2008 8:50:34 PM}	tpdf->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}	
// COMMENT: {12/6/2008 8:50:34 PM}	vtkTransform *t = vtkTransform::New();
// COMMENT: {12/6/2008 8:50:34 PM}	vtkMatrix4x4 *m = vtkMatrix4x4::New();
// COMMENT: {12/6/2008 8:50:34 PM}	t->Concatenate(m);
// COMMENT: {12/6/2008 8:50:34 PM}	m->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}	t->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CAxesActor *aa = CAxesActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	aa->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CMediaZoneActor *mza = CMediaZoneActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	mza->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	srcWedgeSource *ws = srcWedgeSource::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ws->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CICHeadZoneActor *ich = CICHeadZoneActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ich->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CICChemZoneActor *icc = CICChemZoneActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	icc->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CBCZoneActor *bc = CBCZoneActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	bc->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CWellActor *wa = CWellActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	wa->Delete();
// COMMENT: {12/6/2008 8:50:34 PM}
// COMMENT: {12/6/2008 8:50:34 PM}	CRiverActor *ra = CRiverActor::New();
// COMMENT: {12/6/2008 8:50:34 PM}	ra->Delete();
}


void TestPrism::testOStream(void)
{
	const char input[] = //"\t-prism"
		"\t\t-top       constant MAP 80\n"
		"\t\t-bottom    constant MAP -300\n"
		"\t\t-perimeter points   MAP\n"
		"\t\t\t278300 824600 80\n"
		"\t\t\t280600 824600 80\n"
		"\t\t\t279600 819600 80\n";

	::input_error = 0;
	std::istringstream iss(input);

	Prism aprism;
	while(aprism.Read(iss) && iss);

	std::ostringstream oss;
	oss << aprism;
	TRACE(oss.str().c_str());
	CPPUNIT_ASSERT(::input_error == 0);
}

void TestPrism::testHDFSerialize(void)
{
	const char input[] =
		"\t-perimeter SHAPE     map  ..\\setup\\phast\\examples\\capecod\\ArcData\\coast.shp\n"
		"\t-top       arcraster map  ..\\setup\\phast\\examples\\capecod\\ArcData\\elevation-90.txt\n"
		"\t-bottom    arcraster map  ..\\setup\\phast\\examples\\capecod\\ArcData\\bedrock-90.txt\n"
		"\t-active 1";

	::input_error = 0;
	std::istringstream iss(input);

	Prism prism;
	while(prism.Read(iss))
	{
		if (iss.rdstate() & std::ios::eofbit) break;
		iss.clear();
	}
	prism.Tidy();


	hid_t file_id = H5Fcreate("Test/prism.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	CPPUNIT_ASSERT(file_id > 0);

	herr_t e = CGlobal::HDFSerializePrism(true, file_id, prism);
	CPPUNIT_ASSERT(e >= 0);

	herr_t s = H5Fclose(file_id);
	CPPUNIT_ASSERT(e >= 0);


	/* Create a new file using default properties. */
	file_id = H5Fopen("Test/prism.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
	CPPUNIT_ASSERT(file_id > 0);

	Prism cp;
	e = CGlobal::HDFSerializePrism(false, file_id, cp);
	CPPUNIT_ASSERT(e >= 0);
	cp.Tidy();

	s = H5Fclose(file_id);
	CPPUNIT_ASSERT(e >= 0);

	CPPUNIT_ASSERT(prism == cp);

	Clear_NNInterpolatorList();
	Clear_file_data_map();
	Clear_KDtreeList();
}
