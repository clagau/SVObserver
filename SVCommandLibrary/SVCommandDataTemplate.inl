//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplate
//* .File Name       : $Workfile:   SVCommandDataTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:34  $
//******************************************************************************

#ifndef SVCOMMANDDATATEMPLATE_INL
#define SVCOMMANDDATATEMPLATE_INL

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplate< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::SVCommandDataTemplate()
: SVCommandDataInterface(), m_Data()
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplate< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::SVCommandDataTemplate( const SVDataType& p_rData, bool p_ReadOnly )
: SVCommandDataInterface( p_ReadOnly ), m_Data( p_rData )
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplate< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::~SVCommandDataTemplate()
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
HRESULT SVCommandDataTemplate< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::GetData( SVArgType& p_rData ) const
{
	p_rData = m_Data;

	return S_OK;
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
HRESULT SVCommandDataTemplate< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::SetData( const SVDataType& p_rData )
{
	HRESULT l_Status = S_OK;

	if( ( this->m_DataType & SV_COMMAND_DATA_TYPE_READ_ONLY ) == 0 )
	{
		m_Data = p_rData;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

#endif

