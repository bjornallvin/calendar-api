# calendar-api

Christmas calendar with leds in each slots. Arduino coontrols the state of the leds. Arduino can receive commands on its serial bus. A raspberry pi is connected to the Arduino via USB. It hosts a express server that exposes an API. Via the API you can control the calendar lights. The API will be used by a web client (another project)
