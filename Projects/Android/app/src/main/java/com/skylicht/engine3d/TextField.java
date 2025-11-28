package com.skylicht.engine3d;

import android.app.Activity;
import android.app.Dialog;
import android.content.Intent;
import android.os.Debug;
import android.text.Editable;
import android.text.InputFilter;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;

public class TextField {
    public static TextField sInstance = null;

    static public TextField getInstance() {
        if (sInstance == null) sInstance = new TextField();
        return sInstance;
    }

    public void init(Activity context) {
        init();
    }

    public String Text;
    public int MaxLength;
    public int Height;

    public native void init();

    public native void onChange(String text);

    public native void onDone(String text);

    public static void show(String text, int maxLength, int height)
    {
        GameInstance.Activity.runOnUiThread(()->{
            getInstance().showInputActivity(text, maxLength, height);
        });
    }

    public void showInputActivity(String text, int maxLength, int height) {
        Text = text;
        MaxLength = maxLength;
        Height = height;

        Dialog dialog = new Dialog(GameInstance.Activity);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.setContentView(R.layout.activity_input);

        Window window = dialog.getWindow();
        if (window != null) {
            WindowManager.LayoutParams layoutParams = window.getAttributes();
            layoutParams.gravity = Gravity.BOTTOM;
            layoutParams.width = WindowManager.LayoutParams.MATCH_PARENT;
            layoutParams.height = WindowManager.LayoutParams.WRAP_CONTENT;
            window.setAttributes(layoutParams);
            window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE);
            window.setDimAmount(0.0f);
        }

        EditText editText = dialog.findViewById(R.id.input_text);
        Button sendButton = dialog.findViewById(R.id.input_done);

        TextField textField = TextField.getInstance();
        int numLines = textField.Height / 50;
        Log.w("Skylicht", "Show text field, num line: " + numLines);
        editText.setText(textField.Text);
        editText.setMinLines(numLines);
        editText.setMaxLines(numLines);
        editText.setFilters(new InputFilter[]{new InputFilter.LengthFilter(textField.MaxLength)});
        editText.requestFocus();

        sendButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Text = editText.getText().toString();
                onDone(Text);
                dialog.dismiss();
            }
        });

        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void afterTextChanged(Editable editable) {
                Text = editable.toString();
                onChange(Text);
            }
        });

        // show dialog
        dialog.show();
    }
}
