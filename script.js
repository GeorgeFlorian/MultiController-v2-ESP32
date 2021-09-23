const ROOT_URL = window.location.hostname; // for production
let conn_status = 0;

// mission:
// create api handler in main.cc
// create api handler in script.js

// parse form data into json
function toJSONstring(form) {
  console.log(form.name)
  let object = {};
  // console.log(form);
  let formData = new FormData(form);
  formData.forEach((value, key) => {
    // Reflect.has in favor of: object.hasOwnProperty(key)
    if (!Reflect.has(object, key)) {
      object[key] = value;
      // console.log("Reflect [key]");
      // console.log(key);
      // console.log("Reflect value");
      // console.log(value);
      return;
    }
    if (!Array.isArray(object[key])) {
      object[key] = [object[key]];
      // console.log("Array object[key]");
      // console.log(object[key]);
      // console.log("Array [object[key]]");
      // console.log([object[key]]);
    }
    object[key].push(value);
    // console.log("object[key]");
    // console.log(object[key]);
    // console.log("value");
    // console.log(value);
  });
  form.reset();
  // console.log("JSON.stringify(object):");
  // console.log(JSON.stringify(object));
  return JSON.stringify({ [form.name]: object });
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

// send/post json
async function post_data(api_path, json_data) {
  let post_url = ROOT_URL + api_path;
  console.log(post_url);
  const response = await fetch(post_url, {
    method: "POST",
    headers: {
      "Accept": "application/json",
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
  // console.log("post_data RESPOSNE: ");
  // console.log(response);
  return response.status;
}

async function get_settings() {
  try {
    get_json("/api/settings/get", {
      timeout: 5000,
    }).then((json_data) => {
      console.log("Received settings: " + json_data);
      for (let i in json_data) {
        for (const key in json_data[i]) {
          // console.log(key);
          if (json_data[i].hasOwnProperty(key)) {
            // console.log(json_data[i][key]);
            let elem = document.getElementById(key);
            // console.log(elem);
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

// send JSON data to server on /api/settings/post
function save_settings(form, destination) {
  let json_data = toJSONstring(form);
  console.log("save_settings json_data");
  console.log(json_data);
  // post_data(`/api/settings/${destination}`, json_data).then((response) => {
  //   // console.log("api/settings/post RESPONSE: ");
  //   // console.log(response);
  //   conn_status = 1;
  //   // logs to page
  // });
}

window.addEventListener("load", function () {
  // handle form submit
  let network_settings = document.getElementById('network_settings');
  network_settings.addEventListener('submit', function (e) {
    e.preventDefault();
    save_settings(network_settings, 'network');
  })
});

// function to enable or disable network inputs based on connection type
function check_connection() {
  let check_network_connection = document.getElementById(
    "check_network_connection"
  );

  check_network_connection.addEventListener("change", function (e) {
    let target = e.target;
    let wifi = document.querySelectorAll(".connection");
    let currentValue = "Current Value Placeholder";
    switch (target.id) {
      case "wifi":
        wifi.forEach((element) => element.removeAttribute("disabled"));
        wifi.forEach((element) =>
          element.setAttribute("placeholder", `${currentValue}`)
        );
        break;
      case "ethernet":
        wifi.forEach((element) => element.setAttribute("disabled", ""));
        wifi.forEach((element) =>
          element.setAttribute("placeholder", "Ethernet Connection")
        );
        break;
      default:
        break;
    }
  });
}

// function to enable or disable network inputs based on IP type
function check_type() {
  let check_ip_type = document.getElementById("check_ip_type");
  check_ip_type.addEventListener("change", function (e) {
    let target = e.target;
    let ip = document.querySelectorAll(".ip");
    let currentValue = "Current Value Placeholder";
    switch (target.id) {
      case "dhcp":
        ip.forEach((element) => element.setAttribute("disabled", ""));
        ip.forEach((element) => element.setAttribute("placeholder", "DHCP IP"));
        break;
      case "static":
        ip.forEach((element) => element.removeAttribute("disabled"));
        ip.forEach((element) =>
          element.setAttribute("placeholder", `${currentValue}`)
        );
        break;
      default:
        break;
    }
  });
}
