//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplateFacade
//* .File Name       : $Workfile:   SVCommandDataTemplateFacade.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:26  $
//******************************************************************************

#ifndef SVCOMMANDDATATEMPLATEFACADE_INL
#define SVCOMMANDDATATEMPLATEFACADE_INL

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::SVCommandDataTemplateFacade()
: m_DataType( p_BaseDataType )
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::SVCommandDataTemplateFacade( bool p_ReadOnly )
: m_DataType( p_BaseDataType | ( p_ReadOnly ? SV_COMMAND_DATA_TYPE_READ_ONLY : 0 ) )
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::~SVCommandDataTemplateFacade()
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
unsigned long SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::GetDataType() const
{
	return m_DataType;
}

#endif

