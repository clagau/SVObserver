//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputObject
//* .File Name       : $Workfile:   SVRemoteOutputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:52:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <comdef.h>
#include "SVRemoteOutputObject.h"
#include "SVObjectLibrary\SVClsIds.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVPPQObject.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVRemoteOutputObject, SVRemoteOutputObjectGUID );

SVRemoteOutputObject::SVRemoteOutputObject(void)
: m_pValueObject(NULL)
{
}

SVRemoteOutputObject::~SVRemoteOutputObject(void)
{
}

SVString SVRemoteOutputObject::GetInputValueObjectName()
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

SVValueObjectClass* SVRemoteOutputObject::GetValueObject()
{
	return m_pValueObject;
}

SVString SVRemoteOutputObject::GetGroupID( ) const
{
	return m_strGroupID;
}


// Parameters >> Tree
bool SVRemoteOutputObject::GetParameters( SVObjectXMLWriter& rWriter ) const
{
	_variant_t svVariant;

	svVariant = SVGUID( outObjectInfo.UniqueObjectID ).ToVARIANT();
	rWriter.WriteAttribute( CTAG_UNIQUE_REFERENCE_ID, svVariant );
	svVariant.Clear();

	svVariant = SVGUID( m_InputObjectId ).ToVARIANT();
	rWriter.WriteAttribute( CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, svVariant );
	svVariant.Clear();

	svVariant = m_strGroupID.ToVARIANT();
	rWriter.WriteAttribute( CTAG_REMOTE_GROUP_ID, svVariant );
	svVariant.Clear();

	svVariant = m_strObjectName.ToVARIANT();
	rWriter.WriteAttribute( CTAG_REMOTE_OUTPUT_NAME, svVariant );
	svVariant.Clear();

	return true;
}

HRESULT SVRemoteOutputObject::GetMaterials( SVMaterialsTreeAdapter& p_rMaterials, SVMaterialsTreeAdapter::SVTreeContainer* p_pParent )
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
	l_hr = l_Materials.UpdateMaterial( CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, l_vVariant );
	l_vVariant.Clear();

	l_vVariant = m_strGroupID.ToVARIANT();
	l_hr = l_Materials.UpdateMaterial( CTAG_REMOTE_GROUP_ID, l_vVariant );
	l_vVariant.Clear();


	SVMaterialsTreeAdapter::SVTreeElement l_Element( CTAG_REMOTE_OUTPUT_PARAMETERS, l_Materials );
	p_pParent->insert( p_pParent->end(), l_Element );

	return l_hr;
}

HRESULT SVRemoteOutputObject::Update( SVMaterialsTreeAdapter& p_rMaterialsTree )
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

	KeepPrevError( l_hr, l_Materials.GetMaterial( CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, l_vVariant ));
	if( l_hr == S_OK )
	{
		SVGUID l_guid( l_vVariant );
		m_InputObjectId = l_guid;
	}


	return l_hr;
}


// Sets parameters from Tree Control
BOOL SVRemoteOutputObject::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;
	_variant_t svVariant;

	bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiParent, svVariant );
	if ( bOk )
	{
		SVGUID ObjectID = svVariant;

		bOk = SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

		if( bOk )
		{
			outObjectInfo.UniqueObjectID = ObjectID;

			bOk = SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}
		else
		{
			OutputDebugString( "Remote Output Unable to set RemoteOutputObject Unique Id\n");
		}
	}

	if( bOk )
	{
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, htiParent, svVariant );
			if ( bOk )
			{
				m_InputObjectId = SVGUID( svVariant );
			}
		}

		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_REMOTE_GROUP_ID, htiParent, svVariant );
			if ( bOk )
			{
				m_strGroupID = svVariant;
			}
		}

		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_REMOTE_OUTPUT_NAME, htiParent, svVariant );
			if ( bOk )
			{
				m_strObjectName = svVariant;
			}
		}

	}
	return bOk;
}


HRESULT SVRemoteOutputObject::SetInputObject(SVValueObjectClass* p_pObject )
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


HRESULT SVRemoteOutputObject::SetInputObjectId( GUID p_ObjectId )
{
	m_InputObjectId = p_ObjectId;
	m_pValueObject = dynamic_cast<SVValueObjectClass*>(SVObjectManagerClass::Instance().GetObjectA( p_ObjectId ));
	return S_OK;
}

HRESULT SVRemoteOutputObject::GetInputValueObjectGUID( GUID& p_rGUID ) const
{
	p_rGUID = m_InputObjectId;
	return S_OK;
}

SVGUID SVRemoteOutputObject::GetInputValueObjectGUID() const
{
	return m_InputObjectId;
}

HRESULT SVRemoteOutputObject::SetGroupID( const SVString& p_strGroupID )
{
	m_strGroupID = p_strGroupID;
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputObject.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:52:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   11 Oct 2012 10:59:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   25 Sep 2012 15:26:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Sep 2012 18:32:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Sep 2012 15:37:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jan 2012 11:50:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 14:46:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 14:44:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Mar 2003 10:08:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed VariantClear in constructors to VariantInit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Jan 2003 13:42:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with SetInputs and SetToolParameterList from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2003 11:40:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified the base OutputObject class and its derived for easier RemoteOutput support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:52:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
