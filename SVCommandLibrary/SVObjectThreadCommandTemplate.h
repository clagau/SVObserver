//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectThreadCommandTemplate
//* .File Name       : $Workfile:   SVObjectThreadCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:01:08  $
//******************************************************************************

#pragma once

#include "ObjectInterfaces/ICommand.h"

template<typename CommandPtr>
class SVObjectThreadCommandTemplate : public SvOi::ICommand
{
public:
	SVObjectThreadCommandTemplate(const CommandPtr& rpCommand);

	virtual ~SVObjectThreadCommandTemplate();

	virtual unsigned long GetCommandType() const override {return 1UL;}
	virtual HRESULT Execute() override;

private:
	CommandPtr m_pCommand;

};

#include "SVObjectThreadCommandTemplate.inl"

