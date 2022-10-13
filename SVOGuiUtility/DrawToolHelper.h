#pragma once

#pragma region Includes
#include "DrawToolHelperStructs.h"
#pragma endregion Includes

namespace SvOgu
{
std::map<long, variant_t> createOverlayData(DrawNodeType type, const ValueController& rValueController);
void setOverlayProperties(DrawNodeType type, ValueController& rValueController, VARIANT* ParameterList, VARIANT* ParameterValue);
}
