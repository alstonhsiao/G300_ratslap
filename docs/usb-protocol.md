# USB Protocol Reference #

Low-frequency deep reference for the Logitech G300/s USB HID protocol that
RatSlap uses to configure the mouse. Pull this out only when working on
USB comms, button mapping internals, or protocol reverse-engineering.

## Device Identity

Logitech G300/s USB IDs: vendor `0x046d`, product `0xc246` (defined in
`src/main.c`). The G300 and G300s are functionally identical (same VID:PID).

## Existing Reference Files in Repo

| File                                        | Contents                                                       |
|---------------------------------------------|----------------------------------------------------------------|
| `docs/G300s_USB_sniffing.txt`               | Raw usbmon captures, control message decoding, byte-level field layout for button sets, DPI, colour, report rate, modifier/key codes |
| `docs/G300s_Default_Configuration.txt`      | Default factory output of `--print F3/F4/F5` (colour, report rate, DPI, button mappings) |

## Protocol Summary (from sniffing captures)

The mouse uses HID Set Report control messages (`bmRequestType 0x21`,
`bRequest 0x09`) with feature report IDs `0xF0`–`0xF5`:

- `0xF0` — mode/command selector (start edit, launch editor, etc.)
- `0xF1` — finish/reset (2-byte payload)
- `0xF2` — unknown / set idle companion (2-byte payload)
- `0xF3` / `0xF4` / `0xF5` — button set for mode F3 / F4 / F5 (35-byte payload)

### 35-Byte Button Set Layout (report `0xF5` example)

| Offset | Field            | Encoding                                                    |
|--------|------------------|-------------------------------------------------------------|
| 0      | Button set ID    | `F3` / `F4` / `F5`                                          |
| 1      | LED colour       | `00` black, `01` red, `02` green, `03` yellow, `04` blue, `05` magenta, `06` cyan, `07` white |
| 2      | Report rate      | `00`=1000Hz, `01`=125Hz, `02`=250Hz, `03`=500Hz             |
| 3–6    | DPI positions 1–4| High nibble: `8`=default, `0`=not default. Low nibble: DPI step (1–10 mapping 250–2500 dpi) |
| 7      | Shift DPI point  | `40`=none, `01`–`0a`=250–2500 dpi                           |
| 8–9    | Button 1 (Left)  | 3 bytes: misc code + modifier bitmap + key code             |
| 10–11  | Button 2 (Right) | same layout                                                  |
| 12–13  | Button 3 (Mid)   | same layout                                                  |
| 14–15  | Button 4 (G4)    | same layout                                                  |
| 16–17  | Button 5 (G5)    | same layout                                                  |
| 18–20  | G6               | same layout                                                  |
| 21–23  | G7               | same layout                                                  |
| 24–26  | G8               | same layout                                                  |
| 27–29  | G9               | same layout                                                  |

### Button Assignment Byte Layout (per button, 3 bytes)

| Byte | Purpose      | Values                                                       |
|------|--------------|--------------------------------------------------------------|
| 0    | Misc/action  | `01`–`09`=Button1–11, `0a`=DPI Up, `0b`=DPI Down, `0c`=DPI Cycle, `0d`=Mode Switch, `0e`=DPI Shift, `0f`=DPI Default |
| 1    | Modifier bitmap | `01`=LCtrl, `02`=LShift, `04`=LAlt, `08`=LSuper, `10`=RCtrl, `20`=RShift, `40`=RAlt, `80`=RSuper |
| 2    | Key code     | USB HID usage codes (see `G300s_USB_sniffing.txt` for full list) |

Scroll wheel (buttons 4 and 5) cannot be remapped.

## Adding a New Key/Button Name

Update the key table in `src/main.c`. Key codes come from the USB HID usage
table; see links in `README.md` under "Key names" and the full byte-level
listing in `docs/G300s_USB_sniffing.txt`.