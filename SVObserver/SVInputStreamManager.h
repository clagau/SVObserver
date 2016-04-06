//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, INc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputStreamManager
//* .File Name       : $Workfile:   SVInputStreamManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:01:40  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <string>

#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace Json
{
	class Value;
};

class SVInputStreamManager
{
public:
	friend class SVObserverApp;

	static SVInputStreamManager& Instance();

	virtual ~SVInputStreamManager();

	HRESULT QueryInputStreamNames( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	HRESULT QueryInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	HRESULT ClearInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	SVGUID FindInputStream( const std::string& p_rName );

	HRESULT InsertInputStream( const std::string& p_rName, const SVGUID& p_rObjectId );
	HRESULT EraseInputStream( const SVGUID& p_rObjectId );

	HRESULT ProcessJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

protected:
	typedef std::map< std::string, SVGUID > SVInputStreamMap;

	SVInputStreamManager();

	void Startup(unsigned short p_PortNumber);
	void Shutdown();

	HRESULT SendCommandToInputStream( const std::string& p_rName, const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	HRESULT ProcessStreamManagerJsonCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	HRESULT ProcessJsonToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, Json::Value& p_rJson ) const;
	HRESULT ProcessJsonValuesToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, Json::Value& p_rJson ) const;

	HRESULT ConvertJsonToCommandDataHolder( SVCommandDataHolder& p_rDataHolder, const std::string& p_rLabel, const std::string& p_rJson ) const;

	SVJsonCommandServer m_SocketServer;
	SVCriticalSection m_Lock;
	SVInputStreamMap m_InputStreams;

private:
	SVInputStreamManager( const SVInputStreamManager& p_rObject );
	const SVInputStreamManager& operator=( const SVInputStreamManager& p_rObject );
};

