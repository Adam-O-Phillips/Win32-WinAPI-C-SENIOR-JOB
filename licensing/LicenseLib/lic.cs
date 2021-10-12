using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Standard.Licensing;
using Standard.Licensing.Validation;

namespace LicenseLib
{
    public class KLicense
    {
        public string MyPasswd = "WinManagerLicense";
        public void create_signature_key()
        {
            var sig_privkey = "";
            var sig_pubkey = "";
            var keyGenerator = Standard.Licensing.Security.Cryptography.KeyGenerator.Create();
            var keyParir = keyGenerator.GenerateKeyPair();
            sig_privkey = keyParir.ToEncryptedPrivateKeyString(MyPasswd);
            sig_pubkey = keyParir.ToPublicKeyString();
            File.WriteAllText("priv.key", sig_privkey);
            File.WriteAllText("pub.key", sig_pubkey);
        }

        public string generate_licese_code(string DevId, string privkey)
        {
            DateTime dt = DateTime.Now;
            TimeSpan ts = new TimeSpan(0, 0, 0, 0, 30);
            dt.Add(ts);
            var license = License.New()
                    .WithProductFeatures(new Dictionary<string, string>
                    {
                        {"Device Id", DevId }
                    })
                    .ExpiresAt(dt)
                    .CreateAndSignWithPrivateKey(privkey, MyPasswd);

            return license.ToString();
        }

        public string validate_license(string req_code, string active_code, string pubkey)
        {
            if (active_code == "")
                return "empty_license";

            var license = License.Load(active_code);
            IDictionary<string, string> all_info = license.ProductFeatures.GetAll();
            string strDevId = all_info["Device Id"];

            if (req_code != strDevId)
                return "invalid_request";

            var validationFailures = license.Validate()
                    .Signature(pubkey)
                    .AssertValidLicense();
            var result_str = "";
            foreach (var fail in validationFailures)
                result_str += fail.GetType().Name + ":" + fail.Message + "\n";
            if (String.IsNullOrEmpty(result_str))
                result_str = "OK!";
            return result_str;
        }
    }
}
