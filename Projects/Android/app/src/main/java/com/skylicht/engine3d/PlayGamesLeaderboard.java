package com.skylicht.engine3d;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;

import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.tasks.Task;

public class PlayGamesLeaderboard {
    public static PlayGamesLeaderboard sInstance = null;
    private LeaderboardsClient mLeaderboardsClient;
    private ActivityResultLauncher<Intent> mLeaderboardLauncher;

    public static PlayGamesLeaderboard getInstance() {
        if (sInstance == null) sInstance = new PlayGamesLeaderboard();
        return sInstance;
    }

    public void init(Activity context) {
        mLeaderboardsClient = PlayGames.getLeaderboardsClient(context);
        if (context instanceof FullscreenActivity) {
            mLeaderboardLauncher = ((FullscreenActivity)context).registerForActivityResult(
                    new ActivityResultContracts.StartActivityForResult(), result -> {
                    });
        }
        init();
    }

    public void startPostScore(String id, long score) {
        if (id == null || id.length() == 0 || mLeaderboardsClient == null)
            return;

        GameInstance.Activity.runOnUiThread(() ->
                mLeaderboardsClient.submitScoreImmediate(id, score)
                        .addOnSuccessListener(result ->
                                Log.w("Skylicht", "Posted leaderboard score: " + id + " - " + score))
                        .addOnFailureListener(e ->
                                Log.w("Skylicht", "Post leaderboard score failed: " + id, e)));
    }

    public void startShowDefaultLeaderboardUI(String id) {
        if (mLeaderboardsClient == null)
            return;

        GameInstance.Activity.runOnUiThread(() -> {
            Task<Intent> intentTask = id == null || id.length() == 0
                    ? mLeaderboardsClient.getAllLeaderboardsIntent()
                    : mLeaderboardsClient.getLeaderboardIntent(id);
            intentTask.addOnSuccessListener(intent -> {
                if (mLeaderboardLauncher != null) {
                    mLeaderboardLauncher.launch(intent);
                } else {
                    Log.w("Skylicht", "Leaderboard activity launcher is not initialized");
                }
            }).addOnFailureListener(e ->
                    Log.w("Skylicht", "Show leaderboard failed: " + id, e));
        });
    }

    public static void postScore(String id, long score) {
        getInstance().startPostScore(id, score);
    }

    public static void showDefaultLeaderboardUI(String id) {
        getInstance().startShowDefaultLeaderboardUI(id);
    }

    public native void init();
}
