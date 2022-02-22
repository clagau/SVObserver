//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplate
//* .File Name       : $Workfile:   SVCommandDataTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandDataTemplateFacade.h"
#pragma endregion Includes

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType = SVTemplateDataType >
class SVCommandDataTemplate : public SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >
{
public:
	typedef SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType > SVCommandDataInterface;
	typedef typename SVTemplateDataType SVDataType;
	typedef typename SVTemplateArgType SVArgType;

	SVCommandDataTemplate();
	explicit SVCommandDataTemplate(const SVDataType& p_rValue, bool p_ReadOnly = false);

	virtual ~SVCommandDataTemplate();

	virtual HRESULT GetData( SVArgType& p_rData ) const override;
	virtual HRESULT SetData( const SVDataType& p_rData ) override;

protected:
	SVDataType m_Data;
};

#include "SVCommandDataTemplate.inl"

