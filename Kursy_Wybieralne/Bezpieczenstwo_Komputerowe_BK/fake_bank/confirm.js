var ls = localStorage.getItem("last");
if(ls !== null) {
    var number = JSON.parse(ls);
    document.body.innerHTML = document.body.innerHTML.replace('236814', number);
}
