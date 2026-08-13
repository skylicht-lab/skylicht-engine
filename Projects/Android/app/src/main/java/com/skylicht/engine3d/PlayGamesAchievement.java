package com.skylicht.engine3d;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;

import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.AnnotatedData;
import com.google.android.gms.games.achievement.Achievement;
import com.google.android.gms.games.achievement.AchievementBuffer;
import com.google.android.gms.games.PlayGames;

public class PlayGamesAchievement {
    public static PlayGamesAchievement sInstance = null;
    private AchievementsClient mAchievementsClient;
    private ActivityResultLauncher<Intent> mAchievementsLauncher;

    static public PlayGamesAchievement getInstance() {
        if (sInstance == null) sInstance = new PlayGamesAchievement();
        return sInstance;
    }

    public void init(Activity context) {
        mAchievementsClient = PlayGames.getAchievementsClient(context);
        if (context instanceof FullscreenActivity) {
            mAchievementsLauncher = ((FullscreenActivity)context).registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
            });
        }
        init();
    }

    public void startUpdateAchievement(String id, int step, float percent) {
        if (id == null || id.length() == 0 || mAchievementsClient == null)
            return;

        GameInstance.Activity.runOnUiThread(() -> {
            if (percent >= 100.0f) {
                mAchievementsClient.unlock(id);
                Log.w("Skylicht", "Unlock achievement: " + id);
            } else if (step > 0) {
                mAchievementsClient.setSteps(id, step);
                Log.w("Skylicht", "Set achievement steps: " + id + " - " + step);
            }
        });
    }

    public void startShowDefaultAchievementsUI() {
        if (mAchievementsClient == null)
            return;

        GameInstance.Activity.runOnUiThread(() -> {
            mAchievementsClient.getAchievementsIntent()
                    .addOnSuccessListener((Intent intent) -> {
                        if (mAchievementsLauncher != null) {
                            mAchievementsLauncher.launch(intent);
                        } else {
                            GameInstance.Activity.startActivity(intent);
                        }
                    })
                    .addOnFailureListener(e ->
                            Log.w("Skylicht", e != null ? e.getMessage() : "Show achievements failed"));
        });
    }

    public void startFetch() {
        if (mAchievementsClient == null) {
            onFetchData(new String[0], new int[0], new float[0], new int[0], new int[0]);
            return;
        }

        GameInstance.Activity.runOnUiThread(() ->
                mAchievementsClient.load(false)
                        .addOnSuccessListener((AnnotatedData<AchievementBuffer> result) -> {
                            AchievementBuffer achievements = result.get();
                            if (achievements == null) {
                                onFetchData(new String[0], new int[0], new float[0], new int[0], new int[0]);
                                return;
                            }

                            int count = achievements.getCount();
                            String[] ids = new String[count];
                            int[] unlocks = new int[count];
                            float[] percents = new float[count];
                            int[] currentSteps = new int[count];
                            int[] totalSteps = new int[count];

                            for (int i = 0; i < count; i++) {
                                Achievement achievement = achievements.get(i);
                                boolean unlocked = achievement.getState() == Achievement.STATE_UNLOCKED;
                                boolean incremental = achievement.getType() == Achievement.TYPE_INCREMENTAL;
                                int total = incremental ? achievement.getTotalSteps() : 0;
                                int current = incremental ? achievement.getCurrentSteps() : 0;

                                ids[i] = achievement.getAchievementId();
                                unlocks[i] = unlocked ? 1 : 0;
                                percents[i] = unlocked ? 100.0f : (total > 0 ? (float)current * 100.0f / (float)total : 0.0f);
                                currentSteps[i] = current;
                                totalSteps[i] = total;
                            }

                            achievements.release();
                            onFetchData(ids, unlocks, percents, currentSteps, totalSteps);
                        })
                        .addOnFailureListener(e -> {
                            Log.w("Skylicht", e != null ? e.getMessage() : "Fetch achievements failed");
                            onFetchData(new String[0], new int[0], new float[0], new int[0], new int[0]);
                        }));
    }

    public static void updateAchievement(String id, int step, float percent) {
        PlayGamesAchievement.getInstance().startUpdateAchievement(id, step, percent);
    }

    public static void showDefaultAchievementsUI() {
        PlayGamesAchievement.getInstance().startShowDefaultAchievementsUI();
    }

    public static void fetch() {
        PlayGamesAchievement.getInstance().startFetch();
    }

    public native void init();

    public native void onFetchData(String[] ids, int[] unlocks, float[] percents, int[] currentSteps, int[] totalSteps);
}
