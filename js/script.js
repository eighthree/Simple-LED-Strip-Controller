function makeAjaxCall(e) {
    var t = new XMLHttpRequest;
    t.onreadystatechange = function() {
        t.readyState == XMLHttpRequest.DONE && (200 == t.status ? console.log("response form Ajax") : 400 == t.status ? alert("There was an error 400") : alert("something else other than 200 was returned"))
    }, t.open("GET", e, !0), t.send()
}

function changeClass() {
    for (var e = document.getElementsByTagName("button"), t = 0; t < e.length; t++) {
        var n = e[t].getAttribute("id");
        this.getAttribute("id") == n ? document.getElementById(n).className = n : document.getElementById(n).className = "inactive"
    }
}

function update(e) {
    document.getElementById("on").className = "on", document.getElementById("off").className = "inactive", document.getElementById("on").style.backgroundColor = "#" + e;
    for (var t = document.getElementsByTagName("button"), n = 0; n < t.length; n++) {
        var a = t[n].getAttribute("id");
        "on" != a && (document.getElementById(a).className = "inactive")
    }
}
window.onload = function() {
    for (var e = document.getElementsByTagName("button"), t = 0; t < e.length; t++) {
        var n = e[t].getAttribute("id");
        document.getElementById(n).addEventListener("click", changeClass)
    }
};
