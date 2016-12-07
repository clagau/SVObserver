//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandResultsTemplate
//* .File Name       : $Workfile:   SVCommandResultsTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandResultsFacade.h"
#pragma endregion Includes

template< typename SVResultCollection >
class SVCommandResultsTemplate : public SVCommandResultsFacade< SVResultCollection > 
{
public:
	SVCommandResultsTemplate();
	SVCommandResultsTemplate( const SVCommandResultsTemplate& p_rObject );

	virtual ~SVCommandResultsTemplate();

	const SVCommandResultsTemplate& operator=( const SVCommandResultsTemplate& p_rObject );

	virtual HRESULT GetResults( SVResultCollection& p_rResults ) const override;

protected:
	SVResultCollection m_Results;

};

#include "SVCommandResultsTemplate.inl"


