//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantValueObjectClass
//* .File Name       : $Workfile:   SVVariantValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:20:02  $
//******************************************************************************

#ifndef SVVARIANTVALUEOBJECTCLASS_H
#define SVVARIANTVALUEOBJECTCLASS_H

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"

class SVVariantValueObjectClass : public SVValueObjectClassImpl< _variant_t >
{
	SV_DECLARE_CLASS( SVVariantValueObjectClass );

public:
	SVVariantValueObjectClass( LPCTSTR ObjectName );
	SVVariantValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVVARIANTVALUEOBJECT );
	SVVariantValueObjectClass( const SVVariantValueObjectClass& rhs );
	const SVVariantValueObjectClass& operator = ( const SVVariantValueObjectClass& rhs );

	virtual ~SVVariantValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	BOOL SetType( int vt );
	HRESULT SetValueKeepType( int iBucket, LPCTSTR value ) {return SetValueKeepType(iBucket, 0, value);}
	HRESULT SetValueKeepType( int iBucket, int iIndex, LPCTSTR value );
	HRESULT SetDefaultValue( const VARIANT& rValue, bool bResetAll );
	HRESULT GetDefaultValue( _variant_t& rValue );
	long GetDefaultType();

	IMPLEMENT_VALUE_OBJECT_GET_SET_NO_T()

protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, BOOL value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, BYTE value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, DWORD value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, long value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, BYTE& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, BOOL& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;

	static CString ToString(const VARIANT& rvt, bool bScript = false );

private:
	void LocalInitialize();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVVariantValueObjectClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:20:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Sep 2012 15:20:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Nov 2010 11:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 14:27:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:59:08   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
