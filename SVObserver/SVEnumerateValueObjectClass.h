//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnumerateValueObjectClass
//* .File Name       : $Workfile:   SVEnumerateValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:50  $
//******************************************************************************

#ifndef SVENUMERATEVALUEOBJECTCLASS_H
#define SVENUMERATEVALUEOBJECTCLASS_H

#include <vector>
#include <utility>
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#include "ObjectInterfaces\IEnumerateValueObject.h"

typedef std::pair<CString, long> SVEnumeratePair;
typedef std::vector<SVEnumeratePair> SVEnumerateVector;

class SVEnumerateValueObjectClass : public SVValueObjectClassImpl <long>, public SvOi::IEnumerateValueObject
{
	SV_DECLARE_CLASS( SVEnumerateValueObjectClass );
public:
	SVEnumerateValueObjectClass( LPCTSTR ObjectName );
	SVEnumerateValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVENUMERATEVALUEOBJECT );
	const SVEnumerateValueObjectClass& operator = (const SVEnumerateValueObjectClass& rhs);

	virtual ~SVEnumerateValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rStrScript, CString& rStrAliasTable, int Indent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	BOOL GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const;
	BOOL GetEnumeratorName( long lValue, CString& rEnumerator ) const;
	BOOL GetEnumTypes( SVEnumerateVector& rVec ) const;
	BOOL GetEnumTypes( CString& rStrEnumList ) const;
	int  GetFirstEnumTypePos() const;
	BOOL GetNextEnumType( int& rIterator, CString& rStrEnum, long& rEnumValue ) const;

	BOOL SetEnumTypes( const SVEnumerateVector& rVec );
	BOOL SetEnumTypes( LPCTSTR szEnumList );
	BOOL SetEnumTypes( int StringResourceID );

	HRESULT SetDefaultValue( CString strValue, bool bResetAll );

	//using base::SetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT SetDefaultValue( long lValue, bool bResetAll ) {return base::SetDefaultValue(lValue, bResetAll);}

#pragma region IEnumerateValueObject
	virtual SvOi::NameValueList GetEnumList() const override;
#pragma endregion IEnumerateValueObject

	IMPLEMENT_VALUE_OBJECT_GET_SET()

	// this function should move to a library!
	static bool ToNumber(const CString& str, long& rlValue);

protected:

	virtual HRESULT GetValueAt( int nBucket, int iIndex, DWORD& rValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual HRESULT SetValueAt( int nBucket, int iIndex, int value );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, DWORD value );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );
	
	virtual HRESULT CompareWithCurrentValueImpl( const CString& rstrCompare ) const;
	virtual HRESULT GetNormalizedValueImpl( const CString& strValue, CString& rstrNormalized ) const;

private:
	void LocalInitialize();
	
#pragma region Member Variables
private:
	// Sorted Table of all defined enumeration values.
	SvCl::SVObjectLongArrayClass m_enumValueTable;
	// String table of defined enumerations, 1 by 1 to value table
	SvCl::SVObjectCStringArrayClass m_enumStringTable;
#pragma endregion Member Variables
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEnumerateValueObjectClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:26:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jan 2013 10:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Sep 2012 15:03:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 13:46:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 10:21:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:41:18   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
