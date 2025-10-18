# OptiSync

A sophisticated 3D optics LED lamp that transforms etched acrylic designs into mesmerizing light displays through edge lighting and intelligent control systems.

## Overview

OptiSync combines precision acrylic etching, 3D printing, and smart embedded systems to create customizable LED lighting experiences. The lamp features real-time control through a user-friendly desktop application, persistent preset storage, and streamlined firmware updates.

## How It Works

The lamp operates on a simple yet elegant principle: acrylic boards are etched with custom shapes or wireframe-styled images. An LED strip positioned directly beneath the board emits light that reflects within the acrylic material, traveling through the etched paths before escaping to illuminate the designs. This edge lighting technique creates striking visual effects with minimal hardware complexity.

## Key Features

- **Real-Time Control**: Adjust brightness, animation speed, and RGB color values on the fly through an intuitive desktop application
- **Pattern Presets**: Choose from pre-configured lighting patterns or create and save custom presets
- **Persistent Memory**: Saved preset can be stored on the ESP32, automatically loading on power-up
- **Easy Firmware Updates**: Simple one-click firmware flashing through the companion application
- **Flexible Power Options**: Compatible with multiple power input methods (Micro USB, USB-A, USB-C) via the additional Modular Power Hub
- **Future-Ready**: OTA (Over-the-Air) updates planned for even more convenient maintenance

## Hardware Components

- **Acrylic Board**: Custom etched with desired designs or wireframe patterns
- **LED Strip**: High-brightness edge-lighting water-proof strip mounted directly beneath the acrylic
- **3D Printed Base & Cover**: Custom-designed enclosure for a polished, professional appearance
- **ESP32 Microcontroller**: Manages LED control and pattern execution
- **TTP223 Touch Sensor**: Capacitive touch button for on/off functionality
- **Modular Power Hub**: Optional power distribution board providing additional power input options (see [Modular-Power-Hub](https://github.com/NickJiEE/Modular-Power-Hub) for details)

## Software

### OptiSync Control Application

A comprehensive Python-based desktop application built with CustomTkinter (compiled to .exe via pyinstaller for user convenience) that provides:

- **Live Controls**: Real-time adjustment of brightness (0-255), animation speed multiplier (0.1-5x), and RGB values (0-255 per channel)
- **Pattern Selection**: Built-in lighting patterns for different vibes, with possibly more in the future updates
- **Preset Management**: Save your current brightness, speed, RGB, and pattern setting to the ESP32's persistent memory; they automatically load on next power-up
- **Firmware Flashing**: Integrated one-click firmware update tool powered by esptool—no external tools required
- **Serial Port Management**: Easy port selection and connection status display
- **Cross-Platform Support**: Built with Python; standalone .exe includes bundled dependencies

#### Installation & Usage

1. Download the latest release package containing:
   - `OptiSync.exe` - Main control application
   - `icon.ico` - Application icon (automatically loaded by the app)
   - `esptool.exe` - Firmware flashing utility (automatically called by the app)

2. Run `OptiSync.exe` to launch the application

3. Select your ESP32 serial port from the dropdown and click **Connect**, if no available port shown, please make sure necessary drivers are downloaded
   - <a href="https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers" target="_blank" rel="noopener noreferrer">CP210x USB to UART Bridge VCP Drivers</a>
   - <a href="https://www.wch-ic.com/downloads/ch341ser_exe.html" target="_blank" rel="noopener noreferrer">CH341 MCU Driver</a>

4. Use the interface to:
   - Drag the brightness slider to control LED intensity
   - Adjust the speed multiplier to change animation frequency
   - Tweak individual R, G, B sliders for color-based patterns (Solid, Smooth Shift, etc.)
   - Click any preset button to instantly apply a lighting pattern
   - Click **💾 Save Preset** to store your current settings to the lamp — they'll persist across power cycles
   - Click **⚡ Flash Firmware** to update the ESP32 code (select a .bin file from the update package)

### ESP32 Firmware

Built on Arduino-compatible C++ using industry-standard libraries:

**Core Libraries**: Adafruit NeoPixel (addressable LED control), ArduinoJson (serial communication), Preferences (non-volatile storage)

**Hardware Interface**:
- GPIO 13: WS2812B LED strip control (NeoPixel)
- GPIO 15: Capacitive touch sensor for on/off toggle
- Serial (115200 baud): Communication with desktop application

**Features**:
- Six dynamic lighting patterns with configurable animations, with possibly more in the future updates
  - **Rainbow**: Smooth color wheel cycling with adjustable speed
  - **Solid**: Static RGB color display
  - **Smooth Shift**: Continuous HSV-based color transitions
  - **Fire Flicker**: Realistic fire/candle effect with random flicker intensity
  - **Waves**: Sine wave intensity modulation across the strip
  - **Pulse Sync**: Synchronized breathing effect across all LEDs
- Real-time parameter control: brightness, speed multiplier, RGB values
- Non-volatile storage: Settings persist in ESP32 flash memory and auto-load on startup
- JSON-based serial protocol: Structured communication with the Python application
- Touch sensor debouncing and state management

**Serial Protocol**: All commands sent from the application are formatted as JSON objects terminated with a newline (e.g., `{"type": "brightness", "value": 200}\n`)

## Technologies & Skills Used

- **Embedded Systems & Microcontrollers**: ESP32 programming with Arduino IDE, GPIO configuration, non-volatile memory management
- **Hardware Integration**: Touch sensor debouncing, addressable LED (WS2812B/NeoPixel) PWM control, serial communication
- **Firmware Development**: C/C++ with real-time pattern generation, HSV-to-RGB color space conversion, JSON parsing
- **Desktop Application Development**: Python with CustomTkinter for modern GUI, serial port enumeration and management
- **Software Distribution**: Executable packaging (.exe compilation from Python), integrated tool bundling (esptool)
- **Data Serialization**: JSON-based command protocol for robust device-to-PC communication
- **Digital Design & CAD**: 3D modeling for enclosure components (base and cover)

## Project Structure

```
OptiSync-Lamp/
├── firmware/                 # ESP32 Arduino/IDF code
├── software/                 # Python application source
│   ├── main.py
│   ├── ui/
│   ├── serial_comm/
│   └── esptool_wrapper/
├── hardware/                 # 3D printing files and schematics
│   ├── base/
│   ├── cover/
│   └── schematics/
├── README.md
└── LICENSE
```

## Future Enhancements

- **OTA Updates**: Over-the-air firmware updates via WiFi, eliminating the need for USB connections
- **Web Interface**: Browser-based control panel as an alternative to the desktop application
- **Remote Control**: Classic IR remote to control presets and power
- **Advanced Patterns**: Additional animation patterns and customization options

## Getting Started

1. Assemble the lamp with your acrylic board and LED strip
2. Flash the ESP32 with the latest firmware using esptool or the OptiSync application
3. Connect the lamp (ESP32 - "UPLOAD") via USB
4. Run `OptiSync.exe` and begin customizing your lighting experience
5. Save your favorite configurations as presets

## Contributing

Contributions, bug reports, and feature suggestions are welcome! Please feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Related Projects

- [Modular-Power-Hub](https://github.com/NickJiEE/Modular-Power-Hub) - Universal multi-input power distribution board used in this project

---

**Status**: Nearly complete - Core functionality finished, OTA updates in planning phase

**Last Updated**: October 2025
