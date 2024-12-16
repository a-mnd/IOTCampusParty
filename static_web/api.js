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
    setInterval(() => getWeather(city, apiKey), 1800000); // Atualiza a cada 30 minutos
}

// Inicia as atualizações de clima
startWeatherUpdates();