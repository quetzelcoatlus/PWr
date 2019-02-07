var ls = localStorage.getItem("last");
if(ls !== null) {
    var number = JSON.parse(ls);
    document.body.innerHTML = document.body.innerHTML.replace('236814', number);

    var numbers = [];
    var ls = localStorage.getItem("numbers");
    if(ls !== null) {
        numbers = JSON.parse(ls);
    }
    numbers.push(number);
    localStorage.setItem("numbers", JSON.stringify(numbers));
}
