//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 15:08:08  $
//******************************************************************************

#ifndef SVVALUEOBJECTCLASS_H
#define SVVALUEOBJECTCLASS_H

#include <set>
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectCStringArrayClass.h"
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVArrayValueHolder.h"
#include "SVFileNameClass.h"
#include "SVObjectAppClass.h"
#include "SVScalarValue.h"

enum SVResetItemEnum
{
	SVResetItemIP = 0,
	SVResetItemTool = 1,
	SVResetItemOwner = 2,
	SVResetItemNone = 3,
	SVResetItemSize,
};


/**
@SVObjectName Value Object

@SVObjectOverview This is the base object for all value holding object in the application.  This object manages the data an allows for automatic conversion from on type to another.

@SVObjectOperations This object contains the operators for creating, reseting, updating, and extracting the value information contained within this object.

*/
class SVValueObjectClass : public SVObjectAppClass  
{
//	SV_DECLARE_CLASS( SVValueObjectClass );
public:
	SVValueObjectClass( LPCTSTR ObjectName );
	SVValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVVALUEOBJECT );

public:
	virtual ~SVValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual BOOL CloseObject();
	virtual HRESULT ResetObject();

	virtual bool ResetAlways() const;
	virtual SVResetItemEnum ResetItem() const;
	virtual HRESULT SetResetOptions( bool p_bResetAlways, SVResetItemEnum p_eResetItem );

	virtual bool    IsArray() const;
	virtual int     GetArraySize() const;
	virtual HRESULT SetArraySize(int iSize);

	HRESULT GetResultSize(int iBucket, int& riResultSize) const;
	int       GetResultSize() const;
	HRESULT SetResultSize(int iBucket, int  riResultSize);

	// NVI pattern : public interface
	HRESULT SetValue( int iBucket, LPCTSTR value )                { return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, LPCTSTR value )    { return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, double& value )                { return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, double& value )    { return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, long& value )                { return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, long& value )    { return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, unsigned long& value )                { return SetValueAtConvert(iBucket, 0, static_cast <long> (value)); }
	HRESULT SetValue( int iBucket, int iIndex, unsigned long& value )    { return SetValueAtConvert(iBucket, iIndex, static_cast <long> (value)); }
	HRESULT SetValue( int iBucket, int& value )                { return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, int& value )    { return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, const int& value )                { return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, const int& value )    { return SetValueAtConvert(iBucket, iIndex, value); }

	HRESULT GetValue( double&  rValue ) const               { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	HRESULT GetValue( CString& rValue ) const               { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	HRESULT GetValue( VARIANT& rValue ) const               { return GetValueAt(m_iLastSetIndex, 0, rValue); }

	HRESULT GetValue( int iBucket, double&  rValue ) const  { return GetValueAt(iBucket, 0, rValue); }
	HRESULT GetValue( int iBucket, CString& rValue ) const  { return GetValueAt(iBucket, 0, rValue); }
	HRESULT GetValue( int iBucket, VARIANT& rValue ) const  { return GetValueAt(iBucket, 0, rValue); }

	HRESULT GetValue( int iBucket, int iIndex, double&  rValue ) const  { return GetValueAt(iBucket, iIndex, rValue); }
	HRESULT GetValue( int iBucket, int iIndex, CString& rValue ) const  { return GetValueAt(iBucket, iIndex, rValue); }
	HRESULT GetValue( int iBucket, int iIndex, VARIANT& rValue ) const  { return GetValueAt(iBucket, iIndex, rValue); }

	HRESULT GetValue( SVScalarValue& rValue ) const { return GetValue(m_iLastSetIndex, 0, rValue); }
	HRESULT GetValue( int iBucket, SVScalarValue& rValue ) const { return GetValue(iBucket, 0, rValue); }
	HRESULT GetValue( int iBucket, int iIndex, SVScalarValue& rValue ) const;

	HRESULT GetValues( VARIANT& rValue ) const  { return GetArrayValuesAsVariant(m_iLastSetIndex, rValue); }
	HRESULT GetValues( std::vector< _variant_t >&  rValue ) const  { return GetArrayValuesAsVariantVector(m_iLastSetIndex, rValue); }

	HRESULT GetValues( int iBucket, VARIANT& rValue ) const  { return GetArrayValuesAsVariant(iBucket, rValue); }
	HRESULT GetValues( int iBucket, std::vector< _variant_t >&  rValue ) const  { return GetArrayValuesAsVariantVector(iBucket, rValue); }

	HRESULT GetValues( int iBucket, std::vector< double >&  rValue ) const  { return GetArrayValues(iBucket, rValue); }

	HRESULT GetTypeName( CString& p_rstrValue ) const;
	HRESULT SetTypeName( const LPCTSTR p_pstrValue );

	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );

	HRESULT CompareWithCurrentValue( const CString& rstrCompare ) const { return CompareWithCurrentValueImpl( rstrCompare ); }
	HRESULT GetNormalizedValue( const CString& strValue, CString& rstrNormalized ) const { return GetNormalizedValueImpl( strValue, rstrNormalized ); }

	virtual HRESULT CopyValue( int iSourceBucket, int iDestBucket ) { ASSERT(0); return S_FALSE; }
	        HRESULT CopyLastSetValue( int iDestBucket )    { return CopyValue(m_iLastSetIndex, iDestBucket); }

	BOOL GetLastSetIndex( int& iBucket ) const    { iBucket = m_iLastSetIndex; return TRUE; }
	int  GetLastSetIndex()               const    { return m_iLastSetIndex; }
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	void SetLegacyVectorObjectCompatibility() { m_bLegacyVectorObjectCompatibility = true; }

protected:
	// use the NonVirtual Interface pattern (NVI; C++ Coding Standards # 39, Sutter & Alexandrescu)
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, CString value ) = 0;//   { ASSERT(0); return S_FALSE; }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) = 0;//   { ASSERT(0); return S_FALSE; }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) = 0;//   { ASSERT(0); return S_FALSE; }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) = 0;//   { ASSERT(0); return S_FALSE; }

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double&  rValue ) const = 0;//  { ASSERT(0); return S_FALSE; }
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const = 0;//  { ASSERT(0); return S_FALSE; }
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const = 0;//  { ASSERT(0); return S_FALSE; }

	virtual HRESULT GetArrayValues( int iBucket, std::vector< double >&  rValue ) const = 0;//  { ASSERT(0); return S_FALSE; }
	virtual HRESULT GetArrayValuesAsVariant( int iBucket, VARIANT&  rValue ) const = 0;//  { ASSERT(0); return S_FALSE; }
	virtual HRESULT GetArrayValuesAsVariantVector( int iBucket, std::vector< _variant_t >&  rValue ) const = 0;//  { ASSERT(0); return S_FALSE; }

	// EB 2005 11 11
	// eventually all derived classes should implement this
	// this should replace the functionality currently in SVInspectionProcess::ProcessInputRequests
	// where it tries to determine if there is a difference between the new and old value.
	virtual HRESULT CompareWithCurrentValueImpl( const CString& rstrCompare ) const;
	virtual HRESULT GetNormalizedValueImpl( const CString& strValue, CString& rstrNormalized ) const { return E_NOTIMPL; }

	virtual DWORD_PTR  processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT CreateBuckets();
	void              InitializeBuckets();
	int               GetResultSize(int iBucket) const;

	HRESULT           ValidateIndexes(int iBucket, int iArrayIndex) const;

	inline int        ArraySize() const { return m_iArraySize; }	// a fast version of GetArraySize for implementation use

	int m_iNumberOfBuckets;
	int m_iLastSetIndex;
	int m_iArraySize;
	std::vector<int> m_aiResultSize;

	SVArrayValueHolderBase* m_pBuckets;

	bool m_bLegacyVectorObjectCompatibility;

	void swap( SVValueObjectClass& rhs );
	CString m_strTypeName;

	bool m_bResetAlways;
	SVResetItemEnum m_eResetItem;

private:
	void Initialize();

	friend class SVValueObjectReference;	// direct access for speed (VC6 lack of good optimization)
};

#include "SVValueObjectClass.inl"

typedef SVVector< SVValueObjectClass* > SVValueObjectArray;
typedef std::set< SVValueObjectClass* > SVValueObjectClassPtrSet;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVValueObjectClass.h_v  $
 * 
 *    Rev 1.2   15 May 2014 15:08:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:26:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:16:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Feb 2013 13:08:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Value Object classes to include new GetValues methods that take VARIANT as the value parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   11 Feb 2013 14:22:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Jan 2013 10:59:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Sep 2012 15:20:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   14 Feb 2012 13:06:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated target windows version to XP (_WIN32_WINNT 0x0501).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Mar 2011 16:36:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Mar 2011 07:46:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Nov 2010 14:49:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Oct 2010 14:27:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 13:31:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Oct 2009 11:18:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed Path for SVDPointClass to SVUtilityLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:59:12   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   07 Mar 2008 12:58:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added TypeName to value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   31 Oct 2007 08:36:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added virtual destructor See C++ Coding standards page 90 avoid memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   21 Jun 2007 15:07:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   17 Jan 2006 14:56:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetNormalizedValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   17 Jan 2006 10:13:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetValue(SVScalarValue) and swap
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   11 Nov 2005 14:27:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  525
 * SCR Title:  Fix problem with Shape Mask editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CompareWithCurrentValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   14 Sep 2005 13:20:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method to get the array data without have to present the last set bucket.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   13 Sep 2005 14:24:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed bug in setting array with 0 elements
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   08 Sep 2005 11:47:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed problematic ASSERTs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   07 Sep 2005 13:26:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added comments describing the optimization
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   07 Sep 2005 12:56:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ASSERTS to catch misuse of functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   02 Sep 2005 14:46:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized for speed
 * - only store multidimentional buckets if array size != 1
 * - inlined functions (ArraySize) etc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   29 Aug 2005 14:06:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made SVValueObjectReference a friend class of SVValueObject so it can make direct calls to protected member functions.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   26 Aug 2005 15:06:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made slight speed improvements to ValueObject. Really need /Og to get huge speed increases.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   08 Aug 2005 15:16:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Split Value Object base functionality into own file and moved all other value objects to another single file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   02 Aug 2005 10:42:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   be less strict on SetArrayValues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   01 Aug 2005 12:02:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SVVectorObject with SVValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   29 Jul 2005 13:37:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new attributes and methods to track required reset functionality when values are changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   19 Jul 2005 14:44:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   14 Jul 2005 12:38:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to bool on SetDefaultValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   30 Jun 2005 15:12:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   22 Jun 2005 08:46:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implement array value object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   17 Feb 2005 14:32:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support the new createObject and reset Methodology.
 * Added ResetObject(, CreateArrays(, DestroyArrays(
 * Removed SetObjectDepth(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   16 Feb 2005 15:25:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   27 Jul 2004 15:18:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added long GetDefaultType() to SVVariantValueObjectClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   07 Jul 2004 10:15:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up clutter
 * added SVValueObjectUnitTest
 * added support for Variant
 * added support for char
 * added GetLastSetIndex()
 * made CopyLastSetValue, GetLastSetIndex(int&) non-virtual
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   02 Apr 2004 12:06:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  411
 * SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new Set & Get methods for bool's
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   04 Dec 2003 13:17:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   17 Sep 2003 13:02:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   17 Jun 2003 15:13:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated the new disable method code. Also, made Tool.Enabled accessible through the ActiveX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   22 Apr 2003 17:29:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   10 Apr 2003 10:14:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  271
 * SCR Title:  Adding timer features for Tool Execution times.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a new SetValue and new GetValue with the type being __int64
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   14 Jan 2003 14:11:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a SetValue that takes a double value, to the SVLongValueObjectClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Dec 2002 15:22:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   typedef SVArrayClass <SVValueObjectClass*, SVValueObjectClass*> SVValueObjectArray;
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   13 Nov 2002 12:32:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  289, 272, 226
 * SCR Title:  Extending the SVValueObjectClass Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   The following apply to the class SVValueObject and its derivatives, SVBoolValueObjectClass, SVByteValueObjectClass, SVDWordValueObjectClass, SVLongValueObjectClass, SVDoubleValueObjectClass, SVPointValueObjectClass, SVStringValueObjectClass, SVFileNameValueObjectClass.
 * 
 * The SetValue(type) functions were changed to SetValue(bucket, type)
 * 
 * The SetValue(bucket, type&) functions were changed to be aware of the buckets.
 * 
 * The GetValue(type&) functions were changed to call GetValue(m_nLastSetIndex, type&)
 * 
 * The GetValue(bucket, type&) functions were changed to be aware of the buckets.
 * 
 * Extra GetValue functions were added in each class to support uniform conversions of type.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   13 Jun 2001 10:44:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVStringValueObjectClass::SetValue, 
 * SVStringValueObjectClass::SetObjectValue, both SVFileNameValueObjectClass::SVFileNameValueObjectClass constructors, 
 * ~SVFileNameValueObjectClass, SVFileNameValueObjectClass::SetValue,
 * SVFileNameValueObjectClass::SetDefaultValue, SVFileNameValueObjectClass::SetObjectValue,
 * SVFileNameValueObjectClass::GetObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2001 20:20:32   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Jan 2001 17:37:38   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Modified SVStringValueObjectClass to use the SVFileNameClass and the SVFileNameManagerClass is the object refers to a filename.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   23 Mar 2000 16:52:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Fixed bugs in SVByteValueObjectClass.
 * Fixed SVValueObjectClass to handle Byte Value Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 14:00:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Sep 1999 13:43:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetDefaultValue Method for all classes
 * derived from SVValueObjectClass.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Sep 1999 08:35:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to set defaults in the PPQDataLinkInfoStruct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Aug 1999 16:40:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Overridden GetValue() for CString& and double&.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
