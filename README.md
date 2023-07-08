# RoboLRS

## PPM Signal

![PPM signal](https://europe1.discourse-cdn.com/arduino/original/4X/3/b/2/3b2cda0ed5446572945fcb76ed7ff628b01a473a.gif)

## Pinout

![esp32 pinout](https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/ESP32-DOIT-DEVKIT-V1-Board-Pinout-30-GPIOs-Copy.png?quality=100&strip=all&ssl=1)

## Useful git commands

`git submodule update --init --recursive`


## Useful PIO commands
`pio run -e transmitter --upload-port <COM_PORT> -t upload` for uploading to tx module 
<br>
`pio run -e reciever --upload-port <COM_PORT> -t upload` for uploading to rx module

## Tasks

- [ ] Make a interface for adding , deleting , finding slaves
- [x] ota giving cpu reset error
- [ ] oled screen + menu interface
- [ ] Range Test

## Notes

- [x] have to turn off internal module (EdgeTX/edgetx#3364)
- [ ] Different channel for different bot
