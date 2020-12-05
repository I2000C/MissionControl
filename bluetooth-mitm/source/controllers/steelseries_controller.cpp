/*
 * Copyright (C) 2020 ndeadly
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "steelseries_controller.hpp"
#include <stratosphere.hpp>

namespace ams::controller {

    namespace {

        const constexpr float stick_scale_factor = float(UINT12_MAX) / UINT8_MAX;

    }

    void SteelseriesController::UpdateControllerState(const bluetooth::HidReport *report) {
        auto steelseries_report = reinterpret_cast<const SteelseriesReportData *>(&report->data);

        switch(steelseries_report->id) {
            case 0x01:
                this->HandleInputReport0x01(steelseries_report);
                break;
            default:
                // Todo: handle this properly
                this->HandleMfiInputReport(steelseries_report);
                break;
        }
    }

    void SteelseriesController::HandleInputReport0x01(const SteelseriesReportData *src) {
        m_left_stick = this->PackStickData(
            static_cast<uint16_t>(stick_scale_factor * -static_cast<int8_t>(~src->input0x01.left_stick.x + 1) + 0x7ff) & 0xfff,
            static_cast<uint16_t>(stick_scale_factor * (UINT8_MAX + static_cast<int8_t>(~src->input0x01.left_stick.y + 1)) + 0x7ff) & 0xfff
        );
        m_right_stick = this->PackStickData(
            static_cast<uint16_t>(stick_scale_factor * -static_cast<int8_t>(~src->input0x01.right_stick.x + 1) + 0x7ff) & 0xfff,
            static_cast<uint16_t>(stick_scale_factor * (UINT8_MAX + static_cast<int8_t>(~src->input0x01.right_stick.y + 1)) + 0x7ff) & 0xfff
        );

        m_buttons.dpad_down   = (src->input0x01.dpad == SteelseriesDPad_S)  ||
                                (src->input0x01.dpad == SteelseriesDPad_SE) ||
                                (src->input0x01.dpad == SteelseriesDPad_SW);
        m_buttons.dpad_up     = (src->input0x01.dpad == SteelseriesDPad_N)  ||
                                (src->input0x01.dpad == SteelseriesDPad_NE) ||
                                (src->input0x01.dpad == SteelseriesDPad_NW);
        m_buttons.dpad_right  = (src->input0x01.dpad == SteelseriesDPad_E)  ||
                                (src->input0x01.dpad == SteelseriesDPad_NE) ||
                                (src->input0x01.dpad == SteelseriesDPad_SE);
        m_buttons.dpad_left   = (src->input0x01.dpad == SteelseriesDPad_W)  ||
                                (src->input0x01.dpad == SteelseriesDPad_NW) ||
                                (src->input0x01.dpad == SteelseriesDPad_SW);

        m_buttons.A = src->input0x01.buttons.B;
        m_buttons.B = src->input0x01.buttons.A;
        m_buttons.X = src->input0x01.buttons.Y;
        m_buttons.Y = src->input0x01.buttons.X;

        m_buttons.R = src->input0x01.buttons.R;
        m_buttons.L = src->input0x01.buttons.L;

        m_buttons.minus = src->input0x01.buttons.select;
        m_buttons.plus  = src->input0x01.buttons.start;       
    }

    void SteelseriesController::HandleMfiInputReport(const SteelseriesReportData *src) {
        m_left_stick = this->PackStickData(
            static_cast<uint16_t>(stick_scale_factor * -static_cast<int8_t>(~src->input_mfi.left_stick.x + 1) + 0x7ff) & 0xfff,
            static_cast<uint16_t>(stick_scale_factor * (-static_cast<int8_t>(~src->input_mfi.left_stick.y + 1)) + 0x7ff) & 0xfff
        );
        m_right_stick = this->PackStickData(
            static_cast<uint16_t>(stick_scale_factor * -static_cast<int8_t>(~src->input_mfi.right_stick.x + 1) + 0x7ff) & 0xfff,
            static_cast<uint16_t>(stick_scale_factor * (-static_cast<int8_t>(~src->input_mfi.right_stick.y + 1)) + 0x7ff) & 0xfff
        );

        m_buttons.dpad_up    = src->input_mfi.buttons.dpad_up > 0;
        m_buttons.dpad_right = src->input_mfi.buttons.dpad_right > 0;
        m_buttons.dpad_down  = src->input_mfi.buttons.dpad_down > 0;
        m_buttons.dpad_left  = src->input_mfi.buttons.dpad_left > 0;

        m_buttons.A = src->input_mfi.buttons.A > 0;
        m_buttons.B = src->input_mfi.buttons.B > 0;
        m_buttons.X = src->input_mfi.buttons.X > 0;
        m_buttons.Y = src->input_mfi.buttons.Y > 0;

        m_buttons.R  = src->input_mfi.buttons.R1 > 0;
        m_buttons.L  = src->input_mfi.buttons.L1 > 0;
        m_buttons.ZR = src->input_mfi.buttons.R2 > 0;
        m_buttons.ZL = src->input_mfi.buttons.L2 > 0;

        m_buttons.home = src->input_mfi.buttons.menu;
    }

}
