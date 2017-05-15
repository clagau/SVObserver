///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// This file defines function to get access to data from SVObserverApp
//******************************************************************************
#pragma once

namespace SvOi
{
	//************************************
	/// Return if the application has unrestricted file access.
	/// \returns bool
	//************************************
	bool isUnrestrictedFileAccess();

	//************************************
	/// Return if the application is ready and valid for editing.
	/// \returns bool
	//************************************
	bool isOkToEdit();

	//************************************
	/// Return the source image depth
	/// \returns long
	//************************************
	long GetSourceImageDepth();
} //namespace SvOi
