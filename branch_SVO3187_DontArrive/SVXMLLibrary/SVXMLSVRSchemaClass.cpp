//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLSVRSchemaClass
//* .File Name       : $Workfile:   SVXMLSVRSchemaClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:57:14  $
//******************************************************************************

#include "stdafx.h"
#import <msxml6.dll> // MSXML
#include "SVXMLSVRSchemaClass.h"
#include "SVXMLLibraryGlobals.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\SVUtilityGlobals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVXMLSVRSchemaClass::SVXMLSVRSchemaClass()
{
	m_opDOM = nullptr;
}

SVXMLSVRSchemaClass::~SVXMLSVRSchemaClass()
{

}

HRESULT SVXMLSVRSchemaClass::SetSVRSchema (SVDOMClass*	p_opDOM)
{
	HRESULT	hr;

	SVXML::IXMLDOMElementPtr oDOMRootPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;


	hr = S_OK;

	while (1)
	{
		if (nullptr == p_opDOM)
		{
			hr = -1900;
			break;
		}

		m_opDOM = p_opDOM;


		hr = m_bstrAttributeDefinitionName.CopyFromWChar (L"AttributeType");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrAttributeNameTitle.CopyFromWChar (L"name");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrAttributeDefinitionStringType.CopyFromWChar (L"string");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrAttributeType1Title.CopyFromWChar (L"dt:type");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementDefinitionName.CopyFromWChar (L"ElementType");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementDefinitionMixedContent.CopyFromWChar (L"mixed");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementNameTitle.CopyFromWChar (L"name");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementContentTitle.CopyFromWChar (L"content");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementDefinitionClosedModel.CopyFromWChar(L"closed");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementModelTitle.CopyFromWChar (L"model");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrAttributeElementName.CopyFromWChar (L"attribute");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		hr = m_bstrAttributeType2Title.CopyFromWChar (L"type");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrAttributeDefinitionYesRequired.CopyFromWChar (L"yes");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrAttributeDefinitionNoRequired.CopyFromWChar (L"no");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}


		hr = m_bstrAttributeRequiredTitle.CopyFromWChar (L"required");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}


		hr = m_bstrElementElementName.CopyFromWChar (L"element");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementMinOccursTitle.CopyFromWChar (L"minOccurs");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_bstrElementMaxOccursTitle.CopyFromWChar (L"maxOccurs");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}




//-	Set up schema header element -
//-	<Schema name="SVR00001" 
//-            xmlns="urn:schemas-microsoft-com:xml-data"
//-            xmlns:dt="urn:schemas-microsoft-com:datatypes">
//-	Creates m_oSchemaBaseElementPtr.
		hr = SetSVRSchemaHeader ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up Name attribute for NODE or DATA elements.
//-	  <AttributeType name="Name" dt:type="string"/>
//-	Appends to m_oSchemaBaseElementPtr.
		hr = SetSVRSchemaNameAttribute ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up Value attribute for NODE or DATA elements.
//-	  <AttributeType name="Value" dt:type="string"/>
//-	Appends to oSchemaBaseElementPtr.
		hr = SetSVRSchemaValueAttribute ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up Type attribute for NODE or DATA elements.
//-	  <AttributeType name="Type" dt:type="string"/>
//-	Appends to oSchemaBaseElementPtr.
		hr = SetSVRSchemaTypeAttribute ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up DATA element.
//-	  <ElementType name="DATA" content="mixed" model="closed">
		hr = SetSVRSchemaDATAElement ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}


//-	Set up NODE element.
//-	  <ElementType name="NODE" content="mixed" model="closed">
		hr = SetSVRSchemaNODEElement ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//===========================================================================
//-   Revision History records.
//-
//-	Set up Format attribute for Revision elements.
//-	  <AttributeType name="Format" dt:type="string"/>
		hr = SetSVRSchemaFormatAttribute ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}


//-	Set up FormatVersion attribute for Revision elements.
//-	  <AttributeType name="FormatVersion" dt:type="string"/>
		hr = SetSVRSchemaFormatVersionAttribute ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up Revision attribute for Revision elements.
//-	  <AttributeType name="Revision" dt:type="string"/>
		hr = SetSVRSchemaRevisionAttribute ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up Revision element.
//-	  <ElementType name="Revision" content="mixed" model="closed">
		hr = SetSVRSchemaRevisionElement ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up RevisionHistory element.
//-	  <ElementType name="RevisionHistory" content="mixed" model="closed">
		hr = SetSVRSchemaRevisionHistoryElement ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}


//-	Set up IsActive attribute.
//-     <AttributeType name="IsActive" dt_type="string"/>
		hr = SetSVRSchemaIsActiveAttribute ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up Encryption element.
//-     <ElementType name="Encryption" content="mixed" model="closed">
//-       <attribute type="IsActive" required="yes"/>
//-       <element type="DATA" minOccurs="0" maxOccurs="1"/>
//-     </ElementType>
		hr = SetSVRSchemaEncryptionElement ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}



//===========================================================================
//-	Attach to Root element.
		hr = m_opDOM->GetRootNode (oDOMRootPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
	
		oReturnElementPtr = oDOMRootPtr->appendChild (m_oSchemaBaseElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1700;
			break;
		}

		hr = m_opDOM->ClearNameSpace ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		break;
	}

	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaHeader ()
{
	HRESULT	hr;

	SVBStr			bstrAttributeTitle;
	SVBStr			bstrNameSpace;
	SVBStr			bstrSchemaDefinitionBranchName;

	CComVariant		ccvAttributeValue;



	hr = S_OK;

	while (1)
	{
		bstrSchemaDefinitionBranchName = g_csSchema;

//-	Set up schema header element -
//-	<Schema name="SVR00001" 
//-            xmlns="urn:schemas-microsoft-com:xml-data"
//-            xmlns:dt="urn:schemas-microsoft-com:datatypes">

		hr = bstrNameSpace.CopyFromWChar (L"urn:schemas-microsoft-com:xml-data");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_opDOM->SetNameSpace (bstrNameSpace);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = m_opDOM->CreateElement (bstrSchemaDefinitionBranchName, 
			       						  m_oSchemaBaseElementPtr);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		ccvAttributeValue = g_csSVR00001SchemaName;			// Schema name
		m_oSchemaBaseElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, 
			                                    ccvAttributeValue);
		ccvAttributeValue.Clear ();

		hr = bstrAttributeTitle.CopyFromWChar(L"xmlns:dt");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		ccvAttributeValue = "urn:schemas-microsoft-com:datatypes";
		m_oSchemaBaseElementPtr->setAttribute ((BSTR) bstrAttributeTitle, 
			                                     ccvAttributeValue);
		ccvAttributeValue.Clear ();
		

		break;
	}


	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaNameAttribute ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;
	
	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;


	hr = S_OK;

	while (1)
	{
//-	Set up Name attribute for NODE or DATA elements.
//-	  <AttributeType name="Name" dt:type="string"/>

		m_opDOM->CreateElement (m_bstrAttributeDefinitionName, 
									   oAttributeElementPtr);

		ccvAttributeValue = g_wcsName;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionStringType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType1Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1697;
			break;
		}

		break;
	}
	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaValueAttribute ()
{
	HRESULT	hr;
	
	CComVariant		ccvAttributeValue;

	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;

	
	hr = S_OK;

	while (1)
	{

		m_opDOM->CreateElement (m_bstrAttributeDefinitionName, 
										 oAttributeElementPtr);

		ccvAttributeValue = g_csValue;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionStringType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType1Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1698;
			break;
		}

		break;
	}

	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaTypeAttribute ()
{
	HRESULT	hr;
	
	CComVariant		ccvAttributeValue;

	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;

	
	hr = S_OK;

	while (1)
	{
//-	Set up Type attribute for NODE or DATA elements.
//-	  <AttributeType name="Type" dt:type="string"/>

		m_opDOM->CreateElement (m_bstrAttributeDefinitionName, 
		 							   oAttributeElementPtr);

		ccvAttributeValue = g_wcsType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionStringType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType1Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1699;
			break;
		}

		break;
	}

	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaDATAElement ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;

	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oElementElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;


	hr = S_OK;

	while (1)
	{


//-	Set up DATA element.
//-	  <ElementType name="DATA" content="mixed" model="closed">
//-
//-   content is mixed because I (JAB) sometimes go in and edit the XML file 
//-	for readabillity. When I do this, the white space that I put in (cr, 
//-	lf, tabs, and spaces) are considerred text by the XML parser and an 
//-	error if the schema indicates that there should be elements only.  
//-   Although we do not use text, it is allowed.

		m_opDOM->CreateElement (m_bstrElementDefinitionName, 
										oElementElementPtr);

		ccvAttributeValue = g_wcsData;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionMixedContent;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementContentTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionClosedModel;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementModelTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();


//-	Set up Name attribute of the DATA element.
//-	  <attribute type="Name" required="yes"/>

		m_opDOM->CreateElement (m_bstrAttributeElementName, 
		   							oAttributeElementPtr);

		ccvAttributeValue = g_wcsName;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1702;
			break;
		}


//-	Set up Type attribute of the DATA element.
//-	  <attribute type="Type" required="yes"/>
		m_opDOM->CreateElement (m_bstrAttributeElementName, 
										 oAttributeElementPtr);

		ccvAttributeValue = g_wcsType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1704;
			break;
		}

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1701;
			break;
		}


		break;
	}

	return hr;
}




HRESULT SVXMLSVRSchemaClass::SetSVRSchemaNODEElement ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;

	
	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oElementElementPtr;
	SVXML::IXMLDOMElementPtr oElement2ElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;


	hr = S_OK;

	while (1)
	{

//-	Set up NODE element.
//-	  <ElementType name="NODE" content="mixed" model="closed">
//-
//-   content is mixed because I (JAB) sometimes go in and edit the XML file 
//-	for readabillity. When I do this, the white space that I put in (cr, 
//-	lf, tabs, and spaces) are considerred text by the XML parser and an 
//-	error if the schema indicates that there should be elements only.  
//-   Although we do not use text, it is allowed.

		m_opDOM->CreateElement (m_bstrElementDefinitionName, 
									   oElementElementPtr);

		ccvAttributeValue = (BSTR) g_wcsNode;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionMixedContent;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementContentTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionClosedModel;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementModelTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();


//-	Set up Name attribute of the NODE element.
//-	  <attribute type="Name" required="yes"/>

		m_opDOM->CreateElement (m_bstrAttributeElementName, 
		   						   oAttributeElementPtr);

		ccvAttributeValue = g_wcsName;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1705;
			break;
		}


//-	Set up Name attribute of the NODE element.
//-	  <attribute type="Type" required="no"/>

		m_opDOM->CreateElement (m_bstrAttributeElementName, 
										oAttributeElementPtr);

		ccvAttributeValue = g_wcsType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionNoRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1744;
			break;
		}

//-	Set up NODE child of the NODE element.
//-   May have any number of child NODE elements.
//-	  <element type="NODE" minOccurs="0" maxOccurs="*"/>

		m_opDOM->CreateElement (m_bstrElementElementName,
 										oElement2ElementPtr);
		ccvAttributeValue = (BSTR) g_wcsNode;
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "0";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMinOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "*";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMaxOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oElement2ElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1707;
			break;
		}


//-	Set up DATA child of the NODE element.
//-   May have either 0 or 1 child DATA elements.
//-	  <element type="DATA" minOccurs="0" maxOccurs="1"/>

		m_opDOM->CreateElement (m_bstrElementElementName,
 										 oElement2ElementPtr);
		ccvAttributeValue = g_wcsData;
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "0";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMinOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "1";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMaxOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oElement2ElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1706;
			break;
		}

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1708;
			break;
		}


		break;
	}

	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaFormatAttribute ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;

	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;



	hr = S_OK;

	while (1)
	{

//-	Set up Format attribute for Revision elements.
//-	  <AttributeType name="Format" dt:type="string"/>

		m_opDOM->CreateElement (m_bstrAttributeDefinitionName, 
									   oAttributeElementPtr);

		ccvAttributeValue = g_wcsFormat;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionStringType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType1Title, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1709;
			break;
		}

		break;
	}

	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaFormatVersionAttribute ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;

	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;



	hr = S_OK;

	while (1)
	{

//-	Set up FormatVersion attribute for Revision elements.
//-	  <AttributeType name="FormatVersion" dt:type="string"/>

		m_opDOM->CreateElement (m_bstrAttributeDefinitionName, 
									   oAttributeElementPtr);

		ccvAttributeValue = "FormatVersion";
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionStringType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType1Title, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1710;
			break;
		}

		break;
	}

	return hr;
}



HRESULT SVXMLSVRSchemaClass::SetSVRSchemaRevisionAttribute ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;

	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;



	hr = S_OK;

	while (1)
	{

//-	Set up Revision attribute for Revision elements.
//-	  <AttributeType name="Revision" dt:type="string"/>

		m_opDOM->CreateElement (m_bstrAttributeDefinitionName, 
									   oAttributeElementPtr);

		ccvAttributeValue = "Revision";
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionStringType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType1Title, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1711;
			break;
		}

		break;
	}

	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaRevisionElement ()
{
	HRESULT	hr;

	SVBStr			bstrElementDefinitionClosedModel;

	CComVariant		ccvAttributeValue;
	
	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oElementElementPtr;
	SVXML::IXMLDOMElementPtr oElement2ElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;


	hr = S_OK;

	while (1)
	{

		hr = bstrElementDefinitionClosedModel.CopyFromWChar (L"closed");
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	Set up Revision element.
//-	  <ElementType name="Revision" content="mixed" model="closed">
//-
//-   content is mixed because I (JAB) sometimes go in and edit the XML file 
//-	for readabillity. When I do this, the white space that I put in (cr, 
//-	lf, tabs, and spaces) are considerred text by the XML parser and an 
//-	error if the schema indicates that there should be elements only.  
//-   Although we do not use text, it is allowed.

		m_opDOM->CreateElement (m_bstrElementDefinitionName, 
										oElementElementPtr);

		ccvAttributeValue = "Revision";
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionMixedContent;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementContentTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionClosedModel;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementModelTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();


//-	Set up Format attribute of the Revision element.
//-	  <attribute type="Format" required="yes"/>

		m_opDOM->CreateElement (m_bstrAttributeElementName, 
									   oAttributeElementPtr);

		ccvAttributeValue = g_wcsFormat;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1712;
			break;
		}

//-	Set up FormatVersion attribute of the Revision element.
//-	  <attribute type="FormatVersion" required="yes"/>

		m_opDOM->CreateElement (m_bstrAttributeElementName, 
										 oAttributeElementPtr);

		ccvAttributeValue = "FormatVersion";
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1713;
			break;
		}

//-	Set up Revision attribute of the Revision element.
//-	  <attribute type="Revision" required="yes"/>

		m_opDOM->CreateElement (m_bstrAttributeElementName, 
										oAttributeElementPtr);

		ccvAttributeValue = "Revision";
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1714;
			break;
		}

//-	Set up DATA child of the Revision element.
//-   May have either 0 or 1 child DATA elements.
//-	  <element type="DATA" minOccurs="0" maxOccurs="1"/>

		m_opDOM->CreateElement (m_bstrElementElementName,
 										oElement2ElementPtr);
		ccvAttributeValue = g_wcsData;
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "0";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMinOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "1";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMaxOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oElement2ElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1715;
			break;
		}

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1716;
			break;
		}

		break;
	}


	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaRevisionHistoryElement ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;
	
//	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oElementElementPtr;
	SVXML::IXMLDOMElementPtr oElement2ElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;




	hr = S_OK;

	while (1)
	{

//-	Set up RevisionHistory element.
//-	  <ElementType name="RevisionHistory" content="mixed" model="closed">
//-
//-   content is mixed because I (JAB) sometimes go in and edit the XML file 
//-	for readabillity. When I do this, the white space that I put in (cr, 
//-	lf, tabs, and spaces) are considerred text by the XML parser and an 
//-	error if the schema indicates that there should be elements only.  
//-   Although we do not use text, it is allowed.

		m_opDOM->CreateElement (m_bstrElementDefinitionName, 
										oElementElementPtr);

		ccvAttributeValue = "RevisionHistory";
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionMixedContent;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementContentTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionClosedModel;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementModelTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();



//-	Set up Revision child of the RevisionHistory element.
//-   Must have at least 1 child Revision elements.
//-	  <element type="Revision" minOccurs="1" maxOccurs="*"/>

		m_opDOM->CreateElement (m_bstrElementElementName,
 										 oElement2ElementPtr);
		ccvAttributeValue = "Revision";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "1";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMinOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "*";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMaxOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oElement2ElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1717;
			break;
		}

	
//-	Set up DATA child of the RevisionHistory element.
//-   May have at 0 or 1 child DATA elements.
//-	  <element type="DATA" minOccurs="0" maxOccurs="1"/>

		m_opDOM->CreateElement (m_bstrElementElementName,
 										 oElement2ElementPtr);
		ccvAttributeValue = g_wcsData;
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "0";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMinOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "1";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMaxOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oElement2ElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1719;
			break;
		}
		
		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1720;
			break;
		}

		break;
	}

	return hr;
}


HRESULT SVXMLSVRSchemaClass::SetSVRSchemaIsActiveAttribute ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;

	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;



	hr = S_OK;

	while (1)
	{

//-	Set up IsActive attribute.
//-	  <AttributeType name="IsActive" dt:type="string"/>

		m_opDOM->CreateElement (m_bstrAttributeDefinitionName, 
									   oAttributeElementPtr);

		ccvAttributeValue = g_wcsIsActive;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionStringType;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType1Title, 
			                                 ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1901;
			break;
		}

		break;
	}

	return hr;
}



HRESULT SVXMLSVRSchemaClass::SetSVRSchemaEncryptionElement ()
{
	HRESULT	hr;

	CComVariant		ccvAttributeValue;
	
	SVXML::IXMLDOMElementPtr oAttributeElementPtr;
	SVXML::IXMLDOMElementPtr oElementElementPtr;
	SVXML::IXMLDOMElementPtr oElement2ElementPtr;
	SVXML::IXMLDOMElementPtr oReturnElementPtr;




	hr = S_OK;

	while (1)
	{

//-	Set up Encryption element.
//-	  <ElementType name="Encryption" content="mixed" model="closed">
//-
//-   content is mixed because I (JAB) sometimes go in and edit the XML file 
//-	for readabillity. When I do this, the white space that I put in (cr, 
//-	lf, tabs, and spaces) are considerred text by the XML parser and an 
//-	error if the schema indicates that there should be elements only.  
//-   Although we do not use text, it is allowed.

		m_opDOM->CreateElement (m_bstrElementDefinitionName, 
										oElementElementPtr);

		ccvAttributeValue = g_wcsEncryption;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementNameTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionMixedContent;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementContentTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrElementDefinitionClosedModel;
		oElementElementPtr->setAttribute ((BSTR) m_bstrElementModelTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();


//-	Set up IsActive attribute of the Encryption element.
//-	  <attribute type="IsActive" required="yes"/>

		m_opDOM->CreateElement (m_bstrAttributeElementName, 
										 oAttributeElementPtr);

		ccvAttributeValue = g_wcsIsActive;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) m_bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) m_bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1902;
			break;
		}


//-	Set up DATA child of the RevisionHistory element.
//-   May have at 0 or 1 child DATA elements.
//-	  <element type="DATA" minOccurs="0" maxOccurs="1"/>

		m_opDOM->CreateElement (m_bstrElementElementName,
 										oElement2ElementPtr);
		ccvAttributeValue = g_wcsData;
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "0";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMinOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = "1";
		oElement2ElementPtr->setAttribute ((BSTR) m_bstrElementMaxOccursTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oElement2ElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1904;
			break;
		}
		
		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (nullptr == oReturnElementPtr)
		{
			hr = -1903;
			break;
		}

		break;
	}

	return hr;
}

