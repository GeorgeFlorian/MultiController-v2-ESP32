const ROOT_URL = window.location.href;

window.onload = function() {
    let form = document.getElementById("settings_form");

    function processForm(e) {
        if (e.preventDefault) e.preventDefault();

        let formData = JSON.stringify(form).serializeArray();
        console.log(formData);

        // You must return false to prevent the default form behavior
        return false;
    }

    if (form.attachEvent) {
        form.attachEvent("submit", processForm);
    } else {
        form.addEventListener("submit", processForm);
    }
};