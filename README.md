# 🚦 IoT-Based Accident Prevention & Real-Time Monitoring System for Hill Bend Roads

![Python](https://img.shields.io/badge/Python-3.8+-blue?style=flat-square&logo=python)
![Flask](https://img.shields.io/badge/Flask-2.0-black?style=flat-square&logo=flask)
![YOLOv5](https://img.shields.io/badge/YOLOv5-Object%20Detection-red?style=flat-square)
![ESP32-CAM](https://img.shields.io/badge/ESP32--CAM-IoT-green?style=flat-square)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-blue?style=flat-square&logo=opencv)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)

> **Minor Project-I** | B.Tech CSE | Vel Tech Rangarajan Dr. Sagunthala R&D Institute of Science and Technology  
> **Team:** Aakash S S · Sanjay G · Allwin Aji | **Guide:** Dr. M. Saravana Karthikeyan

---

## 📌 Overview

Accidents on hill bend roads are a major concern due to sharp curves, blind spots, and unpredictable vehicle movement. Traditional safety measures like convex mirrors and IR sensors are often ineffective in fog, rain, or low-light conditions.

This project presents a **smart IoT solution** using an **ESP32-CAM** module and **YOLOv5** deep learning model to detect vehicles approaching from either side of a hill bend in real-time — automatically triggering traffic signals, voice alerts, and a live web dashboard to prevent head-on collisions.

---

## 🎯 Features

- 📷 **Real-time vehicle detection** using YOLOv5 on live ESP32-CAM feed
- 🚦 **Automated traffic signal control** (Red/Green LEDs) via GPIO over Wi-Fi
- 🌐 **Live web dashboard** with video stream and detection status
- 🔊 **Voice alerts** using browser SpeechSynthesis API
- 📍 **GPS-based proximity trigger** (planned) to activate streaming only when vehicles are near
- 📊 **Centralized monitoring** — view traffic status remotely from any device
- ⚡ **Bandwidth-efficient** — streams and alerts only when needed

---

## 🛠️ Tech Stack

| Layer | Technology |
|-------|-----------|
| Microcontroller | ESP32-CAM (AI Thinker) |
| Object Detection | YOLOv5 (Ultralytics) |
| Backend | Python + Flask |
| Image Processing | OpenCV |
| Frontend | HTML + Tailwind CSS |
| Communication | HTTP over Wi-Fi (IEEE 802.11) |
| Hardware | LED traffic lights, breadboard, jumper wires |
| GPS (planned) | Neo-6M GPS Module |

---

## 🏗️ System Architecture

```
ESP32-CAM (captures JPEG frames)
        ↓
Flask Server (Python backend)
        ↓
YOLOv5 Processing (vehicle detection)
        ↓
Decision Logic (LEFT / RIGHT / BOTH / NONE)
        ↓
┌───────────────┬──────────────────┬──────────────────┐
│ Traffic LEDs  │  Web Dashboard   │  Voice Alerts    │
│ (GPIO/HTTP)   │  (Live Stream)   │  (Speech API)    │
└───────────────┴──────────────────┴──────────────────┘
```

---

## 📂 Project Structure

```
hill-bend-iot/
├── app.py                  # Flask backend + YOLOv5 detection loop
├── templates/
│   └── index.html          # Live dashboard UI
├── yolov5s.pt              # YOLOv5 model weights (download separately)
├── ESP32_cam_code/
│   └── esp32_cam.ino       # Arduino sketch for ESP32-CAM
├── requirements.txt
└── README.md
```

---

## ⚙️ How It Works

1. **ESP32-CAM** connects to Wi-Fi and serves JPEG frames at `/capture`
2. **Flask server** fetches frames every second from the ESP32-CAM
3. **YOLOv5** runs inference and detects vehicles (`car`, `truck`, `bus`, `motorcycle`)
4. Frame is split at midpoint into **LEFT** and **RIGHT** zones
5. Based on detection:

| Scenario | Action |
|----------|--------|
| Vehicle on LEFT only | Left = GREEN, Right = RED |
| Vehicle on RIGHT only | Right = GREEN, Left = RED |
| Vehicles on BOTH sides | Both = RED (halt) |
| No vehicle | Both = GREEN |

6. Status is pushed to the **web dashboard** and read aloud via **voice alert**

---

## 🚀 Getting Started

### Prerequisites
- Python 3.8+
- Arduino IDE with ESP32 board support
- YOLOv5 (`yolov5s.pt`) model weights

### 1. Clone the repository
```bash
git clone https://github.com/aakash1552005/IoT-Hill-Bend-Accident-Prevention.git
cd IoT-Hill-Bend-Accident-Prevention
```

### 2. Install Python dependencies
```bash
pip install flask opencv-python torch torchvision requests numpy
```

### 3. Flash the ESP32-CAM
- Open `ESP32_cam_code/esp32_cam.ino` in Arduino IDE
- Update your Wi-Fi credentials:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```
- Install libraries: `ESPAsyncWebServer`, `AsyncTCP`
- Flash to ESP32-CAM using USB-to-TTL adapter

### 4. Update the ESP32-CAM IP in `app.py`
```python
ESP32_CAM_URL = "http://<YOUR_ESP32_IP>/capture"
```

### 5. Run the Flask server
```bash
python app.py
```

### 6. Open the dashboard
```
http://localhost:5000
```

---

## 🔌 Hardware Setup

| Component | Quantity |
|-----------|----------|
| ESP32-CAM (AI Thinker) | 1 |
| Red LEDs | 2 |
| Green LEDs | 2 |
| Resistors (220Ω) | 4 |
| Breadboard | 1 |
| Jumper Wires | As needed |
| USB-to-TTL Adapter | 1 |
| 5V/2A Power Supply | 1 |

**GPIO Pin Mapping (ESP32):**
- GPIO 12 → LEFT RED
- GPIO 13 → LEFT GREEN  
- GPIO 14 → RIGHT RED
- GPIO 15 → RIGHT GREEN

---

## 📸 Demo

| Hardware Prototype | Live Dashboard |
|:-----------------:|:--------------:|
| ESP32-CAM + LEDs on breadboard | Web UI with live video stream + voice alerts |

> The prototype was tested using a miniature road model with a toy car — the system successfully detected vehicle presence and toggled LED signals in real-time.

---

## 📊 Results

| Scenario | Detection Accuracy | Response Time |
|----------|--------------------|---------------|
| Single vehicle (clear lighting) | ~90% | < 1 sec |
| Both-side vehicles | ~87% | < 1 sec |
| No vehicle (no false positive) | ~95% | < 1 sec |

---

## 🔮 Future Enhancements

- [ ] GPS-based proximity trigger (Neo-6M) — auto-activate only when vehicles approach
- [ ] Night vision / thermal camera support
- [ ] SMS alerts via Twilio/SMS gateway
- [ ] Edge computing for offline detection
- [ ] V2I (Vehicle-to-Infrastructure) communication
- [ ] Multi-bend centralized monitoring dashboard

---

## 📄 Project Report

This project was submitted as **Minor Project-I** for B.Tech CSE at Vel Tech University, November 2025.

**Plagiarism Score:** 8% (92% unique) ✅

---

## 👨‍💻 Team

| Name | Roll No |
|------|---------|
| Aakash S S | 23UECS0999 |
| Sanjay G | 23UECS0873 |
| Allwin Aji | 23UECS0947 |

**Guide:** Dr. M. Saravana Karthikeyan, Associate Professor, Dept. of CSE, Vel Tech University

---

## 📜 License

This project is licensed under the MIT License.

---

## 🔗 Related Links

- 🔗 [GitHub Profile](https://github.com/aakash1552005)
- 💼 [LinkedIn](https://www.linkedin.com/in/aakash-s-s-88b91a301)
