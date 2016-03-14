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

#ifndef SVCOMMANDABSTRACTFACTORYSINGLETON_H
#define SVCOMMANDABSTRACTFACTORYSINGLETON_H

#include "SVCommandAbstractFactoryTemplate.h"

/*
*/
template< typename SVAbstractFactory >
class SVCommandAbstractFactorySingleton
{
public:
	virtual ~SVCommandAbstractFactorySingleton();

	static SVCommandAbstractFactorySingleton& Instance();

	SVCommandAbstractFactoryTemplatePtr GetAbstractFactory() const;

	bool IsValidCommandType( unsigned long p_CommandType ) const;

	SVCommandTemplatePtr CreateCommand( unsigned long p_CommandType ) const;

protected:
	SVCommandAbstractFactoryTemplatePtr m_pAbstractFactory;

	SVCommandAbstractFactorySingleton();

private:
	SVCommandAbstractFactorySingleton( const SVCommandAbstractFactorySingleton& p_rObject );

	const SVCommandAbstractFactorySingleton& operator=( const SVCommandAbstractFactorySingleton& p_rObject );

};

#include "SVCommandAbstractFactorySingleton.inl"

#endif

