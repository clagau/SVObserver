//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLSVRSchemaClass
//* .File Name       : $Workfile:   SVXMLSVRSchemaClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:57:28  $
//******************************************************************************

#ifndef SVXMLSVRSCHEMACLASS_H
#define SVXMLSVRSCHEMACLASS_H

#include "SVDOM.h"		//	SVXML, SVBStr, SVDOMClass

// SVXMLSVRSchemaClass ------------------------------------------------------
// This class should be used only within the SVXMLClass for the purpose of 
// generating the schema for the XML document.  The purpose of this class 
// is only to organize a large amount of tedious code.
class SVXMLSVRSchemaClass  
{
public:
	SVXMLSVRSchemaClass();
	virtual ~SVXMLSVRSchemaClass();

	HRESULT	SetSVRSchema (SVDOMClass*	p_opDOM);


protected:

	SVXML::IXMLDOMElementPtr	m_oSchemaBaseElementPtr;

	SVBStr							m_bstrAttributeDefinitionName;
	SVBStr							m_bstrAttributeDefinitionStringType;
	SVBStr							m_bstrAttributeDefinitionNoRequired;
	SVBStr							m_bstrAttributeDefinitionYesRequired;
	SVBStr							m_bstrAttributeElementName;
	SVBStr							m_bstrAttributeNameTitle;
	SVBStr							m_bstrAttributeRequiredTitle;
	SVBStr							m_bstrAttributeType1Title;
	SVBStr							m_bstrAttributeType2Title;
	SVBStr							m_bstrElementContentTitle;
	SVBStr							m_bstrElementDefinitionClosedModel;
	SVBStr							m_bstrElementDefinitionMixedContent;
	SVBStr							m_bstrElementDefinitionName;
	SVBStr							m_bstrElementElementName;
	SVBStr							m_bstrElementMaxOccursTitle;
	SVBStr							m_bstrElementMinOccursTitle;
	SVBStr							m_bstrElementModelTitle;
	SVBStr							m_bstrElementNameTitle;
	



	SVDOMClass*						m_opDOM;

// SetSVRSchemaHeader () ----------------------------------------------------
// This function may only be called by SetSVRSchema ().
// This function will create m_oSchemaBaseElementPtrl.
	HRESULT	SetSVRSchemaHeader ();

// SetSVRSchemaNameAttribute () ---------------------------------------------
// This function may only be called by SetSVRSchema ().
// This function will append to m_oSchemaBaseElementPtrl.
	HRESULT	SetSVRSchemaNameAttribute ();

// SetSVRSchemaValueAttribute () --------------------------------------------
// This function may only be called by SetSVRSchema ().
// This function will append to m_oSchemaBaseElementPtrl.
	HRESULT	SetSVRSchemaValueAttribute ();

// SetSVRSchemaTypeAttribute () ---------------------------------------------
// This function may only be called by SetSVRSchema ().
// This function will append to m_oSchemaBaseElementPtrl.
	HRESULT	SetSVRSchemaTypeAttribute ();

// SetSVRSchemaDATAElement () -----------------------------------------------
// This function may only be called by SetSVRSchema ().
	HRESULT	SetSVRSchemaDATAElement ();

// SetSVRSchemaNODEElement () -----------------------------------------------
// This function may only be called by SetSVRSchema ().
	HRESULT	SetSVRSchemaNODEElement ();

// SetSVRSchemaFormatAttribute () -------------------------------------------
// This function may only be called by SetSVRSchema ().
	HRESULT	SetSVRSchemaFormatAttribute ();

// SetSVRSchemaFormatVersionAttribute () ------------------------------------
// This function may only be called by SetSVRSchema ().
	HRESULT	SetSVRSchemaFormatVersionAttribute ();

// SetSVRSchemaRevisionAttribute () -----------------------------------------
// This function may only be called by SetSVRSchema ().
	HRESULT	SetSVRSchemaRevisionAttribute ();

// SetSVRSchemaRevisionElement () -------------------------------------------
// This function may only be called by SetSVRSchema ().
	HRESULT	SetSVRSchemaRevisionElement ();

// SetSVRSchemaRevisionHistoryElement () ------------------------------------
// This function may only be called by SetSVRSchema ().
	HRESULT	SetSVRSchemaRevisionHistoryElement ();

	HRESULT  SetSVRSchemaIsActiveAttribute ();

	HRESULT	SetSVRSchemaEncryptionElement ();



};

#endif // !defined(AFX_SVXMLSVRSCHEMACLASS_H__D0F4B83A_1F86_4F26_B769_74C6B866CED3__INCLUDED_)

