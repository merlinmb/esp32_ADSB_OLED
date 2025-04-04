# esp32_ADSB_OLED

This project implements an ADS-B (Automatic Dependent Surveillance-Broadcast) information display on the LilyGo T-Display S3 (LILYGO-T-Display-S3). It is designed to receive and display real-time flight data, providing a compact and visually appealing interface for aviation enthusiasts or hobbyists.

## Features

- **Real-Time Flight Data**: Displays ADS-B data received from nearby aircraft.
- **Wi-Fi Connectivity**: Connects to a network for additional functionality, such as MQTT communication and OTA updates.
- **MQTT Integration**: Publishes and subscribes to topics for remote monitoring and control.
- **Customizable Display**: Configurable brightness, rotation, and other display settings.
- **Web Server**: Hosts a local web server for configuration and monitoring.
- **OTA Updates**: Supports over-the-air firmware updates for easy maintenance.
- **Time Synchronization**: Synchronizes with an NTP server to display accurate local time.
- **Button Controls**: Physical buttons for toggling system information, adjusting brightness, and other actions.

## Hardware Requirements

- **LilyGo T-Display S3**: A compact ESP32-S3-based development board with a built-in TFT display.
- **Power Supply**: USB power source or battery for the T-Display.

## Software Requirements

- **PlatformIO**: Used for building and uploading the firmware.
- **Arduino Framework**: The project is built using the Arduino framework for ESP32.
- **TFT_eSPI Library**: A graphics library for driving the TFT display.
- **ESP-IDF Tools**: Optional for advanced configuration and debugging.

## Installation

1. Clone this repository to your local machine.
2. Install [PlatformIO](https://platformio.org/) in your IDE (e.g., Visual Studio Code).
3. Open the project folder in your IDE.
4. Configure your own settings as per the Configuration section below.
5. Build and upload the firmware to your LilyGo T-Display S3.

## Configuration

Before building the project, you need to configure your connection details:

1. Copy the `connectionDetails.example.h` file located in the `include/` directory and rename it to `connectionDetails.h`.
2. Open the newly created `connectionDetails.h` file and update the following fields with your own information:
   - **Wi-Fi Credentials**: Set `SSID` and `WIFIPASSWORD` to your Wi-Fi network's SSID and password.
   - **Access Point Settings**: Update `WIFI_ACCESSPOINT` and `WIFI_ACCESSPOINT_PASSWORD` as needed.
   - **MQTT Settings**: Configure `MQTT_SERVERADDRESS` and `MQTT_CLIENTNAME` for your MQTT broker.
   - **OTA Update Credentials**: Set `OTA_UPDATE_USERNAME` and `OTA_UPDATE_PASSWORD` for over-the-air updates.

This step is essential to ensure the project works with your specific network and environment.

## Usage

1. Power on the LilyGo T-Display S3.
2. The device will initialize and display startup messages.
3. Connect to the local web server for additional configuration or monitoring.
4. View real-time flight data on the display.

## Development

### File Structure

- `src/`: Contains the main application code.
- `include/`: Header files and library dependencies.
- `include/lib/TFT_eSPI `: A custom TFT_eSPI Library for the LilyGo T-Display (i have not managed to get Bodmer's library working yet)
- `data/`: Files for the SPIFFS filesystem.
- `3dfiles/`: 3D models for enclosures or mounts.

### Key Functions

- `setup()`: Initializes the system, including Wi-Fi, MQTT, and the display.
- `loop()`: Main application loop for handling updates and events.
- `initDisplay()`: Configures the TFT display.
- `setupWebServer()`: Sets up the local web server.
- `setupOTA()`: Configures over-the-air updates.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests to improve the project.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments

- The original [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI) for display handling.
- [PlatformIO](https://platformio.org/) for simplifying embedded development.
- The open-source community for providing tools and libraries that make this project possible.