//*****************************************************************************
/// \copyright (c) 2021,2021 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file ObjectIDParameters.h
/// The structure containing the ObjectID parameters required for triggering
//******************************************************************************
#pragma once

namespace SvTrig
{
struct ObjectIDParameters
{
	long m_startObjectID {0L};
	long m_triggerPerObjectID {1L};
	long m_objectIDCount {-1L};
	mutable long m_currentObjectID {0L};
};
} //namespace SvOlib