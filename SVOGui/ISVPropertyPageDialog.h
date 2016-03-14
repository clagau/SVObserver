//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ISVPropertyPageDialog
//* .File Name       : $Workfile:   ISVPropertyPageDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:46  $
//******************************************************************************

#pragma once

namespace Seidenader {	namespace SVOGui	{
	class ISVPropertyPageDialog
	{
	public:
		virtual bool QueryAllowExit() = 0;
	};
}}  //namespace Seidenader::SVOGui

namespace SvOg = Seidenader::SVOGui;

