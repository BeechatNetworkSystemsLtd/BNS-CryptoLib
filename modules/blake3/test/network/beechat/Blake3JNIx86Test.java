package network.beechat;

import java.io.*;
import java.util.Scanner;
import java.nio.charset.StandardCharsets;

public class Blake3JNIx86Test {

    public static void main(String[] args) throws FileNotFoundException {
        File input = new File("input.txt");
        long start = System.currentTimeMillis();
        try {
            Blake3 hasher = new Blake3();

            Scanner sc = new Scanner(input);
            String line = null;

            while (sc.hasNextLine()) {
                hasher.update(sc.nextLine());
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
    }

}

