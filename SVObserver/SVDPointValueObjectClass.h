//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointValueObjectClass
//* .File Name       : $Workfile:   SVDPointValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:20:56  $
//******************************************************************************

#ifndef SVDPOINTVALUEOBJECTCLASS_H
#define SVDPOINTVALUEOBJECTCLASS_H

#include <assert.h>
#include <atltypes.h>
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"

class SVDPointValueObjectClass : public SVValueObjectClassImpl <SVDPointClass>
{
	SV_DECLARE_CLASS( SVDPointValueObjectClass );

public:
	SVDPointValueObjectClass( LPCTSTR ObjectName );
	SVDPointValueObjectClass( SVObjectClass* POwner = NULL , 
	                          int StringResourceID = IDS_CLASSNAME_SVDPOINTVALUEOBJECT );
	const SVDPointValueObjectClass& operator = (const SVDPointValueObjectClass& rhs);

	virtual ~SVDPointValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	HRESULT GetDefaultValue( POINT& rPoint );
	HRESULT SetDefaultValue( POINT Point, bool bResetAll );
	HRESULT GetDefaultValue( CPoint& rPoint );
	HRESULT SetDefaultValue( CPoint Point, bool bResetAll );

	//using base::GetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT GetDefaultValue( SVDPointClass& rPoint ) {return base::GetDefaultValue(rPoint);}
	//using base::SetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT SetDefaultValue( SVDPointClass& Point, bool bResetAll ) {return base::SetDefaultValue(Point, bResetAll);}

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:

	virtual HRESULT SetValueAt( int nBucket, int iIndex, const POINT Point );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, const CPoint Point );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );

	virtual HRESULT GetValueAt( int nBucket, int iIndex, CPoint& rPoint ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, POINT& rPoint ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, double& rstrValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, CString& rstrValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDPointValueObjectClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:20:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   04 Sep 2012 15:00:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Mar 2011 07:18:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   28 Oct 2010 13:44:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Oct 2009 11:18:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed Path for SVDPointClass to SVUtilityLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Apr 2008 14:45:30   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
