package com.skylicht.engine3d;

import android.app.Activity;
import android.util.Log;

import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.PlayGames;

public class PlayGamesAchievement {
    public static PlayGamesAchievement sInstance = null;
    private AchievementsClient mAchievementsClient;

    static public PlayGamesAchievement getInstance() {
        if (sInstance == null) sInstance = new PlayGamesAchievement();
        return sInstance;
    }

    public void init(Activity context) {
        mAchievementsClient = PlayGames.getAchievementsClient(context);
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

    public static void updateAchievement(String id, int step, float percent) {
        PlayGamesAchievement.getInstance().startUpdateAchievement(id, step, percent);
    }

    public native void init();
}
