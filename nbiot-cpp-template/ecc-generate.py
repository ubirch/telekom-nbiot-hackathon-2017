# ecc-generate.py
#
# Generate an ECC keypair and dump it in C-compatible form.
# Also registers the public key with the ubirch backend if needed.
import json

import ed25519
import os
import re
import sys

from uuid import uuid4
from time import time
from datetime import datetime

import requests

privateKey, publicKey = ed25519.create_keypair(entropy=os.urandom)

publicKeyEncoded = publicKey.to_ascii(encoding="hex").decode('latin1')
privateKeyEncoded = privateKey.to_ascii(encoding="hex").decode('latin1')

deviceId = ""
while True:
    deviceId = raw_input("Enter Calliope mini ID: ")
    if (len(deviceId) == 8 and int(deviceId, 16) != 0):
        break
    else:
        print "ERROR: A device ID has 8 characters consisting of 0-9 and A-F."

raw_input("Press Enter to send public key to ubirch key server (Ctrl-C to abort):")

ts = time()
created = datetime.utcfromtimestamp(ts).strftime('%Y-%m-%dT%H:%M:%S.000Z')
validNotAfter = datetime.utcfromtimestamp(ts + 3.154e+7).strftime('%Y-%m-%dT%H:%M:%S.000Z')
pubKeyInfo = {
    "hwDeviceId": deviceId.lower(),
    "pubKey": publicKey.to_ascii(encoding="base64").decode('latin1'),
    "pubKeyId": str(uuid4()),
    "algorithm": "ed25519",
    # "previousPubKeyId": None,
    "created": created,
    "validNotBefore": created,
    "validNotAfter": validNotAfter
}
jsonPubKeyInfo = json.dumps(pubKeyInfo, sort_keys=True, separators=(',', ':'), encoding="UTF-8")
signedPubKeyInfo = privateKey.sign(msg=jsonPubKeyInfo, encoding="base64").decode('latin1')

payload = {
    "pubKeyInfo": pubKeyInfo,
    "signature": signedPubKeyInfo,
    # "previousPubKeySignature": None
}

print jsonPubKeyInfo
try:
    publicKey.verify(sig=signedPubKeyInfo, msg=jsonPubKeyInfo, encoding="base64")
except ed25519.BadSignatureError:
    print "VERIFICATION FAILED"
    exit()

# encode final payload
jsonPayload =json.dumps(payload, sort_keys=True, separators=(',', ':'), encoding="UTF-8")
# print jsonPayload

try:
    r = requests.post("https://key.demo.ubirch.com/api/keyService/v1/pubkey",
                      data=jsonPayload, headers={'content-type': 'application/json'})
    print r.text
    print
    print "COPY THE PRIVATE KEY INTO YOUR PXT SCRIPT:"
    print("public key : '" + publicKeyEncoded + "'")
    print("private key: '" + privateKeyEncoded + publicKeyEncoded + "'")
    bytes = re.findall("[0-9a-f]{2}", privateKeyEncoded + publicKeyEncoded)

    print
    print "COPY THE FOLLOWING LINES INTO YOUR C++ CODE:"
    print "/* ==== ECC KEYS ================= */"
    print "static unsigned char vrfyKey[crypto_sign_PUBLICKEYBYTES] = {"
    for n in range(32, 64, 16):
        for b in range(16):
            sys.stdout.write("0x" + bytes[n + b] + ", ")
        print
    print "};"
    print "static unsigned char signKey[crypto_sign_SECRETKEYBYTES] = {"
    for n in range(0, 64, 16):
        for b in range(16):
            sys.stdout.write("0x" + bytes[n + b] + ", ")
        print
    print "};"
    print "/* ==== ECC KEYS ================= */"
    print
except:
    print "SUBMIT FAILED!"
