package net.test;

import com.beechat.network.*;

import java.io.*;
import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.TimeUnit;
import java.security.MessageDigest;
import java.lang.Thread;

import android.os.Handler;

class PrimeThread extends Thread {
    String std_output;
    Runnable updateProgress;
    Handler h;

    PrimeThread(Handler h, Runnable updateProgress) {
        std_output = new String();
        this.updateProgress = updateProgress;
        this.h = h;
    }

    public String getText() {
        return std_output;
    }

    public void run() {
        long mult = 4;
        long exp_num = 6;
        long kilobyte = 1 << 10;
        long megabyte = kilobyte << 10;
        long gigabyte = megabyte << 10;
        long size = 16; // 1 Mb
        long buffer_size = 65536; // 64 kb
        Blake3 hasher = null;

        byte[] buffer = new byte[(int)buffer_size];

        new Random().nextBytes(buffer);
        buffer_size *= size;
        String cout = new String();

        std_output += "  Size  |   Blake3   |  SHA-256  |  SHA-512  |   SHA-1   |\n";
        std_output += "--------|------------|-----------|-----------|-----------|\n";
        h.post(updateProgress);

        for (int ii = 0; ii < exp_num; ii++, size *= mult, buffer_size *= mult) {
            if (buffer_size >= gigabyte) {
                cout += String.format("%4d Gb | ", buffer_size / gigabyte);
            }
            else if (buffer_size >= megabyte) {
                cout += String.format("%4d Mb | ", buffer_size / megabyte);
            }
            else if (buffer_size >= kilobyte) {
                cout += String.format("%4d kb | ", buffer_size / kilobyte);
            }
            else {
                cout += String.format("%4d b | ", buffer_size);
            }

            try {
                hasher = new Blake3();
            } catch (Exception ex) {
                System.out.println("Exception: " + ex.getMessage());
                ex.printStackTrace();
            }

            long start = System.nanoTime();
            try {
                for (long i = 0; i < size; i++) {
                    hasher.update(buffer);
                }

                hasher.finalize(Blake3.OUT_LEN);
            } catch (Exception ex) {
                System.out.println("Exception: " + ex.getMessage());
                ex.printStackTrace();
            }
            long temptime = TimeUnit.MILLISECONDS.convert(System.nanoTime() - start, TimeUnit.NANOSECONDS);
            cout += String.format("%7d ms |", temptime);

            start = System.currentTimeMillis();
            try {
                MessageDigest md = MessageDigest.getInstance("SHA-256");

                for (long i = 0; i < size; i++) {
                    md.update(buffer);
                }

                md.digest();
            } catch (Exception ex) {
                System.out.println("Exception: " + ex.getMessage());
                ex.printStackTrace();
            }
            temptime = System.currentTimeMillis() - start;
            cout += String.format("%7d ms |", temptime);

            start = System.currentTimeMillis();
            try {
                MessageDigest md = MessageDigest.getInstance("SHA-512");

                for (long i = 0; i < size; i++) {
                    md.update(buffer);
                }

                md.digest();
            } catch (Exception ex) {
                System.out.println("Exception: " + ex.getMessage());
                ex.printStackTrace();
            }
            temptime = System.currentTimeMillis() - start;
            cout += String.format("%7d ms |", temptime);

            start = System.currentTimeMillis();
            try {
                MessageDigest md = MessageDigest.getInstance("SHA-1");

                for (long i = 0; i < size; i++) {
                    md.update(buffer);
                }

                md.digest();
            } catch (Exception ex) {
                System.out.println("Exception: " + ex.getMessage());
                ex.printStackTrace();
            }
            temptime = System.currentTimeMillis() - start;
            cout += String.format("%7d ms |\n", temptime);

            std_output += cout;
            h.post(updateProgress);
            cout = "";
        }
        std_output += "\nDone.";
        h.post(updateProgress);
    }
}

