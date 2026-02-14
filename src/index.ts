import { NativeModules } from 'react-native';

declare global {
  var __SyncFile_readTextFile: ((path: string) => string) | undefined;
  var __SyncFile_readBinaryFile: ((path: string) => ArrayBuffer) | undefined;
  var __SyncFile_existsSync: ((path: string) => boolean) | undefined;
}

let installed = false;

function ensureInstalled(): void {
  if (installed || typeof global.__SyncFile_readTextFile === 'function') {
    installed = true;
    return;
  }
  const result = NativeModules.SyncFile?.install();
  if (!result) {
    throw new Error(
      'react-native-sync-file: failed to install JSI bindings. ' +
      'Make sure you have rebuilt the native app after installing the package.'
    );
  }
  installed = true;
}

/** Read an entire file as a UTF-8 string (synchronous). */
export function readTextFileSync(path: string): string {
  ensureInstalled();
  return global.__SyncFile_readTextFile!(path);
}

/** Read an entire file into an ArrayBuffer (synchronous). */
export function readBinaryFileSync(path: string): ArrayBuffer {
  ensureInstalled();
  return global.__SyncFile_readBinaryFile!(path);
}

/** Check if a file or directory exists (synchronous). */
export function existsSync(path: string): boolean {
  ensureInstalled();
  return global.__SyncFile_existsSync!(path);
}
