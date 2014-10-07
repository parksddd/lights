function HTTPGET(url) {
  var req = new XMLHttpRequest();
  req.open("GET", url, false);
  req.send(null);
  return req.responseText;
}

var getLights = function(button) {
  var root = "http://192.168.200.14";
  var url = root.concat(button);
  var response = HTTPGET(url);
  console.log(response);
  var dict = {"RESPONSE": response};
  Pebble.sendAppMessage(dict);
};

Pebble.addEventListener("ready",
  function(e) {
    getLights("/");                         
});

Pebble.addEventListener("appmessage",
  function(e) {
    getLights(e.payload.CMD);    
});