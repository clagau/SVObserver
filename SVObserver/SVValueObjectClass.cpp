//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectClass
//* .File Name       : $Workfile:   SVValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 15:08:10  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVValueObjectClass.h"

#include "SVInspectionProcess.h"
#include "SVTool.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//SV_IMPLEMENT_CLASS(SVValueObjectClass, SVValueObjectClassGuid);

SVValueObjectClass::SVValueObjectClass( LPCSTR ObjectName )
: SVObjectAppClass(ObjectName) 
{
	Initialize();
}


SVValueObjectClass::SVValueObjectClass(SVObjectClass* POwner, int StringResourceID)
: SVObjectAppClass(POwner, StringResourceID) 
{
	Initialize();
}

SVValueObjectClass::~SVValueObjectClass()
{
	if( GetInspection() != NULL )
	{
		GetInspection()->UnregisterSubObject( this );
	}

	if( GetTool() != NULL )
	{
		GetTool()->UnregisterSubObject( this );
	}
	
	SetObjectDepth(-1);
	if ( m_pBuckets )
		delete m_pBuckets;
}

void SVValueObjectClass::InitializeBuckets()
{
	objectDepth = 2;
	m_iArraySize = 1;

	CreateBuckets();

	if( 1 < m_iNumberOfBuckets )
	{
		m_iLastSetIndex = 1;
	}
}

HRESULT SVValueObjectClass::CreateBuckets( )
{
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), m_iArraySize);

	return S_OK;
}

BOOL SVValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = SVObjectAppClass::CreateObject( pCreateStructure );

	if( GetTool() != NULL )
	{
		GetTool()->RegisterSubObject( this );
	}
	
	if( GetInspection() != NULL )
	{
		GetInspection()->RegisterSubObject( this );
	}
	
	ObjectAttributesAllowedRef() |= SV_CH_VALUE | SV_CH_CONDITIONAL | SV_DD_VALUE;	// derived classes need to reset this

	if ( m_iNumberOfBuckets < 2 )
	{
		l_bOk = l_bOk && SetObjectDepth( 2 );
	}

	l_bOk = l_bOk && (CreateBuckets() == S_OK);

	if( l_bOk )
	{
		m_iLastSetIndex = 1;
	}

	return l_bOk;
}

BOOL SVValueObjectClass::CloseObject()
{
	if( GetInspection() != NULL )
	{
		GetInspection()->UnregisterSubObject( this );
	}

	if( GetTool() != NULL )
	{
		GetTool()->UnregisterSubObject( this );
	}
	
	return SVObjectAppClass::CloseObject();
}

HRESULT SVValueObjectClass::ResetObject()
{
	HRESULT l_hrOk = CreateBuckets();

	HRESULT hr = SVObjectAppClass::ResetObject();
	if ( l_hrOk == S_OK )
	{
		l_hrOk = hr;
	}

	return l_hrOk;
}

bool SVValueObjectClass::ResetAlways() const
{
	return m_bResetAlways;
}

SVResetItemEnum SVValueObjectClass::ResetItem() const
{
	return m_eResetItem;
}

HRESULT SVValueObjectClass::SetResetOptions( bool p_bResetAlways, SVResetItemEnum p_eResetItem )
{
	m_bResetAlways = p_bResetAlways;
	m_eResetItem = p_eResetItem;

	return S_OK;
}

HRESULT SVValueObjectClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	return SVObjectAppClass::SetObjectValue( p_rValueName, p_rVariantValue );
}

HRESULT SVValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	return SVObjectAppClass::SetObjectValue(pDataObject);
}

#ifdef USE_OBJECT_SCRIPT
void SVValueObjectClass::GetObjectScript(CString& rstrScript, CString& rstrAliasTable, int iIndent)
{
	// Get the Heading (Class Info)
	SVObjectAppClass::GetObjectScript(rstrScript, rstrAliasTable, iIndent);
}
#endif

DWORD_PTR SVValueObjectClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	switch (dwPureMessageID)
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}

	case SVMSGID_COPY_RESULT_DATA_FORWARD:
		HRESULT hrRet = CopyLastSetValue( static_cast<int>(DwMessageValue) );

		return (hrRet == S_OK) ? SVMR_SUCCESS : SVMR_NO_SUCCESS;
		break;
	}

	return (SVObjectAppClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext) | DwResult);
}

bool SVValueObjectClass::IsArray() const
{
	return m_iArraySize > 1;
}

int SVValueObjectClass::GetArraySize() const
{
	return m_iArraySize;
}

HRESULT SVValueObjectClass::SetArraySize(int iSize)
{
	HRESULT hr = S_OK;

	m_iArraySize = std::max(iSize,1);	// minimum one array element

	if( m_iArraySize <= 1 )
	{
		if( m_strTypeName.Find( _T(" Array") ) != -1 )
		{
			m_strTypeName = m_strTypeName.Left( m_strTypeName.GetLength() - 6 );
		}
	}
	else
	{
		if( m_strTypeName.Find( _T(" Array") ) == -1 )
		{
			m_strTypeName += _T(" Array") ;
		}
	}

	// default number of results == array size
	m_aiResultSize.resize(m_iNumberOfBuckets);
	std::fill(m_aiResultSize.begin(), m_aiResultSize.end(), iSize);	// allow the result size to be 0

	m_auObjectAttributesSet.resize( m_iArraySize, m_uDefaultObjectAttributesSet );

	return hr;
}

HRESULT SVValueObjectClass::GetResultSize(int iBucket, int& riResultSize) const
{
	HRESULT hr = S_FALSE;

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		ASSERT( m_aiResultSize.size() == m_iNumberOfBuckets );
		riResultSize = m_aiResultSize[iBucket];
		hr = S_OK;
	}

	return hr;
}

int SVValueObjectClass::GetResultSize() const
{
	int iResultSize=0;
	GetResultSize(m_iLastSetIndex, iResultSize);
	return iResultSize;
}

// protected, only to be used by derived classes
int SVValueObjectClass::GetResultSize(int iBucket) const
{
	return m_aiResultSize[iBucket];
}

HRESULT SVValueObjectClass::SetResultSize(int iBucket, int riResultSize)
{
	HRESULT hr = S_FALSE;

	ASSERT( riResultSize <= m_iArraySize );

	if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
	{
		ASSERT( m_aiResultSize.size() == m_iNumberOfBuckets );
		m_aiResultSize[iBucket] = riResultSize;
		hr = S_OK;
	}

	return hr;
}

HRESULT SVValueObjectClass::CompareWithCurrentValueImpl( const CString& rstrCompare ) const
{
	// default implementation
	HRESULT hr = S_FALSE;
	CString strCurrentValue;
	HRESULT hrGet = GetValue( strCurrentValue );
	if ( hrGet == S_OK )
	{
		hr = ( (strCurrentValue == rstrCompare) ? S_OK : S_FALSE );
	}
	return hr;
}

HRESULT SVValueObjectClass::GetValue( int iBucket, int iIndex, SVScalarValue& rValue ) const
{
	rValue.strName = GetCompleteObjectName();
	//rValue.strType = ;
	HRESULT hr = GetValue( iBucket, iIndex, rValue.strValue );

	return hr;
}

HRESULT SVValueObjectClass::GetTypeName( CString& p_rstrValue )const 
{
	p_rstrValue = m_strTypeName;	

	return S_OK;
}

HRESULT SVValueObjectClass::SetTypeName( LPCTSTR p_pstrValue )
{
	m_strTypeName = p_pstrValue;	

	return S_OK;
}

void SVValueObjectClass::Initialize()
{
	m_iLastSetIndex = -1;
	m_iNumberOfBuckets = 0;
	m_iArraySize = 1;
	m_pBuckets = NULL;
	m_bLegacyVectorObjectCompatibility = false;
	m_bResetAlways = false;
	m_eResetItem = SVResetItemIP;

	ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;// | SV_CH_VALUE | SV_CH_CONDITIONAL;	// These are included in the DEFAULT
}

#pragma region IValueObject
HRESULT SVValueObjectClass::SetValue( const _variant_t& rValue )
{
	HRESULT hr = S_OK;
	if (!IsArray() || 0 == (VT_ARRAY & rValue.vt) || nullptr == rValue.parray)
	{
		hr = SetValue(m_iLastSetIndex, 0, SVString(rValue).ToString()); 
	}
	else
	{
		SVSAFEARRAY safeArray( rValue );

		for (int i=0; i<safeArray.size(); i++)
		{
			variant_t tmpVar;
			HRESULT tempHr = safeArray.GetElement( i, tmpVar );
			if (S_OK == hr)
			{
				hr = tempHr;
			}
			
			tempHr = SetValue(m_iLastSetIndex, i, SVString(tmpVar).ToString()); 
			if (S_OK == hr)
			{
				hr = tempHr;
			}
		}
	}

	return hr;
}
HRESULT SVValueObjectClass::GetValue( _variant_t& rValue ) const
{ 
	if (!IsArray())
	{
		return GetValue(*(rValue.GetAddress())); 
	}
	else
	{
		return GetValues(*(rValue.GetAddress())); 
	}
}
#pragma endregion IValueObject
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVValueObjectClass.cpp_v  $
 * 
 *    Rev 1.3   15 May 2014 15:08:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 12:26:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:37:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 May 2013 11:25:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:16:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Jan 2013 10:59:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Sep 2012 15:20:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   06 Apr 2011 12:59:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with value objects not getting un-registered from the tool and inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Mar 2011 16:36:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Mar 2011 15:29:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Mar 2011 07:46:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 14:27:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Feb 2009 13:45:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  652
 * SCR Title:  Fix Bug in Value Definition List
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to SetArraySize to append the TypeName with the word Array if the ArraySize is greater than 1.  And code to remove the Word Array if the size is less than or equal to 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:59:10   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   07 Mar 2008 08:13:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  616
   SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Type Name support to ValueObjects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   20 Jan 2006 15:07:18   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     SV_ALL_ATTRIBUTES was renamed to SV_DEFAULT_ATTRIBUTES
   added SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES
   added SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   19 Jan 2006 08:55:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed object attributes for Conditional History
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   17 Jan 2006 09:53:40   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added Conditional History attributes
   added SVScalarValue accessor functions
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   11 Nov 2005 14:27:34   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  525
   SCR Title:  Fix problem with Shape Mask editor
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added CompareWithCurrentValue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   08 Sep 2005 11:53:02   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Add message handle for ResetObject in process message. 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   08 Aug 2005 15:16:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Split Value Object base functionality into own file and moved all other value objects to another single file.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   01 Aug 2005 12:01:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     replaced SVVectorObject with SVValueObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   29 Jul 2005 13:37:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new attributes and methods to track required reset functionality when values are changed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   28 Jul 2005 15:27:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     in SetArraySize, use m_uDefaultObjectAttributesSet
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   27 Jul 2005 15:24:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     removed the improper isCreated assignment
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   27 Jul 2005 08:12:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed the isObjectValid and isCreated flags
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   26 Jul 2005 12:27:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed loading problem for SVFileNameValueObjects (SetObjectValue)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   19 Jul 2005 14:44:04   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed Get/Set
   fixed Variant merge error - SVRemoveEscapedSpecialCharacters
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   14 Jul 2005 12:38:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed object script to store full array of last set bucket, instead of first element in array of bucket 0 and 1
   changed BOOL to bool on SetDefaultValue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   22 Jun 2005 08:46:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implement array value object
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   17 Feb 2005 14:32:54   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modifications to support the new createObject and reset Methodology.
   Added ResetObject(, CreateArrays(, DestroyArrays(
   Removed SetObjectDepth(
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   15 Feb 2005 15:06:14   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  468
   SCR Title:  Fix Variant Object Script
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified SVVariantValueObjectClass::SetObjectValue to remove special characters when the variant is a BSTR
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   06 Aug 2004 12:11:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  416
   SCR Title:  Add Variant Value Object
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     fixed parser to treat floating point as US locale
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   27 Jul 2004 15:18:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  416
   SCR Title:  Add Variant Value Object
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added long GetDefaultType() to SVVariantValueObjectClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   07 Jul 2004 10:15:10   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  416
   SCR Title:  Add Variant Value Object
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     cleaned up clutter
   added SVValueObjectUnitTest
   added support for Variant
   added support for char
   added GetLastSetIndex()
   made CopyLastSetValue, GetLastSetIndex(int&) non-virtual
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   02 Apr 2004 12:06:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  411
   SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new Set & Get methods for bool's
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   04 Dec 2003 13:17:20   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 Sep 2003 13:02:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  384
   SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   17 Jun 2003 15:13:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  373
   SCR Title:  Add a new disable method that doesn't copy forward all result values
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated the new disable method code. Also, made Tool.Enabled accessible through the ActiveX.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   22 Apr 2003 17:29:50   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   14 Jan 2003 14:11:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added a SetValue that takes a double value, to the SVLongValueObjectClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   18 Dec 2002 08:26:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  299
   SCR Title:  SIAC needs to be able to request Source Images
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   09 Dec 2002 20:21:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added SV_PRINTABLE attribute flag as a default.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   13 Nov 2002 12:32:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  289, 272, 226
   SCR Title:  Extending the SVValueObjectClass Object
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     The following apply to the class SVValueObject and its derivatives, SVBoolValueObjectClass, SVByteValueObjectClass, SVDWordValueObjectClass, SVLongValueObjectClass, SVDoubleValueObjectClass, SVPointValueObjectClass, SVStringValueObjectClass, SVFileNameValueObjectClass.
   
   The SetValue(type) functions were changed to SetValue(bucket, type)
   
   The SetValue(bucket, type&) functions were changed to be aware of the buckets.
   
   The GetValue(type&) functions were changed to call GetValue(m_iLastSetIndex, type&)
   
   The GetValue(bucket, type&) functions were changed to be aware of the buckets.
   
   Extra GetValue functions were added in each class to support uniform conversions of type.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
