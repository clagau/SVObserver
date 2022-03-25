//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMultiDocTemplate
//* .File Name       : $Workfile:   SVMultiDocTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:37:18  $
//******************************************************************************

#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVMultiDocTemplate 

// MDI support (zero or more documents)
class SVMultiDocTemplate : public CMultiDocTemplate
{
public:
	SVMultiDocTemplate( UINT NIDResource, CRuntimeClass* PDocClass, CRuntimeClass* PFrameClass, CRuntimeClass* PViewClass );
	virtual ~SVMultiDocTemplate();

	virtual void SetDefaultTitle( CDocument* PDocument ) override;

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const override;
	virtual void AssertValid() const override;
#endif //_DEBUG
};
