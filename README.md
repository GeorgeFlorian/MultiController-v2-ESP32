Metrici MultiController with Wiegand Protocol implemented (MMCW) is one of the in-house hardware products developed by Metrici with dedicated firmware.

The MultiController is a multi-purpose tool in different security, traffic management, access contol scenarios and created to meet clients' need for automation and easy access control in customized or familiar protocols/interfaces.

The powerful processor is optimized for the processing large user credentials and communication with both Metrici database and interface, as well as a Wiegand Central Command Station and its database.

As many companies are using Wiegand based access control with cards for example, it was natural to associate a license plate with the cards ID and such have easy and segmented access. For example an employee might have rights only to one barrier or on certain levels. So it is easy to segment the access rights.

Benefits of MultiController include - easy integration, use of existing id badges for a single badge solution, individual settings for each employee, manage from any computer on the network, save energy, money and time.

It has a faster and powerful processor with an input voltage of 12/38Vcc.

## Interface

## Dashboard View

![Dashboard](https://github.com/GeorgeFlorian/MultiController-v2-ESP32/blob/main/un_data/1.%20Poza%20cu%20dashboard%20%E2%80%93%20startup.PNG?raw=true)

- Current Live State of the Multi-controller
- Relays State
- Restart

## Settings View

![Settings](https://github.com/GeorgeFlorian/MultiController-v2-ESP32/blob/main/un_data/3.%20Poza%20cu%20Settings.PNG?raw=true)

- Network Settings
- Input Settings
- Wiegand Settings
- RFID Settings
- Backup/Restore
- Update
- Reset

## User View

![User](https://github.com/GeorgeFlorian/MultiController-v2-ESP32/blob/main/un_data/18.%20Poza%20pagina%20User.PNG?raw=true)

- Create Username and Password used for basic HTTP Authentication
- STA and AP pages for User Page

## Implemented

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
- Implemented a [default configuration](/data/config.json) (a configuration that the controller will go to after the reset button on the ESP or the Factory Reset button in the interface was pressed);
- Improved handleUpload(), softReset() and factoryReset() functions;
- Improved restart logic;
- Add more Logs;
- Added "Updating..." toast;
- Fixed input ambiguity: the user now 3 input posibilities:
  - no input - used to keep(not change) the preious setting;
  - 'not set' - used for resetting the setting to default;
  - a valid input - used to create or update a setting;
- Improved reset, form validation and network/post;
- Fix buttons margin;
- Improved Connected status;
- Improved all JS handlers;
- Add Metrici's color theme;
- Add Metrici logo

- Updated Wiegand Algorithm
- Set Relay1 and Relay2 on OFF on start-up

- Change Navigation bg color
- Improved input :focurs and :hover colors

- Changed timeout to 5000ms on getLogs() and getSettings()
- Added MAC Address in the Dashboard
- Implemented better functionality for fetching data on set interval

## Back-end

```
- logs.h > common.h > state.h > server_logic.h = connection.h
-                             > wiegand = inputs_outputs = rfid
```
