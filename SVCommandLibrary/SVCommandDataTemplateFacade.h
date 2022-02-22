//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplateFacade
//* .File Name       : $Workfile:   SVCommandDataTemplateFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandDataFacade.h"
#pragma endregion Includes

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType = SVTemplateDataType >
class SVCommandDataTemplateFacade : public SVCommandDataFacade
{
public:
	typedef typename SVTemplateDataType SVDataType;
	typedef typename SVTemplateArgType SVArgType;

	virtual ~SVCommandDataTemplateFacade();

	virtual unsigned long GetDataType() const override;

	virtual HRESULT GetData( SVArgType& p_rData ) const = 0;
	virtual HRESULT SetData( const SVDataType& p_rData ) = 0;

protected:
	SVCommandDataTemplateFacade();
	explicit SVCommandDataTemplateFacade(bool p_ReadOnly);

	const unsigned long m_DataType;

};

#include "SVCommandDataTemplateFacade.inl"

