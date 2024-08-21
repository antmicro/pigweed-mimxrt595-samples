extern "C" {
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"

#include "usb_device_descriptor.h"
#include "virtual_com.h"
}

#include "fastboot.h"
#include "pw_thread/thread.h"
#include "pw_thread/detached_thread.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"

#define APP_TASK_STACK_SIZE 5000L

extern usb_cdc_vcom_struct_t s_cdcVcom;
static pw::thread::freertos::StaticContextWithStack<APP_TASK_STACK_SIZE / sizeof(portSTACK_TYPE)> s_thread_static_ctx = {};
static pw::thread::freertos::Options s_thread_opts = pw::thread::freertos::Options()
    .set_name("usb_cdc_vcom")
    .set_priority(4)
    .set_stack_size(APP_TASK_STACK_SIZE / sizeof(portSTACK_TYPE))
    .set_static_context(s_thread_static_ctx);

namespace pw::system {

void UserAppInit() {
    pw::thread::DetachedThread(s_thread_opts, [] {
        APPTask(&s_cdcVcom);
    });
}

}  // namespace pw::system