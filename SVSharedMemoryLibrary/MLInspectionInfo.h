/*****************************************************************************/
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLInspectionInfo.h
/// All Rights Reserved 
//*****************************************************************************

///struct with size Information for Imagestore
//******************************************************************************

#pragma once


namespace SvSml
{
	///struct with size Information for Imagestore
	struct MLInspectionInfo
	{
		int StoreIndex{-1};
		std::string PPQName;
		DWORD PPQIndex{0};
	};
	typedef std::unique_ptr<MLInspectionInfo> MLInspectionInfoPointer;
	typedef  std::map<std::string, MLInspectionInfoPointer>  MLInspectionInfoMap; //< map InspectionName  InspectionInfoPtr
} //namespace SvSml
