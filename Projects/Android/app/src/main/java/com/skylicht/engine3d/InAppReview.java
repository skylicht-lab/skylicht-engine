package com.skylicht.engine3d;

import android.app.Activity;
import android.util.Log;

import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;
import com.google.android.play.core.review.testing.FakeReviewManager;
import com.google.android.gms.tasks.Task;

public class InAppReview {
    public static InAppReview sInstance = null;
    private ReviewManager mReviewManager;
    private ReviewManager mFakeReviewManager;

    static public InAppReview getInstance() {
        if (sInstance == null) sInstance = new InAppReview();
        return sInstance;
    }

    public void init(Activity context) {
        mReviewManager = ReviewManagerFactory.create(context);
        mFakeReviewManager = new FakeReviewManager(context);
        init();
    }

    public void startReview(boolean isTesting) {
        GameInstance.Activity.runOnUiThread(() -> {
            ReviewManager manager = isTesting ? mFakeReviewManager : mReviewManager;
            Task<ReviewInfo> request = manager.requestReviewFlow();
            request.addOnCompleteListener(task -> {
                if (task.isSuccessful()) {
                    ReviewInfo reviewInfo = task.getResult();
                    Task<Void> flow = manager.launchReviewFlow(GameInstance.Activity, reviewInfo);
                    flow.addOnCompleteListener(reviewTask -> {
                        Log.w("Skylicht", "InAppReview flow finished");
                    });
                } else {
                    Log.w("Skylicht", "InAppReview request failed");
                }
            });
        });
    }

    public static void showInAppReview(boolean isTesting) {
        InAppReview.getInstance().startReview(isTesting);
    }

    public native void init();
}
