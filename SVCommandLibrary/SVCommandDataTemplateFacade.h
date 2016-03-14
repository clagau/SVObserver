//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplateFacade
//* .File Name       : $Workfile:   SVCommandDataTemplateFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:34  $
//******************************************************************************

#ifndef SVCOMMANDDATATEMPLATEFACADE_H
#define SVCOMMANDDATATEMPLATEFACADE_H

#include "SVCommandDataFacade.h"

/*
*/
template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType = SVTemplateDataType >
class SVCommandDataTemplateFacade : public SVCommandDataFacade
{
public:
	typedef SVTemplateDataType SVDataType;
	typedef SVTemplateArgType SVArgType;

	virtual ~SVCommandDataTemplateFacade();

	virtual unsigned long GetDataType() const;

	virtual HRESULT GetData( SVArgType& p_rData ) const = 0;
	virtual HRESULT SetData( const SVDataType& p_rData ) = 0;

protected:
	SVCommandDataTemplateFacade();
	SVCommandDataTemplateFacade( bool p_ReadOnly );

	const unsigned long m_DataType;

};

#include "SVCommandDataTemplateFacade.inl"

#endif

