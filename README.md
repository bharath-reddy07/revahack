# HARDWARE PASSWORD MANAGER
## Introduction

Password security is of paramount importance in today's digital landscape, where individuals and organizations rely heavily on online services, websites, and various digital platforms.
Passwords serve as the primary defense mechanism for protecting personal and sensitive information, and their strength and integrity are crucial in preventing unauthorized access and potential data breaches.
Software-based password management offers convenience and security by enabling users to store and manage their passwords digitally. However, it also comes with its own set of challenges and risks such as Single Point of Failure: With software-based password managers, users typically need to remember one master password to access their entire password database. If this master password is compromised or forgotten, it can lead to a complete loss of access to all stored passwords.
Thus it is imperative that we switch to hardware based password management system to enhance the security and ensure privacy of the user.


## How our solution works

The hardware-based password manager uses a microcontroller ESP32 that securely stores a user's passwords. 
When connected to a computer, it allows the user to select the desired account. With a simple click on a rotary encoder, the microcontroller automatically inputs the corresponding password into the computer, eliminating the need for manual entry. 
This streamlined process ensures efficient and convenient access to various accounts while maintaining a high level of security. The device serves as a reliable and user-friendly solution for managing multiple passwords in a hardware-encrypted environment.


## Tech Stack

## Hardware 

ESP 32                                                                           
Rotary Encoder                                                                 
OLED
Buzzer
LED

## Software

Arduino IDE
Flask
