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

#pragma once

/**
@SVObjectName Matrox Enum/Bit Set Collection/Convertor

@SVObjectOverview This Class is used to hold mappings of simple or complex Enums/Bit Sets for the Matrox API.

@SVObjectOperations The ConvertEnumToMatroxType method converts a simple (Enum) SVEnumType to a Matrox Type. 
The ConvertBitSetToMatroxType method converts a complex SVEnumType (BitSet) to a Matrox Type Bit Combination. 
The ConvertEnumFromMatroxType method converts a simple Matrox Type (Enum) to a SVEnumType. 
The ConvertBitSetFromMatroxType method converts a complex Matrox Type (BitSet) to a SVEnumType Bit Combination. 
*/

template<typename SVEnumType>
HRESULT ConvertEnumToMatroxType(const std::vector<std::pair<SVEnumType, long long>>& rEnumPairList, SVEnumType type, long long& rMatroxType);
template<typename SVEnumType>
HRESULT ConvertBitSetToMatroxType(const std::vector<std::pair<SVEnumType, long long>>& rEnumPairList, SVEnumType type, long long& rMatroxType);

template<typename SVEnumType>
HRESULT ConvertEnumFromMatroxType(const std::vector<std::pair<SVEnumType, long long>>& rEnumPairList, long long MatroxType, SVEnumType& type);

#include "SVMatroxEnumConvertor.inl"

