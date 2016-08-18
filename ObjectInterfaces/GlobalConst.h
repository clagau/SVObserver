//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file GlobalConst.h
//*****************************************************************************
/// Global constants used in interfaces
//******************************************************************************
#pragma once

namespace Seidenader { namespace ObjectInterfaces
{
	#pragma region Declarations
	const int			cRingBufferDepthMin = 2;
	const int			cRingBufferDepthMax = 30;
	const int			cRingBufferNumberOutputImages = 2;
	const int			cTableMaxRowMin = 1;
	const int			cTableMaxRowMax = 10000;

	#pragma endregion Declarations
} /* namespace ObjectInterfaces */ } /* namespace Seidenader */

namespace SvOi = Seidenader::ObjectInterfaces;
