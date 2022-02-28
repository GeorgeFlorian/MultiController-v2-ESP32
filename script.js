const ROOT_URL = window.location.hostname; // for production
let connected = false;

const dashboardPage = document.getElementById('index');
const settingsPage = document.getElementById('settings');
const userPage = document.getElementById('user_body');

// show a toast notification
// type: true for info, false for error
function toast(message, type, dest = '') {
  Toastify({
    text: message,
    duration: 3000,
    close: true,
    gravity: 'bottom', // `top` or `bottom`
    position: 'right', // `left`, `center` or `right`
    stopOnFocus: true, // Prevents dismissing of toast on hover
    className: type ? 'info' : 'error',
    destination: dest,
    // onClick: function () { } // Callback after click
  }).showToast();
}

const updatingToast = (message, type = true, dest = '') => {
  const fc = () => toast(message, type, dest);
  return setInterval(fc, 3000);
};

const checkConnected = () => {
  const foo = () => {
    // console.log(connected ? 'Connected to the device !' : 'Not connected to the device !');
    if (!connected) toast('You are not connected to the device !', false);
  };
  return setInterval(foo, 3000);
};

// check if connected to ESP
function changeConnectedInInterface() {
  let conn = document.getElementById('connected');
  conn.innerHTML = connected ? 'Yes' : 'No';
  // console.log(connected);
}

function ValidateIPaddressOnChange(input) {
  let ipformat =
    /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
  let strtype = '';
  switch (input.id) {
    case 'ip_address':
      strtype = 'IP Address';
      break;
    case 'gateway':
      strtype = 'Gateway';
      break;
    case 'subnet':
      strtype = 'Subnet Mask';
      break;
    case 'dns':
      strtype = 'DNS';
      break;
    case 'ip_1':
      strtype = 'Input 1 Address';
      break;
    case 'ip_2':
      strtype = 'Input 2 Address';
      break;
    case 'ip_rfid':
      strtype = 'RFID Address';
      break;
    default:
      break;
  }

  let element = document.getElementById(input.id);
  if (input.value == 'not set' || input.value.length == 0) {
    element.classList.remove('correct');
    element.classList.remove('wrong');
  } else if (!input.value.match(ipformat) && !input.placeholder != 'DHCP IP') {
    element.classList.remove('correct');
    element.classList.add('wrong');
    input.focus();
    alert(strtype + ' is invalid!');
  } else if (input.value != null) {
    element.classList.remove('wrong');
    element.classList.add('correct');
  }
}

function validateForm(form) {
  let ipformat =
    /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;

  let number_format = /^([0-9]{1,5})$/;
  let url_format =
    /^https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)$/;

  /*
    The user has 3 input posibilities:
     - no input - used to keep(not change) the preious setting;
     - 'not set' - used for resetting the setting
     - a valid input - used  to create or update a setting
     */
  for (let i = 0; i < form.elements.length - 1; i++) {
    // console.log(form.elements[i].ariaLabel, form.elements[i].value);
    if (form.elements[i].ariaLabel.includes('Address')) {
      // Network Settings form
      if (
        form.elements[i].ariaLabel.includes('Network') &&
        form.elements[i].placeholder != 'DHCP IP' &&
        !form.elements[i].value.match(ipformat)
      ) {
        alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
        form.elements[i].focus();
        return false;
      }
      // the other forms that have an IP address input
      if (
        form.elements[i].value.length == 0 ||
        form.elements[i].value.toLowerCase() === 'not set'
      ) {
        continue;
      } else if (!form.elements[i].value.match(ipformat)) {
        alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
        form.elements[i].focus();
        return false;
      }
    } else if (form.elements[i].ariaLabel.includes('Number')) {
      if (
        form.elements[i].value.length == 0 ||
        form.elements[i].value.toLowerCase() === 'not set'
      ) {
        continue;
      } else if (!form.elements[i].value.match(number_format)) {
        alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
        form.elements[i].focus();
        return false;
      }
    } else if (form.elements[i].ariaLabel.includes('Database')) {
      if (
        form.elements[i].value.length == 0 ||
        form.elements[i].value.toLowerCase() === 'not set'
      ) {
        continue;
      } else if (!form.elements[i].value.match(url_format)) {
        alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
        form.elements[i].focus();
        return false;
      }
    }
  }
  return true;
}
// function to enable or disable network inputs based on connection type
function checkConnectionType() {
  let checked_element;
  let radio_buttons = document.getElementsByName('connection');
  radio_buttons.forEach((element) => {
    if (element.checked) {
      checked_element = element;
    }
  });
  let wifi = document.querySelectorAll('.connection');
  let current_placeholder = {};
  wifi.forEach((element) => {
    let classList = element.className.split(' ');
    current_placeholder[element.id] = classList[0];
  });
  switch (checked_element.id) {
    case 'wifi':
      wifi.forEach((element) => {
        element.removeAttribute('readonly');
        element.setAttribute(
          'placeholder',
          `${current_placeholder[element.id]}`
        );
      });
      break;
    case 'ethernet':
      wifi.forEach((element) => {
        element.setAttribute('readonly', '');
        element.value = '';
        element.setAttribute('placeholder', 'Ethernet Connection');
      });
      break;
    default:
      break;
  }
}

// function to enable or disable network inputs based on IP type
function checkIpType() {
  let checked_element;
  let radio_buttons = document.getElementsByName('ip_type');
  radio_buttons.forEach((element) => {
    if (element.checked) {
      checked_element = element;
    }
  });

  let ip = document.querySelectorAll('.ip');
  let current_placeholder = {};
  ip.forEach((element) => {
    let classList = element.className.split(' ');
    current_placeholder[element.id] = classList[0];
  });
  switch (checked_element.id) {
    case 'dhcp':
      ip.forEach((element) => {
        element.setAttribute('readonly', '');
        element.value = '';
        element.classList.remove('wrong');
        element.classList.remove('correct');
        element.setAttribute('placeholder', 'DHCP IP');
      });
      break;
    case 'static':
      ip.forEach((element) => {
        element.removeAttribute('readonly');
        element.setAttribute(
          'placeholder',
          `${current_placeholder[element.id]}`
        );
      });
      break;
    default:
      break;
  }
}

// parse form data into json
function toJSONstring(form) {
  // console.log(`form: ${form.name}`);
  let object = {};
  // console.log(form);
  let formData = new FormData(form);
  formData.forEach((value, key) => {
    if (
      key === 'connection' ||
      key === 'ip_type' ||
      key === 'state1' ||
      key === 'state2'
    ) {
      // take the radio input that is checked and replace key with it's id
      let radio_btn = document.getElementsByName(key);
      if (radio_btn[0].checked) {
        // console.log(radio_btn[0].id);
        value = radio_btn[0].value;
        if (key === 'state1' || key === 'state2')
          toast(
            `${key === 'state1' ? 'Relay1 is On.' : 'Relay2 is On.'}`,
            true
          );
      } else if (radio_btn[1].checked) {
        // console.log(radio_btn[1].id);
        value = radio_btn[1].value;
        if (key === 'state1' || key === 'state2')
          toast(
            `${key === 'state1' ? 'Relay1 is Off.' : 'Relay2 is Off.'}`,
            true
          );
      }
    }

    // Reflect.has in favor of: object.hasOwnProperty(key)
    if (!Reflect.has(object, key)) {
      object[key] = value;
      return;
    }
    if (!Array.isArray(object[key])) {
      object[key] = [object[key]];
    }
    object[key].push(value);
    form.reset();
  });

  return JSON.stringify({ [form.name]: object }, null, 2);
  // return JSON.stringify({ [form.name]: object });
}

function handleJSON(json_data) {
  for (let i in json_data) {
    // console.log("Received settings: " + json_data[i]);
    for (let key in json_data[i]) {
      // console.log(`key: ${key}`);
      if (json_data[i].hasOwnProperty(key)) {
        // console.log(`json_data[i][key]: ${json_data[i][key]}`);
        if (!settingsPage) {
          // if I am not on the Settings page
          let elem = document.getElementById(key);
          // console.log(elem);
          if (key === 'username' || key === 'password') continue;
          else if (key === 'state1' || key === 'state2') {
            let radio_btn = document.getElementsByName(key);
            if (json_data[i][key] === 'On') {
              radio_btn[0].checked = true;
              radio_btn[1].checked = false;
            }
            if (json_data[i][key] === 'Off') {
              radio_btn[0].checked = false;
              radio_btn[1].checked = true;
            }
          } else {
            // if (key === 'state1' || key === 'state2')
            // console.log(key);
            elem.innerHTML = json_data[i][key] + '';
          }
        } else {
          // if (!settingsPage) // if I am on the Settings page
          if (key === 'connection') {
            let radio_btn = document.getElementsByName(key);
            if (json_data[i][key] === 'WiFi') {
              radio_btn[0].checked = true;
              radio_btn[1].checked = false;
            }
            if (json_data[i][key] === 'Ethernet') {
              radio_btn[0].checked = false;
              radio_btn[1].checked = true;
            }
          } else if (key === 'ip_type') {
            let radio_btn = document.getElementsByName(key);
            if (json_data[i][key] === 'DHCP') {
              radio_btn[0].checked = true;
              radio_btn[1].checked = false;
            }
            if (json_data[i][key] === 'Static') {
              radio_btn[0].checked = false;
              radio_btn[1].checked = true;
            }
          } else if (
            key === 'state1' ||
            key === 'state2' ||
            key === 'username' ||
            key === 'password' ||
            key === 'mac_address_wifi' ||
            key === 'mac_address_eth'
          ) {
            continue;
          } else {
            let elem = document.getElementById(key);
            elem.value = json_data[i][key];
          }
          checkConnectionType();
          checkIpType();
        } // if (!settingsPage)
      } // if (json_data[i].hasOwnProperty(key))
    } // for (let key in json_data[i])
  } // for (let i in json_data)
}

// get json
async function getData(api_path, options = {}) {
  const { timeout } = options;
  const controller = new AbortController();
  const timeoutID = setTimeout(() => controller.abort(), timeout);

  const response = await fetch(api_path, {
    ...options,
    signal: controller.signal,
    // headers: {
    //   'Content-Type': 'application/json',
    //   'Accept': 'application/json'
    // }
  });
  clearTimeout(timeoutID);

  // console.log(response.json);
  return response;
}

async function getSettings() {
  return getData('/api/settings/get', {
    timeout: 8000,
  })
    .then((response) => {
      if (!response.ok) {
        throw new Error(`HTTP error, status = ${response.status}`);
      }
      connected = true;
      return response.json();
    })
    .then((json_data) => {
      // console.log("Received settings: " + json_data);
      handleJSON(json_data);
    })
    .catch(function (error) {
      connected = false;
      console.log('Error inside getSettings() catch: ', error);
    });
}

async function getLogs() {
  return getData('/api/logs', {
    timeout: 8000,
  })
    .then((response) => {
      if (!response.ok) {
        throw new Error(`HTTP error, status = ${response.status}`);
      }
      connected = true;
      return response.text();
    })
    .then((text) => {
      // console.log('getlogs():', text);
      document.getElementById('logs').innerHTML = text;
    })
    .catch(function (error) {
      connected = false;
      console.log('Error inside getLogs() catch: ', error);
    });
}

function getFormMessage(form) {
  switch (form.name) {
    case 'network_settings':
      return 'Saved Network settings.';
      break;
    case 'input':
      return 'Saved Input settings.';
      break;
    case 'output':
      return 'Saved Output settings.';
      break;
    case 'wiegand':
      return 'Saved Wiegand settings.';
      break;
    case 'rfid':
      return 'Saved RFID settings.';
      break;
    case 'user':
      return 'Saved User settings.';
      break;
    default:
      return '';
      break;
  }
}

// send/post json
async function postData(json_data, api_path) {
  // let post_url = ROOT_URL + api_path;
  // console.log(post_url);
  const response = await fetch(api_path, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: json_data,
    redirect: 'follow',
  });
  // console.log("postData response: ", response);
  return response;
}
// send JSON data to server on /api/${destination}
function saveSettings(form, destination) {
  let json_data = toJSONstring(form);
  // console.log("json_data:");
  // let res;
  // console.log(json_data);
  postData(json_data, `/api/${destination}`)
    .then((response) => {
      // res = response;
      // console.log('res: ', res);
      // console.log(`/api/${destination} Response: `);
      if (!response.ok) {
        // connected = false;
        if (form.name != 'relay1' && form.name != 'relay2')
          toast(`Settings not saved !`, false);
        throw new Error(`HTTP error, status = ${response.status}`);
      }
      // connected = true;
      if (form.name != 'relay1' && form.name != 'relay2')
        toast(`${getFormMessage(form)}`, true);
      // response.redirect(response.status, response.url);
      return response.text();
    })
    .then((text) => {
      if (destination === 'network/post') {
        connected = false;
        updatingToast(`You are no longer connected to the device !`, false);
        updatingToast(`Please navigate to ${text}`, true, text);
      }
      // console.log('res: ', res);
      // if (!res.redirected) {
      //     res.redirect(res.url, 301);
      // // window.location.href = res.url;
      // }
    });
}

// handle user form submit
function saveUser() {
  let user_form = document.getElementById('user_form');
  user_form.addEventListener('submit', function (e) {
    e.preventDefault();
    // validate user settings
    let username = document.getElementById('username').value;
    let user_pass = document.getElementById('password').value;
    if (username.length > 0 && user_pass.length === 0) {
      toast('Please enter a password alongside the username !', false);
    } else {
      saveSettings(user_form, 'user/post');
      user_form.reset();
      getLogs();
      if (username.length > 0 && user_pass.length > 0) {
        toast('Please reload the page ');
      }
    }
  });
}

// function that sends relays status to api/settings/relays
function saveRelayState() {
  let relay1 = document.getElementById('relay1');
  relay1.addEventListener('change', function (e) {
    e.preventDefault();
    saveSettings(relay1, 'relay/post');
    getSettings();
    getLogs();
  });
  let relay2 = document.getElementById('relay2');
  relay2.addEventListener('change', function (e) {
    e.preventDefault();
    saveSettings(relay2, 'relay/post');
    getSettings();
    getLogs();
  });
}

function delay(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

async function mySetInterval(callback, ms) {
  while (true) {
    await callback();
    await delay(ms);
  }
}

// User page
if (userPage) {
  window.addEventListener('load', function () {
    saveUser();
    checkConnected();
    mySetInterval(getLogs, 1000);
  });
}

// Dashboard page
if (dashboardPage) {
  window.addEventListener('load', function () {
    setInterval(changeConnectedInInterface, 500);
    checkConnected();

    saveRelayState();
    mySetInterval(getSettings, 1000);
    mySetInterval(getLogs, 1000);
  });
}

const formHandler = function (e, form, path) {
  e.preventDefault();
  if (validateForm(form)) {
    saveSettings(form, path);
    form.reset();
    getSettings();
    getLogs();
  }
};

// Settings page
if (settingsPage) {
  // Overlay
  const overlay = document.querySelector('.overlay');
  // Modal
  const modal = document.querySelector('.modal');
  const modal_message = document.querySelector('.modal-label');
  const reset_btn = document.querySelector('.reset_btn');
  // Open Modal buttons
  const soft_reset_modal_btn = document.getElementById('soft_reset_modal_btn');
  const factory_reset_modal_btn = document.getElementById(
    'factory_reset_modal_btn'
  );
  // Close Modal elements
  const elementsCloseModal = document.querySelectorAll('.close-modal');
  // Soft Reset callback
  const soft_reset = function (e) {
    e.preventDefault();
    fetch('/api/soft-reset')
      .then((response) => {
        if (!response.ok) {
          toast('Soft Reset failed !', false);
          throw new Error(`HTTP error, status = ${response.status}`);
        }
        toast('Soft resetting !', true);
        return response.text();
      })
      .then((text) => {
        toast(text, true);
      });

    getSettings();
    getLogs();
    closeModal();
  };

  // Factory Reset callback
  const factory_reset = function (e) {
    e.preventDefault();
    fetch('/api/factory-reset')
      .then((response) => {
        if (!response.ok) {
          toast('Factory Reset failed !', false);
          throw new Error(`HTTP error, status = ${response.status}`);
        }
        toast('Factory resetting !', true);
        return response.text();
      })
      .then((text) => {
        updatingToast(
          `Please wait 10 seconds then navigate to ${text}`,
          true,
          text
        );
      });
    getSettings();
    getLogs();
    closeModal();
  };

  const openModal = function (e, message) {
    e?.preventDefault();
    // Set styles for modal
    document.querySelector('body').style.overflowY = 'hidden';
    modal.classList.remove('hidden');
    overlay.classList.remove('hidden');
    // Get reset type
    const reset_type = e.target.dataset.reset;
    reset_btn.textContent = `${reset_type} Reset`;
    modal_message.textContent = message;
    // handle reset
    if (reset_type.toLowerCase() === 'soft') {
      // handle soft_reset_form
      const soft_reset_form = reset_btn.parentElement;
      if (!soft_reset_form) return;
      soft_reset_form.removeEventListener('submit', soft_reset, false);
      soft_reset_form.removeEventListener('submit', factory_reset, false);
      soft_reset_form.addEventListener('submit', soft_reset, false);
    }
    if (reset_type.toLowerCase() === 'factory') {
      // handle factory_reset_form
      const factory_reset_form = reset_btn.parentElement;
      if (!factory_reset_form) return;
      factory_reset_form.removeEventListener('submit', soft_reset, false);
      factory_reset_form.removeEventListener('submit', factory_reset), false;
      factory_reset_form.addEventListener('submit', factory_reset, false);
    }
  };

  const closeModal = function (e = null) {
    e?.preventDefault();
    // Set styles for modal
    document.querySelector('body').style.overflowY = 'visible';
    modal.classList.add('hidden');
    overlay.classList.add('hidden');
  };

  // Load event for Settings Page
  window.addEventListener('load', function () {
    getSettings();
    setInterval(changeConnectedInInterface, 500);
    checkConnected();

    mySetInterval(getLogs, 1000);

    const soft_reset_msg = 'Are you sure you want to Soft Reset ?';
    const factory_reset_msg = 'Are you sure you want to Factory Reset ?';

    soft_reset_modal_btn.addEventListener('click', function (e) {
      openModal(e, soft_reset_msg);
    });
    factory_reset_modal_btn.addEventListener('click', function (e) {
      openModal(e, factory_reset_msg);
    });
    elementsCloseModal.forEach((el) =>
      el.addEventListener('click', closeModal)
    );
    document.addEventListener('keydown', function (e) {
      if (e.key === 'Escape' && !modal.classList.contains('hidden')) {
        closeModal(e);
      }
    });
    // handle network_form submit
    const network_form = document.getElementById('network_settings');
    network_form.addEventListener('submit', function (e) {
      formHandler(e, this, 'network/post');
    });
    // handle input_form
    const input_form = document.getElementById('input');
    input_form.addEventListener('submit', function (e) {
      formHandler(e, this, 'input/post');
    });
    // handle output_form
    const output_form = document.getElementById('output');
    output_form.addEventListener('submit', function (e) {
      formHandler(e, this, 'output/post');
    });
    // handle wiegand_form
    const wiegand_form = document.getElementById('wiegand');
    wiegand_form.addEventListener('submit', function (e) {
      formHandler(e, this, 'wiegand/post');
    });
    // handle rfid_form
    const rfid_form = document.getElementById('rfid');
    rfid_form.addEventListener('submit', function (e) {
      formHandler(e, this, 'rfid/post');
    });
    // handle update_form
    const update_form = document.getElementById('update_form');
    update_form.addEventListener('submit', function (e) {
      const update_file = document.getElementById('update_file');
      const filename = update_file.files[0].name;
      if (update_file.files.length > 0) {
        switch (filename) {
          case 'spiffs.bin':
          case 'firmware.bin':
            toast(`File ${filename} was successfully uploaded !`, true);
            toast(`The update process has started...`, true);
            updatingToast('Updating...', true);
            break;
          default:
            toast('File was not uploaded. Try again !', false);
            e.preventDefault();
            break;
        }
      }
      getSettings();
      getLogs();
    });
    // handle backup / restore_form
    const restore_form = document.getElementById('restore_form');
    restore_form.addEventListener('submit', function (e) {
      const restore_file = document.getElementById('restore_file');
      const filename = restore_file.files[0].name;
      if (restore_file.files.length > 0) {
        switch (filename) {
          case 'config.json':
            toast(`File ${filename} was successfully uploaded !`, true);
            break;
          default:
            toast('File was not uploaded. Try again !', false);
            e.preventDefault();
            break;
        }
      }
      getSettings();
      getLogs();
    });

    const check_network_connection = document.getElementById(
      'check_network_connection'
    );
    check_network_connection.addEventListener(
      'change',
      checkConnectionType,
      false
    );

    const check_ip_type = document.getElementById('check_ip_type');
    check_ip_type.addEventListener('change', checkIpType, false);
  });
}
