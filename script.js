const ROOT_URL = window.location.hostname; // for production
let connected = false;

// show a toast notification
// type: true for info, false for error
function toast(message, type) {
    Toastify({
        text: message,
        duration: 3000,
        close: true,
        gravity: "bottom", // `top` or `bottom`
        position: "right", // `left`, `center` or `right`
        stopOnFocus: true, // Prevents dismissing of toast on hover
        className: type ? "info" : "error",
        // onClick: function () { } // Callback after click
    }).showToast();
}

// check if connected to ESP
function connectedStatus() {
    let conn = document.getElementById("connected");
    conn.innerHTML = connected ? "Yes" : "No";
}

function ValidateIPaddressOnChange(input) {
    var ipformat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
    var strtype = "";
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

    if (!input.value.match(ipformat) && !input.placeholder != 'DHCP IP') {
        document.getElementById(input.id).className =
            document.getElementById(input.id).className.replace(/(?:^|\s)correct(?!\S)/g, '');
        document.getElementById(input.id).className += " wrong";
        input.focus();
        alert(strtype + " is invalid!");
    } else if (input.value != null) {
        document.getElementById(input.id).className =
            document.getElementById(input.id).className.replace(/(?:^|\s)wrong(?!\S)/g, '');
        document.getElementById(input.id).className += " correct";
    }
}

function ValidateIPaddress(form) {
    var ipformat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;

    for (let i = 0; i < form.elements.length - 1; i++) {
        // Select only the ip Address inputs
        if (form.elements[i].ariaLabel.includes("Address")) {
            if (form.elements[i].ariaLabel.includes("Network") && form.elements[i].placeholder != 'DHCP IP' &&
                !form.elements[i].value.match(ipformat)) {
                alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
                form.elements[i].focus();
                return false;
            } else if (form.elements[i].value.length > 0 && !form.elements[i].value.match(ipformat)) {
                alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
                form.elements[i].focus();
                return false;
            }
        }
    }
    return true;
}


// function to enable or disable network inputs based on connection type
function checkConnection() {
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
function checkType() {
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

    return JSON.stringify({
        [form.name]: object
    }, null, 2);
    // return JSON.stringify({ [form.name]: object });
}

// get json
async function get_json(api_path, options = {}) {
    // const url = api_path;
    // console.log(ROOT_URL);
    // console.log(api_path);
    const { timeout = 8000 } = options;
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
    return response.json();
}

async function getSettings() {
    try {
        get_json("/api/settings/get", {
            timeout: 5000,
        }).then((json_data) => {
            // console.log("Received settings: " + json_data);
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
                            }
                            checkConnection();
                            checkType();

                        } // if (whichPage === null)
                    } // if (json_data[i].hasOwnProperty(key))
                } // for (let key in json_data[i])
            } // for (let i in json_data)
            connected = true;
        });
    } catch (error) {
        // request timeout
        connected = false;
        console.log(error.name === "AbortError");
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
    });
    //check
    if (!response.ok) {
        connected = false;
        const message = `An error has occured: ${response.status}`;
        throw new Error(message);
    }
    // console.log("postData response: ");
    // console.log(response);
    return response;
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

// send JSON data to server on /api/${destination}
function saveSettings(form, destination) {
    let json_data = toJSONstring(form);
    // console.log("json_data:");
    console.log(json_data);
    postData(json_data, `/api/${destination}`).then((response) => {
        console.log(`/api/${destination} Response: `);
        if (response.status == 200 || response.ok) {
            connected = true;
            if (form.name != 'relay1' && form.name != 'relay2') toast(`${getFormMessage(form)}`, true);
        }
        else {
            connected = false;
            if (form.name != 'relay1' && form.name != 'relay2') toast(`Settings not saved !`, false);
        }
    });
}

// function that sends relays status to api/settings/relays
function saveRelayState() {
    let relay1 = document.getElementById("relay1");
    relay1.addEventListener('change', function (e) {
        e.preventDefault();
        saveSettings(relay1, 'relay/post');
    });
    let relay2 = document.getElementById("relay2");
    relay2.addEventListener('change', function (e) {
        e.preventDefault();
        saveSettings(relay2, 'relay/post');
    });
}
// handle user form submit
function saveUser() {
    let user_form = document.getElementById("user_form");
    user_form.addEventListener('submit', function (e) {
        e.preventDefault();
        saveSettings(user_form, 'user/post');
        user_form.reset();
    });
}

async function getLogs() {
    options = {};
    const { timeout = 8000 } = options;
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
        // console.log('getlogs(): ');
        response.text().then(text => {
            document.getElementById('logs').innerHTML = text;
            // console.log(text);
        });
    });
    clearTimeout(timeoutID);
}

if (document.getElementById("user_body")) {
    window.addEventListener("load", function () {
        saveUser();
        setInterval(getLogs, 1000);
    });
}

if (document.getElementById("index")) {
    window.addEventListener("load", function () {
        getSettings();
        saveRelayState();
        connectedStatus();
        setInterval(connectedStatus, 500);
        setInterval(getSettings, 1000);
        setInterval(getLogs, 1000);
    });
}

// attach addEventListener() only to the page that has <body id="settings"></body>
if (document.getElementById("settings")) {
    window.addEventListener("load", function () {
        getSettings();
        connectedStatus();
        setInterval(connectedStatus, 500);
        setInterval(getLogs, 1000);
        // handle network_form submit
        let network_form = document.getElementById("network_settings");
        network_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (ValidateIPaddress(network_form)) {
                saveSettings(network_form, "network/post");
                network_form.reset();
            }
            getSettings();
        });
        // handle input_form
        let input_form = document.getElementById("input");
        input_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (ValidateIPaddress(input_form)) {
                saveSettings(input_form, "input/post");
                input_form.reset();
            }
        });
        // handle output_form
        let output_form = document.getElementById("output");
        output_form.addEventListener("submit", function (e) {
            e.preventDefault();
            saveSettings(output_form, "output/post");
            output_form.reset();
        });
        // handle wiegand_form
        let wiegand_form = document.getElementById("wiegand");
        wiegand_form.addEventListener("submit", function (e) {
            e.preventDefault();
            saveSettings(wiegand_form, "wiegand/post");
            wiegand_form.reset();
        });
        // handle rfid_form
        let rfid_form = document.getElementById("rfid");
        rfid_form.addEventListener("submit", function (e) {
            e.preventDefault();
            if (ValidateIPaddress(rfid_form)) {
                saveSettings(rfid_form, "rfid/post");
                rfid_form.reset();
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
                        toast(`File ${filename} was successfully uploaded !`);
                        break;
                    default:
                        toast('File was not uploaded. Try again !', false);
                        e.preventDefault();
                        break;
                }
            }
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
                    toast('Soft Reset succeeded !', true);
                    return response.text();
                });
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
                    toast('Factory Reset succeeded !', true);
                    return response.text();
                })
                .then(text => {
                    console.log(text);
                    toast(`Please navigate to ${text}`, true);
                });
        });

        let check_network_connection = document.getElementById("check_network_connection");
        check_network_connection.addEventListener('change', checkConnection, false);

        let check_ip_type = document.getElementById("check_ip_type");
        check_ip_type.addEventListener('change', checkType, false);

        connectedStatus();
    });
}