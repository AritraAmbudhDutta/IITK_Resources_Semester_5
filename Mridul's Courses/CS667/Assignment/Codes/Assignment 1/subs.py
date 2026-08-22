import streamlit as st
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

# Initialize Streamlit app
st.title("MQTT Distance Data Display")
st.subheader(f"Subscribed to topic: {topic}")

# Streamlit placeholders for displaying the data
placeholder = st.empty()
temperature_placeholder = st.empty()
humidity_placeholder = st.empty()
motion_placeholder = st.empty()
distance_placeholder = st.empty()

# Streamlit chart placeholders with labels
st.write("Temperature Over Time")
temp_chart = st.line_chart()

st.write("Humidity Over Time")
hum_chart = st.line_chart()

st.write("Motion Detected Over Time (Discrete Points)")
motion_chart_placeholder = st.empty()

st.write("Distance Over Time")
distance_chart = st.line_chart()

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            st.write(f"Failed to connect, return code {rc}")

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        try:
            message = str(msg.payload.decode())
        except Exception as e:
            print(f"Error decoding message: {e}")
            return  # Skip further processing if decoding fails

        print(f"Received {message} from {msg.topic} topic")
        
        try:
            data = json.loads(message)
            temperature = data.get("temperature")
            humidity = data.get("humidity")
            motion = data.get("motion")
            distance = data.get("distance")
            
            # Display parsed data in Streamlit
            temperature_placeholder.text(f"Temperature: {temperature} °C")
            humidity_placeholder.text(f"Humidity: {humidity} %")
            motion_placeholder.text(f"Motion Detected: {motion}")
            distance_placeholder.text(f"Distance: {distance} cm")

            # Update the dataframe with new values
            global df
            new_row = pd.DataFrame({
                "timestamp": [pd.Timestamp.now()],
                "temperature": [temperature], 
                "humidity": [humidity], 
                "motion": [motion], 
                "distance": [distance]
            })
            df = pd.concat([df, new_row], ignore_index=True)
            
            # Update the charts
            temp_chart.line_chart(df['temperature'])
            hum_chart.line_chart(df['humidity'])
            
            # Use Altair for motion chart as discrete points
            motion_chart = alt.Chart(df).mark_circle(size=60).encode(
                x='timestamp:T',
                y='motion:N',
                color='motion:N',
                tooltip=['timestamp:T', 'motion:N']
            ).properties(
                width=700,
                height=400
            )
            motion_chart_placeholder.altair_chart(motion_chart)
            
            # Filter the distance chart for sensible values
            distance_chart.line_chart(df[df['distance'] < 60]['distance'])

        except json.JSONDecodeError:
            print("Failed to parse JSON data")

    client.subscribe(topic)
    client.on_message = on_message


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()

if __name__ == '__main__':
    run()