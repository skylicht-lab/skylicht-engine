package com.skylicht.engine3d;

public class NativeInterface {
    public static NativeInterface sInstance = null;

    static public NativeInterface getInstance() {
        if (sInstance == null)
            sInstance = new NativeInterface();
        return sInstance;
    }

    public native void mainInitApp(int width, int height);

    public native void mainLoop();

    public native void mainExitApp();

    public native void mainPauseApp();

    public native void mainResumeApp(int showConnecting);

    public native void mainReleaseDevice();

    public native void mainResizeWindow(int w, int h);

    public native void mainTouchDown(int touchID, int x, int y);

    public native void mainTouchMove(int touchID, int x, int y);

    public native void mainTouchUp(int touchID, int x, int y);

    public native void updateAccelerometer(float x, float y, float z);

    public native void setAccelerometerSupport(int enable);

    public native void updateCurrentLocation(double lat, double lng);

    public native void setApkPath(String path);

    public native void setObbPath(String path);

    public native void setBundleID(String id);

    public native void setSaveFolder(String path);

    public native void setDownloadFolder(String path);

    public native void setCPID(String cpid);

    public native void setDeviceID(String deviceID);

    public native void setAndroidDeviceInfo(String manu, String model, String os);

    public native boolean onBack();

    public static void systemGC() {
        System.gc();
    }

    public static void quitApplication() {
        System.exit(0);
    }

    public static boolean isNetworkAvailable() {
        return GameInstance.Activity.isNetworkAvailable();
    }

    public static void openURL(String url) {
        GameInstance.Activity.openURL(url);
    }

    static {
        System.loadLibrary("SampleSkinnedMesh");
    }
}
