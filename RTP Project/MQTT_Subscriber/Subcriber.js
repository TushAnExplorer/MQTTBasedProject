const mqtt = require('mqtt');
const mqttClient = mqtt.connect('mqtt://192.168.1.25:1883', {ClientID: "Client_Subcribe_RP", username: "Tush", password: "Jeni" });

const queryTopic1 = 'Accelerometer';
const queryTopic2 = 'Hall_Effect';
const queryTopic3 = 'BMP180';

mqttClient.on('connect', function () {
    console.log('Server connected to Mqtt broker');
    mqttClient.subscribe(queryTopic1);
});


mqttClient.on('connect', function () {
    console.log('Server connected to Mqtt broker');
    mqttClient.subscribe(queryTopic2);
});


mqttClient.on('connect', function () {
    console.log('Server connected to Mqtt broker');
    mqttClient.subscribe(queryTopic3);
});
//============================================

//=================================================
const WebSocketProducer = require('ws');

// Creating a Websocket producer
const wsp = new WebSocketProducer.Server({ port: 8085 });

// Event listener when there was connection
wsp.on('connection', (ws) => {
    onConnection(ws);

    // If there was a message on that connection
    ws.on('message', (message) => {
        console.log("RX from client: " + message);
    });

    ws.on('error', (err) => {
        console.log("Error on Websocket: " + err);
    });

    ws.on('close', () => {
        console.log("Client closed the connection ");
    });
});


//================================================


function onConnection(ws) {

// On receiving message from any client
mqttClient.on('message', function (topic, messageSensor) {
//if(topic.toString() == "BMP180"){
    //console.log("Producer 1");
    var status=ws.send(messageSensor.toString());
    //console.log("Producer 1:",messageSensor.toString());
//}
    console.log('Received Data from ',topic.toString() + ' : ', messageSensor.toString());
});
//sleep(1000);

}


//====================================

const WebSocketConsumer = require('ws');

const ws = new WebSocketConsumer('ws://127.0.0.1:8085');

ws.on('open', () =>{
    console.log('Connected to the server');
});

ws.on('message', (msg) => {
    //console.log("Consumer 1");
    console.log(msg);
});

//setTimeout(Subscriber_Callback,1000);




