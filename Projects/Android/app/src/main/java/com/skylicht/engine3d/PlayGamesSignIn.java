package com.skylicht.engine3d;

import android.app.Activity;
import android.util.Log;

import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;
import com.google.android.gms.games.Player;
import com.google.android.gms.games.PlayersClient;

public class PlayGamesSignIn {
    public static PlayGamesSignIn sInstance = null;
    private GamesSignInClient mGamesSignInClient;
    private String mClientId;

    static public PlayGamesSignIn getInstance() {
        if (sInstance == null) sInstance = new PlayGamesSignIn();
        return sInstance;
    }

    public void init(Activity context) {
        PlayGamesSdk.initialize(context);
        mClientId = context.getString(R.string.pgs_server_client_id);
        mGamesSignInClient = PlayGames.getGamesSignInClient(context);
        init();
    }

    public void startSignIn() {
        GameInstance.Activity.runOnUiThread(()->{
            mGamesSignInClient.signIn().addOnCompleteListener(task -> {
                boolean ok = task.isSuccessful() && task.getResult().isAuthenticated();
                if (ok) {
                    requestCode(mClientId);
                } else {
                    Log.w("Skylicht", "startSignIn - failed");
                    onSignInFailed("");
                }
            });
        });
    }

    private void requestCode(String serverClientId) {
        Log.w("Skylicht", "Request code: " + serverClientId);

        PlayersClient players = PlayGames.getPlayersClient(GameInstance.Activity);
        players.getCurrentPlayer().addOnSuccessListener((Player p) -> {
            String playerId = p.getPlayerId();
            String gamerName = p.getDisplayName();

            mGamesSignInClient.requestServerSideAccess(serverClientId, true).addOnSuccessListener(authCode -> {
                Log.w("Skylicht", "requestServerSideAccess: " + gamerName);
                onSignInSuccess(playerId, gamerName, authCode);
            }).addOnFailureListener(e -> {
                Log.w("Skylicht", e.getMessage());
                onSignInFailed(e.getMessage());
            });
        });
    }

    public static void signIn() {
        PlayGamesSignIn.getInstance().startSignIn();
    }

    public native void init();

    public native void onSignInSuccess(String id, String name, String authCode);

    public native void onSignInFailed(String msg);
}
