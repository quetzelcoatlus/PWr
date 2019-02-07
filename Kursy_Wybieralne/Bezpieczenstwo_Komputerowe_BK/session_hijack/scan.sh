#!/bin/bash

#stdbuf flushuje wyjscie do pliku z kazda linia
$(stdbuf -oL tshark -T fields -e http.host -e http.request.uri -e http.cookie -Y http.cookie -E separator=/s > tshark.dump)
