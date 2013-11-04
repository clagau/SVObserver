//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxEnumConvertor
//* .File Name       : $Workfile:   SVMatroxEnumConvertor.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:13:06  $
//******************************************************************************

#include "SVMatroxEnumConvertor.h"
#include "SVMatroxErrorEnum.h"

template<typename SVEnumType, typename MatroxType>
SVMatroxEnumConvertor<SVEnumType, MatroxType>::SVMatroxEnumConvertor()
{
}

template<typename SVEnumType, typename MatroxType>
template<class _Iter>
SVMatroxEnumConvertor<SVEnumType, MatroxType>::SVMatroxEnumConvertor(_Iter first, _Iter last)
{
	for (; first != last; ++first)
		m_map.insert(*first);
}

template<typename SVEnumType, typename MatroxType>
HRESULT SVMatroxEnumConvertor<SVEnumType, MatroxType>::ConvertEnumToMatroxType(SVEnumType type, MatroxType& matroxType)
{
	HRESULT hr(SVMEE_INTERNAL_CONVERSION_ERROR);

	SVMatroxEnumConvertor<SVEnumType, MatroxType>::SVMatroxEnumMap::const_iterator it = m_map.get<from>().find(type);
	if (it != m_map.end())
	{
		hr = S_OK;
		matroxType = it->second;
	}
	return hr;
}

template<typename SVEnumType, typename MatroxType>
HRESULT SVMatroxEnumConvertor<SVEnumType, MatroxType>::ConvertBitSetToMatroxType(SVEnumType type, MatroxType& matroxType)
{
	HRESULT hr(SVMEE_INTERNAL_CONVERSION_ERROR);

	matroxType = 0;

	// iterate over set of enums as bit set and apply conversion
	SVMatroxEnumConvertor<SVEnumType, MatroxType>::SVMatroxEnumMap::const_iterator it;
	for (it = m_map.get<from>().begin(); it != m_map.get<from>().end();++it)
	{
		if ((type & it->first) == it->first)
		{
			matroxType |= it->second;
			hr = S_OK;
		}
	}
	return hr;
}

template<typename SVEnumType, typename MatroxType>
HRESULT SVMatroxEnumConvertor<SVEnumType, MatroxType>::ConvertEnumFromMatroxType(MatroxType matroxType, SVEnumType& type)
{
	HRESULT hr(SVMEE_INTERNAL_CONVERSION_ERROR);

	typedef typename SVMatroxEnumConvertor<SVEnumType, MatroxType>::SVMatroxEnumMap::index_const_iterator<to>::type Iterator;
	Iterator it = m_map.get<to>().find(type);
	if (it != m_map.get<to>().end())
	{
		hr = S_OK;
		type = it->first;
	}
	return hr;
}

template<typename SVEnumType, typename MatroxType>
HRESULT SVMatroxEnumConvertor<SVEnumType, MatroxType>::ConvertBitSetFromMatroxType(MatroxType matroxType, SVEnumType& type)
{
	HRESULT hr(SVMEE_INTERNAL_CONVERSION_ERROR);

	type = 0;

	// iterate over set of enums as bit set and apply conversion
	typedef typename SVMatroxEnumConvertor<SVEnumType, MatroxType>::SVMatroxEnumMap::index_const_iterator<to>::type Iterator;
	Iterator it = m_map.get<to>().find(type);
	for (it = m_map.get<to>().begin(); it != m_map.get<to>().end();++it)
	{
		if ((matroxType & it->second) == it->second)
		{
			type |= it->first;
			hr = S_OK;
		}
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxEnumConvertor.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:13:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:34:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/