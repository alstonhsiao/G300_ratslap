#ifndef HID_COMPAT_H
#define HID_COMPAT_H

/* Compatibility shim for platforms without <linux/hid.h>.
 * RatSlap only uses HID_REQ_SET_REPORT and HID_REQ_GET_REPORT,
 * which are USB HID class specification constants, not Linux-specific. */

#ifndef HID_REQ_SET_REPORT
#define HID_REQ_SET_REPORT 0x09
#endif

#ifndef HID_REQ_GET_REPORT
#define HID_REQ_GET_REPORT 0x01
#endif

#endif /* HID_COMPAT_H */