package com.skylicht.engine3d;

import android.Manifest;
import android.annotation.SuppressLint;

import androidx.activity.OnBackPressedCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import com.google.android.play.core.assetpacks.AssetPackLocation;
import com.google.android.play.core.assetpacks.AssetPackManager;
import com.google.android.play.core.assetpacks.AssetPackManagerFactory;
import com.google.android.play.core.assetpacks.model.AssetPackStorageMethod;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.graphics.Rect;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowMetrics;

import java.io.File;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
public class FullscreenActivity extends AppCompatActivity {
    private final Handler mHideHandler = new Handler(Looper.getMainLooper());
    private GLES3View mContentView;
    private final Runnable mHideRunnable = this::hide;

    AssetPackManager mAssetPackManager;

    private final OnBackPressedCallback mBackPressedCallback = new OnBackPressedCallback(true) {
        @Override
        public void handleOnBackPressed() {
            if (NativeInterface.getInstance().onBack()) {
                setEnabled(false);
                getOnBackPressedDispatcher().onBackPressed();
                setEnabled(true);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getOnBackPressedDispatcher().addCallback(this, mBackPressedCallback);

        // setup fullscreen
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setupFullscreenWindow();

        NativeInterface.getInstance().setMainActivity(this);

        // asset pack manager
        mAssetPackManager = AssetPackManagerFactory.getInstance(getApplicationContext());

        Log.w("Skylicht", "Init AssetPackManager");

        // get source directory
        final ApplicationInfo applicationInfo = this.getApplicationInfo();
        NativeInterface.getInstance().addApkPath(applicationInfo.sourceDir);

        String[] bundles = {"appdata"};
        for (String bundle : bundles) {
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
            } else {
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
        DisplayMetrics dm = getScreenMetrics();
        GameInstance.ScreenWidth = dm.widthPixels;
        GameInstance.ScreenHeight = dm.heightPixels;

        // create OpenGL ES 3.0 view
        mContentView = new GLES3View(getApplication());
        setContentView(mContentView);

        // Init native plugin
        PlayGamesSignIn.getInstance().init(this);
        PlayGamesAchievement.getInstance().init(this);
        TextField.getInstance().init(this);
        Clipboard.getInstance().init(this);
        PlayStoreController.getInstance().init(this);
        InAppReview.getInstance().init(this);
        PostNotification.getInstance().init(this);
    }

    private void setupFullscreenWindow() {
        Window window = getWindow();
        WindowCompat.setDecorFitsSystemWindows(window, false);
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            WindowManager.LayoutParams attributes = window.getAttributes();
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_ALWAYS;
            } else {
                attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            }
            window.setAttributes(attributes);
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (mContentView != null) {
            mContentView.post(() -> mContentView.requestLayout());
        }
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

        hideSystemBars();
    }

    private void hideSystemBars() {
        WindowInsetsControllerCompat windowInsetsController = WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());
        windowInsetsController.setSystemBarsBehavior(WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        windowInsetsController.hide(WindowInsetsCompat.Type.systemBars());
    }

    /**
     * Schedules a call to hide() in delay milliseconds, canceling any
     * previously scheduled calls.
     */
    private void delayedHide(int delayMillis) {
        mHideHandler.removeCallbacks(mHideRunnable);
        mHideHandler.postDelayed(mHideRunnable, delayMillis);
    }

    private DisplayMetrics getScreenMetrics() {
        DisplayMetrics dm = new DisplayMetrics();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            WindowMetrics windowMetrics = getWindowManager().getCurrentWindowMetrics();
            Rect bounds = windowMetrics.getBounds();
            dm.widthPixels = bounds.width();
            dm.heightPixels = bounds.height();
        } else {
            getLegacyScreenMetrics(dm);
        }
        return dm;
    }

    @SuppressWarnings("deprecation")
    private void getLegacyScreenMetrics(DisplayMetrics dm) {
        getWindowManager().getDefaultDisplay().getMetrics(dm);
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
        delayedHide(0);

        createSaveFolder();
        NativeInterface.getInstance().mainResumeApp(1);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            delayedHide(0);
        }
    }

    /**
     * CHECK NETWORK AVAILABLE AND OPEN URL FUNCTION
     */

    @SuppressLint("MissingPermission")
    public boolean isNetworkAvailable() {
        Context context = getApplicationContext();
        final ConnectivityManager connectivityManager = ((ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE));
        if (connectivityManager == null) {
            return false;
        }

        Network activeNetwork = connectivityManager.getActiveNetwork();
        if (activeNetwork == null) {
            return false;
        }

        NetworkCapabilities capabilities = connectivityManager.getNetworkCapabilities(activeNetwork);
        return capabilities != null && capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
    }

    public void openURL(final String url) {
        FullscreenActivity.this.runOnUiThread(() -> {
            try {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                startActivity(browserIntent);
            } catch (Exception ignored) {

            }
        });
    }

    public void openApplicationSetting() {
        FullscreenActivity.this.runOnUiThread(() -> {
            Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS, Uri.fromParts("package", getPackageName(), null));
            startActivity(intent);
        });
    }

    /**
     * CREATE SAVE FOLDER
     */

    protected boolean createFolder(File folder) {
        if (folder == null) {
            return false;
        }

        boolean success = true;
        if (!folder.exists()) {
            success = folder.mkdirs();
        }

        return success;
    }

    protected File getStorageFolder(String type, File fallbackRoot) {
        File folder = getApplicationContext().getExternalFilesDir(type);
        if (folder != null) {
            return folder;
        }

        Log.w("Skylicht", "External storage folder is unavailable: " + type);
        if (fallbackRoot == null) {
            return null;
        }

        return new File(fallbackRoot, type);
    }

    protected void createSaveFolder() {
        // https://developer.android.com/training/data-storage
        File dataFolder = getStorageFolder("data", getFilesDir());
        if (createFolder(dataFolder)) {
            NativeInterface.getInstance().setSaveFolder(dataFolder.getAbsolutePath());
        } else {
            // Do something else on failure
            Log.w("Skylicht", "Can not create data folder: " + dataFolder);
        }

        File downloadFolder = getStorageFolder("download", getCacheDir());
        if (createFolder(downloadFolder)) {
            NativeInterface.getInstance().setDownloadFolder(downloadFolder.getAbsolutePath());
        } else {
            Log.w("Skylicht", "Can not create download folder: " + downloadFolder);
        }
    }

    private void checkReadWritePermission() {
        // In Android 4.4 (API level 19) or higher, we do not need request Write for getExternalFilesDir() folder
        GameInstance.HaveReadWritePermission = true;
    }

    ActivityResultLauncher<Intent> launcherSetting = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> checkReadWritePermission());

    private void gotoSettings() {
        final Intent intent;
        intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS, Uri.fromParts("package", getPackageName(), null));
        launcherSetting.launch(intent);
    }
}
