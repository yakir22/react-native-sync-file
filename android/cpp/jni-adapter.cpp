#include <jni.h>
#include <jsi/jsi.h>
#include "SyncFileBridge.h"

extern "C" JNIEXPORT void JNICALL
Java_com_syncfile_SyncFileModule_nativeInstall(JNIEnv *, jclass, jlong jsiRuntimePtr) {
  auto *runtime = reinterpret_cast<facebook::jsi::Runtime *>(jsiRuntimePtr);
  if (runtime) {
    syncfile::install(*runtime);
  }
}
