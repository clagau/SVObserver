//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonStreamPacket
//* .File Name       : $Workfile:   SVJsonStreamPacket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   02 Oct 2014 10:40:48  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <sstream>
#pragma endregion Includes

namespace SvSol
{
	class SVJsonStreamPacket
	{
	private:
		std::stringstream m_packet;
		std::basic_string<TCHAR> m_pending;
		int m_count;

	public:
		SVJsonStreamPacket();
	void Add(const std::string& data);
		bool IsComplete() const;
		bool IsValid() const;
		std::istream& GetJsonDataStream();
		void Clear();
		void Reset();
	};
} //namespace SvSol

