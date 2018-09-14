//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Form1.cs
//* .File Name       : $Workfile:   MainForm.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.13  $
//* .Check In Date   : $Date:   28 Jan 2015 13:40:24  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using SeidenaderVision;
using SVRemoteControlLib;
using System.IO;
using System.Xml;

namespace SVRemoteTest
{
	public partial class MainForm : Form
	{
		#region Constructor
		public MainForm()
		{
			m_propertiesPage = new PropertiesPage(m_test);
			m_continualResultPage = new ContinualResultForm(m_test);
			InitializeComponent();

		

			AddTabPages();
		}
		#endregion

		#region Data Arrived Delegate and Event
		#endregion

		#region public methods
		#region Populate PopertyGrid
		delegate void PopulatePropertyGridDelegate(Type objectType, MethodInfo method);
		public void PopulatePopertyGrid(Type objectType, MethodInfo method)
		{
			if (InvokeRequired)
			{
				Invoke(new PopulatePropertyGridDelegate(PopulatePopertyGrid), new object[] { objectType, method });
			}
			else
			{
				m_test.SetSelectedMethod(objectType, method);
				if (ConnectTag == method.Name)
				{
					List<SVDynamicProperty> proTree = m_test.DynamicProperties.Properties;
					foreach (SVDynamicProperty property in proTree)
					{
						if (AddressTag == property.Name)
						{
							if (property.Value == null || property.Value.ToString() == String.Empty)
							{
								property.Value = m_defaultAddress;
							}
						}
					}
				}
				m_propertiesPage.RefreshPropertyGrid();
				m_continualResultPage.RefreshPropertyGrid();
			}
		}
		#endregion

		public bool ShouldAllImagesLoaded()
		{
			return (DisplayTabControl.SelectedTab.Text != "Continual Results" || m_continualResultPage.ShouldAllImagesLoad);
		}

		void RememberUserValues(int selectedTabIndex)
		{
			Type objectType = null;
			MethodInfo method = null;

			// Remember Input values by Object Type
			switch (m_selectedTabIndex)
			{
				case 0: // remote commands
					objectType = this.axSVRemoteCtrl1.GetType();
					method = m_remoteCmdForm.SelectedItem;
					break;

	
			}
			if (objectType != null & method != null)
			{
				m_test.RememberInputValues(objectType, method.Name);
			}
			m_test.ClearSelectedMethod();
		}
		#endregion

		#region private methods
		#region SetupInterfaces
		// Execute this in the MainForm Thread
		internal delegate void SetupInterfacesHandler();
		private void SetupInterfaces()
		{
			try
			{
				if (InvokeRequired)
				{
					Invoke(new SetupInterfacesHandler(SetupInterfaces));
				}
			
			}
			catch (Exception e)
			{
				MessageBox.Show(e.Message, "SetupInterfaces Setup Error");
			}
		}
		#endregion

		#region OnNotification
		public delegate void NotificationEventHandler(object p_Sender, AxSVRemoteControlLib._ISVRemoteCtrlEvents_OnNotifyEvent e);

		private void axSVRemoteCtrl1_OnNotification(object sender, AxSVRemoteControlLib._ISVRemoteCtrlEvents_OnNotifyEvent e)
		{
			try
			{
				if (!IsDisposed)
				{
                    if (this.InvokeRequired)
                    {
                        // this gets called from a thread so we have to invoke to get into the Form's thread
                        NotificationEventHandler d = new NotificationEventHandler(axSVRemoteCtrl1_OnNotification);
                        this.BeginInvoke(d, new object[] { sender, e });
                    }
                    else
                    {
						switch (e.type)
						{
							case SVNotificationTypesEnum.LastModified:
								{
									System.DateTime dt = GetLastModifiedDate(e.data.ToString());
									TextBox_LastModified.Text = dt.ToLocalTime().ToString();
									break;
								}
							case SVNotificationTypesEnum.CurrentMode:
								{
									SVRemoteControlLib.SVObserverModes mode = ExtractModeFromXML(e.data.ToString());
									Mode_Label.Text = mode.ToString();
									break;
								}
							case SVNotificationTypesEnum.Connected:
								{
									//put code in for when connected
									m_propertiesPage.ShowComProperties(axSVRemoteCtrl1.GetType(), axSVRemoteCtrl1);
									// Do GetDevicemode here 
									Mode_Label.Text = QueryDeviceMode();
									break;
								}
							case SVNotificationTypesEnum.Disconnected:
								{
									//put code in for when disconnected
									m_propertiesPage.ShowComProperties(axSVRemoteCtrl1.GetType(), axSVRemoteCtrl1);
									Mode_Label.Text = m_unknownMode;
									break;
								}
                            case SVNotificationTypesEnum.MessageNotification:
                                {
                                    SVObserverMessage SVError = ExtractErrorFromXML(e.data.ToString()); ;
                                    label_ErrorNr.Text = SVError.Number.ToString();
                                    label_ErrorType.Text = SVError.Type.ToString();
                                    label_ErrorText.Text = SVError.Text;
                                    break;
                                }

                            case SVNotificationTypesEnum.LoadConfig:
                                {
                                    SVObserverMessage SVError = ExtractErrorFromXML(e.data.ToString()); ;
                                    label_ErrorNr.Text = "";
                                    label_ErrorType.Text = "";
                                    label_ErrorText.Text = SVError.Text;
                                    break;
                                }

                            default:
								{
									// Do nothing.
									break;
								}
						}
					}
				}
			}
			catch (Exception)
			{
			}
		}
		#endregion

		private void AddTabPages()
		{
			m_remoteCmdForm.TopLevel = false;
			m_remoteCmdForm.Parent = RemoteTabPage;
			m_remoteCmdForm.SetCommandList(axSVRemoteCtrl1.GetType());
			m_remoteCmdForm.m_CommandEvent += OnRemoteCommandSelected;
			m_remoteCmdForm.Visible = true;
		

			m_propertiesPage.TopLevel = false;
			m_propertiesPage.Parent = Panel_Properties;
			Panel_Properties.Controls.Add(m_propertiesPage);
			m_propertiesPage.Dock = DockStyle.Fill;
			m_propertiesPage.Visible = true;

			m_continualResultPage.TopLevel = false;
			m_continualResultPage.Parent = Panel_ContinualResults;
			Panel_ContinualResults.Controls.Add(m_continualResultPage);
			m_continualResultPage.Dock = DockStyle.Fill;
			m_continualResultPage.Visible = true;

			setPropertyTabs();

			// wire up delegate for data arrived event (internal event)
		//	m_DataArrivedEvent += OnDataArrivedEventHandler;
		}

	
		private void OnRemoteCommandSelected(object sender, EventArgs e)
		{
			Type objectType = this.axSVRemoteCtrl1.GetType();
			MethodInfo method = m_remoteCmdForm.SelectedItem;
			if (method != null)
			{
				PopulatePopertyGrid(objectType, method);
			}
			// Clear Status
			statusTextBox.Text = string.Empty;
			setPropertyTabs();
		}

	
		private void ExecCommand(Type objectType, object obj, MethodInfo method)
		{
			statusTextBox.Text = "Executing " + method.Name;
			m_test.ExecCommand(objectType, obj, method);

			m_propertiesPage.RefreshPropertyGrid();
			m_continualResultPage.RefreshPropertyGrid();

			if (m_test.HasException)
			{
				// Update Status with exception info
				statusTextBox.Text = m_test.GetExceptionMessage();
			}
			else
			{
				statusTextBox.Text = m_test.ErrorText;
			}
		}

		private void ExecButton_Click(object sender, EventArgs e)
		{
			statusTextBox.Text = "-----";
			statusTextBox.Refresh();

			if (this.tabControl.SelectedTab == this.RemoteTabPage)
			{
				MethodInfo method = m_remoteCmdForm.SelectedItem;
				if (method != null)
				{
					Type objectType = this.axSVRemoteCtrl1.GetType();
					ExecCommand(objectType, axSVRemoteCtrl1, method);
				}

				if (ConnectTag == method.Name || DisconnectTag == method.Name)
				{
					//Show Updated VPAddress
					m_propertiesPage.ShowComProperties(axSVRemoteCtrl1.GetType(), axSVRemoteCtrl1);

					if (ConnectTag == method.Name)
					{
						// Set up Input/Output brokers, FailStatus stream
						//SetupInterfaces();
						Mode_Label.Text = QueryDeviceMode();
					}
					else
					{
						Mode_Label.Text = m_unknownMode;
					}
				}
			}
		}

		private List<string> QueryRejectCondList(string name)
		{
			List<string> list = new List<string>();
			try
			{
				object values = axSVRemoteCtrl1.QueryRejectCondList(name);
				if (null != values)
				{
					String[] items = values as String[];
					if (null != items)
					{
						list.AddRange(items);
					}
				}
			}
			catch (Exception e)
			{
				System.Diagnostics.Trace.WriteLine(e.Message);
			}
			return list;
		}

		private List<string> QueryFailStatusList(string name)
		{
			List<string> list = new List<string>();
			try
			{
				object values = axSVRemoteCtrl1.QueryFailStatusList(name);
				if (null != values)
				{
					String[] items = values as String[];
					if (null != items)
					{
						list.AddRange(items);
					}
				}
			}
			catch (Exception e)
			{
				System.Diagnostics.Trace.WriteLine(e.Message);
			}
			return list;
		}

		private string QueryDeviceMode()
		{
			string modeName = m_unknownMode;
			try
			{
				object value = axSVRemoteCtrl1.GetDeviceMode();
				if (null != value)
				{
					SVRemoteControlLib.SVObserverModes mode = (SVRemoteControlLib.SVObserverModes)value;
					modeName = mode.ToString();
				}
			}
			catch (Exception e)
			{
				System.Diagnostics.Trace.WriteLine(e.Message);
			}
			return modeName;
		}

		private void MainForm_Shown(object sender, EventArgs e)
		{
			m_test.LoadUserEnteredValues();
	
			// Show Properties for SVRemoteControlLib.SVRemoteCtrl
			m_propertiesPage.ShowComProperties(axSVRemoteCtrl1.GetType(), axSVRemoteCtrl1);
		}

		private void tabControl_Selected(object sender, TabControlEventArgs e)
		{
			int selectedTabIndex = e.TabPageIndex;
			if (selectedTabIndex != m_selectedTabIndex)
			{
				RememberUserValues(m_selectedTabIndex);
			}
			m_selectedTabIndex = selectedTabIndex;
			if (e.TabPage == RemoteTabPage)
			{
				Type objectType = this.axSVRemoteCtrl1.GetType();
				MethodInfo method = m_remoteCmdForm.SelectedItem;
				if (method != null)
				{
					PopulatePopertyGrid(objectType, method);
				}
				else
				{
					m_propertiesPage.ClearPropertyGrid();
					m_continualResultPage.ClearPropertyGrid();
				}
				// Show Properties for SVRemoteControlLib.SVRemoteCtrl
				m_propertiesPage.ShowComProperties(axSVRemoteCtrl1.GetType(), axSVRemoteCtrl1);
			}
			setPropertyTabs();
		}

		private void SetComProperty(Type type, object obj, object newValue, PropertyDescriptor propertyDescriptor)
		{
			PropertyInfo property = type.GetProperty(propertyDescriptor.Name, BindingFlags.Public | BindingFlags.Instance | BindingFlags.SetProperty);
			if (property != null)
			{
				object value = newValue;
				// check if Conversion is needed (stupid Properrty Grid always sets values to Type String)
				if (property.PropertyType == typeof(Int32))
				{
					Int32Converter converter = new Int32Converter();
					value = converter.ConvertFromString(newValue.ToString());
				}
				else if (property.PropertyType == typeof(Int16))
				{
					Int16Converter converter = new Int16Converter();
					value = converter.ConvertFromString(newValue.ToString());
				}
				try
				{
					property.SetValue(obj, value, null);
				}
				catch (Exception e)
				{
					System.Diagnostics.Trace.WriteLine(e.Message);
				}
			}
		}

		private void comObjectPropertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
		{
			// check active tab page
			if (tabControl.SelectedTab == RemoteTabPage)
			{
				try
				{
					SetComProperty(axSVRemoteCtrl1.GetType(), axSVRemoteCtrl1, e.ChangedItem.Value, e.ChangedItem.PropertyDescriptor);
					if (ServerAddressTag == e.ChangedItem.PropertyDescriptor.Name || ServerTypeTag == e.ChangedItem.PropertyDescriptor.Name)
					{
						//Show Updated VPAddress
						m_propertiesPage.ShowComProperties(axSVRemoteCtrl1.GetType(), axSVRemoteCtrl1);

						// Set up Input/Output brokers, Fail Status Stream
						//SetupInterfaces();
					}
				}
				catch (Exception exception)
				{
					System.Diagnostics.Trace.WriteLine(exception.Message);
				}
			}
			setPropertyTabs();
		}

		private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
		{
	
			// Get the last Settings
			RememberUserValues(m_selectedTabIndex);

	
			// Persist the User Entered Values
			m_test.SaveUserEnteredValues();
		}

		private static DateTime GetLastModifiedDate(string p_Data)
		{
			DateTime l_Retval = DateTime.Now;

			try
			{
				string l_TimevalStr = "";

				/* assume XML */
				StringReader stream = new StringReader(p_Data);
				XmlReaderSettings settings = new XmlReaderSettings();
				settings.IgnoreComments = true;
				settings.ValidationType = ValidationType.Schema;
				settings.ConformanceLevel = ConformanceLevel.Document;

				XmlReader l_Reader = XmlReader.Create(stream, settings);

				bool bFound = false;
				while (l_Reader.Read() && !bFound)
				{
					if (l_Reader.IsStartElement())
					{
						if (!l_Reader.IsEmptyElement)
						{
							if (MainForm.TimeStampTag == l_Reader.LocalName)
							{
								l_Reader.Read();
								if (l_Reader.NodeType == XmlNodeType.Text)
								{
									l_TimevalStr = l_Reader.ReadContentAsString();
									bFound = true;
								}
							}
						}
					}
				}
				l_Reader.Close();

				if (l_TimevalStr != string.Empty)
				{
					int l_timeVal = Convert.ToInt32(l_TimevalStr); // value of time_t in an ordinary integer
					l_Retval = new System.DateTime(1970, 1, 1).AddSeconds(l_timeVal);
				}
			}
			catch (Exception e)
			{
				MessageBox.Show(e.ToString());
			}
			return l_Retval;
		}

		private static SVObserverMessage ExtractErrorFromXML(string data)
		{
			SVObserverMessage retval = new SVObserverMessage();
			try
			{
				string valStr = "";

				/* assume XML */
				StringReader stream = new StringReader(data);
				XmlReaderSettings settings = new XmlReaderSettings();
				settings.IgnoreComments = true;
				settings.ValidationType = ValidationType.Schema;
				settings.ConformanceLevel = ConformanceLevel.Document;

				XmlReader reader = XmlReader.Create(stream, settings);
				bool bFound = false;
				while (reader.Read() && !bFound)
				{
					if (reader.IsStartElement())
					{
						if (!reader.IsEmptyElement)
						{
							if (ErrorNumberTag == reader.LocalName)
							{
								reader.Read();
								if (reader.NodeType == XmlNodeType.Text)
								{
									valStr = reader.ReadContentAsString();
									if (valStr != string.Empty)
									{
										retval.Number = Convert.ToUInt32(valStr);
									}
								}
							}
							else if (MessageTextTag == reader.LocalName)
							{
								reader.Read();
								if (reader.NodeType == XmlNodeType.Text)
								{
									retval.Text = reader.ReadContentAsString();
								}
							}
							else if (MessageTypeTag == reader.LocalName)
							{
								reader.Read();
								if (reader.NodeType == XmlNodeType.Text)
								{
									valStr = reader.ReadContentAsString();
									if (valStr != string.Empty)
									{
										retval.Type = (SVObserverMessage.MessageType)Convert.ToUInt32(valStr);
									}
								}
							}
						}
					}
				}
				reader.Close();
			}
			catch (Exception e)
			{
				MessageBox.Show(e.ToString());
			}

			return retval;
		}

		private static SVRemoteControlLib.SVObserverModes ExtractModeFromXML(string data)
		{
			SVRemoteControlLib.SVObserverModes retval = SVRemoteControlLib.SVObserverModes.Available;

			try
			{
				string valStr = "";

				/* assume XML */
				StringReader stream = new StringReader(data);
				XmlReaderSettings settings = new XmlReaderSettings();
				settings.IgnoreComments = true;
				settings.ValidationType = ValidationType.Schema;
				settings.ConformanceLevel = ConformanceLevel.Document;

				XmlReader reader = XmlReader.Create(stream, settings);

				bool bFound = false;
				while (reader.Read() && !bFound)
				{
					if (reader.IsStartElement())
					{
						if (!reader.IsEmptyElement)
						{
							if (MainForm.ModeTag == reader.LocalName)
							{
								reader.Read();
								if (reader.NodeType == XmlNodeType.Text)
								{
									valStr = reader.ReadContentAsString();
									bFound = true;
								}
							}
						}
					}
				}
				reader.Close();

				if (valStr != string.Empty)
				{
					int val = Convert.ToInt32(valStr); // value of Mode String
					retval = (SVRemoteControlLib.SVObserverModes)val;
				}
			}
			catch (Exception e)
			{
				MessageBox.Show(e.ToString());
			}
			return retval;
		}

		private void OnSelectedPropertyTab(object sender, TabControlEventArgs e)
		{
			if (e.TabPage == PropertiesPage)
			{
				tabControl.Enabled = true;
				ExecButton.Enabled = true;
				m_continualResultPage.StatusTextEvent -= setStatusText;
				m_continualResultPage.stopCommand();
				m_continualResultPage.ClearImageList();
			}
			else if (e.TabPage == ContinualResultsPage)
			{
				tabControl.Enabled = false;
				ExecButton.Enabled = false;
				MethodInfo method = m_remoteCmdForm.SelectedItem;
				if (method != null)
				{
					Type objectType = this.axSVRemoteCtrl1.GetType();
					m_continualResultPage.setCommand(objectType, axSVRemoteCtrl1, method);
				}
				m_continualResultPage.StatusTextEvent += setStatusText;
			}
		}

		private void setPropertyTabs()
		{
			if (tabControl.SelectedTab == RemoteTabPage && null != m_remoteCmdForm.SelectedItem &&
				 (GetProductTag == m_remoteCmdForm.SelectedItem.Name || GetRejectTag == m_remoteCmdForm.SelectedItem.Name))
			{
				if (!DisplayTabControl.TabPages.Contains(ContinualResultsPage))
				{
					DisplayTabControl.TabPages.Add(ContinualResultsPage);
				}
			}
			else
			{
				if (DisplayTabControl.TabPages.Contains(ContinualResultsPage))
				{
					DisplayTabControl.TabPages.Remove(ContinualResultsPage);
				}
			}
		}

		private void setStatusText(object sender, String text)
		{
			statusTextBox.Text = text;
			statusTextBox.Refresh();
		}
		#endregion

		#region Member Variables
		private CommandForm m_remoteCmdForm = new CommandForm();
		private SVRemoteTestHelper m_test = new SVRemoteTestHelper();
		private PropertiesPage m_propertiesPage = null;
		private ContinualResultForm m_continualResultPage = null;
		private String m_defaultAddress = "localhost";
		private int m_selectedTabIndex = 0;
		private readonly String m_unknownMode = "UnknownMode";
		private readonly String ConnectTag = "Connect";
		private readonly String DisconnectTag = "Disconnect";
		private readonly String AddressTag = "address";
		private readonly String GetProductTag = "GetProduct";
		private readonly String GetRejectTag = "GetReject";
		private readonly String ServerAddressTag = "ServerAddress";
		private readonly String ServerTypeTag = "ServerType";
		public static readonly String ModeTag = "Mode";
		public static readonly String TimeStampTag = "TimeStamp";
		public static readonly String ErrorNumberTag = "ErrorNumber";
		public static readonly String MessageTextTag = "MessageText";
		public static readonly String MessageTypeTag = "MessageType";
		#endregion
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\MainForm.cs_v  $
 * 
 *    Rev 1.13   28 Jan 2015 13:40:24   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to correct an issue with the saved user entered values when changing tabs.
 * Revised to handle FailStatusStream tab.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.12   16 Jan 2015 10:46:44   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Added FailStatusStream tab and form
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.11   28 Oct 2014 14:35:34   ryoho
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 15
 * SCR Title: Implement a notification when the server gets disconnected
 * Checked in by: rYoho; Rob Yoho
 * Change Description: added case for Connected and Disconnected
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.10   28 Oct 2014 11:55:38   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to load and save user entered parameters
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.9   17 Oct 2014 15:34:28   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 17
 * SCR Title: Enhance SVRC Test App for Features for Testing Run/Reject Server
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to call ContinualResultForm.ClearImageList method when switching away from the Continual Result Form.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.8   08 Sep 2014 18:31:42   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Added delegate for PopulatePropertyGrid
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.7   14 Jul 2014 10:51:22   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: add ContinualResultPage and PropertiesPage
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.6   24 Apr 2014 16:17:20   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Revised OnRemoteCommandSelected to check for null MethodInfo.
 * Revised OnInputBrokerCommandSelected to check for null MethodInfo.
 * Revised OnOutputBrokerCommandSelected to check for null MethodInfo.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.5   09 Sep 2013 16:13:22   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 5
 * SCR Title: Add Last Modified Notification Support to SVRemoteControl, SVRemoteTest
 * Checked in by: bWalter; Ben Walter
 * Change Description: Changed XmlReaderSettings in the GetLastModifiedDate method because the XML notification is no longer a fragment.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.4   20 Jun 2013 13:41:00   bWalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 6
 * SCR Title: Removed Unused Methods from COM Interface
 * Checked in by: bWalter; Ben Walter
 * Change Description: Removed Newtonsoft.Json.
 *   Changed ExecButton_Click to set status text to "-----" initially.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.3   06 Jun 2013 12:17:30   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 5
 * SCR Title: Add Last Modified Notification Support to SVRemoteControl, SVRemoteTest
 * Checked in by: bWalter; Ben Walter
 * Change Description: Changed axSVRemoteCtrl1_OnNotification to check notification type.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   02 Jun 2013 11:38:12   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 5
 * SCR Title: Add Last Modified Notification Support to SVRemoteControl, SVRemoteTest
 * Checked in by: bWalter; Ben Walter
 * Change Description: Added subscription to output broker's OnDataArrive event.
 *   Changed axSVRemoteCtrl1_OnDataArrive to OnDataArrive.
 *   Added axSVRemoteCtrl1_OnNotification method.
 *   Added GetLastModifiedDate method.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   14 May 2013 17:38:44   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:10   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/