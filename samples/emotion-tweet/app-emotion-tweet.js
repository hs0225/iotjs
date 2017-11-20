var https = require('https');
var OAuth2 = require('oauth').OAuth2;
var sentiment = require('sentiment');

var config = require('./config');
var utils = require('./utils');

// ./tools/build.py --cmake-param=-DENABLE_MODULE_HTTPS=ON --jerry-heaplimit=512 --clean --no-check-test

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
          relay(tweet);
        }
      });
    }
});

function relay(tweet) {
  // send request
  var tweetUrl = 'http://localhost:' + config.port + 
                 '/tweet?data=' +
                 JSON.stringify(createData(tweet));
  utils.request(tweetUrl); // TODO: add callback
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
  return data;
}

function getCurrentTimeStr() {
  var currentdate = new Date();
  var datetime = '@' + currentdate.getHours() + ':'
                 + currentdate.getMinutes() + ':'
                 + currentdate.getSeconds();
  return datetime;
}
