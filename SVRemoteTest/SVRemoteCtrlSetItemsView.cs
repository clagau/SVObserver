//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlSetItemsView
//* .File Name       : $Workfile:   SVRemoteCtrlSetItemsView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   28 Jan 2015 13:37:40  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Design;
using System.Windows.Forms;
using SeidenaderVision;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class SVRemoteCtrlSetItemsView : ISVComObjectView
    {
        #region Member Variables
        private bool m_RunOnce = true;
        private List<SVValue> m_valueItems = new List<SVValue>();
        private List<SVInputImage> m_imageItems = new List<SVInputImage>();
        private List<SVError> m_errorFaultItems = new List<SVError>();
        #endregion

        #region Properties
        [Category("Input Parameters")]
		[DisplayName("Values")]
		[Description("List of Values to Set")]
		public List<SVValue> Values
		{
			get { return m_valueItems; }
			set { m_valueItems = value; }
        }

        [Category("Input Parameters")]
        [DisplayName("Images")]
        [Description("List of Images to Set")]
        public List<SVInputImage> Images
        {
            get { return m_imageItems; }
            set { m_imageItems = value; }
        }

        [Category("Input Parameters")]
        [Description("Run Once")]
        public bool RunOnce 
        {
            get { return m_RunOnce; }
            set { m_RunOnce = value; }
        }

        [Category("Error Results")]
        [DisplayName("Faults")]
        [Description("List of Errors")]
        [ReadOnly(true)]
        public List<SVError> Errors
        {
            get { return m_errorFaultItems; }
            set { m_errorFaultItems = value; }
        }
        #endregion

        #region Interface Implementation
        public void GetPropertyValues(List<object> items)
		{
			// Copy .Net Collection to ISVProductItems...
			SVRemoteControlLib.SVProductItemsClass productItems = new SVRemoteControlLib.SVProductItemsClass();
			foreach (SVValue v in m_valueItems)
			{
				SVRemoteControlLib.SVValueObjectClass vObj = new SVRemoteControlLib.SVValueObjectClass();
				vObj.Name = v.Name;
                if( null == v.Array || v.Array.Count == 0)
                {
                    vObj.Add("");
                }
                else
                {
                    foreach (object o in v.Array)
                    {
                        vObj.Add(o.ToString());
                    }
                }
				productItems.Values.Add(vObj);
			}
            foreach (SVInputImage i in m_imageItems)
            {
                SVRemoteControlLib.SVImageObjectClass vObj = new SVRemoteControlLib.SVImageObjectClass();
                vObj.Name = i.Name;
                if (null != i.Picture)
                {
                    vObj.SetImage(SVPictureConvertor.ImageToIPicture(i.Picture));
                    productItems.Images.Add(vObj);
                }
                else
                {
                    String msg = i.Name + " No Image specified ";
                    throw new Exception(msg);
                }
            }
            items.Add(productItems);
            items.Add(m_RunOnce);
		}

		public void SetOutput(String name, object value)
		{
		}

		public void SetResult(object value)
		{
            m_errorFaultItems.Clear();

            SVRemoteControlLib.ISVProductItems productItems = value as SVRemoteControlLib.ISVProductItems;
            foreach (SVRemoteControlLib.ISVErrorObject errorObj in productItems.Errors)
            {
                SVError e = new SVError();
                e.Name = errorObj.Name;
                e.Status = errorObj.Status;
                // Update Error Collection
                m_errorFaultItems.Add(e);
            }
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlSetItemsView.cs_v  $
 * 
 *    Rev 1.4   28 Jan 2015 13:37:40   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Revsied GetPropertyValues method to check for a null image
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.3   16 Jan 2015 10:42:44   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised the Errors property to be read only
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   04 Nov 2014 16:08:16   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised Category attribute tag to Input Parameters to aid in persistence.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   14 May 2013 17:37:06   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:38   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

