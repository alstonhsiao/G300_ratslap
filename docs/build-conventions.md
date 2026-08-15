# Build & Code Conventions #

Low-frequency reference for building RatSlap and following its code style.
Pulled from the Hub to keep the entry point lean.

## Build Commands

```bash
make            # builds the `ratslap` binary (runs gitup, generates git.h, log.h, manpage.1)
make clean      # remove build artefacts
make distclean  # clean + remove binaries and archives
make dist       # build a signed distribution tarball
```

## Auto-Generated Files

The build auto-generates several files from templates. **Do not edit the
generated files directly** — edit the `.TEMPLATE` sources instead.

| Generated              | Template                                    | Purpose                                    |
|------------------------|---------------------------------------------|--------------------------------------------|
| `src/git.h`            | `templates/git.h.TEMPLATE`                  | Version/build metadata from git            |
| `src/log.h`            | `templates/log.h.TEMPLATE`                  | Logging macros derived from build options  |
| `manpage.1`            | `templates/manpage.1.TEMPLATE`              | Man page                                   |
| `make.options.conf`    | `templates/make.options.conf.DEFAULT`       | Build-time debug options                   |

Build options (debug flags) live in `make.options.conf` (root), copied from
`templates/make.options.conf.DEFAULT` on first build. Available options:
`DEBUG`, `DEBUG_USB`, `DEBUG_PARSE`, `DEBUG_KEY`.

### USB Delay Tuning (Performance)

The following compile-time macros control USB settle delays (in
microseconds). All have safe defaults matching the original hard-coded
values. Override them in `make.options.conf` via `-D` flags to tune for
your hardware.

| Macro                      | Default   | Controls                              |
|----------------------------|-----------|---------------------------------------|
| `USB_DELAY_MODE_SAVE`      | `500000`  | Settle time after mode write          |
| `USB_DELAY_MODE_LOAD`      | `10000`   | Settle time after mode read           |
| `USB_DELAY_EDITMODE_STEP`  | `50000`   | Per-step delay in mouse_editmode      |
| `USB_DELAY_EDITMODE_LONG`  | `500000`  | Long settle in mouse_editmode         |

Example `make.options.conf` entry:

```
OPTIONS += -DUSB_DELAY_MODE_SAVE=100000
```

### Read-Back Verification

After `mode_save`, the tool reads back the mode data and compares it to
verify the write succeeded. This can be controlled at runtime and compile
time:

- **Runtime:** `--verify` (default) / `--no-verify` CLI flag.
- **Compile time:** `-DNO_VERIFY_SAVE` in `make.options.conf` to disable
  by default. `-DVERIFY_SAVE_DEFAULT=0` for the same effect.

## Compile Flags

`-O2 -pipe -Wall -Werror -ggdb`

Warnings are treated as errors. New code must compile cleanly under
`-Wall -Werror`.

## Code Style

- **Indentation:** 4 spaces, no tabs; modeline `ts=4 sw=4 tw=80 cindent` with
  `cino=(0,ml,\:0` (K&R brace style, continuation indented under paren).
- **Line length:** 80 columns.
- **Header guards:** `#ifndef APP_H` / `#define APP_H` style (suffix `_H`).
- **Copyright header:** Every C/Makefile file carries the RatSlap GPL v2
  header. New files should include it.
- **Vim modeline:** Present at the top of every source file (under `src/`) —
  preserve it.
- **Source layout:** C source lives in `src/`, build templates in `templates/`,
  documentation in `docs/`. Object files (`.o`) are generated in `src/`
  alongside their `.c` files. Generated headers (`git.h`, `log.h`) land in
  `src/`. The binary, `manpage.1`, and `make.options.conf` stay at root.