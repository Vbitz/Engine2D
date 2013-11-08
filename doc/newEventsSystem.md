Current System
=====================

  sys.drawFunc(function () {})
  sys.keyboardFunc(function (type, key, press) {})

The current system is good but it won't work for much longer and the more work I do the harder it will be to change.

New System
=====================

  sys.on(eventName, function (e) {}); // yay for jquery style

I like the above option but thinking though it more I realeased that there are often situations where I want to filter
the events that come into "sys.on" futher. That's still something I'm thinking over. Right now the best option seems to be
adding a aditional option to "sys.on" which has a object or class instance passed to it which is then avalible somehow to
the code that emit's events. But the key thing there is it turns into a nasty API to work with. I made the same mistake
with LogGraphEvents which ended up being a cludgy API.

I've considered other options for filtering events. Most API's just pass in EventArgs which are thrown away by the client
if they don't care. I could move down that road but the underlying issue is that the calling overhead on these kinds of
things meens that I should be looking for something much faster. The perfect system would be some kind of query type
syntax in C++ something like 

  Engine::Events::Emit("keypress", [&](args) -> bool {return args["key"] == key;}, key, press);

Which is what I'm going to end up doing

The API on the JS Side will be

  sys.on(eventName, {key: "k"}, function () {});
