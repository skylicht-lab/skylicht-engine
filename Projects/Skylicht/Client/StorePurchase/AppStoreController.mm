#ifdef IOS

#include "AppStoreController.h"
#import <StoreKit/StoreKit.h>
#import <Foundation/Foundation.h>

void appstore_onInitialized();
void appstore_onInitializeFailed(int error, const char* message);
void appstore_onPurchaseSucceeded(const char* productId, const char* receipt);
void appstore_onPurchaseFailed(const char* productId, int error);

@interface AppStoreManager : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
+ (instancetype)sharedInstance;
- (void)fetchProducts:(NSSet *)productIdentifiers;
- (void)purchaseProduct:(NSString *)productIdentifier;
- (void)restorePurchases;
@end

@implementation AppStoreManager {
    SKProductsRequest *_productsRequest;
    NSArray<SKProduct *> *_validProducts;
}

+ (instancetype)sharedInstance {
    static AppStoreManager *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppStoreManager alloc] init];
        [[SKPaymentQueue defaultQueue] addTransactionObserver:sharedInstance];
    });
    return sharedInstance;
}

- (void)fetchProducts:(NSSet *)productIdentifiers {
    _productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    _productsRequest.delegate = self;
    [_productsRequest start];
}

- (void)purchaseProduct:(NSString *)productIdentifier {
    if ([SKPaymentQueue canMakePayments]) {
        SKProduct *targetProduct = nil;
        for (SKProduct *product in _validProducts) {
            if ([product.productIdentifier isEqualToString:productIdentifier]) {
                targetProduct = product;
                break;
            }
        }
        
        if (targetProduct) {
            SKPayment *payment = [SKPayment paymentWithProduct:targetProduct];
            [[SKPaymentQueue defaultQueue] addPayment:payment];
        } else {
            appstore_onPurchaseFailed([productIdentifier UTF8String], 0);
        }
    } else {
        appstore_onPurchaseFailed([productIdentifier UTF8String], -1);
    }
}

- (void)restorePurchases {
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error {
    appstore_onInitializeFailed((int)error.code, [[error localizedDescription] UTF8String]);
}

void appstore_onProductsReceived(const char** productIds,
								 const char** titles,
								 const char** descriptions,
								 const char** localizedPrices,
								 double* prices,
								 const char** currencyCodes,
								 int count);

#pragma mark - SKProductsRequestDelegate
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    _validProducts = response.products;
    
    int count = (int)_validProducts.count;
    const char **productIds = (const char **)malloc(sizeof(char *) * count);
    const char **titles = (const char **)malloc(sizeof(char *) * count);
    const char **descriptions = (const char **)malloc(sizeof(char *) * count);
    const char **localizedPrices = (const char **)malloc(sizeof(char *) * count);
    double *prices = (double *)malloc(sizeof(double) * count);
    const char **currencyCodes = (const char **)malloc(sizeof(char *) * count);
    
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    
    for (int i = 0; i < count; i++) {
        SKProduct *product = _validProducts[i];
        productIds[i] = [product.productIdentifier UTF8String];
        titles[i] = [product.localizedTitle UTF8String];
        descriptions[i] = [product.localizedDescription UTF8String];
        
        [numberFormatter setLocale:product.priceLocale];
        NSString *formattedPrice = [numberFormatter stringFromNumber:product.price];
        localizedPrices[i] = [formattedPrice UTF8String];
        
        prices[i] = [product.price doubleValue];
        currencyCodes[i] = [[product.priceLocale objectForKey:NSLocaleCurrencyCode] UTF8String];
    }
    
    appstore_onProductsReceived(productIds, titles, descriptions, localizedPrices, prices, currencyCodes, count);
    
    free(productIds);
    free(titles);
    free(descriptions);
    free(localizedPrices);
    free(prices);
    free(currencyCodes);
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    appstore_onInitializeFailed((int)error.code, [[error localizedDescription] UTF8String]);
}

#pragma mark - SKPaymentTransactionObserver
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    for (SKPaymentTransaction *transaction in transactions) {
        NSString *productId = transaction.payment.productIdentifier;
        
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchased:
            case SKPaymentTransactionStateRestored: {
                NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
                NSData *receiptData = [NSData dataWithContentsOfURL:receiptURL];
                NSString *receiptString = [receiptData base64EncodedStringWithOptions:0];
                
                appstore_onPurchaseSucceeded([productId UTF8String], [receiptString UTF8String]);
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            }
            case SKPaymentTransactionStateFailed: {
                appstore_onPurchaseFailed([productId UTF8String], (int)transaction.error.code);
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            }
            case SKPaymentTransactionStateDeferred:
            case SKPaymentTransactionStatePurchasing:
                break;
        }
    }
}
@end

void appstore_restorePurchase()
{
    [[AppStoreManager sharedInstance] restorePurchases];
}

void appstore_initiatePurchase(const char* productId)
{
    NSString *identifier = [NSString stringWithUTF8String:productId];
    [[AppStoreManager sharedInstance] purchaseProduct:identifier];
}

void appstore_fetchAdditionalProducts(const char** productIds, int count)
{
    NSMutableSet *identifiers = [NSMutableSet setWithCapacity:count];
    for(int i = 0; i < count; i++) {
        [identifiers addObject:[NSString stringWithUTF8String:productIds[i]]];
    }
    [[AppStoreManager sharedInstance] fetchProducts:identifiers];
}

#endif
