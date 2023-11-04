#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2023 Harald Sitter <sitter@kde.org>
# SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

import unittest
from typing import Final

import gi
from appium import webdriver
from appium.options.common.base import AppiumOptions
from appium.webdriver.common.appiumby import AppiumBy
from selenium.webdriver.support.ui import WebDriverWait

gi.require_version('Gdk', '3.0')
from gi.repository import Gdk

EVDEV_OFFSET: Final = 8
WIDGET_ID: Final = "org.kde.plasma.volume"


def keyval_to_keycode(key_val: int) -> int:
    """
    @param key_val see https://www.cl.cam.ac.uk/~mgk25/ucs/keysymdef.h
    """
    match key_val:
        case 0xffe1:  # XK_Shift_L
            return 42 + EVDEV_OFFSET
        case 0xffe9:  #XK_Alt_L
            return 56 + EVDEV_OFFSET
        case 0xffe3:  # XK_Control_L
            return 29 + EVDEV_OFFSET

    keymap = Gdk.Keymap.get_default()
    ret, keys = keymap.get_entries_for_keyval(key_val)
    if not ret:
        raise RuntimeError("Failed to map key!")
    return keys[0].keycode


class DesktopTest(unittest.TestCase):
    """
    Tests for the desktop package
    """

    driver: webdriver.Remote

    @classmethod
    def setUpClass(cls) -> None:
        """
        Initializes the webdriver
        """
        options = AppiumOptions()
        options.set_capability("app", f"plasmawindowed -p org.kde.plasma.nano {WIDGET_ID}")
        options.set_capability("environ", {
            "QT_FATAL_WARNINGS": "1",
            "QT_LOGGING_RULES": "qt.accessibility.atspi.warning=false;kf.plasma.core.warning=false;kf.windowsystem.warning=false;kf.kirigami.platform.warning=false",
        })
        options.set_capability("timeouts", {'implicit': 30000})
        cls.driver = webdriver.Remote(command_executor='http://127.0.0.1:4723', options=options)

    def tearDown(self) -> None:
        """
        Take screenshot when the current test fails
        """
        if not self._outcome.result.wasSuccessful():
            self.driver.get_screenshot_as_file(f"failed_test_shot_plasmashell_#{self.id()}.png")

    @classmethod
    def tearDownClass(cls) -> None:
        """
        Make sure to terminate the driver again, lest it dangles.
        """
        cls.driver.quit()

    def test_0_open(self) -> None:
        """
        Tests the widget can be opened
        """
        self.driver.find_element(AppiumBy.NAME, "No output or input devices found")


if __name__ == '__main__':
    unittest.main()
