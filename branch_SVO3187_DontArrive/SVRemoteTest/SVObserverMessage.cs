using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SVRemoteTest
{
    class SVObserverMessage
    {
        public enum MessageType { MsgUknown = 0, StartMsgBox = 1,  EndMsgBox = 2 , MsgLog = 3 };

        private String      text;
        private MessageType type;
        private UInt32 number;

        public SVObserverMessage()
        {
            type = MessageType.MsgUknown;
            text = "";
            number = UInt32.MaxValue;
        }
        public SVObserverMessage(string Text, MessageType Type, UInt32  Number)
        {
            type = Type;
            text = Text;
            number = Number;
        }


        public String Text
        {
            get { return text; }
            set { text = value; }
        }

        public UInt32 Number
        {
            get { return number; }
            set { number = value; }
        }

        public MessageType Type
        {
            get { return type; }
            set { type = value; }
        }
    }
}
