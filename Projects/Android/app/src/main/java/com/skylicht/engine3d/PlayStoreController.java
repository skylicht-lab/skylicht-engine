package com.skylicht.engine3d;

import android.app.Activity;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;

import java.util.ArrayList;
import java.util.List;

public class PlayStoreController implements PurchasesUpdatedListener {
    public static PlayStoreController sInstance = null;
    private BillingClient mBillingClient;
    private Activity mActivity;

    static public PlayStoreController getInstance() {
        if (sInstance == null) sInstance = new PlayStoreController();
        return sInstance;
    }

    public void init(Activity context) {
        mActivity = context;
        init();
        
        mBillingClient = BillingClient.newBuilder(context)
                .setListener(this)
                .enablePendingPurchases()
                .build();
        
        startConnection();
    }

    private void startConnection() {
        mBillingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    onInitialized();
                } else {
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
                        for (Purchase purchase : purchases) {
                            if (purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED) {
                                sInstance.handlePurchase(purchase);
                            }
                        }
                    }
                }
        );
    }

    public static void initiatePurchase(String productId) {
        if (sInstance == null || sInstance.mBillingClient == null) return;

        List<QueryProductDetailsParams.Product> productList = new ArrayList<>();
        productList.add(QueryProductDetailsParams.Product.newBuilder()
                .setProductId(productId)
                .setProductType(BillingClient.ProductType.INAPP)
                .build());

        QueryProductDetailsParams params = QueryProductDetailsParams.newBuilder()
                .setProductList(productList)
                .build();

        sInstance.mBillingClient.queryProductDetailsAsync(params, (billingResult, productDetailsList) -> {
            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK && !productDetailsList.isEmpty()) {
                ProductDetails productDetails = productDetailsList.get(0);
                
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
                sInstance.onPurchaseFailed(productId, billingResult.getResponseCode());
            }
        });
    }

    public static void fetchAdditionalProducts(String[] productIds) {
        if (sInstance == null || sInstance.mBillingClient == null) return;

        List<QueryProductDetailsParams.Product> productList = new ArrayList<>();
        for (String id : productIds) {
            productList.add(QueryProductDetailsParams.Product.newBuilder()
                    .setProductId(id)
                    .setProductType(BillingClient.ProductType.INAPP)
                    .build());
        }

        QueryProductDetailsParams params = QueryProductDetailsParams.newBuilder()
                .setProductList(productList)
                .build();

        sInstance.mBillingClient.queryProductDetailsAsync(params, (billingResult, productDetailsList) -> {
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
                sInstance.onInitializeFailed(billingResult.getResponseCode(), billingResult.getDebugMessage());
            }
        });
    }

    @Override
    public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {
        if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK && purchases != null) {
            for (Purchase purchase : purchases) {
                handlePurchase(purchase);
            }
        } else if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.USER_CANCELED) {
            // Handle user cancel
        } else {
            // Handle other errors
        }
    }

    private void handlePurchase(Purchase purchase) {
        if (purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED) {
            // For consumable products, we should consume them
            // In this example, let's assume all are consumable for simplicity
            ConsumeParams consumeParams = ConsumeParams.newBuilder()
                    .setPurchaseToken(purchase.getPurchaseToken())
                    .build();

            mBillingClient.consumeAsync(consumeParams, (billingResult, purchaseToken) -> {
                if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    for (String productId : purchase.getProducts()) {
                        onPurchaseSucceeded(productId, purchase.getOriginalJson());
                    }
                }
            });
            
            // If it's non-consumable, you should acknowledge it
            // if (!purchase.isAcknowledged()) { ... }
        }
    }

    public native void init();
    public native void onInitialized();
    public native void onProductsReceived(String[] ids, String[] titles, String[] descriptions, String[] prices, double[] values, String[] currencies);
    public native void onInitializeFailed(int error, String message);
    public native void onPurchaseSucceeded(String productId, String receipt);
    public native void onPurchaseFailed(String productId, int error);
}
