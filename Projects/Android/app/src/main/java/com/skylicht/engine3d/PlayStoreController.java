package com.skylicht.engine3d;

import android.app.Activity;
import android.os.Debug;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.PendingPurchasesParams;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class PlayStoreController implements PurchasesUpdatedListener {
    private static final int PRODUCT_CONSUMABLE = 0;
    private static final int PRODUCT_NON_CONSUMABLE = 1;

    public interface PurchaseSucceededCallback {
        void onPurchaseSucceeded(Activity activity, String productId, double unitPrice, String currencyCode, Purchase purchase);
    }

    public static PlayStoreController sInstance = null;
    private static PurchaseSucceededCallback sPurchaseSucceededCallback = null;

    private BillingClient mBillingClient;
    private Activity mActivity;

    private String mProductId;
    private final Map<String, Integer> mProductTypes = new HashMap<>();
    private final Map<String, ProductDetails> mProductDetails = new HashMap<>();
    private int mPendingRestorePurchases = 0;

    static public PlayStoreController getInstance() {
        if (sInstance == null) sInstance = new PlayStoreController();
        return sInstance;
    }

    public static void setPurchaseSucceededCallback(PurchaseSucceededCallback callback) {
        sPurchaseSucceededCallback = callback;
    }

    public void init(Activity context) {
        mActivity = context;
        init();

        mBillingClient = BillingClient.newBuilder(context)
                .setListener(this)
                .enablePendingPurchases(
                        PendingPurchasesParams.newBuilder()
                                .enableOneTimeProducts()
                                .build()
                )
                .enableAutoServiceReconnection()
                .build();
        
        startConnection();
    }

    public static void restart() {
        if (sInstance != null)
            sInstance.startConnection();
    }

    private void startConnection() {
        mBillingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    Log.w("Skylicht", "Purchase init success!");
                    onInitialized();
                } else {
                    Log.w("Skylicht", "Purchase init fail: " + billingResult.getDebugMessage());
                    onInitializeFailed(billingResult.getResponseCode(), billingResult.getDebugMessage());
                }
            }

            @Override
            public void onBillingServiceDisconnected() {
                // Try to restart the connection on the next request to
                // Google Play by calling the startConnection() method.
                onInitializeFailed(BillingClient.BillingResponseCode.SERVICE_DISCONNECTED, "Billing service disconnected");
            }
        });
    }

    public static void restorePurchase() {
        if (sInstance == null || sInstance.mBillingClient == null) return;
        
        sInstance.mBillingClient.queryPurchasesAsync(
                QueryPurchasesParams.newBuilder()
                        .setProductType(BillingClient.ProductType.INAPP)
                        .build(),
                (billingResult, purchases) -> {
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        sInstance.mPendingRestorePurchases = 0;
                        for (Purchase purchase : purchases) {
                            if (purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED && sInstance.isNonConsumable(purchase)) {
                                sInstance.mPendingRestorePurchases++;
                            }
                        }

                        if (sInstance.mPendingRestorePurchases == 0) {
                            sInstance.onRestorePurchaseCompleted();
                            return;
                        }

                        for (Purchase purchase : purchases) {
                            if (purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED) {
                                sInstance.handlePurchase(purchase, true);
                            }
                        }
                    } else {
                        sInstance.onRestorePurchaseFailed(billingResult.getResponseCode(), billingResult.getDebugMessage());
                    }
                }
        );
    }

    public static void initiatePurchase(String productId) {
        if (sInstance == null) {
            getInstance().onPurchaseFailed(productId, -1, "Billing library initialization failed");
            return;
        }

        if (sInstance.mBillingClient == null) {
            sInstance.onPurchaseFailed(productId, -1, "Billing library initialization failed");
            return;
        }

        sInstance.mProductId = productId;
        List<QueryProductDetailsParams.Product> productList = new ArrayList<>();
        productList.add(QueryProductDetailsParams.Product.newBuilder()
                .setProductId(productId)
                .setProductType(BillingClient.ProductType.INAPP)
                .build());

        QueryProductDetailsParams params = QueryProductDetailsParams.newBuilder()
                .setProductList(productList)
                .build();

        sInstance.mBillingClient.queryProductDetailsAsync(params, (billingResult, productDetailsResult) -> {
            List<ProductDetails> productDetailsList = productDetailsResult.getProductDetailsList();
            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK && !productDetailsList.isEmpty()) {
                ProductDetails productDetails = productDetailsList.get(0);
                sInstance.mProductDetails.put(productDetails.getProductId(), productDetails);
                
                List<BillingFlowParams.ProductDetailsParams> productDetailsParamsList = new ArrayList<>();
                productDetailsParamsList.add(
                        BillingFlowParams.ProductDetailsParams.newBuilder()
                                .setProductDetails(productDetails)
                                .build()
                );

                BillingFlowParams billingFlowParams = BillingFlowParams.newBuilder()
                        .setProductDetailsParamsList(productDetailsParamsList)
                        .build();

                sInstance.mBillingClient.launchBillingFlow(sInstance.mActivity, billingFlowParams);
            } else {
                String error = billingResult.getDebugMessage();
                if (productDetailsList.isEmpty())
                    error = "Product not found";
                sInstance.onPurchaseFailed(productId, billingResult.getResponseCode(), error);
            }
        });
    }

    public static void setProductTypes(String[] productIds, int[] types) {
        if (sInstance == null || productIds == null || types == null) return;

        int count = Math.min(productIds.length, types.length);
        for (int i = 0; i < count; i++) {
            String productId = productIds[i];
            if (productId == null || productId.isEmpty()) {
                continue;
            }
            sInstance.mProductTypes.put(productId, types[i]);
        }
    }

    public static void fetchAdditionalProducts(String[] productIds) {
        if (sInstance == null || sInstance.mBillingClient == null) return;

        List<QueryProductDetailsParams.Product> productList = new ArrayList<>();
        if (productIds != null) {
            for (String id : productIds) {
                if (id == null || id.isEmpty()) {
                    continue;
                }

                productList.add(QueryProductDetailsParams.Product.newBuilder()
                        .setProductId(id)
                        .setProductType(BillingClient.ProductType.INAPP)
                        .build());
            }
        }

        if (productList.isEmpty()) {
            sInstance.onProductsReceived(
                    new String[0],
                    new String[0],
                    new String[0],
                    new String[0],
                    new double[0],
                    new String[0]
            );
            return;
        }

        QueryProductDetailsParams params = QueryProductDetailsParams.newBuilder()
                .setProductList(productList)
                .build();

        sInstance.mBillingClient.queryProductDetailsAsync(params, (billingResult, productDetailsResult) -> {
            List<ProductDetails> productDetailsList = productDetailsResult.getProductDetailsList();
            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                int count = productDetailsList.size();
                String[] ids = new String[count];
                String[] titles = new String[count];
                String[] descriptions = new String[count];
                String[] prices = new String[count];
                double[] values = new double[count];
                String[] currencies = new String[count];

                for (int i = 0; i < count; i++) {
                    ProductDetails pd = productDetailsList.get(i);
                    sInstance.mProductDetails.put(pd.getProductId(), pd);

                    ProductDetails.OneTimePurchaseOfferDetails offer = pd.getOneTimePurchaseOfferDetails();
                    
                    ids[i] = pd.getProductId();
                    titles[i] = pd.getTitle();
                    descriptions[i] = pd.getDescription();
                    
                    if (offer != null) {
                        prices[i] = offer.getFormattedPrice();
                        values[i] = (double)offer.getPriceAmountMicros() / 1000000.0;
                        currencies[i] = offer.getPriceCurrencyCode();
                    } else {
                        prices[i] = "";
                        values[i] = 0.0;
                        currencies[i] = "";
                    }
                }
                sInstance.onProductsReceived(ids, titles, descriptions, prices, values, currencies);
            } else {
                sInstance.onFetchProductFailed(billingResult.getResponseCode(), billingResult.getDebugMessage());
            }
        });
    }

    @Override
    public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {
        if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK && purchases != null) {
            for (Purchase purchase : purchases) {
                handlePurchase(purchase, false);
            }
        } else {
            // Handle other errors
            onPurchaseFailed(mProductId, billingResult.getResponseCode(), billingResult.getDebugMessage());
        }
    }

    private int getProductType(String productId) {
        Integer type = mProductTypes.get(productId);
        return type != null ? type : PRODUCT_CONSUMABLE;
    }

    private boolean isNonConsumable(Purchase purchase) {
        for (String productId : purchase.getProducts()) {
            if (getProductType(productId) == PRODUCT_NON_CONSUMABLE) {
                return true;
            }
        }
        return false;
    }

    private void handlePurchase(Purchase purchase, boolean fromRestore) {
        if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) {
            return;
        }

        if (fromRestore && !isNonConsumable(purchase)) {
            return;
        }

        if (isNonConsumable(purchase)) {
            acknowledgePurchase(purchase, fromRestore);
        } else {
            consumePurchase(purchase);
        }
    }

    private void consumePurchase(Purchase purchase) {
        ConsumeParams consumeParams = ConsumeParams.newBuilder()
                .setPurchaseToken(purchase.getPurchaseToken())
                .build();

        mBillingClient.consumeAsync(consumeParams, (billingResult, purchaseToken) -> {
            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                notifyPurchaseSucceeded(purchase);
            } else {
                for (String productId : purchase.getProducts()) {
                    onPurchaseFailed(productId, billingResult.getResponseCode(), billingResult.getDebugMessage());
                }
            }
        });
    }

    private void acknowledgePurchase(Purchase purchase, boolean fromRestore) {
        if (purchase.isAcknowledged()) {
            notifyPurchaseFinished(purchase, fromRestore);
            if (fromRestore) {
                finishRestorePurchase();
            }
            return;
        }

        AcknowledgePurchaseParams acknowledgeParams = AcknowledgePurchaseParams.newBuilder()
                .setPurchaseToken(purchase.getPurchaseToken())
                .build();

        mBillingClient.acknowledgePurchase(acknowledgeParams, billingResult -> {
            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                notifyPurchaseFinished(purchase, fromRestore);
                if (fromRestore) {
                    finishRestorePurchase();
                }
            } else if (fromRestore) {
                onRestorePurchaseFailed(billingResult.getResponseCode(), billingResult.getDebugMessage());
                finishRestorePurchase();
            } else {
                for (String productId : purchase.getProducts()) {
                    onPurchaseFailed(productId, billingResult.getResponseCode(), billingResult.getDebugMessage());
                }
            }
        });
    }

    private void finishRestorePurchase() {
        mPendingRestorePurchases--;
        if (mPendingRestorePurchases <= 0) {
            mPendingRestorePurchases = 0;
            onRestorePurchaseCompleted();
        }
    }

    private void notifyPurchaseSucceeded(Purchase purchase) {
        notifyPurchaseFinished(purchase, false);
    }

    private void notifyPurchaseFinished(Purchase purchase, boolean fromRestore) {
        for (String productId : purchase.getProducts()) {
            if (fromRestore) {
                if (getProductType(productId) == PRODUCT_NON_CONSUMABLE) {
                    onPurchaseRestored(productId, purchase.getOriginalJson());
                }
            } else {
                notifyPurchaseSucceededCallback(productId, purchase);
                onPurchaseSucceeded(productId, purchase.getOriginalJson());
            }
        }
    }

    private void notifyPurchaseSucceededCallback(String productId, Purchase purchase) {
        if (sPurchaseSucceededCallback == null) {
            return;
        }

        ProductDetails productDetails = mProductDetails.get(productId);
        ProductDetails.OneTimePurchaseOfferDetails offer = productDetails != null ? productDetails.getOneTimePurchaseOfferDetails() : null;
        double unitPrice = offer != null ? (double)offer.getPriceAmountMicros() / 1000000.0 : 0.0;
        String currencyCode = offer != null ? offer.getPriceCurrencyCode() : "";

        try {
            sPurchaseSucceededCallback.onPurchaseSucceeded(mActivity, productId, unitPrice, currencyCode, purchase);
        } catch (Exception e) {
            Log.e("Skylicht", "Purchase succeeded callback failed", e);
        }
    }

    public native void init();
    public native void onInitialized();
    public native void onProductsReceived(String[] ids, String[] titles, String[] descriptions, String[] prices, double[] values, String[] currencies);
    public native void onInitializeFailed(int error, String message);
    public native void onFetchProductFailed(int error, String message);
    public native void onRestorePurchaseFailed(int error, String message);
    public native void onRestorePurchaseCompleted();
    public native void onPurchaseSucceeded(String productId, String receipt);
    public native void onPurchaseRestored(String productId, String receipt);
    public native void onPurchaseFailed(String productId, int error, String message);
}
