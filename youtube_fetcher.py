#!/usr/bin/python3

import urllib.request
import urllib.parse
import re
import sys
import signal

def signal_handler(signal, frame):
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)

if len(sys.argv) < 2:
    sys.exit(1);

try:
    search_query = sys.argv[1]
    query_string = urllib.parse.urlencode({"search_query" : search_query })
    html_content = urllib.request.urlopen("http://www.youtube.com/results?" + query_string)
    search_results = re.findall(r'href=\"\/watch\?v=(.{11})', html_content.read().decode())
    print("http://www.youtube.com/watch?v=" + search_results[0], end="")
except KeyboardInterrupt:
    print("error", end="");
    sys.exit(1);
except:
    print("error", end="");
    sys.exit(1);

