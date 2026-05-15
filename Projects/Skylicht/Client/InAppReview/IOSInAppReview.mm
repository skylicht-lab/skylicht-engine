#ifdef IOS

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

#include "IOSInAppReview.h"

void ios_showInAppReview(bool isTesting)
{
    if (@available(iOS 14.0, *)) {
        UIWindowScene *scene = nil;
        for (UIScene *s in [UIApplication sharedApplication].connectedScenes) {
            if ([s isKindOfClass:[UIWindowScene class]]) {
                scene = (UIWindowScene *)s;
                break;
            }
        }
        if (scene) {
            [SKStoreReviewController requestReviewInScene:scene];
        }
    } else if (@available(iOS 10.3, *)) {
        [SKStoreReviewController requestReview];
    }
}

#endif
