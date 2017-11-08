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
		MLInspectionInfo():TotalDataSize(0),TotalImageSize(0),StoreIndex(-1),TotalDataCount(0), TotalImageCount(0) {};
		DWORD TotalImageCount; //Number of Images for this Inspection 
		DWORD TotalDataCount ; //Number of Datas for this Inspection
		DWORD TotalImageSize; //<size of all Images including  an const offset per image 
		DWORD TotalDataSize; //<size of all datas 
		DWORD StoreIndex;
		std::string PPQName;
		DWORD PPQIndex;
	};
	typedef std::unique_ptr<MLInspectionInfo> MLInspectionInfoPointer;
	typedef  std::map<std::string, MLInspectionInfoPointer>  MLInspectionInfoMap; //< map InspectionName  InspectionInfoPtr
} //namespace SvSml
