# Certificates

## Chrome

Add chrome-key.pem to packaged extension with filename key.pem when you upload to Chrome Web Store

## Safari

Instruction is here: http://developer.streak.com/2013/01/how-to-build-safari-extension-using.html

Use extract-safari-keys.sh to extract certificates from extension.  It will save as cert01, cert02

Use convert-safari-key.sh to extract cert.der and key.pem from .p12 Safari Developer Certificate (with private key included !)

Use src/build-safari.sh to compile extension with extracted keys