var keys = {
  consumer_key: '',
  consumer_secret: '',
  user_id: '',
};

module.exports = {
  hostname: process.env.DEMO_HOSTNAME || 'localhost',
  port: 8085,
  twitter: keys,
};
