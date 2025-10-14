package com.skylicht.engine3d;

import android.Manifest;
import android.annotation.SuppressLint;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.google.android.play.core.assetpacks.AssetPackLocation;
import com.google.android.play.core.assetpacks.AssetPackManager;
import com.google.android.play.core.assetpacks.AssetPackManagerFactory;
import com.google.android.play.core.assetpacks.model.AssetPackStorageMethod;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
public class FullscreenActivity extends AppCompatActivity {
    /**
     * Some older devices needs a small delay between UI widget updates
     * and a change of the status and navigation bar.
     */
    private static final int UI_ANIMATION_DELAY = 300;
    private final Handler mHideHandler = new Handler();
    private GLES3View mContentView;
    private final Runnable mHidePart2Runnable = new Runnable() {
        @SuppressLint("InlinedApi")
        @Override
        public void run() {
            // Delayed removal of status and navigation bar

            // Note that some of these constants are new as of API 16 (Jelly Bean)
            // and API 19 (KitKat). It is safe to use them, as they are inlined
            // at compile-time and do nothing on earlier devices.
            mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        }
    };

    private final Runnable mShowPart2Runnable = () -> {
        // Delayed display of UI elements
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.show();
        }
    };

    private final Runnable mHideRunnable = this::hide;

    boolean mVisible;

    AssetPackManager mAssetPackManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);

        super.onCreate(savedInstanceState);

        // setup fullscreen
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        NativeInterface.getInstance().setMainActivity(this);

        // asset pack manager
        mAssetPackManager = AssetPackManagerFactory.getInstance(getApplicationContext());

        Log.w("Skylicht", "Init AssetPackManager");

        // get source directory
        final ApplicationInfo applicationInfo = this.getApplicationInfo();
        NativeInterface.getInstance().addApkPath(applicationInfo.sourceDir);

        String []bundles = {"appdata"};
        for (String bundle: bundles) {
            AssetPackLocation assetPackPath = mAssetPackManager.getPackLocation(bundle);
            if (assetPackPath != null) {
                int packMethod = assetPackPath.packStorageMethod();
                if (packMethod == AssetPackStorageMethod.APK_ASSETS) {
                    for (String src : applicationInfo.splitSourceDirs) {
                        if (src.contains(bundle)) {
                            NativeInterface.getInstance().addApkPath(src);
                        }
                    }
                } else {
                    Log.w("Skylicht", "The engine only supports install-time asset: " + bundle);
                }
            }
            else {
                Log.w("Skylicht", "Asset not found: " + bundle);
            }
        }

        // permission detect
        checkReadWritePermission();

        // set the main activity
        GameInstance.Activity = this;

        // set the package name
        NativeInterface.getInstance().setAppID(getPackageName());

        // create save folder
        createSaveFolder();

        // set device information
        String deviceName = android.os.Build.MANUFACTURER;
        String deviceFactory = android.os.Build.MODEL;
        String osVersion = "Android " + Build.VERSION.RELEASE;
        NativeInterface.getInstance().setAndroidDeviceInfo(deviceName, deviceFactory, osVersion);

        @SuppressLint("HardwareIds") String androidId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
        NativeInterface.getInstance().setDeviceID(androidId);

        // set the screen size
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        GameInstance.ScreenWidth = dm.widthPixels;
        GameInstance.ScreenHeight = dm.heightPixels;

        // create OpenGL ES 3.0 view
        mContentView = new GLES3View(getApplication());
        setContentView(mContentView);

        mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        // Code below is to handle presses of Volume up or Volume down.
        // Without this, after pressing volume buttons, the navigation bar will
        // show up and won't hide
        mContentView.setOnSystemUiVisibilityChangeListener(visibility -> hide());

        mVisible = true;

    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        mContentView.post(() -> mContentView.requestLayout());
    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        // Trigger the initial hide() shortly after the activity has been
        // created, to briefly hint to the user that UI controls
        // are available.
        delayedHide(0);
    }

    private void hide() {
        // Hide UI first
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.hide();
        }
        mVisible = false;

        // Schedule a runnable to remove the status and navigation bar after a delay
        mHideHandler.removeCallbacks(mShowPart2Runnable);
        mHideHandler.postDelayed(mHidePart2Runnable, UI_ANIMATION_DELAY);
    }

    /*
    private void show() {
        // Show the system bar
        mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
        mVisible = true;

        // Schedule a runnable to display UI elements after a delay
        mHideHandler.removeCallbacks(mHidePart2Runnable);
        mHideHandler.postDelayed(mShowPart2Runnable, UI_ANIMATION_DELAY);
    }
    */

    /**
     * Schedules a call to hide() in delay milliseconds, canceling any
     * previously scheduled calls.
     */
    private void delayedHide(int delayMillis) {
        mHideHandler.removeCallbacks(mHideRunnable);
        mHideHandler.postDelayed(mHideRunnable, delayMillis);
    }

    /**
     * BASIC ENGINE 3D EVENT
     */

    @Override
    protected void onPause() {
        super.onPause();

        // pause native app
        NativeInterface.getInstance().mainPauseApp();

        mContentView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mContentView.onResume();
        createSaveFolder();
        NativeInterface.getInstance().mainResumeApp(1);
    }

    @Override
    public void onBackPressed() {
        if (NativeInterface.getInstance().onBack()) {
            super.onBackPressed();
        }
    }

    /**
     * CHECK NETWORK AVAILABLE AND OPEN URL FUNCTION
     */

    @SuppressLint("MissingPermission")
    public boolean isNetworkAvailable() {
        Context context = getApplicationContext();
        final ConnectivityManager connectivityManager = ((ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE));
        return connectivityManager.getActiveNetworkInfo() != null && connectivityManager.getActiveNetworkInfo().isConnected();
    }

    public void openURL(final String url) {
        FullscreenActivity.this.runOnUiThread(
                () -> {
                    try {
                        Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                        startActivity(browserIntent);
                    } catch (Exception ignored) {

                    }
                }
        );
    }

    /**
     * CREATE SAVE FOLDER
     */

    protected boolean createFolder(String path) {
        File folder = new File(path);

        boolean success = true;
        if (!folder.exists()) {
            success = folder.mkdir();
        }

        return success;
    }

    protected void createSaveFolder() {
        // https://developer.android.com/training/data-storage
        String dataFolder = getApplicationContext().getExternalFilesDir("data").getAbsolutePath();
        if (createFolder(dataFolder)) {
            NativeInterface.getInstance().setSaveFolder(dataFolder);
        } else {
            // Do something else on failure
            Log.w("Skylicht", "Can not create data folder: " + dataFolder);
        }

        String downloadFolder = getApplicationContext().getExternalFilesDir("download").getAbsolutePath();
        if (createFolder(downloadFolder)) {
            NativeInterface.getInstance().setDownloadFolder(downloadFolder);
        }
        else {
            Log.w("Skylicht", "Can not create data folder: " + downloadFolder);
        }
    }

    /**
     * DISPLAY READ WRITE PERMISSION
     */

    private static final int PERM_REQUEST_READWRITE = 1;

    private void checkReadWritePermission() {
        // https://github.com/copolii/runtime_permissions/blob/master/app/src/main/java/ca/mahram/android/runtimepermissions

        // todo check permission
        // enable this code if you need a permission to write STORAGE
        /*
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
            GameInstance.HaveReadWritePermission = false;
            showPermissionRationaleDialog();
        }
        else {
            GameInstance.HaveReadWritePermission = true;
        }
        */

        // In Android 4.4 (API level 19) or higher, we do not need request Write for getExternalFilesDir() folder
        GameInstance.HaveReadWritePermission = true;
    }

    private void showPermissionRationaleDialog() {
        final DialogInterface.OnClickListener listener = (dialog, which) -> {
            if (DialogInterface.BUTTON_POSITIVE == which) {
                requestWritePermission();
            }
        };

        AlertDialog dialog = new AlertDialog.Builder(this)
                .setTitle(R.string.write_access)
                .setMessage(R.string.write_permission_rationale)
                .setPositiveButton(android.R.string.ok, listener)
                .setCancelable(false)
                .create();
        dialog.show();
    }

    ActivityResultLauncher<String> launcherPermission = registerForActivityResult(
            new ActivityResultContracts.RequestPermission(),
            result -> {
                if (result)
                    GameInstance.HaveReadWritePermission = true;
                else
                    onWritePermissionDenied(false);
            });

    void requestWritePermission() {
        // ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE}, PERM_REQUEST_READWRITE);
        launcherPermission.launch(Manifest.permission.WRITE_EXTERNAL_STORAGE);
    }

    /*
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == PERM_REQUEST_READWRITE) {
            if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                // DENIED
                onWritePermissionDenied(false);
            } else {
                if (grantResults[0] == PackageManager.PERMISSION_GRANTED && grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                    // OK RUN GAME
                    GameInstance.HaveReadWritePermission = true;

                } else {
                    // SHOW ALERT
                    onWritePermissionDenied(true);
                }
            }
        }
    }
    */
    void onWritePermissionDenied(boolean dontAskAgain) {
        if (!dontAskAgain) {
            final DialogInterface.OnClickListener listener = (dialog, which) -> {
                if (which != DialogInterface.BUTTON_NEUTRAL) {
                    GameInstance.Activity.finish();
                    return;
                }
                runOnUiThread(() -> gotoSettings());
            };

            AlertDialog dialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.permission_denied)
                    .setMessage(R.string.write_permission_denied)
                    .setPositiveButton(android.R.string.ok, listener)
                    .setNeutralButton(R.string.change, listener)
                    .setCancelable(false)
                    .create();
            dialog.show();
        } else {
            runOnUiThread(() -> gotoSettings());
        }
    }

    ActivityResultLauncher<Intent> launcherSetting = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> checkReadWritePermission());

    private void gotoSettings() {
        final Intent intent;
        intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS, Uri.fromParts("package", getPackageName(), null));
        launcherSetting.launch(intent);
    }
}