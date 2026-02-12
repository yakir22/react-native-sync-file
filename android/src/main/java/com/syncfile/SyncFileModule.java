package com.syncfile;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;

public class SyncFileModule extends ReactContextBaseJavaModule {
  static {
    System.loadLibrary("sync-file");
  }

  private static native void nativeInstall(long jsiRuntimePtr);

  public SyncFileModule(ReactApplicationContext context) {
    super(context);
  }

  @Override
  public String getName() {
    return "SyncFile";
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  public boolean install() {
    try {
      long runtimePtr = getReactApplicationContext().getJavaScriptContextHolder().get();
      if (runtimePtr == 0) return false;
      nativeInstall(runtimePtr);
      return true;
    } catch (Exception e) {
      return false;
    }
  }
}
