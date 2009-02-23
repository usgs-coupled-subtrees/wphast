#include "StdAfx.h"
#include "TestCWPhastDoc.h"

#include "WPhastDoc.h"
#include "Global.h"

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


void TestCWPhastDoc::setUp(void)
{
	CGridLineWidget *glw = CGridLineWidget::New();
	glw->Delete();

	vtkStructuredGrid *sg = vtkStructuredGrid::New();
	sg->Delete();

	vtkHexahedron *h = vtkHexahedron::New();
	h->Delete();

	vtkPlane *p = vtkPlane::New();
	p->Delete();

	vtkImageData *id = vtkImageData::New();
	id->Delete();

	vtkPixel *px = vtkPixel::New();
	px->Delete();

	vtkVoxel *v = vtkVoxel::New();
	v->Delete();

	vtkOutlineFilter *of = vtkOutlineFilter::New();
	of->Delete();

	vtkOutlineSource *os = vtkOutlineSource::New();
	os->Delete();

	vtkCutter *c = vtkCutter::New();
	c->Delete();

	vtkContourValues *cv = vtkContourValues::New();
	cv->Delete();

	vtkConeSource *cs = vtkConeSource::New();
	cs->Delete();

	vtkLODActor *la = vtkLODActor::New();
	la->Delete();

	vtkPlaneSource *ps = vtkPlaneSource::New();
	ps->Delete();

	vtkFeatureEdges *fe = vtkFeatureEdges::New();
	fe->Delete();

	vtkGeometryFilter *gf = vtkGeometryFilter::New();
	gf->Delete();

	CGridActor *ga = CGridActor::New();
	ga->Delete();

	vtkLookupTable *lt = vtkLookupTable::New();
	lt->Delete();

	vtkPropAssembly *pa = vtkPropAssembly::New();
	pa->Delete();

	vtkAxes *a = vtkAxes::New();
	a->Delete();

	vtkAssemblyPaths *aps = vtkAssemblyPaths::New();
	aps->Delete();

	vtkAssemblyPath *ap = vtkAssemblyPath::New();
	ap->Delete();

	vtkAssemblyNode *an = vtkAssemblyNode::New();
	an->Delete();

	vtkCellTypes *ct = vtkCellTypes::New();
	ct->Delete();

	vtkIntArray *ia = vtkIntArray::New();
	ia->Delete();

	vtkCellLinks *cl = vtkCellLinks::New();
	cl->Delete();

	vtkMergePoints *mp = vtkMergePoints::New();
	mp->Delete();

	vtkTransformPolyDataFilter *tpdf = vtkTransformPolyDataFilter::New();
	tpdf->Delete();
	
	vtkTransform *t = vtkTransform::New();
	vtkMatrix4x4 *m = vtkMatrix4x4::New();
	t->Concatenate(m);
	m->Delete();
	t->Delete();

	CAxesActor *aa = CAxesActor::New();
	aa->Delete();

	CMediaZoneActor *mza = CMediaZoneActor::New();
	mza->Delete();

	srcWedgeSource *ws = srcWedgeSource::New();
	ws->Delete();

	CICHeadZoneActor *ich = CICHeadZoneActor::New();
	ich->Delete();

	CICChemZoneActor *icc = CICChemZoneActor::New();
	icc->Delete();

	CBCZoneActor *bc = CBCZoneActor::New();
	bc->Delete();

	CWellActor *wa = CWellActor::New();
	wa->Delete();

	CRiverActor *ra = CRiverActor::New();
	ra->Delete();
}

void TestCWPhastDoc::testCreateObject(void)
{
	CMemoryState oldMemState, newMemState, diffMemState;

	/// _CrtSetBreakAlloc(102602);
	oldMemState.Checkpoint();
	{
		for (int i = 0; i < 10/*0*/; ++i)
		{
			oldMemState.Checkpoint();

			CRuntimeClass* pDocClass = RUNTIME_CLASS(CWPhastDoc);
			CWPhastDoc* pDocument = (CWPhastDoc*)pDocClass->CreateObject();
			CPPUNIT_ASSERT(pDocument != 0);
			switch (i%4)
			{
			case 0:
				pDocument->DoImport("..\\setup\\phast\\examples\\ex1\\ex1.trans.dat");
				break;
			case 1:
				pDocument->DoImport("..\\setup\\phast\\examples\\ex2\\ex2.trans.dat");
				break;
			case 2:
				pDocument->DoImport("..\\setup\\phast\\examples\\ex3\\ex3.trans.dat");
				break;
			case 3:
				pDocument->DoImport("..\\setup\\phast\\examples\\ex4\\ex4.trans.dat");
				break;
			default:
				break;
			}
			pDocument->OnCloseDocument();
			newMemState.Checkpoint();
			if( diffMemState.Difference( oldMemState, newMemState ) != 0)
			{
				diffMemState.DumpStatistics();
				oldMemState.DumpAllObjectsSince();
			}
			CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
		}
	}
	newMemState.Checkpoint();
	if( diffMemState.Difference( oldMemState, newMemState ) != 0)
	{
		diffMemState.DumpStatistics();
		/**
		0 bytes in 0 Free Blocks.
		734 bytes in 51 Normal Blocks.
		0 bytes in 0 CRT Blocks.
		0 bytes in 0 Ignore Blocks.
		0 bytes in 0 Client Blocks.
		Largest number used: 4610 bytes.
		Total allocations: 10991 bytes.
		**/

		// Free Blocks
		afxDump << "diffMemState.m_memState.lSizes[0] = " << diffMemState.m_memState.lSizes[0] << "\n";
		afxDump << "diffMemState.m_memState.lCounts[0] = " << diffMemState.m_memState.lCounts[0] << "\n";

		// Normal Blocks
		afxDump << "diffMemState.m_memState.lSizes[1] = " << diffMemState.m_memState.lSizes[1] << "\n";
		afxDump << "diffMemState.m_memState.lCounts[1] = " << diffMemState.m_memState.lCounts[1] << "\n";

		// CRT Blocks
		afxDump << "diffMemState.m_memState.lSizes[2] = " << diffMemState.m_memState.lSizes[2] << "\n";
		afxDump << "diffMemState.m_memState.lCounts[2] = " << diffMemState.m_memState.lCounts[2] << "\n";

		// Ignore Blocks
		afxDump << "diffMemState.m_memState.lSizes[3] = " << diffMemState.m_memState.lSizes[3] << "\n";
		afxDump << "diffMemState.m_memState.lCounts[3] = " << diffMemState.m_memState.lCounts[3] << "\n";

		// Client Blocks
		afxDump << "diffMemState.m_memState.lSizes[4] = " << diffMemState.m_memState.lSizes[4] << "\n";
		afxDump << "diffMemState.m_memState.lCounts[4] = " << diffMemState.m_memState.lCounts[4] << "\n";

		// Free Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[0] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[0] == 0);

		// Normal Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[1] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[1] == 0);

		// CRT Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[2] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[2] == 0);

		// Ignore Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[3] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[3] == 0);

		// Client Blocks
		CPPUNIT_ASSERT(diffMemState.m_memState.lSizes[4] == 0);
		CPPUNIT_ASSERT(diffMemState.m_memState.lCounts[4] == 0);

		/// _CrtDumpMemoryLeaks(); // this will print the allocations/locations of unfreed memory
	}
	CPPUNIT_ASSERT(diffMemState.Difference( oldMemState, newMemState ) == 0);
}