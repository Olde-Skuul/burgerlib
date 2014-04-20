#!/bin/bash
cd $0/..

# Get the current directory
SCRIPTDIR="$( cd "$( dirname "$0" )" && pwd )"

# Make sure the environment variables are set

# The standard way is to add the variables to ~/.MacOSX/environment.plist
if [ ! -d ~/.MacOSX ]; then mkdir ~/.MacOSX; fi
defaults write ~/.MacOSX/environment SDKS -string "$SCRIPTDIR/sdks"
defaults write ~/.MacOSX/environment P4CONFIG -string "p4config"
plutil -convert binary1 ~/.MacOSX/environment.plist 

# Fix for MacOSX Lion 10.7.4 to reenable environment variables
sudo touch /var/db/.launchd_allow_global_dyld_envvars

# MacOSX Mountain Lion doesn't use ~/.MacOSX/environment
# Use /etc/launchd.conf instead
echo setenv SDKS "$SCRIPTDIR/sdks" > launchd.conf.tmp
echo setenv P4CONFIG p4config >> launchd.conf.tmp
sudo cp launchd.conf.tmp /private/etc/launchd.conf

# Install python argparse for earlier versions of Mac OS X
sudo easy_install argparse

# cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.5.sdk/usr/include/c++/4.2.1
# sudo ln -s i686-apple-darwin9 i686-apple-darwin11
# sudo ln -s x86_64-apple-darwin9 x86_64-apple-darwin11
# sudo ln -s powerpc-apple-darwin9 powerpc-apple-darwin11
# sudo ln -s powerpc64-apple-darwin9 powerpc64-apple-darwin11
# cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.5.sdk/usr/lib
# sudo ln -s libstdc++.6.0.4.dylib libstdc++.dylib