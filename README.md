# react-native-sync-file

Synchronous file access for React Native via JSI. Three functions, ~50 lines of C++ (POSIX), zero dependencies beyond React Native.

## Why

Some libraries (e.g., routing engines, binary parsers) need synchronous file I/O. React Native's built-in file APIs are all async. This package provides sync file reads via JSI — native C++ calls with no bridge overhead.

## Install

```bash
npm install react-native-sync-file
cd ios && pod install
```

Rebuild the native app after installing.

## API

```typescript
import { readTextFileSync, readBinaryFileSync, existsSync } from 'react-native-sync-file';

// Read a text file as UTF-8 string
const text = readTextFileSync('/path/to/file.txt');

// Read a binary file into an ArrayBuffer
const buffer = readBinaryFileSync('/path/to/data.bin');

// Check if a file exists
if (existsSync('/path/to/file')) { ... }
```

All functions are synchronous — they block until the I/O completes. Use them for files that are already on local storage (not network).

## Example: brouter-ts IFileSystem

This was built for [brouter-ts](https://github.com/yakir22/brouter-ts), which needs synchronous random-access reads of RD5 segment files. Here's a complete `IFileSystem` implementation with lazy loading and caching:

```typescript
import { Buffer } from 'buffer';
import { readTextFileSync, readBinaryFileSync, existsSync } from 'react-native-sync-file';
import type { IFileSystem, IBinaryFileHandle } from 'brouter-ts';

class MemoryBinaryFileHandle implements IBinaryFileHandle {
  readonly size: number;
  private data: Buffer;

  constructor(data: Buffer) {
    this.data = data;
    this.size = data.length;
  }

  read(buffer: Uint8Array, bufferOffset: number, length: number, filePosition: number): number {
    const slice = this.data.subarray(filePosition, filePosition + length);
    (buffer as Buffer).set(slice, bufferOffset);
    return slice.length;
  }

  close(): void {}
}

export class RNFileSystem implements IFileSystem {
  // RD5 files cached after first load — reused across route calculations
  private binaryCache = new Map<string, Buffer>();

  readText(path: string): string {
    return readTextFileSync(path);
  }

  openBinary(path: string): IBinaryFileHandle {
    let data = this.binaryCache.get(path);
    if (!data) {
      const arrayBuffer = readBinaryFileSync(path);
      data = Buffer.from(arrayBuffer);
      this.binaryCache.set(path, data);
    }
    return new MemoryBinaryFileHandle(data);
  }

  exists(path: string): boolean {
    return existsSync(path);
  }
}
```

Usage:
```typescript
import { BRouterService } from 'brouter-ts';
import { RNFileSystem } from './RNFileSystem';

const router = new BRouterService('/path/to/BrouterFiles', new RNFileSystem());
const track = router.findRoute('trekking', fromLon, fromLat, toLon, toLat);
router.close();
```

RD5 tiles are loaded one at a time as the routing engine needs them (not all at startup).

## How It Works

- **C++ (shared):** POSIX `open`/`read`/`fstat`/`close`/`access` — works on both iOS and Android
- **iOS:** ObjC++ bridge module installs JSI functions on the JS runtime
- **Android:** JNI adapter calls the same C++ installer via `JavaScriptContextHolder`
- **JS:** Global functions (`__SyncFile_*`) are auto-installed on first use via `NativeModules.SyncFile.install()`

## Requirements

- React Native >= 0.71.0 (JSI support)
- iOS 13+
- Android minSdk 21+
