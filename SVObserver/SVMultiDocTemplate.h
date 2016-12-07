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

#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVMultiDocTemplateClass 

// MDI support (zero or more documents)
class SVMultiDocTemplateClass : public CMultiDocTemplate
{
	DECLARE_DYNAMIC( SVMultiDocTemplateClass )

public:
	SVMultiDocTemplateClass( UINT NIDResource, CRuntimeClass* PDocClass, CRuntimeClass* PFrameClass, CRuntimeClass* PViewClass );
	virtual ~SVMultiDocTemplateClass();

	virtual void SetDefaultTitle( CDocument* PDocument ) override;

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const override;
	virtual void AssertValid() const override;
#endif //_DEBUG
};


