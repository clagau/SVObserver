//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ContinualResultForm
//* .File Name       : $Workfile:   ContinualResult.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   21 Oct 2014 15:38:32  $
//******************************************************************************
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using SeidenaderVision;
using System.Collections;
using System.Windows.Threading;
using System.Diagnostics;

namespace SVRemoteTest
{
	public partial class ContinualResultForm : Form
	{
		#region Public Methods

		#region Constructor
		public ContinualResultForm(SVRemoteTestHelper test)
		{
			m_test = test;
			InitializeComponent();
			ErrorLabel.Text = "";
			timeSpanBox.Text = m_timeSpanInSecond.ToString();
			NumberOfRunsTextBox.Text = m_numberOfRuns.ToString();
			// Hide remote outputs check box
			UseRemoteOutputsCheckBox.Visible = false;
			ResetButton.Visible = false;
		}
		#endregion

		#region setCommand
		public void setCommand(Type objectType, object obj, MethodInfo method)
		{
			m_commandObjectType = objectType;
			m_commandObj = obj;
			m_commandMethod = method;

			if (null != m_commandMethod)
			{
				setControls(m_commandMethod.Name);
			}
			else
			{
				setControls("");
			}
		}
		#endregion

		#region stopCommand
		public void stopCommand()
		{
			if (StartStopButton.Text == "Stop")
			{
				OnStartStopButtonClicked();
			}
		}
		#endregion

		#region Refresh PropertyGrid
		public void RefreshPropertyGrid()
		{
			SVDynamicPropertyCollection properties = m_test.DynamicProperties;
			if (properties != null)
			{
				// set the property list
				ArrayList objs = new ArrayList();
				objs.Add(properties);
				inputPropertyGrid.SelectedObjects = null;
				inputPropertyGrid.SelectedObjects = objs.ToArray();
			}
			else
			{
				inputPropertyGrid.SelectedObjects = null;
				inputPropertyGrid.SelectedObject = m_test.ComPropertiesView;
			}
		}
		#endregion

		#region Clear PropertyGrid
		public void ClearPropertyGrid()
		{
			inputPropertyGrid.SelectedObjects = null;
			inputPropertyGrid.SelectedObject = null;
		}
		#endregion

		#region ClearImageList
		public void ClearImageList()
		{
			if (m_imageList != null)
			{
				m_imageList.Clear();
			}
			try
			{
				ImageSelectionBox.Items.Clear();
				ImageSelectionBox.Visible = false;
				pictureBox1.Image = global::SVRemoteTest.Properties.Resources.Chrysanthemum;
			}
			catch (System.Exception e)
			{
				System.Diagnostics.Trace.WriteLine(e.Message);
			}
		}
		#endregion

		#region SetRejectCondList
		public void SetRejectCondList(List<string> list)
		{
			m_rejectCondList = list;
		}
		#endregion

		#region RemoveRejectCondList
		public void RemoveRejectCondList(List<string> list)
		{
			foreach (string s in list)
			{
				if (m_rejectCondList.Contains(s))
				{
					m_rejectCondList.Remove(s);
				}
			}
		}
		#endregion

		#region CheckeEjects
		private delegate void CheckRejectsDelegate(List<SVValue> values);
		public void CheckRejects(List<SVValue> values)
		{
			//if not GetReject selected, do nothing.
			if (null == m_commandMethod || GetRejectTag != m_commandMethod.Name)
			{
				return;
			}

			if (InvokeRequired)
			{
				CheckRejectsDelegate del = new CheckRejectsDelegate(CheckRejects);
				Invoke(del, new object[] { values });
			}
			else
			{
				bool isRejected = false;
				foreach (SVValue value in values)
				{
					if (UseRemoteOutputs) // use Remote Outputs
					{
						// original way this was done using Remote Outputs
						if (0 != Convert.ToDouble(value.Array[0]) && !value.Name.Contains("Trigger Count"))
						{
							isRejected = true;
							break;
						}
					}
					else // use fail status stream and the reject cond list
					{
						if (m_rejectCondList.Count > 0)
						{
							if (0 != Convert.ToDouble(value.Array[0]) && m_rejectCondList.Contains(value.Name))
							{
								isRejected = true;
								break;
							}
						}
					}
				}
				if (isRejected)
				{
					//set Buttons from down to up
					Button[] rejectButtons = new Button[] { RejectButton5, RejectButton4, RejectButton3, RejectButton2, RejectButton1 };
					for (int i = 0; i + 1 < rejectButtons.Length; ++i)
					{
						if (rejectButtons[i + 1].Visible)
						{
							rejectButtons[i].Text = rejectButtons[i + 1].Text;
							rejectButtons[i].Visible = true;
						}
					}
					rejectButtons.Last().Text = values[0].TriggerCount.ToString();
					rejectButtons.Last().Visible = true;
				}
			}
		}
		#endregion

		#region UseRemoteOutputs Property
		public bool UseRemoteOutputs
		{
			get { return UseRemoteOutputsCheckBox.Checked; }
		}

		public bool ShouldAllImagesLoad
		{
			get { return LoadAllImagesCheckbox.Checked; }
		}
		#endregion

		#region ShowingRejects Property
		public bool ShowingRejects
		{
			get { return (null != m_commandMethod && GetRejectTag == m_commandMethod.Name); }
		}
		#endregion

		#endregion

		#region Private Methods
		#region Exec Command
		private void ExecCommandAndHandleTimerPlusOutput()
		{
			stopTimer();
			m_countMessage++;
			ExecCommand();

			if (IsRunLoopCheckbox.Checked && m_numberOfRuns <= m_countMessage)
			{
				OnStartStopButtonClicked();
			}
			else
			{
				startTimer();
			}
			setErrorLabels();
		}

		private void ExecCommand()
		{
			m_test.ExecCommand(m_commandObjectType, m_commandObj, m_commandMethod);

			if (m_test.HasException)
			{
				SendStatusLastError(m_test.GetExceptionMessage());
				m_countError++;
			}
			else
			{
				try
				{
					m_imageList = (m_test.ComPropertiesView as SVRemoteCtrlGetProductView).ReturnValue.Images;
					SetImagesAndImageControls();
					if (m_imageList.Count == 0)
					{
						SendStatusLastError("No Image found");
						m_countError++;
					}
				}
				catch (Exception e)
				{
					System.Diagnostics.Trace.WriteLine("Exception fetching or displaying image.  " + e.ToString());
					// If there's an exception getting the images or displaying them on the form, count this as an error.
					SendStatusLastError("Error fetching image");
					m_countError++;
				}
			}
		}
		#endregion

		private void setErrorLabels()
		{
			double errorProcent = Convert.ToDouble(m_countError) / Convert.ToDouble(m_countMessage);
			double averageTime = Convert.ToDouble(m_timer.ElapsedMilliseconds) / m_countMessage;
			ErrorLabel.Text = "Error " + errorProcent.ToString("P") + " (" + m_countError + " / " + m_countMessage + "), Average Time = " + averageTime.ToString("F3") + " ms";
			ErrorLabel.Refresh();
		}

		private void startTimer()
		{
			if (m_executeTimer == null)
			{
				m_executeTimer = new DispatcherTimer();
				m_executeTimer.Tick += timer1_Tick;
			}
			m_executeTimer.Interval = TimeSpan.FromSeconds(m_timeSpanInSecond);
			m_executeTimer.Start();
		}

		private void stopTimer()
		{
			if (m_executeTimer != null)
			{
				m_executeTimer.Stop();
			}
		}

		delegate void timerTickDelegate(object sender, EventArgs e);
		private void timer1_Tick(object sender, EventArgs e)
		{
			if (InvokeRequired)
			{
				Invoke(new timerTickDelegate(timer1_Tick), new object[] { sender, e });
			}
			else
			{
				ExecCommandAndHandleTimerPlusOutput();
			}
		}

		private void StartStopButton_Click(object sender, EventArgs e)
		{
			OnStartStopButtonClicked();
		}

		private void OnStartStopButtonClicked()
		{
			if (StartStopButton.Text == "Start")
			{
				SendStatusTextEvent("Polling monitor list...");
				StartStopButton.Text = "Stop";
				m_countMessage = 0;
				m_countError = 0;
				if (IsRunLoopCheckbox.Checked && m_numberOfRuns > 0)
				{
					ResetButton.Enabled = false;
					NumberOfRunsTextBox.Enabled = false;
					IsRunLoopCheckbox.Enabled = false;
					m_timeSpanInSecond = 0;
				}
				else
				{
					ResetButton.Enabled = true;
				}
				m_timer.Restart();
				ExecCommandAndHandleTimerPlusOutput();
			}
			else
			{
				stopTimer();
				StartStopButton.Text = "Start";
				m_timer.Stop();
				ResetButton.Enabled = false;
				IsRunLoopCheckbox.Enabled = true;
				NumberOfRunsTextBox.Enabled = IsRunLoopCheckbox.Checked;
				timerBoxValueChanged(this, null);
			}
		}


		private void ResetButton_Click(object sender, EventArgs e)
		{
			m_countMessage = 0;
			m_countError = 0;
			m_timer.Restart();
		}

		private void SendStatusTextEvent(String text)
		{
			if (null != StatusTextEvent)
			{
				StatusTextEvent(this, text);
			}
		}

		private void SendStatusLastError(String text)
		{
			SendStatusTextEvent("Last error: " + text);
		}

		/// <summary>
		/// This method may cause an exception.
		/// </summary>
		private void SetImagesAndImageControls()
		{
			if (m_imageList.Count != ImageSelectionBox.Items.Count)
			{
				// If the number items in the selection box is wrong, fix it.
				PopulateSelectionBox();
			}
			else
			{
				// Make sure the names in the selection box still match.
				foreach (SVImage i in m_imageList)
				{
					if (!ImageSelectionBox.Items.Contains(i.Name))
					{
						PopulateSelectionBox();
						break;
					}
				}
			}

			if (m_imageList.Count == 0)
			{
				ImageSelectionBox.Visible = false;
			}
			else
			{
				ImageSelectionBox.Visible = true;
				if (ImageSelectionBox.SelectedIndex == -1)
				{
					ImageSelectionBox.SelectedIndex = 0;
				}

				if (ImageSelectionBox.SelectedItem is string)
				{
					pictureBox1.Image = (m_test.ComPropertiesView as SVRemoteCtrlGetProductView).GetPicture((string)ImageSelectionBox.SelectedItem).Picture;
				}
			}
		}

		private void PopulateSelectionBox()
		{
			ImageSelectionBox.Items.Clear();

			foreach (SVImage i in m_imageList)
			{
				ImageSelectionBox.Items.Add(i.Name);
			}
		}

		private void ImageSelectionBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			try
			{
				SetImagesAndImageControls();
			}
			catch (Exception x)
			{
				System.Diagnostics.Trace.WriteLine("Error changing image selection:  " + x.ToString());
				// If there's an exception getting the images or displaying them on the form, count this as an error.
				SendStatusLastError("Error changing image selection");
			}
		}

		private void timerBoxValueChanged(object sender, EventArgs e)
		{
			try
			{
				m_timeSpanInSecond = Convert.ToDouble(timeSpanBox.Text);
			}
			catch
			{
				timeSpanBox.Text = m_timeSpanInSecond.ToString();
			}
		}

		private void NumberOfRunsTextBox_TextChanged(object sender, EventArgs e)
		{
			try
			{
				m_numberOfRuns = Convert.ToDouble(NumberOfRunsTextBox.Text);
			}
			catch
			{
				NumberOfRunsTextBox.Text = m_numberOfRuns.ToString();
			}
		}

		private void IsRunLoopCheckbox_CheckedChanged(object sender, EventArgs e)
		{
			NumberOfRunsTextBox.Enabled = IsRunLoopCheckbox.Checked;
			timeSpanBox.Enabled = !IsRunLoopCheckbox.Checked;
		}

		//************************************
		// Method:    RejectButton_Click
		// Description:  If the reject button click, the reject Command will be send with the proper trigger count.
		// Parameter: object sender The button which was clicked
		// Parameter: EventArgs e
		// Returns:   void
		//************************************
		private void RejectButton_Click(object sender, EventArgs e)
		{
			Button button = sender as Button;
			int triggerCount = Convert.ToInt32(button.Text);
			(m_test.ComPropertiesView as SVRemoteCtrlGetProductView).ProductID = triggerCount;
			inputPropertyGrid.Refresh();

			ExecCommand();
		}

		private delegate void setControlDelegate(string commandName);
		private void setControls(string commandName)
		{
			if (InvokeRequired)
			{
				setControlDelegate del = new setControlDelegate(setControls);
				Invoke(del, new object[] { commandName });
			}
			else
			{
				if (GetRejectTag == commandName)
				{
					labelTimeSpan.Visible = false;
					timeSpanBox.Visible = false;
					IsRunLoopCheckbox.Visible = false;
					NumberOfRunsTextBox.Visible = false;
					ErrorLabel.Visible = false;

					StartStopButton.Visible = false;
					ResetButton.Visible = false;
					UseRemoteOutputsCheckBox.Visible = true;
					LoadAllImagesCheckbox.Visible = false;
				}
				else if (GetProductTag == commandName)
				{
					RejectButton1.Visible = false;
					RejectButton2.Visible = false;
					RejectButton3.Visible = false;
					RejectButton4.Visible = false;
					RejectButton5.Visible = false;

					labelTimeSpan.Visible = true;
					timeSpanBox.Visible = true;
					IsRunLoopCheckbox.Visible = true;
					NumberOfRunsTextBox.Visible = true;
					NumberOfRunsTextBox.Enabled = IsRunLoopCheckbox.Checked;
					timeSpanBox.Enabled = !IsRunLoopCheckbox.Checked;
					ErrorLabel.Visible = true;
					StartStopButton.Visible = true;
					ResetButton.Visible = true;
					ResetButton.Enabled = false;
					UseRemoteOutputsCheckBox.Visible = false;
					LoadAllImagesCheckbox.Visible = true;
				}
				else
				{
					RejectButton1.Visible = false;
					RejectButton2.Visible = false;
					RejectButton3.Visible = false;
					RejectButton4.Visible = false;
					RejectButton5.Visible = false;

					labelTimeSpan.Visible = false;
					timeSpanBox.Visible = false;
					IsRunLoopCheckbox.Visible = false;
					NumberOfRunsTextBox.Visible = false;
					ErrorLabel.Visible = false;
					UseRemoteOutputsCheckBox.Visible = false;
					LoadAllImagesCheckbox.Visible = false;
					ResetButton.Visible = false;
				}
			}
		}
		#endregion

		#region StatusText delegate and event
		public delegate void StatusTextEventHandler(object sender, String text);

		// Declare the event.
		public event StatusTextEventHandler StatusTextEvent;
		#endregion

		#region Member Variables
		private SVRemoteTestHelper m_test = null;
		private Type m_commandObjectType = null;
		private object m_commandObj = null;
		private MethodInfo m_commandMethod = null;
		private DispatcherTimer m_executeTimer = null;
		private Stopwatch m_timer = new Stopwatch();
		private long m_countMessage = 0;
		private long m_countError = 0;
		private List<SVImage> m_imageList = null;
		private double m_timeSpanInSecond = 0.001;
		private double m_numberOfRuns = 10;
		private List<string> m_rejectCondList = new List<string>();
		private readonly string GetRejectTag = "GetReject";
		private readonly string GetProductTag = "GetProduct";
		#endregion
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\ContinualResult.cs_v  $
 * 
 *    Rev 1.5   21 Oct 2014 15:38:32   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 17
 * SCR Title: Enhance SVRC Test App for Features for Testing Run/Reject Server
 * Checked in by: sJones; Steve Jones
 * Change Description: Revsied ClearImageList to check for null object and do exception handling
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.4   20 Oct 2014 12:24:44   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 17
 * SCR Title: Enhance SVRC Test App for Features for Testing Run/Reject Server
 * Checked in by: sJones; Steve Jones
 * Change Description: Revsied ClearImageList to reset the picture display
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.3   17 Oct 2014 15:31:54   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 17
 * SCR Title: Enhance SVRC Test App for Features for Testing Run/Reject Server
 * Checked in by: sJones; Steve Jones
 * Change Description: Added ClearImageList method
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   08 Sep 2014 18:33:44   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Removed ImageLabel and Lablel1
 * Added logic to handle changes in the image list 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   05 Sep 2014 15:23:08   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Changed Continual Results form to only fetch the currently selected image.
 * Added try/catch in ExecCommand method.
 * Changed SetImagesAndImageControls method to call GetPicture.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   14 Jul 2014 10:29:16   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: initilized check in
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
