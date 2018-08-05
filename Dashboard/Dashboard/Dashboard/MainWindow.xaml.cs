using Rug.Osc;
using System;
using System.Collections.ObjectModel;
using System.Threading;
using System.Windows;
using System.Windows.Data;

namespace Dashboard
{

    public partial class MainWindow : Window
    {
        // Constants, such as ports.
        const int SendPortNumber = 5801;
        const int ReceivePortNumber = 5802;

        // OSC relevant variables.
        static OscReceiver Receiver;
        static Thread OscThread;

        // OSC data class for the data grid.
        public class OscData
        {
            public string Path { get; set; }
            public string Data { get; set; }
        }

        // Collection of received packets.
        static ObservableCollection<OscData> ReceivedOscData = new ObservableCollection<OscData>();

        // Constructor for the main dashboard window.
        public MainWindow()
        {

            // Initializes components in the XAML.
            InitializeComponent();

            // Set the data to the data grid.
            OscDataGrid.ItemsSource = ReceivedOscData;

            // Create the OSC receiver.
            Receiver = new OscReceiver(ReceivePortNumber);

            // Create a thread to do the listening.
            OscThread = new Thread(new ThreadStart(ListenLoop));

            // Connect the receiver.
            Receiver.Connect();

            // Start the listening thread.
            OscThread.Start();
        }


        // Loop for listening and updating dashboard data.
        static void ListenLoop()
        {
            try
            {
                while (Receiver.State != OscSocketState.Closed)
                {
                    if (Receiver.State == OscSocketState.Connected)
                    {
                        // Get the next message, this will block until one arrives or the socket is closed.
                        OscPacket Packet = Receiver.Receive();

                        // Slap the packet in the most recent message variable.
                        App.Current.Dispatcher.InvokeAsync(() =>
                        {
                            // Parses the packet into a message.
                            OscMessage LatestMessage = OscMessage.Parse(Packet.ToString());

                            // Overwrite any packets with the same address.
                            bool MessageIsNew = true;
                            foreach (OscData Message in ReceivedOscData)
                            {
                                if (Message.Path == LatestMessage.Address)
                                {
                                    int Index = ReceivedOscData.IndexOf(Message);
                                    ReceivedOscData.RemoveAt(Index);
                                    ReceivedOscData.Insert(Index, new OscData() { Path = LatestMessage.Address, Data = LatestMessage[0].ToString() });
                                    MessageIsNew = false;
                                    break;
                                }
                            }

                            // Put the new packets into the data grid.
                            if (MessageIsNew)
                                ReceivedOscData.Add(new OscData() { Path = LatestMessage.Address, Data = LatestMessage[0].ToString() });
                        });
                    }
                }
            }
            catch (Exception Ex)
            {
                // Display the exeception.
                MessageBox.Show(Ex.Message, "Exception Occurred", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        // Override the OnClosed method to close the receiver.
        protected override void OnClosed(EventArgs e)
        {

            // Close the receiver and the thread.
            Receiver.Close();
            OscThread.Abort();

            // Perform normal closing procedures.
            base.OnClosed(e);

            // Shutdown the application.
            Application.Current.Shutdown();
        }
    }
}