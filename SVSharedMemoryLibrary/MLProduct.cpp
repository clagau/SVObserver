//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file MLProduct.cpp
/// All Rights Reserved 
//*****************************************************************************
/// struct holds result data for getProduct or getReject values 
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "MLProduct.h"
#pragma endregion Includes
namespace SvSml
{
	MLProduct::MLProduct()
	{
		ResetProduct();
	};
	void MLProduct::ResetProduct()
	{
		m_status = E_FAIL;
		m_trigger = -1;
		m_triggerRecordMap.clear();
	};
}


