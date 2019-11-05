# Nginx_module_for_parsing_http_request
Name is hello_world module, however it parses incoming http requests,
so that one can search for existence of some pattern in request. Currently it searches for pattern "*i* * *am* * *hacker*" where * means any number of any character. Anyone can easily put his own desired pattern instead. If it finds it redirects user to other page and keep this event in log file. Currently it uses "KMP (Knuth Morris Pratt) Pattern Searching" algorithm.
