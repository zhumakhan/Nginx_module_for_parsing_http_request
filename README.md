# Nginx_module_for_parsing_http_request
#Name is hello_world module, however it parses incoming http requests,
so that one can search for existence of some pattern in request. Currently it searches for pattern "*i* * *am* * *hacker*" where * means any number of any character. If it finds it reirects user to other page and keep log in file.
