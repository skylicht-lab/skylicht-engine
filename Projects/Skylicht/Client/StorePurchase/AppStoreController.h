#ifdef __cplusplus
extern "C" {
#endif

void appstore_init();
void appstore_restorePurchase();
void appstore_initiatePurchase(const char* productId);
void appstore_fetchAdditionalProducts(const char** productIds, int count);

#ifdef __cplusplus
}
#endif
