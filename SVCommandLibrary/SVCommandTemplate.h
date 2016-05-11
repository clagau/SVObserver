//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandTemplate
//* .File Name       : $Workfile:   SVCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVSharedPtr.h"
#pragma endregion Includes

class SVCommandTemplate
{
public:
	virtual ~SVCommandTemplate();

	unsigned long GetCommandType() const;

	virtual HRESULT Execute() = 0;

protected:
	SVCommandTemplate( unsigned long p_CommandType );

	const unsigned long m_CommandType;

private:
	SVCommandTemplate();
	SVCommandTemplate( const SVCommandTemplate& p_rObject );

	const SVCommandTemplate& operator=( const SVCommandTemplate& p_rObject );

};

typedef SVSharedPtr< SVCommandTemplate > SVCommandTemplatePtr;

