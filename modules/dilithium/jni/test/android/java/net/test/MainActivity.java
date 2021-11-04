package net.test;

import java.io.*;
import java.util.Arrays;
import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import network.beechat.*;
import android.text.method.ScrollingMovementMethod;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        String std_output = "";

        String message = new String("example message for verification");
        Base58 b58 = new Base58();
        byte[] pk = new byte[Dilithium.CRYPTO_PUBLICKEYBYTES];
        byte[] sk = new byte[Dilithium.CRYPTO_SECRETKEYBYTES];
        byte[] sm = new byte[Dilithium.CRYPTO_BYTES + message.length()];
        byte[] m = new byte[Dilithium.CRYPTO_BYTES + message.length()];

        // Step 1: Create key pair
        int rc = Dilithium.crypto_sign_keypair(pk, sk);

        String testOutput = new String(b58.encode(pk));
        std_output += "Sign public key:\n" + testOutput + "\n";
        createFile("key.public", testOutput);
        testOutput = new String(b58.encode(sk));
        createFile("key.secret", testOutput);
        std_output += "Sign secret key:\n" + testOutput + "\n";

        // Step 2: Signing message
        long smlen = Dilithium.crypto_sign(sm, message.getBytes(), message.length(), sk);

        std_output += "\nOriginal message:\n" + message + "\n";
        testOutput = new String(b58.encode(sm));
        std_output += "\nSigned message:\n" + testOutput + "\n";
        createFile("signed.message", testOutput);

        // Step 3: Checking message
        rc = Dilithium.crypto_sign_open(m, b58.decode(readFile("signed.message")), smlen, b58.decode(readFile("key.public")));

        std_output += "\nRestored message:\n" + new String(m) + "\n";

        if (rc != -1) {
            std_output += "\nSuccess!\n";
            rc = 0;
        } else {
            std_output += "\nFailed.\n";
            rc = 1;
        }


        // ------------ DilithiumAes ------------

        std_output += "\n\n\n------- DilithiumAes -------\n";
        // Step 1: Create key pair
        rc = DilithiumAes.crypto_sign_keypair(pk, sk);

        testOutput = new String(b58.encode(pk));
        std_output += "Sign public key:\n" + testOutput + "\n";
        createFile("key.public", testOutput);
        testOutput = new String(b58.encode(sk));
        createFile("key.secret", testOutput);
        std_output += "Sign secret key:\n" + testOutput + "\n";


        // Step 2: Signing message
        smlen = DilithiumAes.crypto_sign(sm, message.getBytes(), message.length(), sk);

        std_output += "\nOriginal message:\n" + message + "\n";
        testOutput = new String(b58.encode(sm));
        std_output += "\nSigned message:\n" + testOutput + "\n";
        createFile("signed.message.aes", testOutput);

        // Step 3: Checking message
        rc = DilithiumAes.crypto_sign_open(m, b58.decode(readFile("signed.message.aes")), smlen, b58.decode(readFile("key.public")));

        std_output += "\nRestored message:\n" + new String(m) + "\n";

        if (rc != -1) {
            std_output += "\nSuccess!\n";
            rc = 0;
        } else {
            std_output += "\nFailed.\n";
            rc = 1;
        }

        TextView text = (TextView)findViewById(R.id.my_text);
        text.setMovementMethod(new ScrollingMovementMethod());
        text.setText(std_output);

///////


/*        Base58 b58 = new Base58();
        String std_output = "";
        byte[] alice_pk = new byte[Kyber512.KYBER_PUBLICKEYBYTES];
        byte[] alice_sk = new byte[Kyber512.KYBER_SECRETKEYBYTES];
        byte[] bob_pk = new byte[Kyber512.KYBER_PUBLICKEYBYTES];
        byte[] bob_sk = new byte[Kyber512.KYBER_SECRETKEYBYTES];
        byte[] alice_skey = new byte[Kyber512.KYBER_SSBYTES];
        byte[] bob_skey = new byte[Kyber512.KYBER_SSBYTES];
        byte[] ct = new byte[Kyber512.KYBER_CIPHERTEXTBYTES];

        // Step 1:
        int rc = Kyber512.crypto_kem_keypair(alice_pk, alice_sk);
        rc = Kyber512.crypto_kem_keypair(bob_pk, bob_sk);

        String testOutput = new String(b58.encode(alice_pk));
        std_output += "===Kyber512===\nAlice's public key:\n" + testOutput + "\n";
        createFile("alice.public", testOutput);

        testOutput = new String(b58.encode(alice_sk));
        std_output += "\nAlice's secret key:\n" + testOutput + "\n";
        createFile("alice.secret", testOutput);

        testOutput = new String(b58.encode(bob_pk));
        std_output += "\nBob's public key:\n" + testOutput + "\n";
        createFile("bob.public", testOutput);

        testOutput = new String(b58.encode(bob_sk));
        std_output += "\nBob's secret key:\n" + testOutput + "\n";
        createFile("bob.secret", testOutput);

        // Step 2:
        Kyber512.crypto_kem_enc(ct, bob_skey, b58.decode(readFile("alice.public")));
        // Step 3:
        Kyber512.crypto_kem_dec(alice_skey, ct, alice_sk);

        testOutput = new String(b58.encode(alice_skey));
        std_output += "\nAlice's skey: " + testOutput + "\n";
        testOutput = new String(b58.encode(bob_skey));
        std_output += "\nBob's skey: " + testOutput + "\n";

        // Step 4:
        std_output += "\nResult:\n";
        if (Arrays.equals(alice_skey, bob_skey)) {
            std_output += "Success!\n";
        } else {
            std_output += "Failed.\n";
        }

        // ----Kyber_90s----:
        // Step 1:
        rc = Kyber512_90s.crypto_kem_keypair(alice_pk, alice_sk);
        rc = Kyber512_90s.crypto_kem_keypair(bob_pk, bob_sk);

        testOutput = new String(b58.encode(alice_pk));
        std_output += "\n\n===Kyber512_90s===\n\nAlice's public key:\n" + testOutput + "\n";
        createFile("alice.public", testOutput);

        testOutput = new String(b58.encode(alice_sk));
        std_output += "\nAlice's secret key:\n" + testOutput + "\n";
        createFile("alice.secret", testOutput);

        testOutput = new String(b58.encode(bob_pk));
        std_output += "\nBob's public key:\n" + testOutput + "\n";
        createFile("bob.public", testOutput);

        testOutput = new String(b58.encode(bob_sk));
        std_output += "\nBob's secret key:\n" + testOutput + "\n";
        createFile("bob.secret", testOutput);

        // Step 2:
        Kyber512_90s.crypto_kem_enc(ct, bob_skey, b58.decode(readFile("alice.public")));
        // Step 3:
        Kyber512_90s.crypto_kem_dec(alice_skey, ct, alice_sk);

        testOutput = new String(b58.encode(alice_skey));
        std_output += "\nAlice's skey: " + testOutput + "\n";
        testOutput = new String(b58.encode(bob_skey));
        std_output += "\nBob's skey: " + testOutput + "\n";

        // Step 4:
        std_output += "\nResult:\n";
        if (Arrays.equals(alice_skey, bob_skey)) {
            std_output += "Success!\n";
        } else {
            std_output += "Failed.\n";
        }

        TextView text = (TextView)findViewById(R.id.my_text);
        text.setMovementMethod(new ScrollingMovementMethod());
        text.setText(std_output);*/
    }

    public void createFile(String path, String value) {
        try {
            BufferedWriter bufferedWriter = new BufferedWriter(
                new FileWriter(
                    new File(
                        getFilesDir() + File.separator + path
                    )
                )
            );
            bufferedWriter.write(value);
            bufferedWriter.close();
        } catch (IOException ex){}
    }

    public String readFile(String path) {
        String value = "";

        try {
            BufferedReader bufferedReader = new BufferedReader(
                new FileReader(
                    new File(
                        getFilesDir() + File.separator + path
                    )
                )
            );
            value = bufferedReader.readLine();
            //Kyber512.crypto_kem_enc(ct, bob_skey, b58.decode(bufferedReader.readLine()));
            bufferedReader.close();
        } catch (FileNotFoundException ex) {} catch (IOException ex2) {}

        return value;
    }
}

