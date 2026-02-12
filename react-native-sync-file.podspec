Pod::Spec.new do |s|
  s.name         = "react-native-sync-file"
  s.version      = "1.0.0"
  s.summary      = "Synchronous file access for React Native via JSI"
  s.homepage     = "https://github.com/yakir22/react-native-sync-file"
  s.license      = "MIT"
  s.author       = "Yakir"
  s.platforms    = { :ios => "13.0" }
  s.source       = { :git => "https://github.com/yakir22/react-native-sync-file.git", :tag => s.version }
  s.source_files = "ios/**/*.{h,m,mm}", "cpp/**/*.{h,cpp}"

  s.dependency "React-Core"
  s.dependency "React-callinvoker"

  s.pod_target_xcconfig = {
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++17"
  }
end
