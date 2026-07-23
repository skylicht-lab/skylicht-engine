package com.skylicht.engine3d;

import android.Manifest;
import android.app.Activity;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.core.app.NotificationManagerCompat;
import androidx.core.content.ContextCompat;
import androidx.work.Data;
import androidx.work.ExistingWorkPolicy;
import androidx.work.OneTimeWorkRequest;
import androidx.work.WorkManager;

import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.TimeUnit;

public class PostNotification {
    public static final int PERMISSION_NOT_DETERMINED = 0;
    public static final int PERMISSION_DENIED = 1;
    public static final int PERMISSION_ALLOWED = 2;

    static final String CHANNEL_ID = "skylicht_engine_notification";
    static final String CHANNEL_NAME = "Skylicht Engine";
    static final String EXTRA_ID = "id";
    static final String EXTRA_TITLE = "title";
    static final String EXTRA_CONTENT = "content";
    private static final String PREF_NAME = "post_notification";
    private static final String PREF_ASKED_PERMISSION = "asked_permission";
    private static final String PREF_SCHEDULED_IDS = "scheduled_ids";
    private static final String WORK_TAG = "post_notification";

    private static PostNotification sInstance;

    private Activity mActivity;
    private NotificationManager mNotificationManager;
    private ActivityResultLauncher<String> mPermissionLauncher;
    private boolean mAskedPermission;
    private SharedPreferences mPreferences;

    public static PostNotification getInstance() {
        if (sInstance == null) {
            sInstance = new PostNotification();
        }
        return sInstance;
    }

    public void init(Activity activity) {
        mActivity = activity;
        mNotificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);
        mPreferences = activity.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE);
        mAskedPermission = mPreferences.getBoolean(PREF_ASKED_PERMISSION, false);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, CHANNEL_NAME, NotificationManager.IMPORTANCE_DEFAULT);
            mNotificationManager.createNotificationChannel(channel);
        }

        mPermissionLauncher = ((FullscreenActivity)activity).registerForActivityResult(new ActivityResultContracts.RequestPermission(), result -> {
            setAskedPermission();
            onPermissionUpdate(result ? PERMISSION_ALLOWED : PERMISSION_DENIED);
        });

        initNative();
    }

    public int checkPermission() {
        if (mActivity == null) {
            return PERMISSION_DENIED;
        }

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) {
            return NotificationManagerCompat.from(mActivity).areNotificationsEnabled() ? PERMISSION_ALLOWED : PERMISSION_DENIED;
        }

        if (ContextCompat.checkSelfPermission(mActivity, Manifest.permission.POST_NOTIFICATIONS) == PackageManager.PERMISSION_GRANTED) {
            return PERMISSION_ALLOWED;
        }

        return mAskedPermission ? PERMISSION_DENIED : PERMISSION_NOT_DETERMINED;
    }

    public void requestPermission() {
        if (mActivity == null) {
            onPermissionUpdate(PERMISSION_DENIED);
            return;
        }

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) {
            onPermissionUpdate(checkPermission());
            return;
        }

        mActivity.runOnUiThread(() -> {
            if (mPermissionLauncher != null) {
                setAskedPermission();
                mPermissionLauncher.launch(Manifest.permission.POST_NOTIFICATIONS);
            } else {
                onPermissionUpdate(PERMISSION_DENIED);
            }
        });
    }

    public void scheduleNotification(String id, String title, String content, long delayFromNow) {
        if (mActivity == null || id == null || id.length() == 0) {
            return;
        }

        Data data = new Data.Builder()
                .putString(EXTRA_ID, id)
                .putString(EXTRA_TITLE, title)
                .putString(EXTRA_CONTENT, content)
                .build();

        OneTimeWorkRequest request = new OneTimeWorkRequest.Builder(PostNotificationWorker.class)
                .setInitialDelay(Math.max(1L, delayFromNow), TimeUnit.SECONDS)
                .setInputData(data)
                .addTag(WORK_TAG)
                .build();

        WorkManager.getInstance(mActivity).enqueueUniqueWork(getWorkName(id), ExistingWorkPolicy.REPLACE, request);
        addScheduledId(id);
    }

    public void removeNotification(String id) {
        if (mActivity == null || id == null || id.length() == 0) {
            return;
        }

        WorkManager.getInstance(mActivity).cancelUniqueWork(getWorkName(id));

        if (mNotificationManager != null) {
            mNotificationManager.cancel(getRequestCode(id));
        }

        removeScheduledId(id);
    }

    public void clear() {
        if (mActivity == null) {
            return;
        }

        Set<String> ids = getScheduledIds();
        WorkManager.getInstance(mActivity).cancelAllWorkByTag(WORK_TAG);
        for (String id : ids) {
            WorkManager.getInstance(mActivity).cancelUniqueWork(getWorkName(id));
            if (mNotificationManager != null) {
                mNotificationManager.cancel(getRequestCode(id));
            }
        }

        if (mPreferences != null) {
            mPreferences.edit().remove(PREF_SCHEDULED_IDS).apply();
        }
    }

    static int getRequestCode(String id) {
        return id.hashCode();
    }

    static String getWorkName(String id) {
        return "post_notification_" + id;
    }

    private void setAskedPermission() {
        mAskedPermission = true;
        if (mPreferences != null) {
            mPreferences.edit().putBoolean(PREF_ASKED_PERMISSION, true).apply();
        }
    }

    private Set<String> getScheduledIds() {
        if (mPreferences == null) {
            return new HashSet<>();
        }
        return new HashSet<>(mPreferences.getStringSet(PREF_SCHEDULED_IDS, new HashSet<>()));
    }

    private void addScheduledId(String id) {
        if (mPreferences == null) {
            return;
        }
        Set<String> ids = getScheduledIds();
        ids.add(id);
        mPreferences.edit().putStringSet(PREF_SCHEDULED_IDS, ids).apply();
    }

    private void removeScheduledId(String id) {
        if (mPreferences == null) {
            return;
        }
        Set<String> ids = getScheduledIds();
        ids.remove(id);
        mPreferences.edit().putStringSet(PREF_SCHEDULED_IDS, ids).apply();
    }

    public native void initNative();
    public native void onPermissionUpdate(int permission);

    public static int nativeCheckPermission() {
        return getInstance().checkPermission();
    }

    public static void nativeRequestPermission() {
        getInstance().requestPermission();
    }

    public static void nativeScheduleNotification(String id, String title, String content, long delayFromNow) {
        getInstance().scheduleNotification(id, title, content, delayFromNow);
    }

    public static void nativeRemoveNotification(String id) {
        getInstance().removeNotification(id);
    }

    public static void nativeClearNotification() {
        getInstance().clear();
    }

}
