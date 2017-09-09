// Ajax Call
function makeAjaxCall(e) {
    var t = new XMLHttpRequest;
    t.onreadystatechange = function() {
        t.readyState == XMLHttpRequest.DONE && (200 == t.status ? console.log("response form Ajax") : 400 == t.status ? alert("There was an error 400") : alert("something else other than 200 was returned"))
    }, t.open("GET", e, !0), t.send()
}

// Change button class to match it's ID
function changeClass() {
    for (var t = document.getElementsByTagName("button"), e = 0; e < t.length; e++) {
        var n = t[e].getAttribute("id");
        this.getAttribute("id") == n ? document.getElementById(n).className = n : document.getElementById(n).className = "inactive"
    }
}

// Updates buttons to reflect change
function update(jscolor) {
    document.getElementById('on').className = "on";
    document.getElementById('off').className = "inactive";
    document.getElementById('on').style.backgroundColor = '#' + jscolor;

    for (var t = document.getElementsByTagName("button"), e = 0; e < t.length; e++) {
        var n = t[e].getAttribute("id");
        document.getElementById(n).className = "inactive"
    }
}

// Add event listeners to each button on the page
window.onload = function() {
    for (var t = document.getElementsByTagName("button"), e = 0; e < t.length; e++) {
        var n = t[e].getAttribute("id");
        document.getElementById(n).addEventListener("click", changeClass)
    }
};
