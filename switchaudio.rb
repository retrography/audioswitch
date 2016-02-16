class Switchaudio < Formula
  desc "Change OS X audio source from the command-line"
  homepage "https://github.com/retrography/audioswitch"
  #url "https://github.com/retrography/switchaudio-osx/archive/1.5.0.tar.gz"
  #sha256 "085dc50577a27cc92b0fcbe0266eaa9d41251e14c78dab4732dce23b842d75db"
  head "https://github.com/retrography/audioswitch.git"

  depends_on :macos => :lion
  depends_on :cmake => :build

  def install
    Dir.mkdir "build"
    Dir.chdir "build" do
      system "cmake", "..", *args
      system "make", "install"
    end
  end

  test do
    system "#{bin}/audioswitch", "-c"
  end
end
