const ROOT_URL = window.location.hostname; // for production
let conn_status = 0;

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
    if (form.elements[i].ariaLabel.includes("Address")) {
      if (form.elements[i].value.match(ipformat)) {
        form.elements[i].focus();
        // check to see if Static IP  type is selected
      } else if (form.elements[i].placeholder != 'DHCP IP' && form.elements[i].value.length > 0) {
        alert(`You have entered an invalid ${form.elements[i].ariaLabel}!`);
        form.elements[i].focus();
        return false;
      }
    }
  }

  return true;
}

// mission:
// create api handler in main.cc
// create api handler in script.js

// parse form data into json
function toJSONstring(form) {
  console.log(`form: ${form.name}`);
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
      } else if (radio_btn[1].checked) {
        // console.log(radio_btn[1].id);
        value = radio_btn[1].value;
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
      for (let i in json_data) {
        console.log("Received settings: " + json_data[i]);
        for (let key in json_data[i]) {
          // console.log(`key: ${key}`);
          if (json_data[i].hasOwnProperty(key)) {
            // console.log(`json_data[i][key]: ${json_data[i][key]}`);
            let elem = document.getElementById(key);
            // console.log(elem);
            if (key === 'password') continue;
            if (key === 'state1' || key === 'state2') {
              let radio_btn = document.getElementsByName(key);
              if (json_data[i][key] === "On") {
                radio_btn[0].checked = true;
                radio_btn[1].checked = false;
              }
              if (json_data[i][key] === "Off") {
                radio_btn[0].checked = false;
                radio_btn[1].checked = true;
              }
            }
            else
              elem.innerHTML = json_data[i][key] + "";
          }
        }
      }
    });
  } catch (error) {
    // request timeout
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
    conn_status = 0;
    const message = `An error has occured: ${response.status}`;
    throw new Error(message);
  }
  // console.log("postData RESPOSNE: ");
  // console.log(response);
  return response.status;
}

// send JSON data to server on /api/${destination}
function saveSettings(form, destination) {
  let json_data = toJSONstring(form);
  // console.log("json_data:");
  console.log(json_data);
  postData(json_data, `/api/${destination}`).then((response) => {
    console.log(`/api/${destination} Response: `);
    console.log(response);
    conn_status = 1;
    // logs to page
  });
}

// function that sends relays status to api/settings/relays
function getRelayState() {
  let relay_form = document.getElementById("relay");
  relay_form.addEventListener('change', function (e) {
    e.preventDefault();
    saveSettings(relay_form, 'relay/state');
    // getSettings();
  });
}

// attach addEventListener() only to the page that has <body id="settings_body"></body>
if (document.getElementById("settings_body")) {
  window.addEventListener("load", function () {
    // handle form submit
    // handle network_form
    let network_form = document.getElementById("network_settings");
    network_form.addEventListener("submit", function (e) {
      e.preventDefault();
      if (ValidateIPaddress(network_form)) {
        saveSettings(network_form, "network/post");
        network_form.reset();
      }
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
    let update_form = document.getElementById("update");
    update_form.addEventListener("submit", function (e) {
      e.preventDefault();
      // saveSettings(update_form, "update");
      // fetch(api/restart)
    });
    // handle soft_reset_form
    let soft_reset_form = document.getElementById("soft_reset_form");
    soft_reset_form.addEventListener("submit", function (e) {
      e.preventDefault();
      fetch('/api/soft-reset')
        .then(response => response.json())
        .then(data => console.log(data));
    });
    // handle factory_reset_form
    let factory_reset_form = document.getElementById("factory_reset_form");
    factory_reset_form.addEventListener("submit", function (e) {
      e.preventDefault();
      fetch('/api/factory-reset')
        .then(response => response.json())
        .then(data => console.log(data));
    });


  });

  // function to enable or disable network inputs based on connection type
  function checkConnection() {
    let check_network_connection = document.getElementById("check_network_connection");

    check_network_connection.addEventListener("change", function (e) {
      let target = e.target;
      let wifi = document.querySelectorAll(".connection");
      let current_placeholder = {};
      wifi.forEach((element) => {
        let classList = element.className.split(' ');
        current_placeholder[element.id] = classList[0];
      });
      switch (target.id) {
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
    });
  }

  // function to enable or disable network inputs based on IP type
  function checkType() {
    let check_ip_type = document.getElementById("check_ip_type");
    check_ip_type.addEventListener("change", function (e) {
      let target = e.target;
      let ip = document.querySelectorAll(".ip");
      let current_placeholder = {};
      ip.forEach((element) => {
        let classList = element.className.split(' ');
        current_placeholder[element.id] = classList[0];
      });
      switch (target.id) {
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
    });
  }
}

