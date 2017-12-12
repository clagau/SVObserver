//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataFacade
//* .File Name       : $Workfile:   SVCommandDataFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:30  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

class SVCommandDataFacade
{
public:
	virtual ~SVCommandDataFacade();

	virtual unsigned long GetDataType() const = 0;

protected:
	SVCommandDataFacade();

private:
	SVCommandDataFacade( const SVCommandDataFacade& p_rObject );

	const SVCommandDataFacade& operator=( const SVCommandDataFacade& p_rObject );

};

typedef std::shared_ptr< SVCommandDataFacade > SVCommandDataFacadePtr;

