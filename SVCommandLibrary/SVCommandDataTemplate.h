//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplate
//* .File Name       : $Workfile:   SVCommandDataTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:34  $
//******************************************************************************

#ifndef SVCOMMANDDATATEMPLATE_H
#define SVCOMMANDDATATEMPLATE_H

#include "SVCommandDataTemplateFacade.h"

/*
*/
template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType = SVTemplateDataType >
class SVCommandDataTemplate : public SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >
{
public:
	typedef SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType > SVCommandDataInterface;

	SVCommandDataTemplate();
	SVCommandDataTemplate( const SVDataType& p_rValue, bool p_ReadOnly = false );

	virtual ~SVCommandDataTemplate();

	virtual HRESULT GetData( SVArgType& p_rData ) const;
	virtual HRESULT SetData( const SVDataType& p_rData );

protected:
	SVDataType m_Data;

};

#include "SVCommandDataTemplate.inl"

#endif

