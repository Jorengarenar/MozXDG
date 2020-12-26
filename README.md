MozXDG
======

XDG wrapper for Mozilla programs (Firefox and Thunderbird)

## Build

```sh
git clone --recursive https://github.com/Jorengarenar/MozXDG.git
cd MozXDG
make
```

## Install

By default `mozxdg` is installed into `~/.local/bin`. If you wish to install it
into another directory (doesn't even need to be in in your `PATH` necessarily),
then specify it in the following way:
```sh
make DESTDIR=the/path/to/my/dir install
```

### Linking

Now you need to link it to overlay the programs you want to wrap.

Important part here is `LINK_DIR` which must be listed in `PATH` environment
variable before the directory which contains actual programs, e.g:
```sh
$ echo "$PATH"
/path/to/dir/with/my/wrappers:/usr/bin/firefox
```

#### All

Links for all default programs (i.e. Firefox and Thunderbird)

```sh
make link-all LINK_DIR=/path/to/dir/with/my/wrappers
```

#### Firefox
```sh
make link-firefox LINK_DIR=/path/to/dir/with/my/wrappers
```

#### Thunderbird
```sh
make link-thunderbird LINK_DIR=/path/to/dir/with/my/wrappers
```

#### Other

If you want this wrapper to overlay another executable (in example `foo`), then:

```sh
make link-other LINK_DIR=path/to/dir/with/my/wrappers WRAPPED_BIN=foo
```

### Before first use

Move your profile file from `~/.mozilla` to `$XDG_DATA_HOME`. Example for Firefox:
```sh
mv ~/.mozilla/firefox/*.default-release $XDG_DATA_HOME/firefox
```

Then remove `~/.mozilla` (or `mv ~/.mozilla $XDG_DATA_HOME/mozilla`)

## Usage

MozXDG can be set into two modes via `MOZXDG_MODE` environment variable:
- Mode `1` (default) - temporally links `$XDG_DATA_HOME/mozilla` as `~/.mozilla`,
                       then unlinks after program finishes
- Mode `2` - constantly removes `~/.mozilla`
- _more modes upon request_

When you run binary other than `firefox` or `thunderbird`, you need to specify
the name of profile directory in `$XDG_DATA_HOME` via `MOZXDG_PROFILE` variable.

## Why?

Because clean `$HOME` is good `$HOME`.

Why not just shell script? From my observations, it just too slow. I use RSS and
often find myself opening multiple URLs at once (with one keypress). With shell
script part of them is opened via wrapper, while other part gets bugged out and
opens in "regular" Firefox.
