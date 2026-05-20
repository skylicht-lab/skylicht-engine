package com.skylicht.engine3d;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.util.Log;

public class Clipboard {
    public static Clipboard sInstance = null;
    private ClipboardManager mClipboardManager;

    static public Clipboard getInstance() {
        if (sInstance == null) sInstance = new Clipboard();
        return sInstance;
    }

    public void init(Activity context) {
        mClipboardManager = (ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
        init();
    }

    public static void copyToClipboard(String text) {
        GameInstance.Activity.runOnUiThread(() -> {
            Clipboard clipboard = getInstance();
            if (clipboard.mClipboardManager != null) {
                ClipData clip = ClipData.newPlainText("Copied Text", text);
                clipboard.mClipboardManager.setPrimaryClip(clip);
            }
        });
    }

    public static String getTextFromClipboard() {
        Clipboard clipboard = getInstance();
        if (clipboard.mClipboardManager != null && clipboard.mClipboardManager.hasPrimaryClip()) {
            ClipData.Item item = clipboard.mClipboardManager.getPrimaryClip().getItemAt(0);
            CharSequence text = item.getText();
            if (text != null) {
                return text.toString();
            }
        }
        return "";
    }

    public native void init();
}