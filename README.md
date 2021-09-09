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

### To implement

- Move all Save buttons to the right side of their respective card
- Make navbar a fixed size

* On page load fill inputs with current settings (get_settings(){JSON})
* Add Dashboard page with state (Relay ON - Relay OFF)
* Save all settings to a single JSON file
* Move from SPIFFS to NVS
