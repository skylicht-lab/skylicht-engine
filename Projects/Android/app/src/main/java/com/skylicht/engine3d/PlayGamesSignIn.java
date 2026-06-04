package com.skylicht.engine3d;

import android.app.Activity;
import android.util.Log;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;
import com.google.android.gms.games.Player;
import com.google.android.gms.games.PlayersClient;

public class PlayGamesSignIn {
    public static PlayGamesSignIn sInstance = null;
    private GamesSignInClient mGamesSignInClient;
    private String mClientId;
    private boolean mIsSignedIn;

    static public PlayGamesSignIn getInstance() {
        if (sInstance == null) sInstance = new PlayGamesSignIn();
        return sInstance;
    }

    public void init(Activity context) {
        PlayGamesSdk.initialize(context);
        mClientId = context.getString(R.string.pgs_server_client_id);
        mGamesSignInClient = PlayGames.getGamesSignInClient(context);
        mIsSignedIn = false;
        init();
    }

    public void startSignIn() {
        GameInstance.Activity.runOnUiThread(()->{
            mGamesSignInClient.signIn().addOnCompleteListener(task -> {
                boolean ok = task.isSuccessful() && task.getResult().isAuthenticated();
                if (ok) {
                    mIsSignedIn = true;
                    requestCode(mClientId);
                } else {
                    mIsSignedIn = false;
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
                mIsSignedIn = false;
                Log.w("Skylicht", e.getMessage());
                onSignInFailed(e.getMessage());
            });
        });
    }

    public void startSignOut() {
        GameInstance.Activity.runOnUiThread(() -> {
            GoogleSignInClient signInClient = GoogleSignIn.getClient(
                    GameInstance.Activity,
                    GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN
            );

            signInClient.signOut()
                    .addOnCompleteListener(task -> {
                        mIsSignedIn = false;

                        if (task.isSuccessful()) {
                            Log.w("Skylicht", "Play Games sign out success");
                        } else {
                            Exception e = task.getException();
                            Log.w("Skylicht", e != null ? e.getMessage() : "Play Games sign out failed");
                        }
                    });
        });
    }

    public static void signIn() {
        PlayGamesSignIn.getInstance().startSignIn();
    }

    public static void signOut() {
        PlayGamesSignIn.getInstance().startSignOut();
    }

    public static boolean isSignedIn() {
        return PlayGamesSignIn.getInstance().mIsSignedIn;
    }

    public native void init();

    public native void onSignInSuccess(String id, String name, String authCode);

    public native void onSignInFailed(String msg);
}
