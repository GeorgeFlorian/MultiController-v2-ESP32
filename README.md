## Interface

### Dashboard View

- Current Live State of the Multi-controller
- Relays State
- Restart

### Settings View

- Network Settings
- Input Settings
- Wiegand Settings
- RFID Settings
- Backup/Restore
- Update
- Reset

### Implemented

- Move File Browser and Update to Settings Page

- Rename File Browser to Backup/Restore (Configuration) / Save Settings to File
- Rename Update to Upgrade Firmware

- Create Reset to factory settings without resetting network settings button (Reset)
- Create Reset to factory settings button (Factory Reset)
- Make navbar a fixed size

- Add Dashboard page with state (Relay ON - Relay OFF)
- On page load fill Dashboard with current settings (get_settings(){JSON})

### To implement

- Save each settings form to file
- Save all settings to a single JSON file
- Open and Read from JSON file to JSON Object
- Move from SPIFFS to LittleFS
- Don't forget to minify files before compressing them into gzip
- (3): In both cases the device will restart ! OR NOT ?!