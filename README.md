# OpenBSD iked

Mirror of OpenBSD's IKEv2 implementation -- [iked(8)](https://man.openbsd.org/iked) and [ikectl(8)](https://man.openbsd.org/ikectl.8) with my hacks.

## branches

- `openbsd`: mirror of:
  - https://github.com/openbsd/src/commits/master/sbin/iked
  - https://github.com/openbsd/src/commits/master/usr.sbin/ikectl
- `master`: `openbsd` with my hacks
  - Always rebased on `openbsd` branch and frequently force-pushed

## How to cherry-picked commits from OpenBSD's source on GitHub

TODO

- `git format-patch -k --stdout 45ae9d61c58e~ -- sbin/iked/ usr.sbin/ikectl/ > ~/patch.orig`
- vscode
- `git am ~/patch`

## Build on Ubuntu

WIP

```sh
# libbsd-dev for tree.h
sudo apt install bmake libbsd-dev
```

TODO: libimsg libressl

## WIP

- Build on Linux
- CI
  - Tests on user-mode Linux like in strongSwan
- AFL
