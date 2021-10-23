const ROOT_URL = window.location.hostname; // for production
let connected = false;

// show a toast notification
// type: true for info, false for error
function toast(message, type, dest = '') {
    Toastify({
        text: message,
        duration: 3000,
        close: true,
        gravity: "bottom", // `top` or `bottom`
        position: "right", // `left`, `center` or `right`
        stopOnFocus: true, // Prevents dismissing of toast on hover
        className: type ? "info" : "error",
        destination: dest
        // onClick: function () { } // Callback after click
    }).showToast();
}

const updatingToast = (message, type = true, dest = '') => {
    const fc = () => toast(message, type, dest);
    return setInterval(fc, 3000);
}

const checkConnected = () => {
    const foo = () => {
        console.log(connected);
        if (!connected) toast("You are not connected to the device !", false);
    }
    return setInterval(foo, 3000);
}

// check if connected to ESP
function changeConnectedInInterface() {
    let conn = document.getElementById("connected");
    conn.innerHTML = connected ? "Yes" : "No";
}

function ValidateIPaddressOnChange(input) {
    let ipformat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
    let strtype = "";
    switch (input.id) {
        case "ip_address":
            strtype = "IP Address";
            break;
        case "gateway":
            strtype = "Gateway";
            break;
        case "subnet":
            strtype = "Subnet Mask";
            break;
        case "dns":
            strtype = "DNS";
            break;
        case "ip_1":
            strtype = "Input 1 Address";
            break;
        case "ip_2":
            strtype = "Input 2 Address";
            break;
        case "ip_rfid":
            strtype = "RFID Address";
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
        alert(strtype + " is invalid!");
    } else if (input.value != null) {
        element.classList.remove('wrong');
        element.classList.add('correct');
    }
}

function validateForm(form) {
    let ipformat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;

    let number_format = /^([0-9]{1,5})$/;
    let url_format = /^https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)$/;

    /*
    The user has 3 input posibilities:
     - no input - used to keep(not change) the preious setting;
     - 'not set' - used for resetting the setting
     - a valid input - used  to create or update a setting
     */
    for (let i = 0; i < form.elements.length - 1; i++) {
        // console.log(form.elements[i].ariaLabel, form.elements[i].value);
        if (form.elements[i].ariaLabel.includes("Address")) {
            // Network Settings form
            if (form.elements[i].ariaLabel.includes("Network") && form.elements[i].placeholder != 'DHCP IP' &&
                !form.elements[i].value.match(ipformat)) {
                alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
                form.elements[i].focus();
                return false;
            }
            // the other forms that have an IP address input
            if (form.elements[i].value.length == 0 || form.elements[i].value.toLowerCase() === 'not set') {
                continue;
            } else if (!form.elements[i].value.match(ipformat)) {
                alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
                form.elements[i].focus();
                return false;
            }
        } else if (form.elements[i].ariaLabel.includes("Number")) {
            if (form.elements[i].value.length == 0 || form.elements[i].value.toLowerCase() === 'not set') {
                continue;
            } else if (!form.elements[i].value.match(number_format)) {
                alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
                form.elements[i].focus();
                return false;
            }
        } else if (form.elements[i].ariaLabel.includes("Database")) {
            if (form.elements[i].value.length == 0 || form.elements[i].value.toLowerCase() === 'not set') {
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
    let radio_buttons = document.getElementsByName("connection");
    radio_buttons.forEach(element => {
        if (element.checked) {
            checked_element = element;
        }
    });
    let wifi = document.querySelectorAll(".connection");
    let current_placeholder = {};
    wifi.forEach((element) => {
        let classList = element.className.split(' ');
        current_placeholder[element.id] = classList[0];
    });
    switch (checked_element.id) {
        case "wifi":
            wifi.forEach((element) => {
                element.removeAttribute("readonly");
                element.setAttribute("placeholder", `${current_placeholder[element.id]}`);
            });
            break;
        case "ethernet":
            wifi.forEach((element) => {
                element.setAttribute("readonly", "");
                element.value = "";
                element.setAttribute("placeholder", "Ethernet Connection")
            });
            break;
        default:
            break;
    }
}

// function to enable or disable network inputs based on IP type
function checkIpType() {
    let checked_element;
    let radio_buttons = document.getElementsByName("ip_type");
    radio_buttons.forEach(element => {
        if (element.checked) {
            checked_element = element;
        }
    });

    let ip = document.querySelectorAll(".ip");
    let current_placeholder = {};
    ip.forEach((element) => {
        let classList = element.className.split(' ');
        current_placeholder[element.id] = classList[0];
    });
    switch (checked_element.id) {
        case "dhcp":
            ip.forEach((element) => {
                element.setAttribute("readonly", "");
                element.value = "";
                element.classList.remove('wrong');
                element.classList.remove('correct');
                element.setAttribute("placeholder", "DHCP IP");
            });
            break;
        case "static":
            ip.forEach((element) => {
                element.removeAttribute("readonly");
                element.setAttribute("placeholder", `${current_placeholder[element.id]}`);
            });
            break;
        default:
            break;
    }
}

async function getLogs() {
    options = {};
    const { timeout = 3000 } = options;
    const controller = new AbortController();
    const timeoutID = setTimeout(() => controller.abort(), timeout);
    await fetch('/api/logs', {
        ...options,
        signal: controller.signal,
        // headers: {
        //   'Content-Type': 'application/json',
        //   'Accept': 'application/json'
        // }
    }).then(response => {
        if (!response.ok) {
            throw new Error(`HTTP error, status = ${response.status}`);
        }
        connected = true;
        return response.text();
    }).then(text => {
        // console.log('getlogs(): ');
        document.getElementById('logs').innerHTML = text;
    }).catch(function (error) {
        // request timeout
        connected = false;
        console.log("Error inside getLogs() catch: ", error);
    });
    clearTimeout(timeoutID);
}

// parse form data into json
function toJSONstring(form) {
    // console.log(`form: ${form.name}`);
    let object = {};
    // console.log(form);
    let formData = new FormData(form);
    formData.forEach((value, key) => {
        if (key === "connection" || key === "ip_type" || key === 'state1' || key === 'state2') {
            // take the radio input that is checked and replace key with it's id
            let radio_btn = document.getElementsByName(key);
            if (radio_btn[0].checked) {
                // console.log(radio_btn[0].id);
                value = radio_btn[0].value;
                if (key === 'state1' || key === 'state2')
                    toast(`${key === 'state1' ? 'Relay1 is On.' : 'Relay2 is On.'}`, true);
            } else if (radio_btn[1].checked) {
                // console.log(radio_btn[1].id);
                value = radio_btn[1].value;
                if (key === 'state1' || key === 'state2')
                    toast(`${key === 'state1' ? 'Relay1 is Off.' : 'Relay2 is Off.'}`, true);
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
    let whichPage = document.getElementById('settings');
    for (let i in json_data) {
        // console.log("Received settings: " + json_data[i]);
        for (let key in json_data[i]) {
            // console.log(`key: ${key}`);
            if (json_data[i].hasOwnProperty(key)) {
                // console.log(`json_data[i][key]: ${json_data[i][key]}`);
                if (whichPage === null) { // if I am not on the Settings page
                    let elem = document.getElementById(key);
                    // console.log(elem);
                    if (key === 'username' || key === 'password') continue;
                    else if (key === 'state1' || key === 'state2') {
                        let radio_btn = document.getElementsByName(key);
                        if (json_data[i][key] === "On") {
                            radio_btn[0].checked = true;
                            radio_btn[1].checked = false;
                        }
                        if (json_data[i][key] === "Off") {
                            radio_btn[0].checked = false;
                            radio_btn[1].checked = true;
                        }
                    } else { // if (key === 'state1' || key === 'state2')
                        // console.log(key);
                        elem.innerHTML = json_data[i][key] + "";
                    }
                } else { // if (whichPage === null) // if I am on the Settings page
                    if (key === 'connection') {
                        let radio_btn = document.getElementsByName(key);
                        if (json_data[i][key] === "WiFi") {
                            radio_btn[0].checked = true;
                            radio_btn[1].checked = false;
                        }
                        if (json_data[i][key] === "Ethernet") {
                            radio_btn[0].checked = false;
                            radio_btn[1].checked = true;
                        }
                    } else if (key === 'ip_type') {
                        let radio_btn = document.getElementsByName(key);
                        if (json_data[i][key] === "DHCP") {
                            radio_btn[0].checked = true;
                            radio_btn[1].checked = false;
                        }
                        if (json_data[i][key] === "Static") {
                            radio_btn[0].checked = false;
                            radio_btn[1].checked = true;
                        }
                    } else if (key === 'state1' || key === 'state2' || key === 'username' || key === 'password') {
                        continue;
                    } else {
                        let elem = document.getElementById(key);
                        elem.value = json_data[i][key];
                    }
                    checkConnectionType();
                    checkIpType();

                } // if (whichPage === null)
            } // if (json_data[i].hasOwnProperty(key))
        } // for (let key in json_data[i])
    } // for (let i in json_data)
}

// get json
async function get_json(api_path, options = {}) {
    // const url = api_path;
    // console.log(ROOT_URL);
    // console.log(api_path);
    const { timeout = 3000 } = options;
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

function getSettings() {
    return get_json("/api/settings/get", {
        timeout: 3000,
    }).then(response => {
        if (!response.ok) {
            throw new Error(`HTTP error, status = ${response.status}`);
        }
        connected = true;
        return response.json();
    }).then((json_data) => {
        // console.log("Received settings: " + json_data);
        handleJSON(json_data);
    }).catch(function (error) {
        // request timeout
        connected = false;
        console.log("Error inside getSettings() catch: ", error);
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
        method: "POST",
        headers: {
            Accept: "application/json",
            "Content-Type": "application/json",
        },
        body: json_data,
        redirect: 'follow'
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
                if (form.name != 'relay1' && form.name != 'relay2') toast(`Settings not saved !`, false);
                throw new Error(`HTTP error, status = ${response.status}`);
            }
            // connected = true;
            if (form.name != 'relay1' && form.name != 'relay2')
                toast(`${getFormMessage(form)}`, true);
            // response.redirect(response.status, response.url);
            return response.text();
        }).then(text => {
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
    let user_form = document.getElementById("user_form");
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

// User page
if (document.getElementById("user_body")) {
    window.addEventListener("load", function () {
        getLogs();

        saveUser()

        checkConnected();
    });
}

// function that sends relays status to api/settings/relays
function saveRelayState() {
    let relay1 = document.getElementById("relay1");
    relay1.addEventListener('change', function (e) {
        e.preventDefault();
        saveSettings(relay1, 'relay/post');
        getSettings();
        getLogs();
    });
    let relay2 = document.getElementById("relay2");
    relay2.addEventListener('change', function (e) {
        e.preventDefault();
        saveSettings(relay2, 'relay/post');
        getSettings();
        getLogs();
    });
}
// Dashboard page
if (document.getElementById("index")) {
    window.addEventListener("load", function () {
        getSettings();
        getLogs();
        setInterval(changeConnectedInInterface, 500);
        checkConnected();

        saveRelayState();

        // setInterval(changeConnectedInInterface, 500);
        // setInterval(getSettings, 1000);
        // setInterval(getLogs, 1000);
        // checkConnected();
    });
}

// Settings page
// attach addEventListener() only to the page that has <body id="settings"></body>
if (document.getElementById("settings")) {
    window.addEventListener("load", function () {
        getSettings();
        getLogs();
        setInterval(changeConnectedInInterface, 500);
        checkConnected();

        // setInterval(getLogs, 1000);    

        // handle network_form submit
        let network_form = document.getElementById("network_settings");
        network_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (validateForm(network_form)) {
                saveSettings(network_form, "network/post");
                network_form.reset();
                getSettings();
                getLogs();
                connected = false;
            }
        });
        // handle input_form
        let input_form = document.getElementById("input");
        input_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (validateForm(input_form)) {
                saveSettings(input_form, "input/post");
                input_form.reset();
                getSettings();
                getLogs();
            }
        });
        // handle output_form
        let output_form = document.getElementById("output");
        output_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (validateForm(output_form)) {
                saveSettings(output_form, "output/post");
                output_form.reset();
                getSettings();
                getLogs();
            }
        });
        // handle wiegand_form
        let wiegand_form = document.getElementById("wiegand");
        wiegand_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (validateForm(wiegand_form)) {
                saveSettings(wiegand_form, "wiegand/post");
                wiegand_form.reset();
                getSettings();
                getLogs();
            }
        });
        // handle rfid_form
        let rfid_form = document.getElementById("rfid");
        rfid_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (validateForm(rfid_form)) {
                saveSettings(rfid_form, "rfid/post");
                rfid_form.reset();
                getSettings();
                getLogs();
            }
        });
        // handle update_form
        let update_form = document.getElementById("update_form");
        update_form.addEventListener("submit", function (e) {
            let update_file = document.getElementById('update_file');
            let filename = update_file.files[0].name;
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
        // handle restore_form
        let restore_form = document.getElementById("restore_form");
        restore_form.addEventListener("submit", function (e) {
            let restore_file = document.getElementById('restore_file');
            let filename = restore_file.files[0].name;
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
        // handle soft_reset_form
        let soft_reset_form = document.getElementById("soft_reset_form");
        soft_reset_form.addEventListener("submit", function (e) {
            e.preventDefault();
            fetch('/api/soft-reset')
                .then(response => {
                    if (!response.ok) {
                        toast('Soft Reset failed !', false);
                        throw new Error(`HTTP error, status = ${response.status}`);
                    }
                    toast('Soft resetting !', true);
                    return response.text();
                }).then(text => {
                    toast(text, true);
                });
            getSettings();
            getLogs();
        });
        // handle factory_reset_form
        let factory_reset_form = document.getElementById("factory_reset_form");
        factory_reset_form.addEventListener("submit", function (e) {
            e.preventDefault();
            fetch('/api/factory-reset')
                .then(response => {
                    if (!response.ok) {
                        toast('Factory Reset failed !', false);
                        throw new Error(`HTTP error, status = ${response.status}`);
                    }
                    toast('Factory resetting !', true);
                    return response.text();
                }).then(text => {
                    updatingToast(`Please wait 10 seconds then navigate to ${text}`, true, text);
                });
            getSettings();
            getLogs();
        });

        let check_network_connection = document.getElementById("check_network_connection");
        check_network_connection.addEventListener('change', checkConnectionType, false);

        let check_ip_type = document.getElementById("check_ip_type");
        check_ip_type.addEventListener('change', checkIpType, false);
    });
}