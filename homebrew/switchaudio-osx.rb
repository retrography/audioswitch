class SwitchaudioOsx < Formula
  desc "Change OS X audio source from the command-line"
  homepage "https://github.com/retrography/switchaudio-osx/"
  #url "https://github.com/retrography/switchaudio-osx/archive/1.5.0.tar.gz"
  #sha256 "085dc50577a27cc92b0fcbe0266eaa9d41251e14c78dab4732dce23b842d75db"
  head "https://github.com/retrography/switchaudio-osx.git"

  depends_on :macos => :lion
  depends_on :xcode => :build

  def install
    xcodebuild "-project", "AudioSwitcher.xcodeproj",
               "-target", "SwitchAudioSource",
               "SYMROOT=build",
               "-verbose"
    prefix.install Dir["build/Release/*"]
    bin.write_exec_script "#{prefix}/SwitchAudioSource"
    chmod 0755, "#{bin}/SwitchAudioSource"
  end

  test do
    system "#{bin}/SwitchAudioSource", "-c"
  end
end
