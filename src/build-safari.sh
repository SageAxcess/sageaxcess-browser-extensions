#!/bin/bash
mkdir SageAxcess.safariextension
cp safari/* SageAxcess.safariextension/
xar -czf SageAxcess.safariextz --distribution SageAxcess.safariextension
xar --sign -f SageAxcess.safariextz --digestinfo-to-sign digest.dat --sig-size `cat ../cert/safari-key-size.txt` --cert-loc ../cert/safari-cert.der --cert-loc ../cert/safari-cert01 --cert-loc ../cert/safari-cert02
openssl rsautl -sign -inkey ../cert/safari-key.pem -in digest.dat -out sig.dat
xar --inject-sig sig.dat -f SageAxcess.safariextz
rm -f *.dat
rm -rf SageAxcess.safariextension
