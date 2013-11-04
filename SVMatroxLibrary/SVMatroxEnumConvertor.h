//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxEnumConvertor
//* .File Name       : $Workfile:   SVMatroxEnumConvertor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:13:00  $
//******************************************************************************

#ifndef SVMATROXENUMCONVERTOR_H
#define SVMATROXENUMCONVERTOR_H

#include "SVContainerLibrary/SVBidirectionalMap.h"

/**
@SVObjectName Matrox Enum/Bit Set Collection/Convertor

@SVObjectOverview This Class is used to hold mappings of simple or complex Enums/Bit Sets for the Matrox API.

@SVObjectOperations The ConvertEnumToMatroxType method converts a simple (Enum) SVEnumType to a Matrox Type. 
The ConvertBitSetToMatroxType method converts a complex SVEnumType (BitSet) to a Matrox Type Bit Combination. 
The ConvertEnumFromMatroxType method converts a simple Matrox Type (Enum) to a SVEnumType. 
The ConvertBitSetFromMatroxType method converts a complex Matrox Type (BitSet) to a SVEnumType Bit Combination. 
*/

template<typename SVEnumType, typename MatroxType>
class SVMatroxEnumConvertor
{
	typedef typename SVBidirectionalMap<SVEnumType, MatroxType>::type SVMatroxEnumMap;
	typedef typename SVMatroxEnumMap::iterator iterator;
	SVMatroxEnumMap m_map;

public:
	SVMatroxEnumConvertor();

	template<class _Iter>
	SVMatroxEnumConvertor(_Iter first, _Iter last);
	
	HRESULT ConvertEnumToMatroxType(SVEnumType type, MatroxType& matroxType);
	HRESULT ConvertBitSetToMatroxType(SVEnumType type, MatroxType& matroxType);

	HRESULT ConvertEnumFromMatroxType(MatroxType matroxType, SVEnumType& type);
	HRESULT ConvertBitSetFromMatroxType(MatroxType matroxType, SVEnumType& type);
};

#include "SVMatroxEnumConvertor.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxEnumConvertor.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:13:00   bWalter
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