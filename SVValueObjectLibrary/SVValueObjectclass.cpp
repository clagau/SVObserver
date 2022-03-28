#pragma region Includes
#include "stdafx.h"
#include "SVValueObjectclass.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

template <> bool SVValueObjectClass<_variant_t>::CheckMaxMin(_variant_t val ) const
{
	if (m_UseMaxMinValue)
	{
		double temp = (double)val;
		double min = (double)m_MinValue;
		double max = (double)m_MaxValue;
		return(temp >= min && temp <= max);
	}

	return true;
}
template <> bool SVValueObjectClass<SVPoint<double>>::CheckMaxMin(SVPoint<double>) const
{
	return true;
}
template <> bool SVValueObjectClass<SVPoint<long>>::CheckMaxMin(SVPoint<long>) const
{
	return true;
}
}