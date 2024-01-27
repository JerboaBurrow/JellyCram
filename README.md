### Signing/Verifying your Download

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

On windows 

- Check md5sum ```md5sum JellyCram.exe```
- Check GPG ```gpg --verify JellyCram.exe.sig```

### License

- The application code (directories: android and desktop) is GPL'd 
- **But the [rendering engine (jGL)](https://github.com/JerboaBurrow/jGL), and [physics, ecs, etc. (Hop)](https://github.com/JerboaBurrow/Hop) are MIT!** 

