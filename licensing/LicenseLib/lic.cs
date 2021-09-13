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
            var license = License.New()
                    .WithProductFeatures(new Dictionary<string, string>
                    {
                        {"Device Id", DevId }
                    })
                    .CreateAndSignWithPrivateKey(privkey, MyPasswd);

            return license.ToString();
        }

        public string validate_license(string active_code, string pubkey)
        {
            var license = License.Load(active_code);
            var validationFailures = license.Validate()
                    .Signature(pubkey)
                    .AssertValidLicense();
            var result_str = "";
            foreach (var fail in validationFailures)
                result_str += fail.GetType().Name + "\n";
            if (String.IsNullOrEmpty(result_str))
                result_str = "OK!";
            return result_str;
        }
    }
}
