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

### User View

- Create Username and Password used for basic HTTP Authentication
- STA and AP pages for User Page

### Implemented

- Move File Browser and Update to Settings Page
- Rename File Browser to Backup/Restore (Configuration) / Save Settings to File
- Rename Update to Upgrade Firmware
- Create Reset to factory settings without resetting network settings button (Soft Reset)
- Create Reset to factory settings button (Factory Reset)
- Make navbar a fixed size
- Add Dashboard page with state (Relay ON - Relay OFF)
- On page load fill Dashboard with current settings (get_settings(){JSON})
- Save settings to config.json on server side
- Back up all settings to download file
- Save each settings form to file
- Clear form after submit
- Handle form validation on front-end (ux) and back-end
- Handle Relay state
- Implement HTTP Authentication
- Implement Logs
- Implement Toastify
- Separate code into headers
- Move Bootstrap to local and cache the files
- Changed /api/settings/get to get settings from live state objects and not from file
- Handle Restore configuration from file
- Implement AP
- Fix Network connection

### To implement

- Update - (3): In both cases the device will restart ! OR NOT ?!
- Fix User change - Save user in flash memory ?
- Fix Connected status

- Add Reset button to each Setting's form ?
- Add more Logs
- Implement loading icon while reconnecting after a restart

- Move from SPIFFS to LittleFS
- Add Metrici's color theme
- Don't forget to minify files before compressing them into gzip
- Check functionality
- Final touch

### Back-end

- logs.h > common.h > state.h > server_logic.h = connection.h
-                             > wiegand = inputs_outputs = rfid
