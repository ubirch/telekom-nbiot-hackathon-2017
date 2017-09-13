import ed25519
import os
import re
import sys

privateKey, publicKey = ed25519.create_keypair(entropy=os.urandom)

publicKeyEncoded = publicKey.to_ascii(encoding="hex").decode('latin1')
privateKeyEncoded = privateKey.to_ascii(encoding="hex").decode('latin1')

print("public key : '" + publicKeyEncoded + "'")
print("private key: '" + privateKeyEncoded + publicKeyEncoded + "'")

bytes = re.findall("[0-9a-f]{2}", privateKeyEncoded+publicKeyEncoded)

print
print "COPY THE FOLLOWING LINES INTO YOUR C++ CODE:"
print "/* ==== ECC KEYS ================= */"
print "static unsigned char vrfyKey[crypto_sign_PUBLICKEYBYTES] = {"
for n in range(32, 64, 16):
      for b in range(16):
            sys.stdout.write("0x"+bytes[n+b]+", ")
      print
print "};"
print "static unsigned char signKey[crypto_sign_SECRETKEYBYTES] = {"
for n in range(0, 64, 16):
      for b in range(16):
            sys.stdout.write("0x"+bytes[n+b]+", ")
      print
print "};"
print "/* ==== ECC KEYS ================= */"
print

deviceId = ""
while True:
      deviceId = raw_input("Enter device ID: ")
      if(len(deviceId) == 8 and int(deviceId, 16) != 0): break
      else: print "ERROR: A device ID has 8 characters consisting of 0-9 and A-F."

raw_input("Press Enter to send public key to ubirch key server (Ctrl-C to abort):")
