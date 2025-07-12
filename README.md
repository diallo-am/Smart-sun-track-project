This project presents a smart solar tracking system that combines real-time sun tracking using LDR sensors and servo motors, with AI-based weather forecasting. The goal is to maximize photovoltaic energy production—even under variable weather conditions.

An intuitive mobile app allows users to monitor sensor values, weather forecasts, and battery charge levels in real-time.

Open Firmware part/report.pdf for more detailled information.

🔧 Key Features
🎯 Real-time sun tracking using LDR sensors and servos

📈 Embedded AI model for local weather prediction

🔋 Live monitoring of solar panel charge levels

📲 Firebase-connected mobile app displays:

Sun tracking data

Weather forecasts

Panel charge status

☁️ Cloud communication using Firebase Realtime Database

🔁 Serial communication (UART) between ESP32 and Arduino Uno

Solar Panel Orientation
🛠️ Technologies Used
ESP32 – Data collection and cloud communication

Arduino Uno – Controls solar panel orientation

LDR Sensors – Detect sun position

TensorFlow Lite (TFLite) – On-device weather forecasting

Firebase – Cloud backend for data sync

Mobile App – User interface (Android/iOS)

UART (Serial) – ESP32 → Arduino Uno communication

🎯 Project Objective
The objective is to build an intelligent and integrated solar tracking system capable of:

Dynamically tracking the sun for maximum exposure

Anticipating weather changes using AI to adjust positioning

Offering remote access to real-time and forecasted data

 Final mobile app (Android/iOS)

 Optional web dashboard

 Battery support for night operation

🤝 Contributing
Contributions are welcome! Feel free to open an issue or submit a pull request.

📄 License
This project is open-source and available under the MIT License.

