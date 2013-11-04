//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataObjectClass
//* .File Name       : $Workfile:   SVDataObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:07:50  $
//******************************************************************************

#ifndef AFX_SVDATAOBJECT_H__BBF5FB61_0041_11D2_AEC4_444553540000__INCLUDED_
#define AFX_SVDATAOBJECT_H__BBF5FB61_0041_11D2_AEC4_444553540000__INCLUDED_

union SVDataUnion
{
	double	DValue;
	DWORD	DWValue;
};

class SVDataListClass : public CArray< SVDataUnion, SVDataUnion& >
{
	public:
		SVDataListClass() {};
		virtual ~SVDataListClass() {};
};


class SVDataObjectClass : public CObject
{
//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************
public:
	void Serialize( CArchive& archive );

//******************************************************************************
// MFC Declarations:
//******************************************************************************
protected:
	DECLARE_SERIAL( SVDataObjectClass )

//******************************************************************************
// Manual Generated Message Map Entries
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Message Map Entries
//******************************************************************************

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVDataObjectClass( int Type = SVDataObjectClass::DWordType );
protected:

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVDataObjectClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
	BOOL IsDWordData();
	BOOL IsDoubleData();

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

	int Add( double DVal );
	int SetDataType( int Type );
	void SetAt( int Index, double DVal );
	void SetDWordAt( int Index, DWORD DWVal );
	void SetDoubleAt( int Index, double DVal );
	void ResetAll( double DVal );
	void ResetDWordAll( DWORD DWVal );
	void ResetDoubleAll( double DVal );
	void RemoveAll();
	void SetSize( int NewSize, int GrowBy = -1 );

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

	SVDataUnion&	 GetAt( int Index );
	DWORD&			 GetDWordAt( int Index );
	double&			 GetDoubleAt( int Index );
	SVDataListClass& GetList();
	int				 GetSize();

//******************************************************************************
// Operation(s) Of Process;
//******************************************************************************

//******************************************************************************
// Operation(s) Of Visual Representation:
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************

//******************************************************************************
// Debug Operation(s):
//******************************************************************************

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	// Predefined object types
	enum { DWordType = 1, DoubleType };

protected:
	int				objectType;
	SVDataListClass dataList;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_SVDATAOBJECT_H__BBF5FB61_0041_11D2_AEC4_444553540000__INCLUDED_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDataObject.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:07:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   17 Apr 2003 17:30:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/