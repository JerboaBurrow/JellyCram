![jelly cram feature-graphics](https://github.com/JerboaBurrow/JellyCram/blob/main/common/jellycram-feature.png)

[![Linux build](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-linux.yml/badge.svg)](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-linux.yml) [![macOS build](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-macos.yml/badge.svg)](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-macos.yml) [![Windows build](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-windows.yml/badge.svg)](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-windows.yml) [![Android build](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-android.yml/badge.svg)](https://github.com/JerboaBurrow/JellyCram/actions/workflows/build-android.yml)

## JellyCram

### A Tetris inspired jelly-physics game with button mashing elements. Free and open source.

### Currently beta testing on itch.io and Google Play

[![Itch.io](https://img.shields.io/badge/Itch-%23FF0B34.svg?style=for-the-badge&logo=Itch.io&logoColor=white)](https://jerboa-app.itch.io/jelly-cram) <a href="https://jerboa.app/jellycram">
<img height="32px" src="https://github.com/JerboaBurrow/JellyCram/assets/84378622/a6d84072-6979-4cb9-9251-a9f1bc0b9926">
</a>

ℹ️ Fill the board with tessellating jelly-ominoes to earn points. 

ℹ️ Control the descent of each piece and its rotation to line them up perfectly.

ℹ️ Keep an eye on the Jiggleometer - pieces must be settled to get deleted!

ℹ️ if a piece goes over the top, it's game over. Clear pieces by filling rows, and keeping the Jiggleometer low!

Play now on Linux 🐧, macOS 🍎, Windows 🪟, and Android 🤖

[Catch a release here](https://github.com/JerboaBurrow/JellyCram/releases)

### Settings

#### Desktop

- By pressing the menu button (three stacked rounded boxes, or the word menu) you may rebind all keys however you wish.
- On Linux and Windows a file ```settings.json``` contains key-bindings and a tutorial completion flag. These may be edited.
- On macOS the settings are stored in ```~Library/Application Support/app.jerboa.jellycram/settings.json```

#### Android

- By pressing the menu button (three stacked rounded boxes, or the word menu) you may invert tap and swipe controls and use the screen or object centric controls.
- In screen centric (not inverted) tapping below the middle pushes up. In object centric tapping below the *current piece* pushes up instead.

---

### Signing/Verifying your Download

The Android build is code-signed, with the same key as used on the Google Play store.

The desktop code is **signed with GPG**, and **md5sums are provided**. However for macOS and Windows additional signing is required for Smart Screen and macOS GateKeeper, which is as yet not planned (costs $100's/year)

#### Windows SmartScreen/User Access Control - "Unkown publisher"

- The code is not yet signed for Windows, you may run by ```left-click``` ```run anyway```. You may need to click ```more info```

#### macOS - "Unidentified developer"

- The code is not yet signed for macOS, you may run by ```control-click`` ```open```, and selecting ```open``` in the pop up. You may have to ```control-click`` ```open``` twice.


#### GPG keys

The games key is hosted on keyserver.ubuntu.com, you may obtain it in gpg like so:

```gpg --keyserver keyserver.ubuntu.com -v --recv-keys 18e4f6b191de23e7978be4b46534065e085babba```

You should see something like:

```
gpg: pub  rsa3072/6534065E085BABBA 2024-01-26  Jerboa-app (JellyCram) <dev@jerboa.app>
gpg: key 6534065E085BABBA: "Jerboa-app (JellyCram) <dev@jerboa.app>" ...
...
```

#### Ways to verify

The files (executable and packed scripts) are provided with md5sums, and GPG signatures. The md5sums are contained within the file ```JellyCram.md5```

On linux or macOS you may verify as follows

- Check md5sum ```md5sum JellyCram```
- Check GPG ```gpg --verify JellyCram.sig```

To install md5sum on macOS with [brew](https://formulae.brew.sh/formula/md5sha1sum) ```brew install md5sha1sum```

On windows 

- Check md5sum ```md5sum JellyCram.exe```
- Check GPG ```gpg --verify JellyCram.exe.sig```

### License

- The application code (directories: android and desktop) is GPL'd 
- **But the [rendering engine (jGL)](https://github.com/JerboaBurrow/jGL), and [physics, ecs, etc. (Hop)](https://github.com/JerboaBurrow/Hop) are MIT!** 

