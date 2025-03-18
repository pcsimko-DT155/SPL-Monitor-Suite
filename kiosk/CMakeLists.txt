cmake_minimum_required(VERSION 3.10)

project(kiosk VERSION 1.0)

install(FILES spl.py DESTINATION kiosk)
install(FILES requirements.txt DESTINATION kiosk)
install(DIRECTORY static DESTINATION kiosk)
install(DIRECTORY templates DESTINATION kiosk)

