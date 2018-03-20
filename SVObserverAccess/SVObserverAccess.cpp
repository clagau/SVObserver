//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVObserverAccessDll.cpp
/// All Rights Reserved 
//*****************************************************************************
///Defines the exported functions for the DLL application.
//******************************************************************************

#include "stdafx.h"
#include "SVObserverAccess.h"

#include "SVSharedMemoryLibrary\ShareControl.h"


namespace SvOa
{

std::unique_ptr<SvSml::ShareControl> ShareControlInstance;

void Init()
{
	ShareControlInstance = std::make_unique<SvSml::ShareControl>();
}

void Finish()
{
	ShareControlInstance.release();
}




bool  QueryListItem(const SvPb::QueryListItemRequest& req, SvPb::QueryListItemResponse& resp, SvPenv::Error& err)
{
	if (ShareControlInstance.get())
	{
		return ShareControlInstance->QueryListItem(req, resp, err);
	}
	else
	{
		return false;
	}
}
bool GetProductOrReject(const SvPb::GetProductRequest& request, SvPb::GetProductResponse& resp, SvPenv::Error& err)
{
	if (ShareControlInstance.get())
	{
		return ShareControlInstance->GetProductOrReject(request, resp, err);
	}
	else
	{
		return false;
	}
}
bool GetFailstatus(const SvPb::GetFailStatusRequest& request, SvPb::GetFailStatusResponse& resp, SvPenv::Error& err)
{

	if (ShareControlInstance.get())
	{
		return ShareControlInstance->GetFailstatus(request, resp, err);
	}
	else
	{
		return false;
	}

}
bool  GetImageFromCurId(const  SvPb::GetImageFromCurIdRequest& req, SvPb::GetImageFromCurIdResponse& resp, SvPenv::Error& err)
{

	if (ShareControlInstance.get())
	{
		return ShareControlInstance->GetImageFromCurId(req, resp, err);
	}
	else
	{
		return false;
	}

}

bool QueryListName(const SvPb::QueryListNameRequest& req, SvPb::QueryListNameResponse& resp, SvPenv::Error& err)
{
	if (ShareControlInstance.get())
	{
		return ShareControlInstance->QueryListName(req, resp, err);
	}
	else
	{
		return false;
	}

}

}




