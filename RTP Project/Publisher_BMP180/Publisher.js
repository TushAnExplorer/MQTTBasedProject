var bmp180 = require('bmp180-sensor');

const mqtt = require('mqtt');
const mqttClient = mqtt.connect('mqtt://192.168.1.25:1883', {ClientID: "Client_Subcribe_RPI", username: "Tush", password: "Jeni" });


const BMP180_topic = 'BMP180';

mqttClient.on('connect', function () {
    console.log('Client connected to Mqtt broker');
});

async function BMP180_Subcriber_Callback(){
const sensor = await bmp180({address: 0x77,
                     mode: 1,
                     units: 'metric'});

var data = await sensor.read()


   // await sensor.close()
	data.Device_ID = 9800;
    console.log(data);

    // Publish message
    mqttClient.publish(BMP180_topic,JSON.stringify(data));
    console.log('Published to server...');


}

setInterval(BMP180_Subcriber_Callback,2000);


