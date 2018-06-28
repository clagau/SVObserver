//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAbstractFactorySingleton
//* .File Name       : $Workfile:   SVCommandAbstractFactorySingleton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:08  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandAbstractFactoryTemplate.h"
#pragma endregion Includes

template< typename SVAbstractFactory >
class SVCommandAbstractFactorySingleton
{
public:
	virtual ~SVCommandAbstractFactorySingleton();

	static SVCommandAbstractFactorySingleton& Instance();

	SVCommandAbstractFactoryTemplatePtr GetAbstractFactory() const;

	bool IsValidCommandType(unsigned long CommandType) const;

	SvOi::ICommandPtr CreateCommand(unsigned long CommandType) const;

protected:
	SVCommandAbstractFactoryTemplatePtr m_pAbstractFactory;

	SVCommandAbstractFactorySingleton();

private:
	SVCommandAbstractFactorySingleton( const SVCommandAbstractFactorySingleton& p_rObject );

	const SVCommandAbstractFactorySingleton& operator=( const SVCommandAbstractFactorySingleton& p_rObject );

};

#include "SVCommandAbstractFactorySingleton.inl"

