//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueObjectClass
//* .File Name       : $Workfile:   SVStringValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:15:24  $
//******************************************************************************

#ifndef SVSTRINGVALUEOBJECTCLASS_H
#define SVSTRINGVALUEOBJECTCLASS_H

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"

class SVStringValueObjectClass : public SVValueObjectClassImpl < CString >
{
	SV_DECLARE_CLASS( SVStringValueObjectClass );

public:
	SVStringValueObjectClass( LPCTSTR ObjectName );
	SVStringValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVSTRINGVALUEOBJECT );
	SVStringValueObjectClass( const SVStringValueObjectClass& rhs);
	const SVStringValueObjectClass& operator = (const SVStringValueObjectClass& rhs);

	virtual ~SVStringValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

//	IMPLEMENT_VALUE_OBJECT_GET_SET()
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, FROMTYPE value) { return SetValueAt(iBucket, 0, value); }
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, int iIndex, FROMTYPE value) { return SetValueAt(iBucket, iIndex, value); }
	
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(TOTYPE& rValue) const { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, TOTYPE& rValue) const { return GetValueAt(iBucket, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, int iIndex, TOTYPE& rValue) const { return GetValueAt(iBucket, iIndex, rValue); }

	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, CString value ) { return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) { return S_FALSE; } //SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) { return S_FALSE; } //return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) { return S_FALSE; } //SetValueAt(iBucket, iIndex, value); }

	__forceinline  HRESULT SetValueAt( int iBucket, int iIndex, value_type value ) {return base::SetValueAt(iBucket, iIndex, value); }
	__forceinline  HRESULT GetValueAt( int iBucket, int iIndex, value_type& rValue ) const {return base::GetValueAt(iBucket, iIndex, rValue); }


protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStringValueObjectClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:15:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Sep 2012 15:20:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 14:11:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:46:02   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
