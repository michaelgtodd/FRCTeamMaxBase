using Rug.Osc;
using System;
using System.Collections.Generic;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace DriverStation
{

    enum EnableState { Enable = 0, Disable = 1 }

    enum DriveMode { TeleOP = 0, Auto = 1, Practice = 2, Test = 3 }

    public partial class MainWindow : Window
    {
        // The list of mode buttons.
        List<Button> ButtonList;

        // IP and port for sending OSC data.
        IPAddress DestinationIP = IPAddress.Parse("10.0.1.42");
        int SendPort = 55851;

        // OSC sender to use through the program.
        OscSender OscSenderInstance;

        // Robot mode and enabled state.
        EnableState RobotEnableState = EnableState.Disable;
        DriveMode RobotDriveMode = DriveMode.TeleOP;

        // Constructor.
        public MainWindow()
        {

            // Initialize the XAML components.
            InitializeComponent();

            // Initialize the button list.
            ButtonList = new List<Button>() { TeleOpButton, AutoButton, PracticeButton, TestButton };

            // Connect the sender.
            OscSenderInstance = new OscSender(DestinationIP, SendPort);
            OscSenderInstance.Connect();

            // Intervals for the On Tick method.
            var DueTime = TimeSpan.FromSeconds(1);
            var Interval = TimeSpan.FromMilliseconds(20);

            // Start the On Tick process.
#pragma warning disable 4014
            RunPeriodicAsync(OnTick, DueTime, Interval, CancellationToken.None);
#pragma warning restore 4014
        }

        // Event handler for when the mode button is clicked.
        private void ModeButtonClick(object Sender, RoutedEventArgs Args)
        {

            // For each button, unset the border.
            foreach (Button ModeButton in ButtonList)
            {
                ModeButton.BorderBrush = Brushes.DimGray;
            }
            ((Button)Sender).BorderBrush = Brushes.Black;

            // Set the drive mode.
            switch (((Button)Sender).Name)
            {
                case "TeleOpButton":
                    RobotDriveMode = DriveMode.TeleOP;
                    break;
                case "AutoButton":
                    RobotDriveMode = DriveMode.Auto;
                    break;
                case "PracticeButton":
                    RobotDriveMode = DriveMode.Practice;
                    break;
                case "TestButton":
                    RobotDriveMode = DriveMode.Test;
                    break;
            }
        }

        // Event handler for when the enable buttons are clicked.
        private void EnableStateClick(object Sender, RoutedEventArgs Args)
        {

            // Set the border, depending on selection.
            EnableButton.BorderBrush = ((Button)Sender).Name == "EnableButton" ? Brushes.Black : Brushes.DimGray;
            DisableButton.BorderBrush = ((Button)Sender).Name == "EnableButton" ? Brushes.DimGray : Brushes.Black;

            // Set the robot state too.
            RobotEnableState = ((Button)Sender).Name == "EnableButton" ? EnableState.Enable : EnableState.Disable;
        }

        // The `onTick` method will be called periodically unless cancelled.
        // Code referenced from: https://stackoverflow.com/questions/14296644/how-to-execute-a-method-periodically-from-wpf-client-application-using-threading
        private static async Task RunPeriodicAsync(Action OnTick,
                                                   TimeSpan DueTime,
                                                   TimeSpan Interval,
                                                   CancellationToken Token)
        {
            // Initial wait time before we begin the periodic loop.
            if (DueTime > TimeSpan.Zero)
                await Task.Delay(DueTime, Token);

            // Repeat this loop until cancelled.
            while (!Token.IsCancellationRequested)
            {
                // Call our onTick function.
                OnTick?.Invoke();

                // Wait to repeat again.
                if (Interval > TimeSpan.Zero)
                    await Task.Delay(Interval, Token);
            }
        }

        // Method run every tick. Should be 50 ticks per second.
        private void OnTick()
        {

            // Send the enable state and the drive mode.

            OscSenderInstance.Send(new OscMessage("/EnableState", (int)RobotEnableState, (int)RobotDriveMode));
        }
    }
}
