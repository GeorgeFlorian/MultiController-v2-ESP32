const ROOT_URL = "http://localhost:3000/"; // for testing with local json server
// const ROOT_URL = window.location.href; // for production
let conn_status = 0;

// window.onload = function () {
//   let form = document.getElementById("settings_form");

//   function processForm(e) {
//     if (e.preventDefault) e.preventDefault();

//     let formData = JSON.stringify(form).serializeArray();
//     console.log(formData);

//     // You must return false to prevent the default form behavior
//     return false;
//   }

//   if (form.attachEvent) {
//     form.attachEvent("submit", processForm);
//   } else {
//     form.addEventListener("submit", processForm);
//   }
// };

// create api handler in main.cc
// create api handler in script.js

window.addEventListener("load", function () {
  // parse form data into json
  const getFormData = (form_id) => {
    const form = document.getElementById(`${form_id}`);
    console.log(form);
    return new FormData(form);
  };

  let settings_form = getFormData("settings_form");

  function toJSONstring(formData) {
    let object = {};
    formData.forEach((value, key) => {
      // Reflect.has in favor of: object.hasOwnProperty(key)
      if (!Reflect.has(object, key)) {
        object[key] = value;
        return;
      }
      if (!Array.isArray(object[key])) {
        object[key] = [object[key]];
      }
      object[key].push(value);
    });
    console.log(JSON.stringify(object));
    return JSON.stringify(object);
  }

  // send/post json
  async function post_data(api_path, json_data) {
    let post_url = ROOT_URL + api_path;
    const response = await fetch(post_url, {
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
    return await response.json();
  }

  function save_settings() {
    //   let form = document.getElementById("settings_form");
    let json_data = toJSONstring(settings_form);
    post_data("api/settings/change", json_data).then((send_response) => {
      console.log(send_response);
      conn_status = 1;
      // logs to page
      // get_settings() // update interface with newer settings
    });
  }
  // get json

  // handle form submit
  settings_form.addEventListener("submit", function (e) {
    e.preventDefault(); // before the code
    save_settings(form);

    // Should be triggered on form submit
    console.log("hi");
  });
});
