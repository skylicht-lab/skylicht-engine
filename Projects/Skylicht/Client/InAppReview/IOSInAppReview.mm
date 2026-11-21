#ifdef IOS

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

#include "IOSInAppReview.h"
#include "IOSScene.h"

void ios_showInAppReview(bool isTesting)
{
    if (@available(iOS 14.0, *)) {
        UIWindowScene *scene = irr::getIOSWindow().windowScene;
        if (scene) {
            [SKStoreReviewController requestReviewInScene:scene];
        }
    } else if (@available(iOS 10.3, *)) {
        [SKStoreReviewController requestReview];
    }
}

#endif
