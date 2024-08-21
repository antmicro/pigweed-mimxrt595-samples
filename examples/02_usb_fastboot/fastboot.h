#ifndef _FASTBOOT_H_
#define _FASTBOOT_H_ 1

#include <stddef.h>
#include <stdint.h>

/* USB VID/PIDs for fastboot devices */
#define FASTBOOT_VID (0x18D1u)
#define FASTBOOT_PID (0xD00Du)

/* To be detected as a fastboot device, the interface descriptor
 * of the USB device must present the following class values. */
#define FASTBOOT_USB_IFC_CLASS (0xFFU)
#define FASTBOOT_USB_IFC_SUBCLASS (0x42U)
#define FASTBOOT_USB_IFC_PROTOCOL (0x03U)

#ifdef __cplusplus
extern "C" {
#endif

/** Enqueue a fastboot packet to send back to the host.
 *
 *  This must be implemented by the underlying transport. The
 *  packet is only enqueued but not actually sent. When the packet
 *  was actually sent to the host, the transport must call
 *  OnFastbootPacketSent
 */
int FastbootSendPacket(uint8_t const* buf, size_t len);

/** Inform that a fastboot packet was received from the host.
 *
 *  This must be called by the underlying transport.
 */
void OnFastbootPacketReceived(uint8_t const* buf, size_t len);

/** Inform that a previously queued packet was sent to the host.
 *
 *  This must be called by the underlying transport.
 */
void OnFastbootPacketSent(uint8_t const* buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif