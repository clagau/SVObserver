//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file ProgressCallback.h
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Progress callback class which is used by the progress interface
//******************************************************************************

#pragma once

#include <string>

class ProgressCallback
{
public:

	/*
	Called at beginning
	*/
	virtual void OnStartWithTotal(std::string filePath, unsigned __int64 totalBytes) {}

	/*
	Called Whenever progress has updated with a bytes complete
	*/
	virtual void OnProgress(std::string filePath, unsigned __int64 bytesCompleted) {}


	/*
	Called When progress has reached 100%
	*/
	virtual void OnDone(std::string filePath) {}

	/*
	Called When single file progress has reached 100%, returns the filepath that completed
	*/
	virtual void OnFileDone(std::string filePath, unsigned __int64 bytesCompleted) {}
};
