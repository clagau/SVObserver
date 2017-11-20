///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// This file defines function to get access to data from SVObserverApp
//******************************************************************************
#pragma once

namespace SvOi
{
	/// Return if the application is ready and valid for editing.
	/// \returns bool
	bool isOkToEdit();

	/// Return the source image depth
	/// \returns long
	long GetSourceImageDepth();

	//! Open the File dialog
	//! \param bOpenFileDialog [in] flag for open or save dialog
	//! \param Extension [in] default extension for the file
	//! \param FileName [in][out] initial filename and after selection the selected file
	//! \param Filter [in] file type filter
	//! \param Path [in][out] initial path and result path
	//! \param Title [in] Window title
	/// \returns result of dialog
	INT_PTR OpenSVFileDialog(bool bOpenFileDialog, LPCTSTR Extention, std::string& FileName, DWORD Flags, LPCTSTR Filter, std::string& Path, LPCTSTR Title);

	//! Open the Folder dialog
	//! \param Path [in][out] initial path and result path
	//! \param Title [in] Window title
	/// \returns result of dialog
	INT_PTR OpenSVFolderDialog(std::string& Path, LPCTSTR Title);

	//! Calls a modeless message box
	//! \param rMessage [in] the message to display
	//! \param hCheckEvent [in] Handle to event which will close the dialog
	void CallModelessMessageBox(std::string &rMessage, HANDLE hCheckEvent);
} //namespace SvOi
