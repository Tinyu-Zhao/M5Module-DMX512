#include "common.h"
#include "view_receiver.h"
#include "view_sender.h"

QueueHandle_t queue;

enum scene_mode_t {
    mode_select,
    mode_receiver,
    mode_sender,

};

void setup(void) {
    /* Configure the DMX hardware to the default DMX settings and tell the DMX
      driver which hardware pins we are using. */
    dmx_config_t dmxConfig = DMX_DEFAULT_CONFIG;
    dmx_param_config(dmxPort, &dmxConfig);

    dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);

    dmx_driver_install(dmxPort, DMX_MAX_PACKET_SIZE, dmxQueueSize, &queue,
                       dmxInterruptPriority);

    drawSelectSetup();
}

int getBtnIndex(int x, int y) {
    for (int i = 0; i < 2; ++i) {
        if (btns[i].contain(x, y)) {
            return i;
        }
    }
    return -1;
}

int focus_idx = -1;

void loop(void) {
    M5.update();
    switch (scene_mode) {
        case mode_receiver:
            if (!view_receiver.loop()) {
                view_receiver.close();
                drawSelectSetup();
            }
            return;

        case mode_sender:
            if (!view_sender.loop()) {
                view_sender.close();
                drawSelectSetup();
            }
            return;

        default:
            break;
    }

    int clicked_idx = -1;

    auto tp = M5.Touch.getDetail();
    if (tp.wasPressed()) {
        focus_idx = getBtnIndex(tp.base_x, tp.base_y);
    }
    if (tp.wasClicked()) {
        clicked_idx = getBtnIndex(tp.base_x, tp.base_y);
    }

    if (M5.BtnA.wasPressed()) {
        focus_idx = 0;
    } else if (M5.BtnA.wasReleased() && focus_idx == 0) {
        focus_idx = -1;
    }
    if (M5.BtnC.wasPressed()) {
        focus_idx = 1;
    } else if (M5.BtnC.wasReleased() && focus_idx == 1) {
        focus_idx = -1;
    }
    if (M5.BtnA.wasClicked()) {
        clicked_idx = 0;
    }
    if (M5.BtnC.wasClicked()) {
        clicked_idx = 1;
    }
    for (size_t i = 0; i < 2; ++i) {
        btns[i].draw(&M5.Display, i == focus_idx, i == clicked_idx);
    }
    delay(10);

    if (clicked_idx >= 0) {
        scene_mode = (clicked_idx == 0) ? mode_receiver : mode_sender;
        switch (scene_mode) {
            case mode_receiver:
                view_receiver.setup();
                break;

            case mode_sender:
                view_sender.setup();
                break;

            default:
                break;
        }
    }
}
