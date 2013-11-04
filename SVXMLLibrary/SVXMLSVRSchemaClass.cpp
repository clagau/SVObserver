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
#include "SVXMLSVRSchemaClass.h"

#include "SVStatusLibrary/SVStatusCodes.h"

#include "SVXMLLibraryGlobals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVXMLSVRSchemaClass::SVXMLSVRSchemaClass()
{
	m_opDOM = NULL;
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
		if (p_opDOM == NULL)
		{
			hr = -1900;
			break;
		}

		m_opDOM = p_opDOM;


		hr = m_bstrAttributeDefinitionName.CopyFromWChar (L"AttributeType");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrAttributeNameTitle.CopyFromWChar (L"name");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrAttributeDefinitionStringType.CopyFromWChar (L"string");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrAttributeType1Title.CopyFromWChar (L"dt:type");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementDefinitionName.CopyFromWChar (L"ElementType");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementDefinitionMixedContent.CopyFromWChar (L"mixed");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementNameTitle.CopyFromWChar (L"name");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementContentTitle.CopyFromWChar (L"content");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementDefinitionClosedModel.CopyFromWChar(L"closed");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementModelTitle.CopyFromWChar (L"model");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrAttributeElementName.CopyFromWChar (L"attribute");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}
		hr = m_bstrAttributeType2Title.CopyFromWChar (L"type");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrAttributeDefinitionYesRequired.CopyFromWChar (L"yes");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrAttributeDefinitionNoRequired.CopyFromWChar (L"no");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


		hr = m_bstrAttributeRequiredTitle.CopyFromWChar (L"required");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


		hr = m_bstrElementElementName.CopyFromWChar (L"element");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementMinOccursTitle.CopyFromWChar (L"minOccurs");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_bstrElementMaxOccursTitle.CopyFromWChar (L"maxOccurs");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}




//-	Set up schema header element -
//-	<Schema name="SVR00001" 
//-            xmlns="urn:schemas-microsoft-com:xml-data"
//-            xmlns:dt="urn:schemas-microsoft-com:datatypes">
//-	Creates m_oSchemaBaseElementPtr.
		hr = SetSVRSchemaHeader ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up Name attribute for NODE or DATA elements.
//-	  <AttributeType name="Name" dt:type="string"/>
//-	Appends to m_oSchemaBaseElementPtr.
		hr = SetSVRSchemaNameAttribute ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up Value attribute for NODE or DATA elements.
//-	  <AttributeType name="Value" dt:type="string"/>
//-	Appends to oSchemaBaseElementPtr.
		hr = SetSVRSchemaValueAttribute ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up Type attribute for NODE or DATA elements.
//-	  <AttributeType name="Type" dt:type="string"/>
//-	Appends to oSchemaBaseElementPtr.
		hr = SetSVRSchemaTypeAttribute ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up DATA element.
//-	  <ElementType name="DATA" content="mixed" model="closed">
		hr = SetSVRSchemaDATAElement ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


//-	Set up NODE element.
//-	  <ElementType name="NODE" content="mixed" model="closed">
		hr = SetSVRSchemaNODEElement ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//===========================================================================
//-   Revision History records.
//-
//-	Set up Format attribute for Revision elements.
//-	  <AttributeType name="Format" dt:type="string"/>
		hr = SetSVRSchemaFormatAttribute ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


//-	Set up FormatVersion attribute for Revision elements.
//-	  <AttributeType name="FormatVersion" dt:type="string"/>
		hr = SetSVRSchemaFormatVersionAttribute ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up Revision attribute for Revision elements.
//-	  <AttributeType name="Revision" dt:type="string"/>
		hr = SetSVRSchemaRevisionAttribute ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up Revision element.
//-	  <ElementType name="Revision" content="mixed" model="closed">
		hr = SetSVRSchemaRevisionElement ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up RevisionHistory element.
//-	  <ElementType name="RevisionHistory" content="mixed" model="closed">
		hr = SetSVRSchemaRevisionHistoryElement ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


//-	Set up IsActive attribute.
//-     <AttributeType name="IsActive" dt_type="string"/>
		hr = SetSVRSchemaIsActiveAttribute ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	Set up Encryption element.
//-     <ElementType name="Encryption" content="mixed" model="closed">
//-       <attribute type="IsActive" required="yes"/>
//-       <element type="DATA" minOccurs="0" maxOccurs="1"/>
//-     </ElementType>
		hr = SetSVRSchemaEncryptionElement ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}



//===========================================================================
//-	Attach to Root element.
		hr = m_opDOM->GetRootNode (oDOMRootPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}
	
		oReturnElementPtr = oDOMRootPtr->appendChild (m_oSchemaBaseElementPtr);
		if (oReturnElementPtr == NULL)
		{
			hr = -1700;
			break;
		}

		hr = m_opDOM->ClearNameSpace ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_opDOM->SetNameSpace (bstrNameSpace);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_opDOM->CreateElement (bstrSchemaDefinitionBranchName, 
			       						  m_oSchemaBaseElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		ccvAttributeValue = g_csSVR00001SchemaName;			// Schema name
		m_oSchemaBaseElementPtr->setAttribute ((BSTR) m_bstrAttributeNameTitle, 
			                                    ccvAttributeValue);
		ccvAttributeValue.Clear ();

		hr = bstrAttributeTitle.CopyFromWChar(L"xmlns:dt");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
		{
			hr = -1702;
			break;
		}


/*- WHEN ELEMENT VALUE WAS AN ATTRIBUTE - JAB01/12/05 -----------------------
//-	Set up Value attribute of the DATA element.
//-	  <attribute type="Value" required="yes"/>

		svmopDOM->CreateElement (bstrAttributeElementName, 
										 oAttributeElementPtr);

		ccvAttributeValue = g_csValue;
		oAttributeElementPtr->setAttribute ((BSTR) bstrAttributeType2Title, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		ccvAttributeValue = (BSTR) bstrAttributeDefinitionYesRequired;
		oAttributeElementPtr->setAttribute ((BSTR) bstrAttributeRequiredTitle, ccvAttributeValue);
		ccvAttributeValue.Clear ();

		oReturnElementPtr = oElementElementPtr->appendChild (oAttributeElementPtr);
		if (oReturnElementPtr == NULL)
		{
			hr = -1703;
			break;
		}
*/


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
		if (oReturnElementPtr == NULL)
		{
			hr = -1704;
			break;
		}

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
		{
			hr = -1706;
			break;
		}

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
		{
			hr = -1715;
			break;
		}

		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
		{
			hr = -1719;
			break;
		}
		
		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
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
		if (oReturnElementPtr == NULL)
		{
			hr = -1904;
			break;
		}
		
		oReturnElementPtr = m_oSchemaBaseElementPtr->appendChild (oElementElementPtr);
		if (oReturnElementPtr == NULL)
		{
			hr = -1903;
			break;
		}

		break;
	}

	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLSVRSchemaClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:57:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code is the refacting of the items from the SVLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
