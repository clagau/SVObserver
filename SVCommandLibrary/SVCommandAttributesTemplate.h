//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAttributesTemplate
//* .File Name       : $Workfile:   SVCommandAttributesTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandAttributesFacade.h"
#pragma endregion Includes

template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection = SVAttributeCollection >
class SVCommandAttributesTemplate : public SVCommandAttributesFacade< SVAttributeCollection >
{
public:
	typedef SVDefaultCollection SVDefaultAttributes;

	static const SVDefaultAttributes m_DefaultAttributes;

	SVCommandAttributesTemplate();
	SVCommandAttributesTemplate( const SVCommandAttributesTemplate& p_rObject );

	virtual ~SVCommandAttributesTemplate();

	const SVCommandAttributesTemplate& operator=( const SVCommandAttributesTemplate& p_rObject );

	virtual HRESULT GetAttributes( SVAttributeCollection& p_rAttributes ) const override;
	virtual HRESULT SetAttributes( const SVAttributeCollection& p_rAttributes ) override;

protected:
	SVAttributeCollection m_Attributes;

};

#include "SVCommandAttributesTemplate.inl"

