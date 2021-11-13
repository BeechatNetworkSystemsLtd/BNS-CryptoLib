package network.beechat;

import java.io.*;
import java.util.Scanner;
import java.nio.charset.StandardCharsets;

public class Blake3JNIx86Test {

    public static void main(String[] args) throws FileNotFoundException, IOException {
        File input = new File("input.txt");
        FileInputStream fin=new FileInputStream(input);
        long start = System.currentTimeMillis();
        byte[] buffer = new byte[65536];
        int len = 0;

        try {
            Blake3 hasher = new Blake3();

            while (fin.available() != 0) {
                len = fin.read(buffer);
                hasher.update(buffer, len);
            }

            String output = hasher.finalize(Blake3.OUT_LEN);
            System.out.println("Hash: " + output);
        } catch (Exception ex) {
            System.out.println("Exception: " + ex.getMessage());
            ex.printStackTrace();
        }
        System.out.println("Time: " + (System.currentTimeMillis() - start) + " ms");
        System.out.println("Input file size: " + (input.length() / 1048576) + "." +
(input.length() % 1048576 / 1024) + " Mb");
        fin.close();
    }

}

