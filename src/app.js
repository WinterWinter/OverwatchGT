var city = "";
var apiKey = null;
var provider = null;
var scale = null;

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
    
  if (pos !== undefined) {
    
    if(provider === '0'){
      url = "http://api.openweathermap.org/data/2.5/weather?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude + '&appid=' + apiKey;
    }
    else{ 
      url = 'http://api.wunderground.com/api/' + apiKey + '/forecast/geolookup/conditions/q/' + pos.coords.latitude + ',' + pos.coords.longitude + '.json';
    }
  }
  else{
  
    if(provider === '0'){
      url = 'http://api.openweathermap.org/data/2.5/weather?&q=' + city + '&appid=' + apiKey;
    }
    else{
      url = 'http://api.wunderground.com/api/' + apiKey + '/forecast/geolookup/conditions/q/' + city + '.json';
    }
  
  }
  
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET',
    function(text) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(text);
      
      //var city_called = json.name;
			//console.log("City in response is " + city_called);
      //console.log("Provider Temp: " + provider);
      
      var temperature;
      

		
      
      if(provider === '0'){
        temperature = Math.round(json.main.temp);
      }
      else{
        if(scale === "C"){
        temperature = json.current_observation.temp_c;
        }
        else if(scale === "F"){
        temperature = json.current_observation.temp_f;
        }
      }
          
      //console.log("Temperature is " + temperature);

      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature        
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

  //Show Configuration

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://winterwinter.github.io/OverwatchGT/';

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  apiKey = configData.apiKey;
  city = configData.city;
  provider = configData.provider;
  scale = configData.scale;
  //console.log("API Key is " + apiKey);
	//console.log("Entered city is " + city);
  localStorage.apiKey = apiKey;
  localStorage.city = city;
  localStorage.provider = provider;
  localStorage.scale = scale;

                        
  // Send to the watchapp via AppMessage
var dict = {
  "KEY_SCALE" : configData.scale,
  "KEY_VIBRATE" : configData.vibrate,
  "KEY_HERO" : configData.hero,
  "KEY_INTERVAL" : configData.interval,
  "KEY_FLICK" : configData.flick,
  "KEY_PROVIDER" : configData.provider
};

  // Send to the watchapp
Pebble.sendAppMessage(dict, function() {
  //console.log('Config data sent successfully!');
}, function(e) {
  //console.log('Error sending config data!');

});
});

function api(){
  
  if(apiKey === undefined || apiKey === null || apiKey === ""){
  Pebble.showSimpleNotificationOnPebble('Weather API', 'An API key is required to fetch OpenWeatherMap data. These can be freely obtained from OpenWeatherMap.org.');
  }
  
}

Pebble.addEventListener('ready', api);

