var city = "";
var apiKey = null; //b0ee87cfeac784b1 wunderground : 4f4f0b5ecd03fb8e857be86378159a38 openweather
var provider = null;
var scale = null;

var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

Pebble.addEventListener('showConfiguration', function(e) {

  Pebble.openURL(clay.generateUrl());
});

var messageKeys = require('message_keys');

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }

  // Get the keys and values from each config item
  var dict = clay.getSettings(e.response);
  
  city = dict[messageKeys.City];
  apiKey = dict[messageKeys.API];
  provider = dict[messageKeys.Provider];
  scale = dict[messageKeys.Scale];
  
  localStorage.city = city;
  localStorage.apiKey = apiKey;
  localStorage.provider = provider;
  localStorage.scale = scale;

  // Send settings values to watch side
  Pebble.sendAppMessage(dict, function(e) {
    //console.log('Sent config data to Pebble');
  }, function(e) {
    //console.log('Failed to send config data!');
    //console.log(JSON.stringify(e));
  });
});

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url;
  var opScale;
  
  if (scale === "C"){
    opScale = "metric";
  }
  else if (scale === "F"){
    opScale = "imperial";
  }
    
  if (pos !== undefined) {
    
    if(provider === '0'){
      url = "http://api.openweathermap.org/data/2.5/weather?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude + '&appid=' + apiKey + '&units=' + opScale;
    }
    else{ 
      url = 'http://api.wunderground.com/api/' + apiKey + '/forecast/geolookup/conditions/q/' + pos.coords.latitude + ',' + pos.coords.longitude + '.json';
    }
  }
  else{
  
    if(provider === '0'){
      url = 'http://api.openweathermap.org/data/2.5/weather?&q=' + city + '&appid=' + apiKey + '&units=' + opScale;
    }
    else{
      url = 'http://api.wunderground.com/api/' + apiKey + '/forecast/geolookup/conditions/q/' + city + '.json';
    }
  
  }
  
  
  xhrRequest(url, 'GET',
    function(text) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(text);
      
      //console.log("URL: " + url);
      //var city_called = json.name;
      //console.log("City: " + city);
      //console.log("Provider: " + provider);
      //console.log("Scale: " + scale);
            
      var temperature;
      
      if(provider === '0'){
        temperature = Math.round(json.main.temp);
      }
      else{
        if(scale === "C"){
        temperature =  Math.round(json.current_observation.temp_c);
        }
        else if(scale === "F"){
        temperature =  Math.round(json.current_observation.temp_f);
        }
      }
                
      //console.log("Temperature: " + temperature);

      // Assemble dictionary using our keys
      var dictionary = {
        "Temperature": temperature        
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          //console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          //console.log("Error sending weather info to Pebble!");
        }
      );}      
  );
}

function locationError(err) {
  //console.log("Error requesting location!");
}

function getWeather() {
  if (city === "") {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
  }else{
    locationSuccess();
  }
}

Pebble.addEventListener('ready', function() {
  //console.log('PebbleKit JS Ready!');
  city = localStorage.city;
  apiKey = localStorage.apiKey;
  provider = localStorage.provider;
  scale = localStorage.scale;
  
  Pebble.sendAppMessage({'0': 0
	}, function(e) {
      //console.log('Sent ready message!');
  }, function(e) {
      //console.log('Failed to send ready message');
			//console.log(e);
  });
});
  

  // Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    //console.log("PebbleKit JS ready!");

  // Get the initial weather
    getWeather();
  });

  // Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    //console.log("AppMessage received!");
    getWeather();
  });

function api(){
  
  if(apiKey === undefined || apiKey === null || apiKey === ""){
  Pebble.showSimpleNotificationOnPebble('Weather API', 'An API key is required to fetch weather data. These can be freely obtained from OpenWeatherMap.org or WUnderground.com.');
  }
  
}

Pebble.addEventListener('ready', api);