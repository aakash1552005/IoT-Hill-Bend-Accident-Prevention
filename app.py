"""
IoT-Based Accident Prevention & Real-Time Monitoring System for Hill Bend Roads
Author: Aakash S S, Sanjay G, Allwin Aji
Guide: Dr. M. Saravana Karthikeyan
Institution: Vel Tech University, Chennai
"""

from flask import Flask, render_template, Response, jsonify
import cv2
import torch
import requests
import numpy as np
import time
import threading

app = Flask(__name__)

# ─────────────────────────────────────────────
# YOLOv5 Model Configuration
# ─────────────────────────────────────────────
model = torch.hub.load('ultralytics/yolov5', 'custom', path='yolov5s.pt')
model.conf = 0.25  # Confidence threshold

# ─────────────────────────────────────────────
# ESP32-CAM Configuration
# Update this IP after flashing ESP32-CAM
# ─────────────────────────────────────────────
ESP32_CAM_IP = "10.211.70.64"
ESP32_CAM_URL = f"http://{ESP32_CAM_IP}/capture"

# Detection state (shared between threads)
detection_result = {"status": "No vehicle detected"}

# ─────────────────────────────────────────────
# Vehicle classes to detect
# ─────────────────────────────────────────────
VEHICLE_CLASSES = ['car', 'truck', 'bus', 'motorcycle']


def get_image():
    """Fetch a JPEG frame from ESP32-CAM via HTTP."""
    try:
        response = requests.get(ESP32_CAM_URL, timeout=3)
        img_array = np.array(bytearray(response.content), dtype=np.uint8)
        img = cv2.imdecode(img_array, -1)
        return img
    except Exception:
        print("[WARN] Failed to fetch image from ESP32-CAM")
        return None


def control_signal(zone: str):
    """Send HTTP command to ESP32-CAM to switch traffic lights."""
    try:
        requests.get(f"http://{ESP32_CAM_IP}/{zone}", timeout=2)
    except Exception:
        print(f"[WARN] Failed to send signal command: {zone}")


def detect_vehicle():
    """
    Main detection loop:
    - Fetch frame from ESP32-CAM
    - Run YOLOv5 inference
    - Classify detections into LEFT / RIGHT zones
    - Control traffic LEDs accordingly
    """
    global detection_result

    while True:
        img = get_image()
        if img is None:
            time.sleep(1)
            continue

        # Run YOLOv5 inference
        results = model(img)
        detected_objects = results.pandas().xyxy[0]

        # Split frame into LEFT and RIGHT detection zones around center
        frame_width = img.shape[1]
        center = frame_width // 2
        zone_width = 240  # pixels on each side of center

        right_min = center - zone_width
        right_max = center - 1
        left_min  = center + 1
        left_max  = center + zone_width

        # Check for vehicles in each zone
        left_detected = any(
            left_min <= obj['xmin'] <= left_max and obj['name'] in VEHICLE_CLASSES
            for _, obj in detected_objects.iterrows()
        )
        right_detected = any(
            right_min <= obj['xmax'] <= right_max and obj['name'] in VEHICLE_CLASSES
            for _, obj in detected_objects.iterrows()
        )

        # Decision logic → control signals
        if left_detected and right_detected:
            control_signal("both")
            detection_result = {"status": "⚠️ Vehicles on both sides — STOP and wait."}

        elif left_detected:
            control_signal("left")
            detection_result = {"status": "✅ Right lane clear — you're free to go."}

        elif right_detected:
            control_signal("right")
            detection_result = {"status": "✅ Left lane clear — you're free to go."}

        else:
            control_signal("none")
            detection_result = {"status": "🟢 Road clear — both lanes open."}

        time.sleep(1)  # Poll every second


# ─────────────────────────────────────────────
# Flask Routes
# ─────────────────────────────────────────────

@app.route('/')
def index():
    return render_template('index.html')


@app.route('/video_feed')
def video_feed():
    """Stream live JPEG frames from ESP32-CAM as MJPEG."""
    def generate():
        while True:
            img = get_image()
            if img is not None:
                _, buffer = cv2.imencode('.jpg', img)
                frame = buffer.tobytes()
                yield (
                    b'--frame\r\n'
                    b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n'
                )
            time.sleep(1)

    return Response(
        generate(),
        mimetype='multipart/x-mixed-replace; boundary=frame'
    )


@app.route('/get_status')
def get_status():
    """Return the latest detection result as JSON."""
    return jsonify(detection_result)


# ─────────────────────────────────────────────
# Entry Point
# ─────────────────────────────────────────────
if __name__ == '__main__':
    # Start detection loop in background thread
    threading.Thread(target=detect_vehicle, daemon=True).start()
    app.run(host='0.0.0.0', port=5000, debug=False)
