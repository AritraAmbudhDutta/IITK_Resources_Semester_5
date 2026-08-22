import tkinter as tk
from paho.mqtt.client import Client

# MQTT callback functions
def on_connect(client, userdata, flags, rc):
    client.subscribe("sensors/#")

def on_message(client, userdata, msg):
    topic = msg.topic.split('/')[-1]
    data = msg.payload.decode()
    
    if topic == "distance":
        distance_label.config(text=f"Distance: {data} cm")
    elif topic == "temperature":
        temperature_label.config(text=f"Temperature: {data} °C")
    elif topic == "humidity":
        humidity_label.config(text=f"Humidity: {data} %")
    elif topic == "motion":
        motion_label.config(text=f"Motion Status: {data}")

# Initialize the MQTT client
client = Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("your_mqtt_broker_ip", 1883, 60)  # Replace with your MQTT broker details

# Create the GUI using Tkinter
root = tk.Tk()
root.title("Real-time Sensor Data")

distance_label = tk.Label(root, text="Distance: N/A", font=("Helvetica", 16))
distance_label.pack(pady=10)

temperature_label = tk.Label(root, text="Temperature: N/A", font=("Helvetica", 16))
temperature_label.pack(pady=10)

humidity_label = tk.Label(root, text="Humidity: N/A", font=("Helvetica", 16))
humidity_label.pack(pady=10)

motion_label = tk.Label(root, text="Motion Status: N/A", font=("Helvetica", 16))
motion_label.pack(pady=10)

# Start the MQTT loop
client.loop_start()

# Start the Tkinter event loop
root.mainloop()
