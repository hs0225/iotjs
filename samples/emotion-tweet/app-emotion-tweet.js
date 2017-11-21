var https = require('https');
var OAuth2 = require('oauth').OAuth2;
var sentiment = require('sentiment');
var url = require('url');

var config = require('./config');
var led = require('./i2c_led');
var utils = require('./utils');

// curl: ./tools/apt-get-install-deps.sh
// x86: build ./tools/build.py --cmake-param=-DENABLE_MODULE_HTTPS=ON --jerry-heaplimit=512 --clean --no-check-test
// rpi: build ./tools/build.py --cmake-param=-DENABLE_MODULE_HTTPS=ON --jerry-heaplimit=512 --clean --no-check-test --target-board=rpi2

var twitterKeys = {
  consumerKey: process.env.TWR_CONSUMER_KEY_ID || config.twitter.consumer_key,
  consumerSecret: process.env.TWR_CONSUMER_SECRET_ID || config.twitter.consumer_secret,
  userID: process.env.TWR_USER_ID || config.twitter.user_id,
}

if(!(twitterKeys.consumerKey && twitterKeys.consumerSecret)) {
  console.error('(!) twitter keys need to be set.');
  process.exit(1);
}

var oauth2 = new OAuth2(
  twitterKeys.consumerKey,
  twitterKeys.consumerSecret,
  'https://api.twitter.com/',
  null,
  'oauth2/token',
  null);

oauth2.getOAuthAccessToken('', {
    'grant_type': 'client_credentials',
  },
  function (e, access_token, refresh_token, results) {
    // https://developer.twitter.com/en/docs/tweets/timelines/api-reference/get-statuses-user_timeline
    // e.g) '/1.1/statuses/user_timeline.json?screen_name=sport&count=5'
    var options = {
      hostname: 'api.twitter.com',
      path: '/1.1/statuses/user_timeline.json?count=1&user_id=' + twitterKeys.userID,
      headers: {
        Authorization: 'Bearer ' + access_token,
      }
    };

    var count = 0;
    var heartbeat = setInterval(function() {
      https.get(options, handler);
      console.log(getCurrentTimeStr(), 'wake#', count++);
    }, 5000);

    // https get request handler
    var cache;
    function handler(res) {
      var buffer = '';

      res.on('data', function (data) {
        buffer += data;
      });

      res.on('end', function () {
        // sweet tweets!
        var tweet = JSON.parse(buffer)[0];
        
        if(cache != tweet.text) {
          cache = tweet.text;
          handleUpdatedTweet(tweet);
        }
      });
    }
});

function showFace(score) {
  if(score > 0) {
    led('smile');
  } else if(score < 0) {
    led('sad');
  } else {
    led('soso');
  }
}

function playMusic(score) {
  var tmp;

  if(score > 0) {
    tmp = 5;
  } else if(score < 0) {
    tmp = -5;
  } else {
    tmp = 0;
  }

  console.log('TODO: play a song based on emotional score:', score);

  // $ sudo apt-get install sox libsox-fmt-all
  // process.runcmd('play music/'+ score + '.mp3 fade t 5 vol 2.5 &');

  // $ sudo apt-get install omxplayer
  // process.runcmd('omxplayer your.mp3" &');
  // setTimeout(function() { process.runcmd('killall -9 omxplayer') }, 5000);
}

function handleUpdatedTweet(tweet) {
  var data = createData(tweet);

  showFace(data.emotion.score);

  playMusic(data.emotion.score);

  utils.request(config.hostname, config.port, '/tweet?score=' + data.emotion.score);
}

function createData(tweet) {
  var emotion_result = sentiment(tweet.text);
  var data = {
    date: tweet.created_at,
    text: tweet.text,
    emotion: {
      score: emotion_result.score,
      comparative: emotion_result.comparative,
    }
  };

  console.log(data);
  return data;
}

function getCurrentTimeStr() {
  var currentdate = new Date();
  var datetime = '@' + currentdate.getHours() + ':'
                 + currentdate.getMinutes() + ':'
                 + currentdate.getSeconds();
  return datetime;
}
