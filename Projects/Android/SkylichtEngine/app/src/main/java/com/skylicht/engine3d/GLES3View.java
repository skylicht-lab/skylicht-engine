package com.skylicht.engine3d;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

import javax.microedition.khronos.opengles.GL10;

public class GLES3View extends GLSurfaceView {
    public GLES3View(Context context) {
        super(context);

        setEGLContextFactory(new ContextFactory());
        setEGLConfigChooser(new ConfigChooser(5, 6, 5, 0, 16, 0));

        setPreserveEGLContextOnPause(true);
        setRenderer(new Renderer());
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
        private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            int[] attrib_list = {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL10.EGL_NONE
            };

            return egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            // native release application
            NativeInterface.getInstance().mainReleaseDevice();

            egl.eglDestroyContext(display, context);
        }
    }

    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {

        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        private static final int EGL_OPENGL_ES3_BIT = 64;
        private static final int[] sConfigAttributes =
                {
                        EGL10.EGL_RED_SIZE, 4,
                        EGL10.EGL_GREEN_SIZE, 4,
                        EGL10.EGL_BLUE_SIZE, 4,
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                        EGL10.EGL_NONE
                };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

            /* Get the number of minimally matching EGL configurations*/
            int[] numConfig = new int[1];
            egl.eglChooseConfig(display, sConfigAttributes, null, 0, numConfig);

            int numConfigs = numConfig[0];

            if (numConfigs <= 0) {
                throw new IllegalArgumentException("No configs match configSpec");
            }

            /* Allocate then read the array of minimally matching EGL configs*/
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, sConfigAttributes, configs, numConfigs, numConfig);

            /* Now return the "best" one*/
            EGLConfig cfg = chooseBestConfig(egl, display, configs);

            // check 888 32
            if (cfg == null) {
                mDepthSize = 32;
                cfg = chooseBestConfig(egl, display, configs);
            }

            // check 888 24
            if (cfg == null) {
                mDepthSize = 24;
                cfg = chooseBestConfig(egl, display, configs);
            }

            // check 888 16
            if (cfg == null) {
                mDepthSize = 16;
                cfg = chooseBestConfig(egl, display, configs);
            }

            // check 565 24
            if (cfg == null) {
                mBlueSize = 5;
                mGreenSize = 6;
                mRedSize = 5;
                mDepthSize = 24;
                cfg = chooseBestConfig(egl, display, configs);
            }

            // check 565 16
            if (cfg == null) {
                mBlueSize = 5;
                mGreenSize = 6;
                mRedSize = 5;
                mDepthSize = 16;
                cfg = chooseBestConfig(egl, display, configs);
            }

            return cfg;
        }

        public EGLConfig chooseBestConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
            for (EGLConfig config : configs) {
                // We want an *exact* match for red/green/blue/alpha
                int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);
                int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

                if (r == mRedSize &&
                        g == mGreenSize &&
                        b == mBlueSize &&
                        a == mAlphaSize &&
                        d >= mDepthSize &&
                        s >= mStencilSize) {
                    Log.w("Skylicht", String.format("Choose EGLConfig (rgba d) (%d %d %d %d %d %d)", r, g, b, a, d, s));
                    return config;
                }
            }

            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue) {
            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }

        // Subclasses can adjust these values:
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        public void onDrawFrame(GL10 gl) {
            if (GameInstance.HaveReadWritePermission) {
                NativeInterface.getInstance().mainLoop();
            } else {
                Log.w("Skylicht", "GameGLESRender no permissions");
            }
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            NativeInterface.getInstance().mainResizeWindow(width, height);

            Log.w("Skylicht", "GameGLESRender onSurfaceChanged: " + width + ":" + height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // init application
            Log.w("Skylicht", "GameGLESRender mainInitApp: " + GameInstance.ScreenWidth + ":" + GameInstance.ScreenHeight);
            NativeInterface.getInstance().mainInitApp(GameInstance.ScreenWidth, GameInstance.ScreenHeight);
        }
    }
}
