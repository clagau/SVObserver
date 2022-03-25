//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ISVPropertyPageDialog
//* .File Name       : $Workfile:   ISVPropertyPageDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:46  $
//******************************************************************************

#pragma once

namespace SvOg
{
	class ISVPropertyPageDialog
	{
	public:
		virtual ~ISVPropertyPageDialog() {}
		virtual bool QueryAllowExit() = 0;
	};
} //namespace SvOg
