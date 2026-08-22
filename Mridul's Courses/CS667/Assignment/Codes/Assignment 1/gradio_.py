import gradio as gr
import random
import json
import pandas as pd
import altair as alt
from paho.mqtt import client as mqtt_client

# MQTT Broker details
broker = 'broker.emqx.io'
port = 1883
topic = "emqx/esp8266"
client_id = f'subscribe-{random.randint(0, 100)}'
username = 'emqx'
password = 'public'

# DataFrame to store received data for plotting
data = {
    "timestamp": [],
    "temperature": [],
    "humidity": [],
    "motion": [],
    "distance": []
}

df = pd.DataFrame(data)

# Global variables to hold the latest values
latest_temperature = None
latest_humidity = None
latest_motion = None
latest_distance = None

# Function to handle MQTT connection
def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print(f"Failed to connect, return code {rc}")

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

# Function to subscribe to the topic and handle incoming messages
def subscribe(client):
    def on_message(client, userdata, msg):
        global df, latest_temperature, latest_humidity, latest_motion, latest_distance
        try:
            message = str(msg.payload.decode())
            print(f"Received {message} from {msg.topic} topic")
            
            data = json.loads(message)
            latest_temperature = data.get("temperature")
            latest_humidity = data.get("humidity")
            latest_motion = data.get("motion")
            latest_distance = data.get("distance")

            new_row = pd.DataFrame({
                "timestamp": [pd.Timestamp.now()],
                "temperature": [latest_temperature], 
                "humidity": [latest_humidity], 
                "motion": [latest_motion], 
                "distance": [latest_distance]
            })
            df = pd.concat([df, new_row], ignore_index=True)

        except Exception as e:
            print(f"Error processing message: {e}")

    client.subscribe(topic)
    client.on_message = on_message

# Function to update charts and display data
def update_charts():
    global df, latest_temperature, latest_humidity, latest_motion, latest_distance

    # Generate charts with Altair
    temp_chart = alt.Chart(df).mark_line().encode(
        x='timestamp:T',
        y='temperature:Q',
        tooltip=['timestamp:T', 'temperature:Q']
    ).properties(title="Temperature Over Time")

    hum_chart = alt.Chart(df).mark_line().encode(
        x='timestamp:T',
        y='humidity:Q',
        tooltip=['timestamp:T', 'humidity:Q']
    ).properties(title="Humidity Over Time")

    motion_chart = alt.Chart(df).mark_circle(size=60).encode(
        x='timestamp:T',
        y='motion:N',
        color='motion:N',
        tooltip=['timestamp:T', 'motion:N']
    ).properties(
        title="Motion Detected Over Time (Discrete Points)",
        width=700,
        height=400
    )

    distance_chart = alt.Chart(df[df['distance'] < 60]).mark_line().encode(
        x='timestamp:T',
        y='distance:Q',
        tooltip=['timestamp:T', 'distance:Q']
    ).properties(title="Distance Over Time")

    # Return the latest parsed data and the charts
    return (
        f"Temperature: {latest_temperature} °C" if latest_temperature is not None else "Temperature: N/A",
        f"Humidity: {latest_humidity} %" if latest_humidity is not None else "Humidity: N/A",
        f"Motion Detected: {latest_motion}" if latest_motion is not None else "Motion Detected: N/A",
        f"Distance: {latest_distance} cm" if latest_distance is not None else "Distance: N/A",
        temp_chart,
        hum_chart,
        motion_chart,
        distance_chart
    )

# Set up Gradio interface
interface = gr.Interface(
    fn=update_charts,
    inputs=[],
    outputs=[
        gr.Textbox(label="Latest Temperature"),
        gr.Textbox(label="Latest Humidity"),
        gr.Textbox(label="Latest Motion"),
        gr.Textbox(label="Latest Distance"),
        gr.Plot(label="Temperature Over Time"),
        gr.Plot(label="Humidity Over Time"),
        gr.Plot(label="Motion Detected Over Time"),
        gr.Plot(label="Distance Over Time")
    ],
    live=True
)

def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_start()  # Start the MQTT client in a non-blocking manner
    interface.launch()   # Launch the Gradio interface

if __name__ == '__main__':
    run()
