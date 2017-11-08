//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file VersionString.h 
/// All Rights Reserved 
//*****************************************************************************

///Class to retrieve version string avoid unnecessary calls of BuildVersionString
//******************************************************************************
#pragma once


//!Class to retrieve version string avoid unnecessary calls of BuildVersionString
class RRSVersionString
{
public: 
  ///Retrieve the version string 
	static  std::string Get();

private: 
	//do not implement
	RRSVersionString();
	//do not implement
	RRSVersionString(RRSVersionString& ref);
	//Calculate Version string  from module name 
	static std::string BuildVersionString();
};


