openssl pkcs12 -in "Safari Developer Certificate.p12" -nodes | openssl x509 -outform der -out safari-cert.der
openssl pkcs12 -in "Safari Developer Certificate.p12" -nodes | openssl rsa -out safari-key.pem
openssl dgst -sign safari-key.pem -binary < safari-key.pem | wc -c safari-key-size.txt
