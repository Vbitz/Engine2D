require 'formula'

# Homebrew Formula for V8 updated to the version Engine2D is using

# When trunk is 3.x, then 3.x is devel and 3.(x-1)
# is stable.
# https://code.google.com/p/v8/issues/detail?id=2545
# http://omahaproxy.appspot.com/

class V8 < Formula
  homepage 'http://code.google.com/p/v8/'
  url 'https://github.com/v8/v8/archive/3.28.28.2.tar.gz'
  sha1 'e07c1776d6e1d3d67e646e84fb3c3631ed8d668a'

  option 'with-readline', 'Use readline instead of libedit'

  # not building on Snow Leopard:
  # https://github.com/Homebrew/homebrew/issues/21426
  depends_on :macos => :lion

  depends_on :python => :build # gyp doesn't run under 2.6 or lower
  depends_on 'readline' => :optional

  resource 'gyp' do
    url 'http://gyp.googlecode.com/svn/trunk', :revision => 1831
    version '1831'
  end

  def install
    # Download gyp ourselves because running "make dependencies" pulls in ICU.
    (buildpath/'build/gyp').install resource('gyp')

    system "make", "native",
                   "library=shared",
                   "snapshot=on",
                   "console=readline",
                   "i18nsupport=off"

    prefix.install 'include'
    cd 'out/native' do
      lib.install Dir['lib*']
      bin.install 'd8', 'lineprocessor', 'process', 'shell' => 'v8'
      bin.install Dir['mksnapshot.*']
    end
  end
end
