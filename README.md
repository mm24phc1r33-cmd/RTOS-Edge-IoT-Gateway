Bilkul. Tumhare RTOS-Based Edge IoT Gateway with Cloud Monitoring project ke GitHub ke liye ye professional README.md content use karo. Isme tumhare actual project ke STM32 + FreeRTOS + ESP32 + UART + ACK + Wi-Fi + ThingSpeak + SystemView workflow ko cover kiya hai.

# RTOS-Based Edge IoT Gateway with Cloud Monitoring

An embedded Edge IoT Gateway developed using **STM32F401RE, FreeRTOS, ESP32, UART, Wi-Fi, and ThingSpeak Cloud**.

The system demonstrates real-time task management on STM32 using FreeRTOS, communication between STM32 and ESP32 over UART, and cloud-based monitoring through ThingSpeak.

---

##  Project Overview

This project implements an **RTOS-based Edge IoT Gateway** in which the STM32F401RE performs real-time data processing and system management, while the ESP32 acts as a Wi-Fi-enabled communication gateway for cloud connectivity.

### System Architecture

```text
             Sensor Data
                  |
                  v
        +-------------------+
        |    STM32F401RE    |
        |                   |
        |    FreeRTOS       |
        |                   |
        |  Sensor Task      |
        |  Communication    |
        |  LED Task         |
        |  Event Handling   |
        |  Watchdog         |
        +---------+---------+
                  |
                  | USART1 / UART
                  |
                  v
        +-------------------+
        |       ESP32       |
        |                   |
        |     UART2         |
        |      Wi-Fi        |
        +---------+---------+
                  |
                  | Wi-Fi
                  |
                  v
        +-------------------+
        |    ThingSpeak     |
        |      Cloud        |
        |                   |
        | Temperature       |
        | Humidity          |
        | Error Count       |
        +-------------------+
 ##Objectives
Implement a real-time embedded application using FreeRTOS.
Demonstrate multitasking on STM32.
Implement safe inter-task communication using FreeRTOS Queues.
Use Event Flags for task synchronization.
Implement periodic operations using Software Timers.
Implement watchdog-based system recovery.
Establish UART communication between STM32 and ESP32.
Implement structured sensor-data packets and ACK-based communication.
Use ESP32 Wi-Fi for cloud connectivity.
Upload sensor data to ThingSpeak Cloud.
Analyze RTOS task execution using SEGGER SystemView.
## Hardware Used
Component	Description
STM32F401RE Nucleo	Main real-time controller
ESP32-WROOM-32	Wi-Fi and cloud communication gateway
USB-TTL Converter	UART debugging and monitoring
LED	System status indication
Sensor Data	Simulated sensor input

Note: Sensor values are currently simulated for system-level testing and cloud integration.

## Software & Tools
STM32CubeIDE
STM32CubeMX
FreeRTOS
Embedded C
Arduino IDE
ESP32 Arduino Core
ThingSpeak Cloud
SEGGER SystemView
X-CTU
Git / GitHub
## STM32 Configuration
Microcontroller

STM32F401RE

Clock
HSI = 16 MHz
PLL = OFF
HCLK = 16 MHz
APB1 = 16 MHz
APB2 = 16 MHz
UART Configuration
Baud Rate : 115200
Data Bits : 8
Parity    : None
Stop Bits : 1
Flow Ctrl : None
STM32 USART1 ↔ ESP32 UART2
STM32 PA9  (USART1_TX)  --->  ESP32 GPIO16 (RX2)

STM32 PA10 (USART1_RX)  <---  ESP32 GPIO17 (TX2)

STM32 GND                ----  ESP32 GND
STM32 USART2 ↔ USB-TTL
STM32 PA2 (USART2_TX) ---> USB-TTL RX

STM32 PA3 (USART2_RX) <--- USB-TTL TX

STM32 GND              ---> USB-TTL GND
## FreeRTOS Tasks

The STM32 firmware is divided into multiple RTOS tasks.

1. Sensor Task

The sensor task generates/processes sensor data and places the data into a FreeRTOS queue.

Example data:

Temperature = 28 °C
Humidity    = 62 %
Status      = OK
Error       = 0
2. Communication Task

The communication task:

Receives messages from ESP32.
Processes ESP32_READY.
Reads sensor data from the queue.
Creates the sensor-data packet.
Sends the packet to ESP32 through USART1.
Processes ACK responses.
3. LED Task

The LED task provides a visual indication that the RTOS application is running.

## Inter-Task Communication

A FreeRTOS Queue is used to safely transfer sensor data between the sensor task and communication task.

+-------------+
| Sensor Task |
+------+------+
       |
       | xQueueSend()
       v
+-------------+
| RTOS Queue  |
+------+------+
       |
       | xQueueReceive()
       v
+---------------------+
| Communication Task  |
+---------------------+

This separates sensor-data generation from communication processing.

## Event Flags

FreeRTOS Event Flags are used for event-based synchronization between system components.

They allow tasks to wait for specific events instead of continuously polling system conditions.

## Software Timers

FreeRTOS software timers are used for time-based and periodic system operations.

This allows timing logic to remain independent of the main task execution.

## Watchdog & Fault Recovery

A watchdog mechanism is implemented to improve system reliability.

If the firmware becomes stuck and the watchdog is not refreshed within the configured period, the MCU can reset and recover the system.

Normal Operation
       |
       v
Watchdog Refreshed
       |
       v
System Running
       |
       X
System Fault / Hang
       |
       v
Watchdog Timeout
       |
       v
MCU Reset
       |
       v
System Recovery
## STM32 ↔ ESP32 Communication

A simple structured text-based packet format is used for communication.

Sensor Packet
SENSOR_DATA,TEMP=28,HUM=62,STATUS=OK,ERROR=0
Packet Structure
Parameter	Description
SENSOR_DATA	Packet identifier
TEMP=28	Temperature value
HUM=62	Humidity value
STATUS=OK	Sensor/system status
ERROR=0	Error count
## Packet Validation & ACK

After receiving a sensor packet, ESP32 validates the packet structure.

If the packet is valid:

ESP32 -> STM32

ACK,SENSOR_DATA

Communication flow:

STM32
   |
   | SENSOR_DATA,TEMP=28,...
   v
ESP32
   |
   | Packet Validation
   v
Valid Packet
   |
   | ACK,SENSOR_DATA
   v
STM32
<img width="568" height="600" alt="Screenshot 2026-09-30 211636" src="https://github.com/user-attachments/assets/78170cef-4a7d-4979-94e5-dac1d1e537bc" />

## ESP32 Wi-Fi

The ESP32 connects to a Wi-Fi network and acts as the communication gateway between the STM32 and ThingSpeak Cloud.

Example serial output:

Wi-Fi Connected!
IP Address: 192.168.xxx.xxx
RSSI: -24 dBm
## ThingSpeak Cloud Integration

After receiving and validating sensor data, the ESP32 uploads the processed values to ThingSpeak.

ThingSpeak Fields
Field	Data
Field 1	Temperature
Field 2	Humidity
Field 3	Error Count

Example:

Temperature = 28
Humidity    = 62
Error Count = 0

Cloud data can then be monitored through ThingSpeak charts.

## Complete Data Flow
+----------------+
| Sensor Data    |
+-------+--------+
        |
        v
+----------------+
| Sensor Task    |
|   FreeRTOS     |
+-------+--------+
        |
        | Queue
        v
+----------------------+
| Communication Task   |
+----------+-----------+
           |
           | USART1
           v
+----------------------+
|        ESP32         |
|      UART2 + Wi-Fi   |
+----------+-----------+
           |
           | Packet Validation
           |
           +----> ACK,SENSOR_DATA
           |
           | Wi-Fi
           v
+----------------------+
|   ThingSpeak Cloud   |
+----------------------+
           |
           v
+----------------------+
| Remote Visualization|
+----------------------+

<img width="1920" height="1080" alt="Screenshot 2026-10-01 003811" src="https://github.com/user-attachments/assets/d3946617-0da9-4040-bffc-c0b3daa80889" />

## SEGGER SystemView

SEGGER SystemView is used to analyze the real-time behavior of the FreeRTOS application.

It helps monitor:

RTOS task execution
Task scheduling
Task switching
Timing behavior
System activity
Real-time debugging
<img width="1920" height="1080" alt="Screenshot 2026-09-30 215508" src="https://github.com/user-attachments/assets/b7f99a01-5daf-48b3-baf4-c5a472ea5869" />

##Example Serial Output
STM32 / X-CTU
MONITOR: RTOS Gateway Running

STM32 ESP32 COMMUNICATION TASK
LED TASK: Running

SENSOR: Data placed into Queue

Sensor packet sent to ESP32
ESP32
Wi-Fi Status: CONNECTED

STM32 -> ESP32 :
SENSOR_DATA,TEMP=28,HUM=62,STATUS=OK,ERROR=0

ESP32 STATUS: SENSOR DATA RECEIVED

PACKET STATUS: VALID

Sensor Packet:
SENSOR_DATA,TEMP=28,HUM=62,STATUS=OK,ERROR=0

ESP32 -> STM32 :
ACK,SENSOR_DATA

CLOUD STATUS:
DATA UPLOADED SUCCESSFULLY
<img width="1896" height="800" alt="Screenshot 2026-10-01 010448" src="https://github.com/user-attachments/assets/0badc863-9d18-4be5-b298-2e1daefe126c" />

## Project Structure
RTOS-Based-Edge-IoT-Gateway/
│
├── STM32/
│   ├── Core/
│   │   ├── Inc/
│   │   └── Src/
│   │
│   ├── Drivers/
│   ├── Middlewares/
│   │   └── FreeRTOS/
│   │
│   └── STM32F401RE.ioc
│
├── ESP32/
│   └── RTOS_Edge_IoT_Gateway/
│       └── RTOS_Edge_IoT_Gateway.ino
│
├── SystemView/
│   └── SEGGER configuration
│
└── README.md
## Key Technologies
STM32F401RE
FreeRTOS
Embedded C
ESP32
UART / USART
Wi-Fi
ThingSpeak
RTOS Queues
Event Flags
Software Timers
Watchdog
Fault Recovery
ACK Protocol
Packet Validation
SEGGER SystemView
STM32CubeIDE
STM32CubeMX
Arduino IDE
## Key Learning Outcomes

Through this project, the following concepts were implemented and analyzed:

Real-time embedded firmware development
FreeRTOS task management
Inter-task communication
RTOS synchronization
UART peripheral communication
Embedded communication protocols
ESP32 Wi-Fi integration
Cloud-based IoT monitoring
Watchdog-based fault recovery
RTOS debugging and task tracing
STM32–ESP32 system integration
## Author

Monika

MSc Tech Engineering Physics
National Institute of Technology, Warangal

GitHub:
https://github.com/mm24phc1r33-cmd

LinkedIn:
https://www.linkedin.com/in/monika-439921323/

## Project Highlights
STM32F401RE
      +
   FreeRTOS
      +
    ESP32
      +
     Wi-Fi
      +
 ThingSpeak
      +
SystemView
      =
End-to-End RTOS Edge IoT Gateway
