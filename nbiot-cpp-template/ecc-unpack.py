# Script to test the message pack messages
# Unpacks the message pack data and decrypts it if a secret is given
#
# @author Matthias L. Jugel

import ed25519
from Crypto.Cipher import AES
import msgpack
import sys

# provide the message as the first argument and the pubkey as second
message = "01cebc9ab239d95218e07a5d20386a74a0fab7898db25d0ba20da14d8559168a9bb0bc2f3512af6a2a5067cc059100e4250b52c20a1588c1ee6f758108cf2252dcad03edb4a5fd067b2274656d7065726174757265223a33307d"
pubkey = "7b60882bee2e3f017907ce84e5e1c87f705760fac5877fe0de7c5806c4691f2f"
#pubkey = None

if len(sys.argv) > 1:
    message = sys.argv[1]

# if necessary provide key as second argument
secret = None
if len(sys.argv) > 2:
    pubkey = sys.argv[2]

if pubkey is not None:
    print "decoding signed message"
    data = message.decode("hex")
    # unpack first element (ignore rest, which is padding)
    unpacker = msgpack.Unpacker()
    unpacker.feed(data)
    marker = unpacker.next()
    if marker == 0x01:
        print "DEVICE   : "+hex(unpacker.next())
        signed = unpacker.next()
        signature = signed[0:64]
        print "SIGNATURE: "+ "".join(format(ord(x), '02x') for x in signature)
        signedMessage = signed[64:]
        print "MESSAGE  : "+str(signedMessage)
        # decode the public key
        vk = ed25519.VerifyingKey(pubkey, encoding="hex")
        try:
            vk.verify(signature, signedMessage, encoding=None)
            print "VERIFIED"
        except ed25519.BadSignatureError:
            print "VERIFICATION FAILED"
    else:
        print "wrong packet marker: "+ord(marker)+" != 0x01 (signed)"
else:
    print "decoding unsigned message"
    data = message.decode("hex")
    # unpack first element (ignore rest, which is padding)
    unpacker = msgpack.Unpacker()
    unpacker.feed(data)
    marker = unpacker.next()
    if marker == 0:
        print "ID  : "+str(unpacker.next())
        print "DATA: "+unpacker.next()
    else:
        print "wrong packet marker: "+ord(marker)+" != 0x00 (unsigned)"