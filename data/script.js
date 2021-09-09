// const ROOT_URL = window.location.href; // for production
// let conn_status = 0;

// // mission:
// // create api handler in main.cc
// // create api handler in script.js

// // parse form data into json
// function toJSONstring(form) {
//   let object = {};
//   // console.log(form);
//   let formData = new FormData(form);
//   formData.forEach((value, key) => {
//     // Reflect.has in favor of: object.hasOwnProperty(key)
//     if (!Reflect.has(object, key)) {
//       object[key] = value;
//       return;
//     }
//     if (!Array.isArray(object[key])) {
//       object[key] = [object[key]];
//     }
//     object[key].push(value);
//   });
//   form.reset();
//   // console.log("JSON.stringify(object):");
//   // console.log(JSON.stringify(object));
//   return JSON.stringify(object);
// }

// // get json
// async function get_json(api_path, options = {}) {
//   const url = ROOT_URL + api_path;
//   const { timeout = 8000 } = options;
//   const controller = new AbortController();
//   const timeoutID = setTimeout(() => controller.abort(), timeout);

//   const response = await fetch(url, {
//     ...options,
//     signal: controller.signal,
//   });
//   clearTimeout(timeoutID);

//   return response.json();
// }

// async function get_info() {
//   try {
//     const response = await get_json("api/info", {
//       timeout: 5000,
//     });
//     console.log("api/info RESPONSE");
//     console.log(response);
//     return response;
//   } catch (error) {
//     // request timeout
//     console.log(error.name === "AbortError");
//   }
// }

// // send/post json
// async function post_data(api_path, json_data) {
//   let post_url = ROOT_URL + api_path;
//   const response = await fetch(post_url, {
//     method: "POST",
//     headers: {
//       Accept: "application/json",
//       "Content-Type": "application/json",
//     },
//     body: json_data,
//   });
//   //check
//   if (!response.ok) {
//     conn_status = 0;
//     const message = `An error has occured: ${response.status}`;
//     throw new Error(message);
//   }
//   // console.log("post_data RESPOSNE: ");
//   // console.log(response);
//   return response.status;
// }

// function save_settings() {
//   let form = document.getElementById("settings_form");
//   let json_data = toJSONstring(form);
//   // console.log("json_data POSTed");
//   // console.log(json_data);
//   post_data("api/settings/change", json_data).then((response) => {
//     // console.log("api/settings/change RESPONSE: ");
//     // console.log(response);
//     conn_status = 1;
//     // logs to page
//     // get_settings() // update interface with newer settings
//     get_info();
//   });
// }

// window.addEventListener("load", function () {
//   console.log(`ROOT_URL: ${ROOT_URL}`);
//   let settings_form = document.getElementById("settings_form");

//   // get_settings();

//   // handle form submit
//   settings_form.addEventListener("submit", function (e) {
//     e.preventDefault(); // before the code
//     save_settings();
//   });
// });

function get_settings(html_inputs, json) {}

window.addEventListener("load", function () {
  // console.log(`ROOT_URL: ${ROOT_URL}`);
  // let settings_form = document.getElementById("settings_form");

  // get_settings();

  // handle form submit
  // settings_form.addEventListener("submit", function (e) {
  //   e.preventDefault(); // before the code
  //   save_settings();
  // });

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
        wifi.forEach((element) => element.setAttribute("placeholder", `${currentValue}`));
        break;
      case "ethernet":
        wifi.forEach((element) => element.setAttribute("disabled", ""));
        wifi.forEach((element) => element.setAttribute("placeholder", "Ethernet Connection"));
        break;
      default:
        break;
    }
  });

  let check_ip_type = document.getElementById("check_ip_type");
  check_ip_type.addEventListener("change", function (e) {
    let target = e.target;
    let ip = document.querySelectorAll(".ip");
    let currentValue = "Current Value Placeholder";
    switch (target.id) {
      case "dhcp":
        ip.forEach((element) => element.setAttribute("disabled", ""));
        ip.forEach((element) => element.setAttribute("placeholder", 'DHCP IP'));
        break;
      case "static":
        ip.forEach((element) => element.removeAttribute("disabled"));
        ip.forEach((element) => element.setAttribute("placeholder", `${currentValue}`));
        break;
      default:
        break;
    }
  });
});
