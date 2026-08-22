import pandas as pd
import random
import json
import plotly.express as px
import plotly.graph_objects as go
from paho.mqtt import client as mqtt_client
import streamlit as st

# MQTT connection details
MQTT_BROKER = 'broker.emqx.io'
MQTT_PORT = 1883
MQTT_TOPIC = "emqx/esp8266"
CLIENT_ID = f'client-{random.randint(1000, 9999)}'
MQTT_USERNAME = 'emqx'
MQTT_PASSWORD = 'public'

# Initialize an empty DataFrame for incoming data
data_columns = {
    "timestamp": [],
    "temperature": [],
    "humidity": [],
    "motion": [],
    "distance": []
}
data_df = pd.DataFrame(data_columns)

# Streamlit app setup
st.title("MQTT Sensor Data Visualization")
st.subheader(f"Listening on: {MQTT_TOPIC}")

# Placeholders for data and charts in Streamlit
temp_display = st.empty()
humidity_display = st.empty()
motion_display = st.empty()
distance_display = st.empty()

# Placeholders for Plotly charts
temp_chart_area = st.empty()
humidity_chart_area = st.empty()
motion_chart_area = st.empty()
distance_chart_area = st.empty()

def initialize_mqtt_client() -> mqtt_client:
    """Establishes an MQTT client connection."""
    
    def handle_connection(client, userdata, flags, rc):
        if rc == 0:
            print("Successfully connected to MQTT Broker")
        else:
            st.write(f"Connection failed with error code {rc}")

    client = mqtt_client.Client(CLIENT_ID)
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = handle_connection
    client.connect(MQTT_BROKER, MQTT_PORT)
    return client

def setup_subscription(client: mqtt_client):
    """Subscribes to the specified MQTT topic and handles incoming messages."""
    
    def handle_message(client, userdata, msg):
        try:
            decoded_msg = msg.payload.decode()
            print(f"Message received: {decoded_msg} from topic: {msg.topic}")
            parsed_data = json.loads(decoded_msg)

            # Extract data fields
            temperature = parsed_data.get("temperature")
            humidity = parsed_data.get("humidity")
            motion = parsed_data.get("motion")
            distance = parsed_data.get("distance")

            # Display data in Streamlit
            temp_display.text(f"Temperature: {temperature} °C")
            humidity_display.text(f"Humidity: {humidity} %")
            motion_display.text(f"Motion: {motion}")
            distance_display.text(f"Distance: {distance} cm")

            # Update the data frame with new data
            add_data_to_df(temperature, humidity, motion, distance)

            # Update the charts with the latest data
            refresh_charts()

        except json.JSONDecodeError:
            print("Error parsing JSON message")

    client.subscribe(MQTT_TOPIC)
    client.on_message = handle_message

def add_data_to_df(temperature, humidity, motion, distance):
    """Appends new data to the DataFrame."""
    global data_df
    new_entry = {
        "timestamp": pd.Timestamp.now(),
        "temperature": temperature,
        "humidity": humidity,
        "motion": motion,
        "distance": distance
    }
    data_df = pd.concat([data_df, pd.DataFrame([new_entry])], ignore_index=True)

def refresh_charts():
    """Updates the Plotly charts in the Streamlit app."""
    global data_df

    # Plot temperature data vs time as a bar plot
    temp_fig = px.bar(data_df, x='timestamp', y='temperature', title='Temperature Over Time', labels={'timestamp': 'Time', 'temperature': 'Temperature (°C)'})
    temp_chart_area.plotly_chart(temp_fig, use_container_width=True)

    # Plot humidity data vs time as a bar plot
    hum_fig = px.bar(data_df, x='timestamp', y='humidity', title='Humidity Over Time', labels={'timestamp': 'Time', 'humidity': 'Humidity (%)'})
    humidity_chart_area.plotly_chart(hum_fig, use_container_width=True)

    # Plot motion data as discrete points
    motion_fig = go.Figure(data=go.Scatter(
        x=data_df['timestamp'],
        y=data_df['motion'],
        mode='markers',
        marker=dict(color='green'),
        name='Motion'
    ))
    motion_fig.update_layout(title='Motion Detection Over Time', xaxis_title='Time', yaxis_title='Motion')
    motion_chart_area.plotly_chart(motion_fig, use_container_width=True)

    # Plot distance data vs time as a bar plot with filtering
    distance_filtered_df = data_df[data_df['distance'] < 60]
    distance_fig = px.bar(distance_filtered_df, x='timestamp', y='distance', title='Distance Over Time', labels={'timestamp': 'Time', 'distance': 'Distance (cm)'})
    distance_chart_area.plotly_chart(distance_fig, use_container_width=True)

def main():
    mqtt_client_instance = initialize_mqtt_client()
    setup_subscription(mqtt_client_instance)
    mqtt_client_instance.loop_forever()

if __name__ == '__main__':
    main()
