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

- Move File Browser and Update to Settings Page;
- Rename File Browser to Backup/Restore (Configuration) / Save Settings to File;
- Rename Update to Upgrade Firmware;
- Create Reset to factory settings without resetting network settings button (Soft Reset);
- Create Reset to factory settings button (Factory Reset);
- Make navbar a fixed size;
- Add Dashboard page with state (Relay ON - Relay OFF);
- On page load fill Dashboard with current settings (get_settings(){JSON});
- Save settings to config.json on server side;
- Back up all settings to download file;
- Save each settings form to file;
- Clear form after submit;
- Handle form validation on front-end (ux) and back-end;
- Handle Relay state;
- Implement HTTP Authentication;
- Implement Logs;
- Implement Toastify;
- Separate code into headers;
- Implemented functionality (io, wiegand, rfid);
- Move Bootstrap to local and cache the files;
- Changed /api/settings/get to get settings from live state objects and not from file;
- Handle Restore configuration from file;
- Fix Network connection;

- Remove AP Mode;
- Implemented Reset button;
- Implemented a [default configuration](/data/config.json)(a configuration that the controller will go to after the reset button on the ESP or the Factory Reset button in the interface was pressed);
- Improved handleUpload(), softReset() and factoryReset() functions;
- Improved restart logic;
- Add more Logs;
- Added "Updating..." toast;
- Fixed input ambiguity: the user now 3 input posibilities:
  - no input - used to keep(not change) the preious setting;
  - 'not set' - used for resetting the setting to default
  - a valid input - used to create or update a setting

### To implement

- Fix buttons margin
- Add Metrici's color theme
- Don't forget to minify files before compressing them into gzip

- Improve User change - Save user in flash memory ?
- Improve Connected status

- Move from SPIFFS to LittleFS

- Test functionality

- Final touch

### Back-end

- logs.h > common.h > state.h > server_logic.h = connection.h
-                             > wiegand = inputs_outputs = rfid
