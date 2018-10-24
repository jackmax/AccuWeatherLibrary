# ESP8266 Arduino AccuWeather API library

Exactly what it says on the box - it allows your ESP8266 using Arduino IDE to connect to AccuWeather servers through their REST API and retrieve weather data.

It should be possible to modify it to work with other boards - only the network code would have to be changed.

It does not retrieve all data that AccuWeather has to offer (e.g. past data or air quality info). It should be relatively easy to add this functionality if someone wants it.

## Usage
*   First, create an ```Accuweather``` object:

    ```Accuweather(const char* apiKey, const int locationID, const char* languageID, const bool isMetric)```

    The meaning of each field is self-explanatory. Read <https://developer.accuweather.com/apis> for more details.

*   Then call **one** of the functions to get the corresponding weather data (current weather, hourly forecast or daily forecast, respectively):
    ```
    int getCurrent(AccuweatherCurrentData* data_ptr);
    int getHourly(AccuweatherHourlyData* data_ptr, int hours);
    int getDaily(AccuweatherDailyData* data_ptr, int days);
    ```
    On success, the return value is 0. On failure, the return value is -1 if the connection failed, or an HTTP error code if the connection succeeded, but the server returned an error code.

    For ```getHourly``` and ```getDaily```, pointers should point to arrays containing at least ```hours``` or ```days``` elements, respectively.

*   Then execute the ```continueDownload()``` function until the return value is 0. A value of -1 means that the connection was broken or that the response length cannot be determined (which should not happen). Values larger than 0 indicate that there is still data remaining.

    Data is downloaded in 128 byte chunks. This helps cut down on memory usage and allows other functions (such as refreshing a display, etc.) to run in between chunks.

    After the return value from ```continueDownload()``` is 0 you **do not need** to close the connection. However, if you want to close it midway you can use ```freeConnection()```.

For exactly what data can be obtained, please see the header file.

## Technical details
This library makes use of **json-streaming-parser** library by Daniel Eichhorn (<https://github.com/squix78/json-streaming-parser>). It keeps a stack of "tokens" representing objects, lists and keys. Tokens are just integer values, so the stack takes up little memory and its contents can be examined efficiently.

When a JSON key is encountered, the parser checks a map to see what token it can be turned into. When a JSON value is encountered, the parser checks the tokens on the stack to interpret what the value represents, then (if necessary) stores it into a data object.

## Extending functionality
In order to interpret a new kind of value, you need to
1. Add an appropriate token to ```enum AccuParserTokens_``` (if necessary).
2. Add a translation from string to token to ```stringToTokenMap``` (if necessary).
3. Add new logic to ```void value(String value)``` function from one of the classes that inherit from ```AccuweatherParser```. This will most likely involve checking what the last few elements of the stack are - this can be done with ```STACK_HAS_SUFFIX(x)``` macro, where ```x``` is a ```static const AccuParserTokens``` array.

## License
This code is available under the MIT license, which means you can use, modify and distribute the code as long as you give credits to me and add a reference back to this repository. Please read LICENSE for more details.

## Credits
The library was written by me, it uses bits of example code from [json-streaming-parser](https://github.com/squix78/json-streaming-parser) library by Daniel Eichhorn and Stream HTTP Client example from the [ESP8266 Arduino Core](https://github.com/esp8266/Arduino/).

AccuWeather is a trademark of AccuWeather Corporation. This library is not affiliated with or endorsed by AccuWeather.