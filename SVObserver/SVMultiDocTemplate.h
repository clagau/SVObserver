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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMultiDocTemplate.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:37:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 11:24:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/