//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : StringToEnum
//* .File Name       : $Workfile:   StringToEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:19:54  $
//******************************************************************************

template<typename Type, typename LookupTable, typename StringType>
class StringToEnum
{
public:
	static bool GetEnum(LookupTable table, StringType value, Type& enumValue)
	{
		bool  bRetVal = false;
		LookupTable::const_iterator it = table.find(value);
		if (it != table.end())
		{
			enumValue = it->second;
			bRetVal = true;
		}
		return bRetVal;
	}
};

