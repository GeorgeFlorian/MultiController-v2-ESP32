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

### To implement

- On page load fill inputs with current settings (get_settings(){JSON})
- Move File Browser and Update to Settings Page

* Rename File Browser to Backup/Restore (Configuration) / Save Settings to File
* Save all settings to a single JSON file
* Rename Update to Upgrade Firmware

- Reset to factory settings button
- Reset to factory settings without resetting network settings button

* Add Dashboard page with state (Relay ON - Relay OFF)

- Move from SPIFFS to NVS
