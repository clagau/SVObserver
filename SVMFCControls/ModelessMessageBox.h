// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CModelessMsgBox
// * .File Name       : $Workfile:   ModelessMessageBox.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:12:36  $
// ******************************************************************************

#pragma once


/// A wrapper fuction to call a modeless messagebox that displays Message in a modeless Message Box
/// this wrapper function was created to allow the inclusion of this header in a project that does not use MFC
void CallModelessMessageBox(SVString &Message, HANDLE hCheckEvent);
//@TODO [Arvid][7.50][10.05.2017] CallModelessMessageBox is used only in two places in the source code
//@TODO [Arvid][7.50][10.05.2017] At some point in time, CModelessMsgBox should be removed altogether and a different modeless message box be used instead
