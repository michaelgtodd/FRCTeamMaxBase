using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace DriverStation
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        // The list of mode buttons.
        List<Button> ButtonList;

        public MainWindow()
        {

            // Initialize the XAML components.
            InitializeComponent();

            // Initialize the button list.
            ButtonList = new List<Button>() { TeleOpButton, AutoButton, PracticeButton, TestButton };
        }

        private void ModeButtonClick(object Sender, RoutedEventArgs Args)
        {

            // For each button, unset the border.
            foreach (Button ModeButton in ButtonList)
            {
                ModeButton.BorderBrush = Brushes.DimGray;
            }
            ((Button)Sender).BorderBrush = Brushes.Black;
        }
    }
}
