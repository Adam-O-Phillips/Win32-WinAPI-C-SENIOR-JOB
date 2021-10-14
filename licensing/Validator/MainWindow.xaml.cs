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
using LicenseLib;

namespace Validator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static string pubKey = "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEmHdk/3FDWYzsbWIb+Bk7JS7WIEd2GRCklF7O/VE9fU0MBi4ZXapzcbJt2+HS5uA9kG/QFNeesyfyp4MKwtj9VA==";
        static string pubRSA2048Key =
            "<?xml version=\"1.0\" encoding=\"utf-16\"?>" +
            "<RSAParameters xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">" +
              "<Exponent>AQAB</Exponent>" +
              "<Modulus>uVirzS7vy49V8EjBGBy1+zL9CD7+fjaH1Xn3COwtOUi1x5HJ1UJGlfVLafJUNPQAZeysq0GPgoYtv/uSzs73ypV4ndizGbDR1gi/qaq/CMEVh3YrOB0GFUD9JX/J3TA+TvH0oFGLSxXh/9Etg7TOZwVrjBj+K00wu+agJJseqdELYqJFn/el7pSdpVz3tu9s12m34rzyntVBLRvBqTE/aZ2+oOj8rBQgLQF7sM1Wn8u4aG7KgegLtzwWs4Uh1bJLVLCTH2ZSrSKixSE6jbSc/2Nn/OlshmGkO4XdLuMAzllFsmT14uajdmcNOiC3HtnpcZsyEjgZUQ9yXIzLNw9T/Q==</Modulus>" +
            "</RSAParameters>";

        DeviceInfo deviceInfo = null;
        public MainWindow()
        {
            InitializeComponent();
            deviceInfo = new DeviceInfo();

            txtDevId.Text = deviceInfo.deviceHash;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (crypto_custom.check_license(txtDevId.Text, txtActivCode.Text, pubRSA2048Key))
                txtResult.Text = "OK => " + "sequence:" + crypto_custom.sequence + ", issued at:" + crypto_custom.issuedAt + ", period:" + crypto_custom.period + ", flag:" + crypto_custom.flag;
            else
                txtResult.Text = "Invalid";
            //             var lic = new KLicense();
            // 
            //             txtResult.Text = lic.validate_license(txtDevId.Text, txtActivCode.Text, pubKey);
        }
    }
}
