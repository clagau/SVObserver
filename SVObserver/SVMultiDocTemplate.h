//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMultiDocTemplateClass
//* .File Name       : $Workfile:   SVMultiDocTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:37:18  $
//******************************************************************************

#ifndef AFX_SVMULTIDOCTEMPLATE_H__E9410A61_1E4B_11D2_AEC4_444553540000__INCLUDED_
#define AFX_SVMULTIDOCTEMPLATE_H__E9410A61_1E4B_11D2_AEC4_444553540000__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// SVMultiDocTemplateClass 

// MDI support (zero or more documents)
class SVMultiDocTemplateClass : public CMultiDocTemplate
{
	DECLARE_DYNAMIC( SVMultiDocTemplateClass )

// Constructors
public:
	SVMultiDocTemplateClass( UINT NIDResource, CRuntimeClass* PDocClass, CRuntimeClass* PFrameClass, CRuntimeClass* PViewClass );
	virtual ~SVMultiDocTemplateClass();

// Implementation
public:

	virtual void SetDefaultTitle( CDocument* PDocument );

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_SVMULTIDOCTEMPLATE_H__E9410A61_1E4B_11D2_AEC4_444553540000__INCLUDED_

