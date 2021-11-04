import sys
sys.path.append('../build')

import pykyber;

# Step 1:
# Alice
pykyber.pqcrystals_kyber512_ref_keypair()
print("Alice's public key:")
alice_pk = pykyber.pqcrystals_get_pk()
print(alice_pk)
print("\nAlice's secret key:")
alice_sk = pykyber.pqcrystals_get_sk()
print(alice_sk)

f = open('alice.public', 'w')
f.write(alice_pk)
f.close()
f = open('alice.secret', 'w')
f.write(alice_sk)
f.close()

# Bob
pykyber.pqcrystals_kyber512_ref_keypair()
print("\nBob's public key:")
bob_pk = pykyber.pqcrystals_get_pk()
print(bob_pk)
print("\nBob's secret key:")
bob_sk = pykyber.pqcrystals_get_sk()
print(bob_sk)

f = open('bob.public', 'w')
f.write(bob_pk)
f.close()
f = open('bob.secret', 'w')
f.write(bob_sk)
f.close()

# Step 2:
# Bob
f = open('alice.public', 'r')
pk = f.readline()
f.close()
pykyber.pqcrystals_kyber512_ref_enc("", pk)
bob_skey = pykyber.pqcrystals_get_skey()
chipertext = pykyber.pqcrystals_get_ct()
f = open('chipher.txt', 'w')
f.write(chipertext)
f.close()

# Step 3:
# Alice
f = open('chipher.txt', 'r')
chipertext = f.readline()
f.close()
pykyber.pqcrystals_kyber512_ref_dec(chipertext, alice_sk)
alice_skey = pykyber.pqcrystals_get_skey()

# Step 4:
print("\nalice_skey:")
print(alice_skey)
print("\nbob_skey:")
print(bob_skey)
print("\nResult:")
if alice_skey == bob_skey :
	print("Success!")
else:
	print("failed.")

