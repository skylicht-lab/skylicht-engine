#ifdef __cplusplus
extern "C" {
#endif

void appstore_init();
void appstore_restorePurchase();
void appstore_initiatePurchase(const char* productId);
void appstore_fetchAdditionalProducts(const char** productIds, int count);

typedef void (*AppStorePurchaseSucceededCallback)(const char* productId, double unitPrice, const char* currencyCode, const char* transactionId, const char* receipt);
void appstore_setPurchaseSucceededCallback(AppStorePurchaseSucceededCallback callback);

#ifdef __cplusplus
}
#endif
