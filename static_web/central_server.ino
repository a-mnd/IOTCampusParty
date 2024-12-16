#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Access point credentials
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Create the WebServer object on port 80
WebServer server(80);

// Data structure to hold sensor data
struct SensorData {
  String location;
  float temperature;
  float humidity;
};

// Array to hold data from multiple rooms
const int maxRooms = 10;
SensorData sensorDataArray[maxRooms];
int roomCount = 0;

// HTML page template
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang='pt-br'>

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Monitor de Temperatura e Humidade</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link
        href="https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&display=swap"
        rel="stylesheet">
    <script type="module" src="https://unpkg.com/ionicons@7.1.0/dist/ionicons/ionicons.esm.js"></script>
    <script nomodule src="https://unpkg.com/ionicons@7.1.0/dist/ionicons/ionicons.js"></script>
    <style>
        * {
            padding: 0;
            margin: 0;
            box-sizing: border-box;
            font-family: "Roboto", sans-serif;
        }

        ::-webkit-scrollbar {
            display: none;
        }

        .conteudo-home {
            display: flex;
            justify-content: center;
            align-items: center;
            margin-bottom: 50px;
        }

        header {
            width: 100%;
            border-radius: 20px;
            box-shadow:
                0 1px 1px rgba(0, 0, 0, 0.1),
                0 2px 2px rgba(0, 0, 0, 0.15),
                0 4px 4px rgba(0, 0, 0, 0.06),
                0 8px 8px rgba(0, 0, 0, 0.04),
                0 16px 16px rgba(0, 0, 0, 0.02);
        }

        .container-header {
            display: flex;
            justify-content: center;
        }


        .boasVindas {
            margin: 20px;
            color: #263375;
            font-weight: 700;
            font-size: 3vw;
            width: 90%;
            text-align: center;
        }

        .boasVindas h2 {
            font-weight: 300;
        }

        .t1 {
            text-align: center;
            font-size: 4vw;
            font-weight: 700;
            margin: 20px;
        }

        .container-home {
            margin: 10px 10px;
            display: grid;
            grid-template-columns: 1fr;
            gap: 15px;
            align-items: center;
            width: 90%;
            margin-bottom: 20px;
            justify-items: center;

        }


        nav {
            position: fixed;
            bottom: 0;
            left: 0;
            width: 100%;
            height: 50px;
            border-radius: 20px;
            background-color: white;
            box-shadow: 0 -4px 6px rgba(0, 0, 0, 0.1);
            display: flex;
            justify-content: center;
            align-items: center;
            z-index: 1000;
        }

        .itens-nav {
            display: flex;
            justify-content: center;
            gap: 70%;
            align-items: center;
            width: 200px;
            font-size: 3vw;
        }

        .itens-nav a {
            text-decoration: none;
            color: #0a3d7d;
            font-weight: 500;
            font-size: 3vw;
        }


        .cards {
            min-height: 100px;
            padding: 15px;
            background-color: #263375;
            border-radius: 30px;
            display: grid;
            grid-template-columns: 1fr 2fr;
            align-items: center;
            justify-content: center;
            color: #ffffff;
            font-size: 2vw;
        }

        .sensor-data-show {
            padding: 15px;
            display: flex;
            flex-direction: column;
            gap: 5px;
            justify-content: center;
            justify-items: center;
            align-items: center;
            font-size: 5vw;
            width: 25vw;
            height: 25vw;
            border-radius: 50%;
            background-color: white;
            color: #0a3d7d;
        }

        .sensor-data-show h4 {
            color: rgba(238, 148, 51, 1);
        }

        .sensor-data-show p {
            color: rgb(11, 117, 255);
        }

        .sensor-data-show hr {
            width: 55%;
            border: 1px solid;
            background-color: #0a3d7d;
            /* margin: 10px 0; */
        }

        .sensor-data-show h4 {
            color: rgba(238, 148, 51, 1);
        }

        .sensor-data-show p {
            color: rgb(11, 117, 255);
        }

        .recommendations {
            padding: 0 10px;
            font-size: 4vw;
            display: grid;
            grid-template-columns: 1fr;
            gap: 10px;
        }

        .recommendations hr {
            border: 2px solid;
            background-color: #FFFFFF;
        }

        #div p {
            font-size: 3vw;
        }

        #div {
            display: grid;
            grid-template-columns: 1fr;
            gap: 10px;
        }
    </style>
</head>

<body onload="saudacao()">

    <header>
        <div class="container-header">
            <div class="boasVindas">
                <h2>Olá, <b class="user">Campuseiro</b></h2>
                <span id="saudacao"></span>
            </div>
        </div>
    </header>


    <main class="conteudo-home">

        <section class="container-home">
            <h1 class="t1">IOT Alarmes Inteligentes</h1>
            %DATA%
        </section>

    </main>

    <nav>
        <div class="itens-nav">
            <a href="#">IOT Alarmes Inteligentes
            </a>
        </div>
    </nav>

    <script>
        document.addEventListener('DOMContentLoaded', function () {
            const recommendationElement = document.getElementById('recommendation');
            let recommendations = [];
            const rooms = %ROOMCOUNT%;
            const temperature = %TEMPERATURE%;
            const humidity = %HUMIDITY%;
            if (temperature > 30) {
                recommendations.push('A temperatura está muito alta. Certifique-se de se manter hidratado.');
            } else if (temperature < 10) {
                recommendations.push('A temperatura está muito baixa. Mantenha-se aquecido.');
            } else {
                recommendations.push('A temperatura está agradável.');
            }
            if (humidity > 70) {
                recommendations.push('A umidade está muito alta. Pode causar desconforto.');
            } else if (humidity < 30) {
                recommendations.push('A umidade está muito baixa. Pode causar secura na pele.');
            } else {
                recommendations.push('A umidade está em um nível confortável.');
            }
            recommendationElement.innerHTML = recommendations.map(rec => `<p>${rec}</p>`).join('');
        });

        function saudacao() {
            var data = new Date();
            var hora = data.getHours();
            var saudacao;

            if (hora >= 5 && hora < 13) {
                saudacao = 'Bom dia';
            } else if (hora >= 13 && hora < 18) {
                saudacao = 'Boa tarde';
            } else {
                saudacao = 'Boa noite';
            }

            document.getElementById('saudacao').innerText = saudacao + ", bem vindo ao IOTai!";
        }

        async function getWeather(city, apiKey) {
            const url = `https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${apiKey}&units=metric&lang=pt`;

            try {
                const response = await fetch(url);
                if (!response.ok) {
                    throw new Error(`Erro ao fazer a solicitação: ${response.status}`);
                }
                const data = await response.json();
                const temp = data.main.temp;
                const tempMin = data.main.temp_min;
                const tempMax = data.main.temp_max;
                const humidity = data.main.humidity;
                const weatherDescription = data.weather[0].description;
                const weatherId = data.weather[0].id;

                const weatherIconClass = getWeatherIconClass(weatherId);

                document.getElementById('weather').innerHTML = `
              <div><span id="temp-api">${temp}° <i class="wi ${weatherIconClass}" id="icone-temperatura"></i><br></span>
              <p id="min-max">
              <i class="wi wi-direction-down">Min:${tempMin}°</i>
              <i class="wi wi-direction-up">Max:${tempMax}°</i><br>
              </p>
              </div>
            
          `;
                document.getElementById('info-desc').innerHTML = `
              <strong>Clima:</strong> ${weatherDescription.charAt(0).toUpperCase() + weatherDescription.slice(1)} <strong>Umidade:</strong> ${humidity}%
          `;
            } catch (error) {
                document.getElementById('weather').innerText = error.message;
                document.getElementById('info-desc').innerText = '';
            }
        }

        function getWeatherIconClass(weatherId) {
            if (weatherId >= 200 && weatherId < 300) {
                return 'wi-thunderstorm';
            } else if (weatherId >= 300 && weatherId < 400) {
                return 'wi-sprinkle';
            } else if (weatherId >= 500 && weatherId < 600) {
                return 'wi-rain';
            } else if (weatherId >= 600 && weatherId < 700) {
                return 'wi-snow';
            } else if (weatherId >= 700 && weatherId < 800) {
                return 'wi-fog';
            } else if (weatherId === 800) {
                return 'wi-day-sunny';
            } else if (weatherId > 800) {
                return 'wi-cloudy';
            } else {
                return 'wi-na';
            }
        }

        const apiKey = '94753d611e61a7b1aaadf87ad8a8b030';
        const city = 'São Paulo';

        // Função para atualizar o clima periodicamente
        function startWeatherUpdates() {
            getWeather(city, apiKey);
            setInterval(() => getWeather(city, apiKey), 300000); // Atualiza a cada 5 minutos
        }

        // Inicia as atualizações de clima
        startWeatherUpdates();
    </script>
</body>

</html>
)rawliteral";

// Function to handle root page
void handleRoot() {
  String dataHtml = "";
  for (int i = 0; i < roomCount; i++) {

    // -----------Nosso---------------
    dataHtml += "<section class="cards"><div class="sensor-data-show">";
    dataHtml += "<h4>" + String(sensorDataArray[i].temperature) + " &deg;C</h4><hr><p>" + String(sensorDataArray[i].humidity) + " %</p></div><div class="recommendations">";
    dataHtml += "<h2>" + sensorDataArray[i].location + "</h2><p><b>Recomendações:</b></p><div id='recommendation'></div></div></section>";
  }

  String html = htmlPage;
  html.replace("%DATA%", dataHtml);
  html.replace("%ROOMCOUNT%", String(roomCount));
  if (roomCount > 0) {
    html.replace("%TEMPERATURE%", String(sensorDataArray[0].temperature));
    html.replace("%HUMIDITY%", String(sensorDataArray[0].humidity));
  } else {
    html.replace("%TEMPERATURE%", "0");
    html.replace("%HUMIDITY%", "0");
  }

  server.send(200, "text/html", html);
}

// Function to handle POST data
void handlePostData() {
  if (server.hasArg("plain") == false) { // Check if body is received
    server.send(400, "text/plain", "Body not received");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Received data: " + body);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  String location = doc["location"];
  float temperature = doc["temperature"];
  float humidity = doc["humidity"];

  bool roomExists = false;

  for (int i = 0; i < roomCount; i++) {
    if (sensorDataArray[i].location == location) {
      sensorDataArray[i].temperature = temperature;
      sensorDataArray[i].humidity = humidity;
      roomExists = true;
      break;
    }
  }

  if (!roomExists && roomCount < maxRooms) {
    sensorDataArray[roomCount].location = location;
    sensorDataArray[roomCount].temperature = temperature;
    sensorDataArray[roomCount].humidity = humidity;
    roomCount++;
  }

  server.send(200, "text/plain", "Data received");
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Start the access point
  WiFi.softAP(ssid, password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Configure the server to serve the root page and handle POST data
  server.on("/", handleRoot);
  server.on("/post-data", HTTP_POST, handlePostData);

  // Start the server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();
}