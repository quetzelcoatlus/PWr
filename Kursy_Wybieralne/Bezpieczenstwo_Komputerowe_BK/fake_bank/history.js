var ls = localStorage.getItem("numbers");
if(ls !== null) {
    var numbers = JSON.parse(ls);
    for(var i = 0; i < numbers.length; i++) {
        document.body.innerHTML = document.body.innerHTML.replace('236814', numbers[i]);
    }
}
