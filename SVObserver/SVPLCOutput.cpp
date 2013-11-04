//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCOutputObject
//* .File Name       : $Workfile:   SVPLCOutput.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:26:06  $
//******************************************************************************

#include "stdafx.h"
#include <comdef.h>
#include "SVPLCOutput.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVCLSIDs.h"
#include "SVPPQObject.h"
#include "SVUtilityLibrary\SVGUID.h"

SV_IMPLEMENT_CLASS( SVPLCOutputObject, SVPLCOutputObjectGUID );

SVPLCOutputObject::SVPLCOutputObject(void)
: m_pValueObject(NULL)
, m_bConvert( true )
, m_lPLCElement( 0 )
, m_lBit( 0 )
, m_bSizeIsInBits( false )
{
}

SVPLCOutputObject::~SVPLCOutputObject(void)
{
}

CString SVPLCOutputObject::GetInputValueObjectName()
{
	// Use the GUID to get an object.
	SVValueObjectClass* l_pObject = dynamic_cast<SVValueObjectClass*>(SVObjectManagerClass::Instance().GetObjectA(m_InputObjectId));
	if( l_pObject!= NULL )
	{	// Get the object name from the object pointer.
		m_strObjectName = l_pObject->GetCompleteObjectName();
		m_pValueObject = l_pObject;
	}
	return m_strObjectName;
}

CString SVPLCOutputObject::GetPLCName( ) const
{
	return m_strPLCId;
}

const long SVPLCOutputObject::GetElement() const
{
	return m_lPLCElement;
}

bool SVPLCOutputObject::SetElement( const long p_lElement )
{
	m_lPLCElement = p_lElement;
	return true;
}

long SVPLCOutputObject::GetSize() const
{
	return m_lSize;
}

bool SVPLCOutputObject::SizeInBits()
{
	return m_bSizeIsInBits;
}

HRESULT SVPLCOutputObject::SetSizeInBits( bool p_bSizeInBits )
{
	m_bSizeIsInBits = p_bSizeInBits;
	return S_OK;
}

// Parameters >> Tree
BOOL SVPLCOutputObject::GetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent ) const
{
	BOOL bOk = FALSE;

	_variant_t svVariant;

	svVariant = SVGUID( outObjectInfo.UniqueObjectID ).ToVARIANT();
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_UNIQUE_REFERENCE_ID, svVariant );
	svVariant.Clear();

	svVariant = m_lPLCElement;
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_ADDRESS, svVariant );
	svVariant.Clear();

	svVariant = SVGUID( m_InputObjectId ).ToVARIANT();
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_INPUT_OBJECT_GUID, svVariant );
	svVariant.Clear();

	svVariant = m_strPLCId;
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_ID, svVariant );
	svVariant.Clear();

	svVariant = m_lSize;
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_ELEMENT_SIZE, svVariant );
	svVariant.Clear();

	svVariant = m_bConvert;
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_CONVERT_DATA, svVariant );
	svVariant.Clear();

	svVariant = static_cast<long>(m_eDataType);
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_DATA_TYPE, svVariant );
	svVariant.Clear();

	svVariant = m_lBit;
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_BIT, svVariant );
	svVariant.Clear();

	svVariant = m_bSizeIsInBits;
	SVNavigateTreeClass::AddItem( rTree, htiParent, CTAG_PLC_SIZE_BITS, svVariant );
	svVariant.Clear();

	bOk = TRUE;
	return bOk;
}

HRESULT SVPLCOutputObject::GetMaterials( SVMaterialsTreeAdapter& p_rMaterials, SVMaterialsTreeAdapter::SVTreeContainer* p_pParent )
{
	HRESULT l_hr = S_FALSE;
	SVMaterials l_Materials;

	_variant_t l_vVariant;

	SVGUID l_GUID( outObjectInfo.UniqueObjectID );
	l_vVariant = l_GUID.ToVARIANT();
	l_hr = l_Materials.UpdateMaterial( CTAG_UNIQUE_REFERENCE_ID, l_vVariant );
	l_vVariant.Clear();

	l_GUID = m_InputObjectId;
	l_vVariant =  l_GUID.ToVARIANT();
	l_hr = l_Materials.UpdateMaterial( CTAG_PLC_ID, l_vVariant );
	l_vVariant.Clear();

	l_vVariant =  m_lPLCElement ;
	l_hr = l_Materials.UpdateMaterial( CTAG_PLC_ADDRESS, l_vVariant );
	l_vVariant.Clear();

	l_vVariant =  m_lSize ;
	l_Materials.UpdateMaterial( CTAG_PLC_ELEMENT_SIZE, l_vVariant );
	l_vVariant.Clear();

	l_vVariant =  m_bConvert ;
	l_Materials.UpdateMaterial( CTAG_PLC_CONVERT_DATA, l_vVariant );
	l_vVariant.Clear();

	l_vVariant =  static_cast<long>(m_eDataType) ;
	l_Materials.UpdateMaterial( CTAG_PLC_DATA_TYPE, l_vVariant );
	l_vVariant.Clear();

	l_vVariant =  m_lBit ;
	l_Materials.UpdateMaterial( CTAG_PLC_BIT, l_vVariant );
	l_vVariant.Clear();

	l_vVariant =  m_bSizeIsInBits ;
	l_Materials.UpdateMaterial( CTAG_PLC_SIZE_BITS, l_vVariant );
	l_vVariant.Clear();

	SVMaterialsTreeAdapter::SVTreeElement l_Element( CTAG_PLC_PARAMETERS, l_Materials );
	p_pParent->insert( p_pParent->end(), l_Element );

	return l_hr;
}

HRESULT SVPLCOutputObject::Update( SVMaterialsTreeAdapter& p_rMaterialsTree )
{
	HRESULT l_hr = S_FALSE;
	SVMaterialsTreeAdapter::SVTreeElement* l_pElement = p_rMaterialsTree.get();
	SVMaterials l_Materials = l_pElement->second;

	_variant_t l_vVariant;

	// Unique GUID for this object.
	l_hr = l_Materials.GetMaterial( CTAG_UNIQUE_REFERENCE_ID, l_vVariant );
	if ( l_hr == S_OK )
	{
		SVGUID l_ObjectID(l_vVariant);

		SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

		outObjectInfo.UniqueObjectID = l_ObjectID;

		SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_ADDRESS, l_vVariant ));
	if( l_hr == S_OK )
	{
		m_lPLCElement = l_vVariant;
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_INPUT_OBJECT_GUID, l_vVariant ));
	if( l_hr == S_OK )
	{
		SVGUID l_guid( l_vVariant );
		m_InputObjectId = l_guid;
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_ID, l_vVariant ));
	if( l_hr == S_OK )
	{
		m_strPLCId = l_vVariant;
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_ELEMENT_SIZE, l_vVariant ));
	if( l_hr == S_OK )
	{
		m_lSize = l_vVariant;
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_CONVERT_DATA, l_vVariant ));
	if( l_hr == S_OK )
	{
		m_bConvert = l_vVariant;
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_DATA_TYPE, l_vVariant ));
	if( l_hr == S_OK )
	{
		m_eDataType = static_cast<SVPLCDataTypeEnum>(l_vVariant.lVal);
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_BIT, l_vVariant ));
	if( l_hr == S_OK )
	{
		m_lBit = l_vVariant;
	}

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_PLC_SIZE_BITS, l_vVariant ));
	if( l_hr == S_OK )
	{
		m_bSizeIsInBits = l_vVariant;
	}

	return l_hr;
}


// Sets parameters from Tree Control
BOOL SVPLCOutputObject::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;
	_variant_t svVariant;

	bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiParent, svVariant );
	if ( bOk )
	{
		SVGUID ObjectID = svVariant;

		SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

		outObjectInfo.UniqueObjectID = ObjectID;

		SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
	}

	if( bOk )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_ADDRESS, htiParent, svVariant );
		if( bOk )
		{
			m_lPLCElement = svVariant;
		}// end if

		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_INPUT_OBJECT_GUID, htiParent, svVariant );
			if ( bOk )
			{
				m_InputObjectId = SVGUID( svVariant );
			}
		}
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_ID, htiParent, svVariant );
			if ( bOk )
			{
				m_strPLCId = svVariant;
			}
		}
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_ELEMENT_SIZE, htiParent, svVariant );
			if ( bOk )
			{
				m_lSize = svVariant;
			}
		}
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_CONVERT_DATA, htiParent, svVariant );
			if ( bOk )
			{
				m_bConvert = svVariant;
			}
		}
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_DATA_TYPE, htiParent, svVariant );
			if ( bOk )
			{
				long l_lTmp = svVariant;
				m_eDataType = static_cast< SVPLCDataTypeEnum >(l_lTmp);
			}
		}
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_BIT, htiParent, svVariant );
			if ( bOk )
			{
				m_lBit = svVariant;
			}
		}

		if ( bOk )
		{
			BOOL bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_PLC_SIZE_BITS, htiParent, svVariant );
			if ( bOk )
			{
				m_bSizeIsInBits = svVariant;
			}
		}

	}
	return bOk;
}


HRESULT SVPLCOutputObject::SetInputObject(SVValueObjectClass* p_pObject )
{
	if( p_pObject != NULL )
	{
		m_InputObjectId = p_pObject->GetUniqueObjectID();
	}
	else
	{
		m_InputObjectId.clear();
	}

	m_pValueObject = p_pObject;
	return S_OK;
}


HRESULT SVPLCOutputObject::SetInputObjectId( GUID p_ObjectId )
{
	m_InputObjectId = p_ObjectId;
	m_pValueObject = dynamic_cast<SVValueObjectClass*>(SVObjectManagerClass::Instance().GetObjectA( p_ObjectId ));
	return S_OK;
}

HRESULT SVPLCOutputObject::GetInputValueObjectGUID( GUID& p_rGUID ) const
{
	p_rGUID = m_InputObjectId;
	return S_OK;
}

HRESULT SVPLCOutputObject::SetPLCName( CString p_strPLCID )
{
	m_strPLCId = p_strPLCID;
	return S_OK;
}
HRESULT SVPLCOutputObject::GetPLCName( CString& p_strPLCID ) const
{
	p_strPLCID = m_strPLCId;
	return S_OK;
}

HRESULT SVPLCOutputObject::SetSize( long p_lSize )
{
	m_lSize = p_lSize;
	return S_OK;
}

bool SVPLCOutputObject::GetConvert() const
{
	return m_bConvert;
}

HRESULT SVPLCOutputObject::SetConvert(const bool p_bConvert)
{
	m_bConvert = p_bConvert;
	return S_OK;
}

SVPLCDataTypeEnum SVPLCOutputObject::GetDataType() const 
{
	return m_eDataType;
}

HRESULT SVPLCOutputObject::SetDataType( SVPLCDataTypeEnum p_eType )
{
	m_eDataType = p_eType; 
	return S_OK;
}

HRESULT SVPLCOutputObject::SetBit( const long p_lBit )
{
	m_lBit = p_lBit;
	return S_OK;
}

long SVPLCOutputObject::GetBit() const
{
	return m_lBit;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCOutput.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:26:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   11 Oct 2012 10:59:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   25 Sep 2012 15:24:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Sep 2012 18:29:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Sep 2012 15:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jan 2012 11:47:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Dec 2010 11:28:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Dec 2010 13:25:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Nov 2010 11:14:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 12:23:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

