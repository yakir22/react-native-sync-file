/**
 * JSI bindings for synchronous file access.
 * Uses POSIX APIs (open/read/fstat/close/access) which work on both iOS and Android.
 */
#include "SyncFileBridge.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <cerrno>
#include <cstring>

using namespace facebook;

namespace syncfile {

static std::vector<uint8_t> readFileBytes(const std::string& path) {
  int fd = ::open(path.c_str(), O_RDONLY);
  if (fd < 0) {
    throw std::runtime_error("Cannot open file: " + path + " (" + std::strerror(errno) + ")");
  }

  struct stat st;
  if (::fstat(fd, &st) != 0) {
    ::close(fd);
    throw std::runtime_error("Cannot stat file: " + path);
  }

  size_t size = static_cast<size_t>(st.st_size);
  std::vector<uint8_t> data(size);

  size_t offset = 0;
  while (offset < size) {
    ssize_t n = ::read(fd, data.data() + offset, size - offset);
    if (n <= 0) break;
    offset += n;
  }
  ::close(fd);

  return data;
}

void install(jsi::Runtime& runtime) {
  // readTextFile(path: string): string
  auto readTextFile = jsi::Function::createFromHostFunction(
    runtime,
    jsi::PropNameID::forAscii(runtime, "__SyncFile_readTextFile"),
    1,
    [](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, size_t count) -> jsi::Value {
      if (count < 1 || !args[0].isString()) {
        throw jsi::JSError(rt, "readTextFile: expected string path");
      }
      auto path = args[0].asString(rt).utf8(rt);
      try {
        auto data = readFileBytes(path);
        return jsi::String::createFromUtf8(rt, data.data(), data.size());
      } catch (const std::exception& e) {
        throw jsi::JSError(rt, e.what());
      }
    }
  );
  runtime.global().setProperty(runtime, "__SyncFile_readTextFile", std::move(readTextFile));

  // readBinaryFile(path: string): ArrayBuffer
  auto readBinaryFile = jsi::Function::createFromHostFunction(
    runtime,
    jsi::PropNameID::forAscii(runtime, "__SyncFile_readBinaryFile"),
    1,
    [](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, size_t count) -> jsi::Value {
      if (count < 1 || !args[0].isString()) {
        throw jsi::JSError(rt, "readBinaryFile: expected string path");
      }
      auto path = args[0].asString(rt).utf8(rt);
      try {
        auto data = readFileBytes(path);

        // Create ArrayBuffer via JS constructor and memcpy into it
        jsi::Function arrayCtor = rt.global().getPropertyAsFunction(rt, "ArrayBuffer");
        jsi::Object arrayBuffer = arrayCtor.callAsConstructor(rt, static_cast<int>(data.size())).asObject(rt);
        jsi::ArrayBuffer jsBuf = arrayBuffer.getArrayBuffer(rt);
        memcpy(jsBuf.data(rt), data.data(), data.size());

        return arrayBuffer;
      } catch (const std::exception& e) {
        throw jsi::JSError(rt, e.what());
      }
    }
  );
  runtime.global().setProperty(runtime, "__SyncFile_readBinaryFile", std::move(readBinaryFile));

  // existsSync(path: string): boolean
  auto existsSyncFn = jsi::Function::createFromHostFunction(
    runtime,
    jsi::PropNameID::forAscii(runtime, "__SyncFile_existsSync"),
    1,
    [](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, size_t count) -> jsi::Value {
      if (count < 1 || !args[0].isString()) {
        throw jsi::JSError(rt, "existsSync: expected string path");
      }
      auto path = args[0].asString(rt).utf8(rt);
      return jsi::Value(::access(path.c_str(), F_OK) == 0);
    }
  );
  runtime.global().setProperty(runtime, "__SyncFile_existsSync", std::move(existsSyncFn));
}

} // namespace syncfile
