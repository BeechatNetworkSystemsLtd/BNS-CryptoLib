package net.test;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity {

    Handler h;
    TextView text;
    PrimeThread testThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final Button button = (Button) findViewById(R.id.refrbutton);
        text = (TextView)findViewById(R.id.my_text);
        text.setMovementMethod(new ScrollingMovementMethod());
        text.setVisibility(View.VISIBLE);

        h = new Handler();
        testThread = new PrimeThread(h, updateProgress);

        button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) // клик на кнопку
            {
                testThread.start();
            }
        });
    }
    Runnable updateProgress = new Runnable() {
        public void run() {
            text.setText(testThread.getText());
        }
    };
}

