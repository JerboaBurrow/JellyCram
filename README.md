### Signing/Verifying your Download

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

