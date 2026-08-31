General Pinouts

ESP32

* 22 for SDA
* 21 for SCL
* 19 - Right wheel forward
* 18 - Right wheel backward
* 17 - Left wheel forward
* 16 - Left wheel backward


curl command to test 

```
curl -X GET "http://192.168.0.152/update?cmd=2&subCmd=1&identifier=1&value1=60&value2=0"
```