#import "SyncFileModule.h"
#import <React/RCTBridge+Private.h>
#import <jsi/jsi.h>
#import "SyncFileBridge.h"

@implementation SyncFileModule

@synthesize bridge = _bridge;

RCT_EXPORT_MODULE(SyncFile)

+ (BOOL)requiresMainQueueSetup {
  return YES;
}

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install) {
  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)_bridge;
  if (!cxxBridge || !cxxBridge.runtime) {
    return @(NO);
  }
  syncfile::install(*(facebook::jsi::Runtime *)cxxBridge.runtime);
  return @(YES);
}

@end
