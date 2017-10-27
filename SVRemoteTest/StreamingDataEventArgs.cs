///******************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
///******************************************************************************
/// This class provides the event arguments for the data arrived internal event
///******************************************************************************
using System;
using System.Collections.Generic;
using System.Text;

namespace SVRemoteTest
{
    public class StreamingDataEventArgs : EventArgs
    {
        private readonly List<SVValue> m_values;
        private readonly string m_type;
        public StreamingDataEventArgs(List<SVValue> values, string type)
        {
            m_values = values;
            m_type = type;
        }
        public string EventType { get { return m_type; } }
        public List<SVValue> Values { get { return m_values; } }
    }
}
